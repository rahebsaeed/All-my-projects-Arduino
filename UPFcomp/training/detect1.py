from ultralytics import YOLO
import cv2

# Load your custom YOLOv8 model
model = YOLO('trafficSigns.pt')  # Replace with your model path

# Open video capture (0 for webcam, or video file path)
cap = cv2.VideoCapture(0)  # Change to your video source if needed

while cap.isOpened():
    # Read a frame from the video
    success, frame = cap.read()
    
    if success:
        # Run YOLOv8 inference on the frame
        results = model(frame)
        
        # Visualize the results on the frame
        annotated_frame = results[0].plot()
        
        # Display the annotated frame
        cv2.imshow("YOLOv8 Inference", annotated_frame)
        
        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
    else:
        # Break the loop if the end of the video is reached
        break

# Release the video capture object and close the display window
cap.release()
cv2.destroyAllWindows()