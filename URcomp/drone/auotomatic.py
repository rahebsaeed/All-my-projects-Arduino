import cv2
import numpy as np
from ultralytics import YOLO
import time

class TelloSquareTracker:
    def __init__(self):
        # Initialize YOLOv8 model with your custom model
        self.model = YOLO('detectModel.pt')  # Your custom trained model
        
        # For testing with PC webcam or IP camera
        self.cap = cv2.VideoCapture(0)  # Use 0 for webcam or 'udp://@0.0.0.0:11111' for Tello
        
        # Target parameters
        self.target_size = 50000  # Target area in pixels
        self.frame_center = None
        
        # Class names from your model (update these based on your model)
        self.class_names = {0: "squareBlue", 1: "squareYellow"}
        self.target_class = "squareYellow"  # The square we want to track
        
        # Movement tracking
        self.centered_frames = 0  # Count consecutive frames where square is centered
        self.required_centered_frames = 10  # Wait for stabilization before moving through
        
        # Bluetooth simulation (for testing)
        self.bt_commands = []
        
    def process_frame(self, frame):
        """Process frame with YOLOv8 and detect all squares with confidence"""
        # Resize frame if needed
        frame = cv2.resize(frame, (640, 480))
        
        # Get frame center (only once)
        if self.frame_center is None:
            h, w = frame.shape[:2]
            self.frame_center = (w // 2, h // 2)
        
        # Run YOLOv8 inference
        results = self.model(frame)
        
        # Find all squares (both classes) with confidence scores
        all_squares = []
        target_squares = []
        
        for result in results:
            for box in result.boxes:
                cls_id = int(box.cls)
                confidence = float(box.conf)  # Get detection confidence
                class_name = self.class_names.get(cls_id, "unknown")
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                all_squares.append((x1, y1, x2, y2, cls_id, confidence))
                
                # Separate target squares (yellow)
                if class_name == self.target_class:
                    target_squares.append((x1, y1, x2, y2, cls_id, confidence))
        
        return frame, all_squares, target_squares
    
    def calculate_movement(self, frame, target_squares):
        """Calculate movement commands based on detected yellow squares only"""
        if not target_squares:
            self.centered_frames = 0
            return "SEARCH"  # No target square found
        
        # Get largest yellow square (with highest confidence if multiple)
        largest_square = max(target_squares, key=lambda s: (s[2]-s[0])*(s[3]-s[1]))
        x1, y1, x2, y2, cls_id, confidence = largest_square
        
        # Calculate square center and area
        cx = (x1 + x2) // 2
        cy = (y1 + y2) // 2
        area = (x2 - x1) * (y2 - y1)
        
        # Calculate offsets from center
        offset_x = cx - self.frame_center[0]
        offset_y = cy - self.frame_center[1]
        
        # Determine if square is centered
        is_centered = abs(offset_x) <= 20 and abs(offset_y) <= 20
        is_proper_size = 0.8 * self.target_size <= area <= 1.2 * self.target_size
        
        # Determine movement commands
        commands = []
        
        # Center horizontally
        if abs(offset_x) > 20:  # Deadzone of 20 pixels
            commands.append("LEFT" if offset_x < 0 else "RIGHT")
        
        # Center vertically
        if abs(offset_y) > 20:
            commands.append("UP" if offset_y < 0 else "DOWN")
        
        # Adjust distance (based on area)
        if area < self.target_size * 0.8:
            commands.append("FORWARD")
        elif area > self.target_size * 1.2:
            commands.append("BACK")
        
        # Update centered frames counter
        if is_centered and is_proper_size:
            self.centered_frames += 1
        else:
            self.centered_frames = 0
        
        # If centered and proper size for enough frames, move through
        if self.centered_frames >= self.required_centered_frames:
            commands = ["THROUGH"]
            self.centered_frames = 0  # Reset after passing through
        
        return commands if commands else "HOVER"
    
    def simulate_bluetooth(self, command):
        """Simulate Bluetooth command sending (print for testing)"""
        if isinstance(command, list):
            for cmd in command:
                print(f"[BLUETOOTH] Sending command: {cmd}")
                self.bt_commands.append(cmd)
        else:
            print(f"[BLUETOOTH] Sending command: {command}")
            self.bt_commands.append(command)
    
    def visualize_frame(self, frame, all_squares, target_squares):
        """Draw all detected squares with confidence percentages"""
        # Draw all detected squares
        for x1, y1, x2, y2, cls_id, confidence in all_squares:
            class_name = self.class_names.get(cls_id, "unknown")
            confidence_pct = confidence * 100
            
            # Use different colors for different classes
            if class_name == "squareYellow":
                color = (0, 255, 255)  # Yellow for target
                thickness = 3  # Thicker border for target
            else:
                color = (255, 0, 0)  # Blue for non-target
                thickness = 1
            
            # Draw rectangle and label with confidence
            cv2.rectangle(frame, (x1, y1), (x2, y2), color, thickness)
            label = f"{class_name} {confidence_pct:.1f}%"
            cv2.putText(frame, label, (x1, y1-10), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
        
        # Draw center crosshair
        cv2.drawMarker(frame, self.frame_center, (0, 0, 255), 
                      cv2.MARKER_CROSS, 20, 2)
        
        # Display status information
        status_text = f"Tracking: {self.target_class}"
        cv2.putText(frame, status_text, (10, 30), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
        
        # Display centered frames count if target detected
        if target_squares:
            cv2.putText(frame, f"Centered: {self.centered_frames}/{self.required_centered_frames}", 
                       (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
        
        return frame
    
    def run(self):
        try:
            while True:
                # Get frame from camera
                ret, frame = self.cap.read()
                if not ret:
                    print("Error reading frame")
                    break
                
                # Process frame and detect all squares with confidence
                processed_frame, all_squares, target_squares = self.process_frame(frame)
                
                # Calculate and send movement commands (using only target squares)
                movement = self.calculate_movement(processed_frame, target_squares)
                self.simulate_bluetooth(movement)
                
                # Visualize all detections with confidence
                visualized_frame = self.visualize_frame(processed_frame, all_squares, target_squares)
                
                # Display frame
                cv2.imshow(f"Tello Tracking - {self.target_class} Only", visualized_frame)
                
                # Exit on 'q' key
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
                
                time.sleep(0.05)
                
        except KeyboardInterrupt:
            pass
        finally:
            self.cap.release()
            cv2.destroyAllWindows()
            print("Bluetooth commands sent during test:", self.bt_commands)

if __name__ == "__main__":
    tracker = TelloSquareTracker()
    tracker.run()