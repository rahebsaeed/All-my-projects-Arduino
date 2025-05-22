import cv2
import numpy as np

import cv2
import numpy as np

class TrafficLightDetector:
    def __init__(self):
        # Updated HSV color ranges with your detected values
        self.color_ranges = {
            'red': ([160, 193, 181], [179, 223, 211]),        # First red range from detection
            'red2': ([160, 196, 182], [179, 226, 212]),  # Red wraps around 180
            'yellow': ([20, 100, 100], [30, 255, 255]),    # Keep existing or update similarly
            'green': ([40, 70, 70], [80, 255, 255]),       # Keep existing or update similarly
        }
        
        # Traffic light structure parameters
        self.min_light_radius = 5
        self.max_light_radius = 50
        self.min_traffic_light_aspect = 1.5  # Height/width ratio
        self.max_traffic_light_aspect = 4.0

        
    def detect_traffic_lights(self, frame):
        """Detect traffic lights and their current state"""
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        # Detect possible traffic light candidates by shape
        candidates = self._find_traffic_light_candidates(gray)
        
        detected_lights = []
        
        for candidate in candidates:
            x, y, w, h = candidate
            roi = frame[y:y+h, x:x+w]
            
            # Detect which light is active
            light_state = self._detect_light_state(roi)
            
            if light_state != 'unknown':
                detected_lights.append({
                    'x': x,
                    'y': y,
                    'w': w,
                    'h': h,
                    'state': light_state
                })
                
        return detected_lights
    
    def _find_traffic_light_candidates(self, gray):
        """Find rectangular regions that could be traffic lights"""
        # Edge detection
        edges = cv2.Canny(gray, 50, 150)
        
        # Find contours
        contours, _ = cv2.findContours(edges, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        
        candidates = []
        
        for cnt in contours:
            # Approximate contour to polygon
            epsilon = 0.02 * cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, epsilon, True)
            
            # We're looking for rectangular shapes
            if len(approx) == 4:
                x, y, w, h = cv2.boundingRect(cnt)
                aspect_ratio = float(h)/w
                
                # Check if aspect ratio matches traffic light
                if (self.min_traffic_light_aspect < aspect_ratio < self.max_traffic_light_aspect and
                    w > 20 and h > 40):  # Minimum size
                    candidates.append((x, y, w, h))
                    
        return candidates
    
    def _detect_light_state(self, roi):
        """Determine which light is active in the traffic light"""
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        height, width = roi.shape[:2]
        
        # Divide ROI into three parts (assuming vertical traffic light)
        red_zone = roi[0:int(height/3), 0:width]
        yellow_zone = roi[int(height/3):int(2*height/3), 0:width]
        green_zone = roi[int(2*height/3):height, 0:width]
        
        # Check each zone for color
        red_score = self._get_color_score(red_zone, 'red')
        yellow_score = self._get_color_score(yellow_zone, 'yellow')
        green_score = self._get_color_score(green_zone, 'green')
        
        # Determine which light is on
        max_score = max(red_score, yellow_score, green_score)
        
        if max_score < 0.1:  # Threshold for detection
            return 'unknown'
        
        if max_score == red_score:
            return 'red'
        elif max_score == yellow_score:
            return 'yellow'
        else:
            return 'green'
    
    def _get_color_score(self, zone, color):
        """Calculate how much of the zone matches the target color"""
        hsv = cv2.cvtColor(zone, cv2.COLOR_BGR2HSV)
        
        if color == 'red':
            lower1 = np.array(self.color_ranges['red'][0])
            upper1 = np.array(self.color_ranges['red'][1])
            lower2 = np.array(self.color_ranges['red2'][0])
            upper2 = np.array(self.color_ranges['red2'][1])
            mask1 = cv2.inRange(hsv, lower1, upper1)
            mask2 = cv2.inRange(hsv, lower2, upper2)
            mask = cv2.bitwise_or(mask1, mask2)
        else:
            lower = np.array(self.color_ranges[color][0])
            upper = np.array(self.color_ranges[color][1])
            mask = cv2.inRange(hsv, lower, upper)
        
        # Calculate percentage of zone that matches the color
        match_pixels = cv2.countNonZero(mask)
        total_pixels = zone.shape[0] * zone.shape[1]
        return float(match_pixels) / total_pixels

    def draw_detections(self, frame, lights):
        """Draw detected traffic lights on frame"""
        for light in lights:
            x, y, w, h = light['x'], light['y'], light['w'], light['h']
            state = light['state']
            
            # Draw bounding box
            color = {'red': (0, 0, 255), 
                    'yellow': (0, 255, 255), 
                    'green': (0, 255, 0)}.get(state, (255, 255, 255))
            
            cv2.rectangle(frame, (x, y), (x+w, y+h), color, 2)
            cv2.putText(frame, f"Traffic Light: {state}", (x, y-10), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

# Example usage
if __name__ == "__main__":
    cap = cv2.VideoCapture(0)  # Or use a video file
    detector = TrafficLightDetector()
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
            
        # Detect traffic lights
        traffic_lights = detector.detect_traffic_lights(frame)
        
        # Draw detections
        detector.draw_detections(frame, traffic_lights)
        
        cv2.imshow('Traffic Light Detection', frame)
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
            
    cap.release()
    cv2.destroyAllWindows()