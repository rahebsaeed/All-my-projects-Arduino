import cv2
from ultralytics import YOLO
import numpy as np
import serial
import time

# ===== SERIAL SETUP (Optional) =====
try:
    ser = serial.Serial('COM6', 115200, timeout=1)
    time.sleep(2)
    serial_connected = True
except:
    print("Serial not connected - running in visualization mode")
    ser = None
    serial_connected = False

# ===== MODEL LOADING =====
traffic_model = YOLO("traficDetectionv8.pt")
cube_model = YOLO("cubeDetection8.pt")

# ===== CAMERA SETUP =====
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

# ===== CONTROL PARAMETERS =====
FRAME_CENTER = (320, 240)
DISTANCE_THRESHOLD = 30  # cm
MIN_CUBE_AREA = 1000

# ===== DISPLAY SETTINGS =====
font = cv2.FONT_HERSHEY_SIMPLEX
font_scale = 0.6
font_thickness = 2

# ===== COMMAND SYSTEM =====
class RobotController:
    def __init__(self):
        self.last_command = None
        self.current_command = None
        self.last_distance = None
        self.command_history = []
        
    def send_command(self, cmd, distance=None):
        self.last_command = self.current_command
        self.current_command = cmd
        self.last_distance = distance
        
        if cmd != self.last_command:
            self.command_history.append(cmd)
            if len(self.command_history) > 5:
                self.command_history.pop(0)
            
            if serial_connected and ser is not None:
                try:
                    ser.write(cmd.encode())
                    print(f"Sent: {cmd} (Distance: {distance}cm)")
                except:
                    print("Serial write failed")

controller = RobotController()

# ===== DISTANCE CALIBRATION =====
def get_distance(box_width, box_height):
    """Convert bounding box size to approximate distance in cm"""
    # Calibrate these values based on your actual cube size
    REFERENCE_WIDTH = 10  # cm (actual width of your cube)
    focal_length = 600    # Camera focal length in pixels
    
    # Calculate distance using simple perspective
    distance = (REFERENCE_WIDTH * focal_length) / box_width
    return round(distance, 1)

# ===== MAIN PROCESSING FUNCTION =====
def process_frame():
    ret, frame = cap.read()
    if not ret:
        return None

    # Initialize display text
    display_text = []
    active_light = None
    traffic_mask = np.zeros(frame.shape[:2], dtype=np.uint8)
    
    # ===== TRAFFIC LIGHT DETECTION =====
    traffic_results = traffic_model(frame, verbose=False)
    for box in traffic_results[0].boxes:
        if box.conf > 0.5:
            x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
            traffic_mask[y1:y2, x1:x2] = 255
            light_color = traffic_model.names[int(box.cls.item())].lower()
            
            if light_color in ['red', 'yellow', 'green']:
                active_light = light_color
                display_text.append(f"Traffic: {light_color}")
    
    traffic_frame = traffic_results[0].plot()

    # ===== CUBE DETECTION =====
    cube_results = cube_model(frame, verbose=False)
    cube_frame = frame.copy()
    closest_cube = None
    max_area = 0
    
    for box in cube_results[0].boxes:
        if box.conf > 0.5:
            x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
            center = ((x1 + x2) // 2, (y1 + y2) // 2)
            area = (x2 - x1) * (y2 - y1)
            
            if traffic_mask[center[1], center[0]] == 0 and area > max_area:
                max_area = area
                closest_cube = (center, (x2 - x1, y2 - y1))
                
                # Draw cube with color based on type
                color_map = {
                    'red': (0, 0, 255),
                    'green': (0, 255, 0),
                    'blue': (255, 0, 0),
                    'yellow': (0, 255, 255)
                }
                class_name = cube_model.names[int(box.cls.item())]
                color = color_map.get(class_name.lower().replace('cube', '').strip(), (255, 255, 0))
                
                cv2.rectangle(cube_frame, (x1, y1), (x2, y2), color, 2)
                label = f"{class_name} {float(box.conf.item()):.2f}"
                cv2.putText(cube_frame, label, (x1, y1 - 10), font, 0.5, color, 1)
    
    # ===== DECISION MAKING =====
    current_distance = None
    
    if active_light == 'red':
        controller.send_command('S')
        display_text.append("Command: STOP (Red Light)")
    elif closest_cube:
        (cx, cy), (w, h) = closest_cube
        current_distance = get_distance(w, h)
        display_text.append(f"Distance: {current_distance}cm")
        
        if current_distance <= DISTANCE_THRESHOLD:
            controller.send_command('S', current_distance)
            display_text.append("Command: STOP (Reached)")
        else:
            offset = cx - FRAME_CENTER[0]
            if abs(offset) > 50:
                if offset < 0:
                    controller.send_command('L', current_distance)
                    display_text.append("Command: LEFT")
                else:
                    controller.send_command('R', current_distance)
                    display_text.append("Command: RIGHT")
            else:
                controller.send_command('F', current_distance)
                display_text.append("Command: FORWARD")
    elif active_light == 'yellow':
        controller.send_command('D')
        display_text.append("Command: SLOW (Yellow Light)")
    elif active_light == 'green':
        controller.send_command('F')
        display_text.append("Command: FORWARD (Green Light)")
    else:
        controller.send_command('S')
        display_text.append("Command: STOP (No Target)")

    # ===== COMBINE AND ANNOTATE FRAMES =====
    combined = cv2.addWeighted(traffic_frame, 0.5, cube_frame, 0.5, 0)
    
    # Draw center guidelines
    cv2.line(combined, (FRAME_CENTER[0], 0), (FRAME_CENTER[0], 480), (0, 255, 0), 1)
    cv2.line(combined, (0, FRAME_CENTER[1]), (640, FRAME_CENTER[1]), (0, 255, 0), 1)
    
    # Display information
    y_offset = 30
    for text in display_text:
        cv2.putText(combined, text, (10, y_offset), font, font_scale, (0, 255, 0), font_thickness)
        y_offset += 25
    
    # Show command history
    cv2.putText(combined, "Last commands:", (10, y_offset+10), font, 0.5, (255, 255, 255), 1)
    for i, cmd in enumerate(controller.command_history[-3:]):
        cv2.putText(combined, cmd, (150 + i*30, y_offset+10), font, 0.7, (0, 255, 255), 2)
    
    return combined

# ===== MAIN LOOP =====
try:
    while True:
        frame = process_frame()
        if frame is None:
            break
        
        cv2.imshow("Robot Controller - Q to quit", frame)
        if cv2.waitKey(1) == ord('q'):
            break
finally:
    if serial_connected and ser is not None:
        ser.close()
    cap.release()
    cv2.destroyAllWindows()