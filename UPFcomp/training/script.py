#!/usr/bin/env python3

import cv2
from ultralytics import YOLO
import numpy as np
import serial
import time
import glob
import os

# ===== SERIAL PORT AUTO-DETECTION =====
def find_serial_port():
    """Automatically detect connected serial devices"""
    possible_ports = glob.glob('/dev/tty[A-Za-z]*')
    for port in possible_ports:
        try:
            s = serial.Serial(port)
            s.close()
            return port
        except (serial.SerialException, OSError):
            continue
    return None

# Initialize serial connection
try:
    #serial_port = find_serial_port() #original

    # hard code the serial port
    serial_port = "/dev/ttyACM0"  #  or whatever your port is

    if serial_port:
        ser = serial.Serial(serial_port, 115200, timeout=1)
        time.sleep(2)  # Wait for connection
        serial_connected = True
        print(f"Connected to serial port: {serial_port}")
    else:
        raise Exception("No serial port found")
except Exception as e:
    print(f"Serial not connected - {e} - running in visualization mode: {e}")  #print the error
    ser = None
    serial_connected = False

# ===== MODEL LOADING =====
MODELS_DIR = os.path.expanduser("~/Documents")
traffic_model_path = os.path.join(MODELS_DIR, "traficDetectionv8.pt")
cube_model_path = os.path.join(MODELS_DIR, "cubeDetection8.pt")

if not os.path.exists(traffic_model_path):
    print(f"Error: Traffic model not found at {traffic_model_path}")
    exit(1)
if not os.path.exists(cube_model_path):
    print(f"Error: Cube model not found at {cube_model_path}")
    exit(1)
try:
    traffic_model = YOLO(traffic_model_path)
    cube_model = YOLO(cube_model_path)
    print("Models loaded successfully")
except Exception as e:
    print(f"Error loading models: {e}")
    exit(1)

# ===== OPTIMIZED CAMERA SETUP =====
def init_camera():
    """Initialize camera with multiple fallback options"""
    # Try different camera backends and indexes
    for backend in [cv2.CAP_V4L2, cv2.CAP_ANY]: # add gstreamer
        for index in [0, 1, 2]:  # Try different camera indexes
            cap = cv2.VideoCapture(index, backend)
            if cap.isOpened():
                print(f"Camera opened at index {index} with backend {backend}")
                time.sleep(1) # Add a short sleep
                # Set optimized parameters
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
                cap.set(cv2.CAP_PROP_FPS, 15)
                cap.set(cv2.CAP_PROP_BUFFERSIZE, 2)
                cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
                return cap

    # If all else fails, try Picamera2
    try:
        from picamera2 import Picamera2
        from libcamera import Transform
        print("Trying Picamera2...")
        picam2 = Picamera2()
        config = picam2.create_preview_configuration(
            main={"size": (640, 480)},
            transform=Transform(hflip=1, vflip=1)
        )
        picam2.configure(config)
        picam2.start()
        return picam2
    except ImportError:
        print("Picamera2 not available")
    except Exception as e:
        print(f"Picamera2 failed: {e}")

    raise RuntimeError("Could not initialize any camera")

# Modify your main loop to handle Picamera2 if used
try:
    camera = init_camera()
    print("Camera initialized successfully")

    frame_fail_count = 0 # Counter for consecutive failed frames

    while True:
        # Handle both OpenCV and Picamera2 cases
        try:
            if isinstance(camera, cv2.VideoCapture):
                ret, frame = camera.read()
            else:  # Picamera2
                frame = camera.capture_array()
                frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        except Exception as e:
            print(f"Error reading frame: {e}")
            frame = None # treat as a failed frame.

        if frame is None:
            print("Failed to capture frame")
            frame_fail_count += 1
            time.sleep(0.1)

            if frame_fail_count > 10: # break if too many failures.
                print("Too many consecutive frame failures.  Exiting")
                break

            continue # try again

        frame_fail_count = 0 # Reset the counter if frame is good

        # Rest of your processing code...
        # ===== CONTROL PARAMETERS =====
        FRAME_CENTER = (320, 240)
        DISTANCE_THRESHOLD = 30  # cm
        MIN_CUBE_AREA = 1000
        FRAME_SKIP = 1  # Process every other frame

        # ===== DISPLAY SETTINGS =====
        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 0.6
        font_thickness = 2

        # ===== ROBOT CONTROLLER CLASS =====
        class RobotController:
            def __init__(self):
                self.last_command = None
                self.command_history = []
                self.last_distance = None

            def send_command(self, cmd, distance=None):
                if cmd != self.last_command:
                    self.command_history.append(cmd)
                    if len(self.command_history) > 5:
                        self.command_history.pop(0)

                    if serial_connected and ser is not None:
                        try:
                            ser.write(cmd.encode())
                            print(f"Sent: {cmd}" + (f" (Distance: {distance}cm)" if distance else ""))
                        except Exception as e:
                            print(f"Serial write failed: {e}")
                    self.last_command = cmd
                    self.last_distance = distance

        controller = RobotController()

        # ===== DISTANCE CALCULATION =====
        def get_distance(box_width):
            """Convert bounding box size to approximate distance in cm"""
            REFERENCE_WIDTH = 10  # cm (actual width of your cube)
            focal_length = 600    # Camera focal length in pixels
            return round((REFERENCE_WIDTH * focal_length) / box_width, 1)

        # ===== MAIN PROCESSING FUNCTION =====
        def process_frame(frame):
            # Initialize variables
            display_text = []
            active_light = None
            closest_cube = None
            max_area = 0

            # Downscale frame for faster processing
            processing_frame = cv2.resize(frame, (320, 240))

            # ===== TRAFFIC LIGHT DETECTION =====
            traffic_results = traffic_model(processing_frame, verbose=False, imgsz=320)
            for box in traffic_results[0].boxes:
                if box.conf > 0.5:
                    light_color = traffic_model.names[int(box.cls.item())].lower()
                    if light_color in ['red', 'yellow', 'green']:
                        active_light = light_color
                        display_text.append(f"Traffic: {light_color}")

            # ===== CUBE DETECTION =====
            cube_results = cube_model(processing_frame, verbose=False, imgsz=320)
            for box in cube_results[0].boxes:
                if box.conf > 0.5:
                    x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
                    area = (x2 - x1) * (y2 - y1)

                    if area > max_area and area > MIN_CUBE_AREA:
                        max_area = area
                        closest_cube = ((x1 + x2) // 2, (y1 + y2) // 2, x2 - x1, y2 - y1)

            # ===== DECISION MAKING =====
            current_distance = None

            if active_light == 'red':
                controller.send_command('S')
                display_text.append("Command: STOP (Red Light)")
            elif closest_cube:
                cx, cy, w, h = closest_cube
                current_distance = get_distance(w)
                display_text.append(f"Distance: {current_distance}cm")

                if current_distance <= DISTANCE_THRESHOLD:
                    controller.send_command('S', current_distance)
                    display_text.append("Command: STOP (Reached)")
                else:
                    offset = cx - FRAME_CENTER[0]//2  # Scaled for 320 width
                    if abs(offset) > 30:
                        cmd = 'L' if offset < 0 else 'R'
                        controller.send_command(cmd, current_distance)
                        display_text.append(f"Command: {cmd}")
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

            # ===== ANNOTATE FRAME =====
            # Scale back up for display
            frame = cv2.resize(frame, (640, 480))

            # Draw center guidelines
            cv2.line(frame, (FRAME_CENTER[0], 0), (FRAME_CENTER[0], 480), (0, 255, 0), 1)
            cv2.line(frame, (0, FRAME_CENTER[1]), (640, FRAME_CENTER[1]), (0, 255, 0), 1)

            # Display information
            y_offset = 30
            for text in display_text:
                cv2.putText(frame, text, (10, y_offset), font, font_scale, (0, 255, 0), font_thickness)
                y_offset += 25

            # Show command history
            cv2.putText(frame, "Last commands:", (10, y_offset+10), font, 0.5, (255, 255, 255), 1)
            for i, cmd in enumerate(controller.command_history[-3:]):
                cv2.putText(frame, cmd, (150 + i*30, y_offset+10), font, 0.7, (0, 255, 255), 2)

            return frame

        # ===== MAIN LOOP =====
        try:
            frame_count = 0
            start_time = time.perf_counter()

            while True:
                ret, frame = cap.read()
                if not ret:
                    print("Failed to capture frame")
                    time.sleep(0.1)
                    continue

                # Process every FRAME_SKIP+1 frame
                if frame_count % (FRAME_SKIP + 1) == 0:
                    processed_frame = process_frame(frame)

                    # Calculate and display FPS
                    frame_count += 1
                    if frame_count % 10 == 0:
                        end_time = time.perf_counter()
                        elapsed_time = end_time - start_time
                        fps = frame_count / elapsed_time
                        cv2.putText(processed_frame, f"FPS: {fps:.1f}",
                                   (500, 30), font, 0.6, (0, 255, 0), 2)

                    cv2.imshow("Robot Controller - Q to quit", processed_frame)

                # Exit on 'q' key
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

        finally:
            # Cleanup resources
            if serial_connected and ser is not None:
                ser.close()
            cap.release()
            cv2.destroyAllWindows()
            print("Resources released and program exited cleanly")

finally:
    if isinstance(camera, cv2.VideoCapture):
        camera.release()
    elif 'picam2' in locals():
        picam2.stop()