import cv2
import numpy as np
from collections import defaultdict
from sklearn.cluster import KMeans

class AutomaticColorDetector:
    def __init__(self):
        # Minimum percentage of non-black pixels to consider detection valid
        self.min_valid_pixels = 0.05  # 5% of image
        
    def detect_dominant_color(self, frame):
        """Detect the dominant color in an image with black background"""
        # Convert to HSV color space
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        
        # Create mask for non-black pixels (V > 20)
        v_channel = hsv[:,:,2]
        non_black_mask = v_channel > 20
        
        # Check if we have enough non-black pixels
        non_black_count = np.sum(non_black_mask)
        total_pixels = frame.shape[0] * frame.shape[1]
        
        if non_black_count / total_pixels < self.min_valid_pixels:
            return None  # Not enough colored pixels
        
        # Get only non-black pixels
        non_black_pixels = hsv[non_black_mask]
        
        # Use K-Means clustering to find dominant colors
        kmeans = KMeans(n_clusters=3, n_init=10)
        kmeans.fit(non_black_pixels)
        
        # Get the dominant cluster (the one with most points)
        dominant_cluster = np.argmax(np.bincount(kmeans.labels_))
        dominant_color = kmeans.cluster_centers_[dominant_cluster]
        
        # Convert back to BGR for display
        dominant_color_bgr = cv2.cvtColor(
            np.uint8([[dominant_color]]), cv2.COLOR_HSV2BGR)[0][0]
        
        # Classify the color
        color_name = self.classify_color(dominant_color)
        
        return {
            'hsv': dominant_color,
            'bgr': dominant_color_bgr,
            'name': color_name,
            'mask': non_black_mask.astype(np.uint8) * 255
        }
    
    def classify_color(self, hsv_color):
        """Classify the color based on HSV values"""
        hue = hsv_color[0]
        saturation = hsv_color[1]
        value = hsv_color[2]
        
        # Color classification logic
        if value < 50:
            return "black"
        elif saturation < 50:
            return "white" if value > 200 else "gray"
        elif hue < 5 or hue > 175:
            return "red"
        elif 5 <= hue < 22:
            return "orange"
        elif 22 <= hue < 33:
            return "yellow"
        elif 33 <= hue < 78:
            return "green"
        elif 78 <= hue < 105:
            return "teal"
        elif 105 <= hue < 130:
            return "blue"
        elif 130 <= hue < 150:
            return "purple"
        elif 150 <= hue < 175:
            return "pink"
        else:
            return "unknown"
    
    def draw_detection_result(self, frame, result):
        """Draw detection results on the frame"""
        if result is None:
            cv2.putText(frame, "No object detected", (10, 30),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            return frame
        
        # Create output frame
        output = frame.copy()
        
        # Draw mask
        mask_rgb = cv2.cvtColor(result['mask'], cv2.COLOR_GRAY2BGR)
        output = cv2.addWeighted(output, 0.7, mask_rgb, 0.3, 0)
        
        # Draw color information
        color_bgr = result['bgr']
        color_text = f"Color: {result['name']} (H:{result['hsv'][0]:.0f}, S:{result['hsv'][1]:.0f}, V:{result['hsv'][2]:.0f})"
        
        cv2.putText(output, color_text, (10, 30),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        
        # Draw color swatch
        swatch_size = 50
        swatch = np.zeros((swatch_size, swatch_size, 3), dtype=np.uint8)
        swatch[:,:] = color_bgr
        output[10:10+swatch_size, 10:10+swatch_size] = swatch
        
        return output

# Main function to test the detector
def main():
    # Initialize detector
    detector = AutomaticColorDetector()
    
    # Initialize camera
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        # Flip frame horizontally (more intuitive)
        frame = cv2.flip(frame, 1)
        
        # Detect dominant color
        result = detector.detect_dominant_color(frame)
        
        # Draw results
        output = detector.draw_detection_result(frame, result)
        
        # Show output
        cv2.imshow('Automatic Color Detection', output)
        
        # Exit on 'q' key
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()