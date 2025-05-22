import cv2
import numpy as np
from sklearn.cluster import KMeans
from collections import deque

class SquareColorDetector:
    def __init__(self):
        self.square_size_cm = 2
        self.pixels_per_cm = None
        self.square_size_pixels = None
        self.square_position = None
        self.color_margin = 15
        self.last_result = None
        self.stable_color_history = deque(maxlen=10)
        self.stable_threshold = 5
        self.stable_intervals = None

    def calibrate(self, frame, object_width_cm, object_width_pixels):
        """Calibrate the pixel-to-cm ratio"""
        self.pixels_per_cm = object_width_pixels / object_width_cm
        self.square_size_pixels = int(self.square_size_cm * self.pixels_per_cm)
        height, width = frame.shape[:2]
        self.square_position = (
            (width - self.square_size_pixels) // 2,
            (height - self.square_size_pixels) // 2
        )
        print(f"[SYSTEM] Calibration complete. Detection area: {self.square_size_pixels}x{self.square_size_pixels}px")

    def get_square_roi(self, frame):
        """Get the region of interest (square area)"""
        x, y = self.square_position
        size = self.square_size_pixels
        return frame[y:y+size, x:x+size]

    def _get_color_name(self, hsv):
        """Classify the color based on HSV values"""
        hue = hsv[0]
        if hue < 5 or hue > 175: return "Red"
        elif 5 <= hue < 22: return "Orange"
        elif 22 <= hue < 33: return "Yellow"
        elif 33 <= hue < 78: return "Green"
        elif 78 <= hue < 130: return "Blue"
        elif 130 <= hue < 150: return "Purple"
        elif 150 <= hue < 175: return "Pink"
        else: return "Unknown"

    def detect_color_ranges(self, roi):
        """Detect color ranges in the ROI"""
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        pixels = hsv.reshape((-1, 3))
        
        if len(pixels) == 0:
            self.last_result = None
            return None
            
        kmeans = KMeans(n_clusters=3, n_init=10)
        kmeans.fit(pixels)
        dominant_cluster = np.argmax(np.bincount(kmeans.labels_))
        dominant_color = kmeans.cluster_centers_[dominant_cluster]
        
        # Calculate bounds
        lower = np.maximum(0, dominant_color - self.color_margin)
        upper = np.minimum([179, 255, 255], dominant_color + self.color_margin)
        
        result = {
            'dominant_hsv': dominant_color,
            'lower': lower.astype(np.uint8),
            'upper': upper.astype(np.uint8),
            'is_red': False,
            'timestamp': cv2.getTickCount()
        }
        
        if dominant_color[0] < 15 or dominant_color[0] > 165:
            result['is_red'] = True
            result['lower2'] = np.array([dominant_color[0] - self.color_margin, lower[1], lower[2]])
            result['upper2'] = np.array([179, upper[1], upper[2]])
            result['lower1'] = np.array([0, lower[1], lower[2]])
            result['upper1'] = np.array([dominant_color[0] + self.color_margin, upper[1], upper[2]])
        
        self._check_stability(result)
        self.last_result = result
        self._print_result_to_console(result)
        return result

    def _check_stability(self, current_result):
        """Check if color has been stable and update intervals"""
        if not self.stable_color_history or not self._colors_similar(current_result, self.stable_color_history[-1]):
            self.stable_color_history.clear()
            self.stable_intervals = None
        
        self.stable_color_history.append(current_result)
        
        if len(self.stable_color_history) >= self.stable_threshold:
            # Initialize intervals if needed
            if self.stable_intervals is None:
                self.stable_intervals = {
                    'lowest_lower': current_result['lower'].copy(),
                    'highest_lower': current_result['lower'].copy(),
                    'lowest_upper': current_result['upper'].copy(),
                    'highest_upper': current_result['upper'].copy()
                }
            
            # Update intervals
            self.stable_intervals['lowest_lower'] = np.minimum(
                self.stable_intervals['lowest_lower'], current_result['lower'])
            self.stable_intervals['highest_lower'] = np.maximum(
                self.stable_intervals['highest_lower'], current_result['lower'])
            self.stable_intervals['lowest_upper'] = np.minimum(
                self.stable_intervals['lowest_upper'], current_result['upper'])
            self.stable_intervals['highest_upper'] = np.maximum(
                self.stable_intervals['highest_upper'], current_result['upper'])

    def _colors_similar(self, result1, result2, threshold=10):
        """Check if two colors are similar"""
        h_diff = abs(result1['dominant_hsv'][0] - result2['dominant_hsv'][0])
        s_diff = abs(result1['dominant_hsv'][1] - result2['dominant_hsv'][1])
        v_diff = abs(result1['dominant_hsv'][2] - result2['dominant_hsv'][2])
        return h_diff < threshold and s_diff < threshold and v_diff < threshold

    def _print_result_to_console(self, result):
        """Print results to console with all intervals"""
        if result is None:
            print("[DETECTION] No color detected in the square area")
            return
            
        print("\n[DETECTION RESULTS]")
        print(f"Dominant HSV: {result['dominant_hsv']}")
        
        color_name = self._get_color_name(result['dominant_hsv'])
        print(f"Color: {color_name}")
        print(f"Range: Lower {result['lower']} - Upper {result['upper']}")
        
        if result['is_red']:
            print(f"Red Range 2: Lower {result['lower2']} - Upper {result['upper2']}")
        
        if self.stable_intervals:
            print("\n[STABLE INTERVALS]")
            print("Lower Bounds:")
            print(f"  Lowest: {self.stable_intervals['lowest_lower']}")
            print(f"  Highest: {self.stable_intervals['highest_lower']}")
            print("Upper Bounds:")
            print(f"  Lowest: {self.stable_intervals['lowest_upper']}")
            print(f"  Highest: {self.stable_intervals['highest_upper']}")

    def draw_interface(self, frame):
        """Draw the interface on the frame"""
        x, y = self.square_position
        size = self.square_size_pixels
        
        cv2.rectangle(frame, (x, y), (x+size, y+size), (255, 255, 255), 2)
        self._draw_text_results(frame)
        
        return frame

    def _draw_text_results(self, frame):
        """Draw text results on the frame with all intervals"""
        text_y = 30
        line_height = 25
        
        # Header
        cv2.putText(frame, "Color Detection Results:", (10, text_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 1)
        text_y += line_height
        
        if self.last_result is None:
            cv2.putText(frame, "No color detected", (10, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)
            return
            
        # Color and stability info
        color_name = self._get_color_name(self.last_result['dominant_hsv'])
        stability_status = "STABLE" if self.stable_intervals else "UNSTABLE"
        color = (0, 255, 0) if stability_status == "STABLE" else (0, 0, 255)
        
        cv2.putText(frame, f"Color: {color_name} ({stability_status})", (10, text_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
        text_y += line_height
        
        # Current values
        h, s, v = self.last_result['dominant_hsv']
        cv2.putText(frame, f"Current HSV: ({h:.0f}, {s:.0f}, {v:.0f})", (10, text_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 200), 1)
        text_y += line_height
        
        # Current ranges
        cv2.putText(frame, "Current Ranges:", (10, text_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 255), 1)
        text_y += line_height
        cv2.putText(frame, f"  Lower: {self.last_result['lower']}", (20, text_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)
        text_y += line_height
        cv2.putText(frame, f"  Upper: {self.last_result['upper']}", (20, text_y), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)
        text_y += line_height
        
        # Stable intervals if available
        if self.stable_intervals:
            cv2.putText(frame, "Stable Intervals:", (10, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 1)
            text_y += line_height
            
            # Lower bounds
            cv2.putText(frame, "Lower Bounds:", (10, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 255), 1)
            text_y += line_height
            cv2.putText(frame, f"  Lowest: {self.stable_intervals['lowest_lower']}", (20, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)
            text_y += line_height
            cv2.putText(frame, f"  Highest: {self.stable_intervals['highest_lower']}", (20, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)
            text_y += line_height
            
            # Upper bounds
            cv2.putText(frame, "Upper Bounds:", (10, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 255), 1)
            text_y += line_height
            cv2.putText(frame, f"  Lowest: {self.stable_intervals['lowest_upper']}", (20, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)
            text_y += line_height
            cv2.putText(frame, f"  Highest: {self.stable_intervals['highest_upper']}", (20, text_y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, (200, 200, 200), 1)

def main():
    detector = SquareColorDetector()
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    
    calibration_done = False
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        frame = cv2.flip(frame, 1)
        
        if not calibration_done:
            # Place a 5cm object in view and measure its pixel width
            # Then replace these values with your measurements
            detector.calibrate(frame, 5.0, 150)
            calibration_done = True
        
        roi = detector.get_square_roi(frame)
        
        if roi.size > 0:
            color_ranges = detector.detect_color_ranges(roi)
            frame = detector.draw_interface(frame)
            cv2.imshow('Detection Area', roi)
        
        cv2.imshow('Color Range Detector', frame)
        
        key = cv2.waitKey(1)
        if key == ord('q'):
            break
    
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()