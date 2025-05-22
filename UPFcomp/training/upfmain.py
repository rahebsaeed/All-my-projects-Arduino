import cv2
import numpy as np
import serial
import time
from ultralytics import YOLO

# Configuration
ESP_CAM_URL = "http://192.168.137.231/stream"
ARDUINO_PORT = "COM12"
BAUD_RATE = 9600
FRAME_WIDTH = 640
FRAME_HEIGHT = 480
TRAFFIC_ROI = (0, 0, FRAME_WIDTH, int(FRAME_HEIGHT/3))
MIN_TRAFFIC_CONFIDENCE = 0.6
TRAFFIC_CLASSES = ['red', 'yellow', 'green']

# Model Initialization
traffic_model = YOLO("traficDetectionv8.pt").eval()
cube_model = YOLO("cubeDetection8.pt").eval()

# Serial Communication
arduino = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=1)
time.sleep(2)

def detect_traffic_lights(frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hsv[:,:,1] = cv2.convertScaleAbs(hsv[:,:,1], alpha=1.5, beta=0)
    roi = frame[TRAFFIC_ROI[1]:TRAFFIC_ROI[3], TRAFFIC_ROI[0]:TRAFFIC_ROI[2]]
    results = traffic_model(roi, verbose=False, imgsz=160)[0]

    # Enhance color saturation (helps with sunlight conditions)
    hsv[:,:,1] = cv2.convertScaleAbs(hsv[:,:,1], alpha=1.5, beta=0)
    
    # Focus on upper portion of the frame
    roi = frame[TRAFFIC_ROI[1]:TRAFFIC_ROI[3], TRAFFIC_ROI[0]:TRAFFIC_ROI[2]]
    
    # Detect traffic lights
    results = traffic_model(roi, verbose=False, imgsz=160)[0]  # Reduced imgsz
    
    detected_lights = []
    for box in results.boxes:
        if box.conf > MIN_TRAFFIC_CONFIDENCE:
            x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
            class_id = int(box.cls.item())
            label = traffic_model.names[class_id].lower()
            
            if label in TRAFFIC_CLASSES:
                # Convert coordinates back to full frame
                x1 += TRAFFIC_ROI[0]
                y1 += TRAFFIC_ROI[1]
                x2 += TRAFFIC_ROI[0]
                y2 += TRAFFIC_ROI[1]
                
                detected_lights.append((x1, y1, x2, y2, label, float(box.conf)))
    
    return detected_lights

# ===== Cube Detection =====
def detect_cubes(frame):
    results = cube_model(frame, verbose=False, imgsz=160)[0]  # Reduced imgsz
    detected_cubes = []
    
    for box in results.boxes:
        if box.conf > 0.5:  # Adjust confidence threshold as needed
            x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
            detected_cubes.append((x1, y1, x2, y2, float(box.conf)))
    
    return detected_cubes

# ===== Visualization Helpers =====
def draw_traffic_debug(frame, lights):
    for x1, y1, x2, y2, label, conf in lights:
        color = {'red': (0,0,255), 'yellow': (0,255,255), 'green': (0,255,0)}.get(label, (255,255,255))
        cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
        cv2.putText(frame, f"{label} {conf:.2f}", (x1, y1-10),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)
    return frame

def draw_cubes_debug(frame, cubes):
    for x1, y1, x2, y2, conf in cubes:
        cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 2)
        cv2.putText(frame, f"cube {conf:.2f}", (x1, y1-10),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 0), 2)
    return frame

# ===== Command Functions =====
def send_command(cmd):
    arduino.write(cmd.encode())
    time.sleep(0.1)  # Small delay between commands

# ===== Main Processing Loop =====
def main():
    # Open video stream from ESP-CAM
    cap = cv2.VideoCapture(ESP_CAM_URL)
    
    if not cap.isOpened():
        print("Error: Could not open video stream")
        return
    
    try:
        while True:
            # Read frame from stream
            ret, frame = cap.read()
            if not ret:
                print("Error: Could not read frame")
                break
            
            # Resize frame for processing
            frame = cv2.resize(frame, (FRAME_WIDTH, FRAME_HEIGHT))
            
            # Detect traffic lights
            traffic_lights = detect_traffic_lights(frame)
            current_light = None
            max_conf = 0
            
            # Process traffic light detections
            for light in traffic_lights:
                x1, y1, x2, y2, label, conf = light
                if conf > max_conf:
                    current_light = label
                    max_conf = conf
            
            # Visualize detections
            debug_frame = frame.copy()
            debug_frame = draw_traffic_debug(debug_frame, traffic_lights)
            
            # Only process cubes if no red light
            cubes = []
            if current_light != 'red':
                cubes = detect_cubes(frame)
                debug_frame = draw_cubes_debug(debug_frame, cubes)
                
                # Simple navigation logic based on cube detection
                if cubes:
                    # Get the largest cube (highest confidence)
                    largest_cube = max(cubes, key=lambda x: x[4])
                    x_center = (largest_cube[0] + largest_cube[2]) // 2
                    
                    # Simple proportional control
                    frame_center = FRAME_WIDTH // 2
                    error = x_center - frame_center
                    
                    if abs(error) > 30:  # Threshold to start turning
                        if error > 0:
                            send_command('R')  # Turn right
                        else:
                            send_command('L')  # Turn left
                    else:
                        send_command('F')  # Move forward
                else:
                    send_command('S')  # Stop if no cubes detected
            else:
                send_command('S')  # Stop for red light
            
            # Show debug view
            cv2.imshow("ESP-CAM Processing", debug_frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
    
    finally:
        cap.release()
        cv2.destroyAllWindows()
        arduino.close()

if __name__ == "__main__":
    main()