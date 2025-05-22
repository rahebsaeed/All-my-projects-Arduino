import cv2
import numpy as np

def nothing(x):
    pass

# Create a window and trackbars
cv2.namedWindow('Color Range Selector')
cv2.createTrackbar('H Min', 'Color Range Selector', 0, 179, nothing)
cv2.createTrackbar('H Max', 'Color Range Selector', 179, 179, nothing)
cv2.createTrackbar('S Min', 'Color Range Selector', 0, 255, nothing)
cv2.createTrackbar('S Max', 'Color Range Selector', 255, 255, nothing)
cv2.createTrackbar('V Min', 'Color Range Selector', 0, 255, nothing)
cv2.createTrackbar('V Max', 'Color Range Selector', 255, 255, nothing)

cap = cv2.VideoCapture(0)  # Use your camera or video file

while True:
    ret, frame = cap.read()
    if not ret:
        break
    
    # Convert to HSV color space
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Get current trackbar positions
    h_min = cv2.getTrackbarPos('H Min', 'Color Range Selector')
    h_max = cv2.getTrackbarPos('H Max', 'Color Range Selector')
    s_min = cv2.getTrackbarPos('S Min', 'Color Range Selector')
    s_max = cv2.getTrackbarPos('S Max', 'Color Range Selector')
    v_min = cv2.getTrackbarPos('V Min', 'Color Range Selector')
    v_max = cv2.getTrackbarPos('V Max', 'Color Range Selector')
    
    # Create lower and upper bounds
    lower = np.array([h_min, s_min, v_min])
    upper = np.array([h_max, s_max, v_max])
    
    # Create mask and result
    mask = cv2.inRange(hsv, lower, upper)
    result = cv2.bitwise_and(frame, frame, mask=mask)
    
    # Display images
    cv2.imshow('Original', frame)
    cv2.imshow('Mask', mask)
    cv2.imshow('Result', result)
    
    # Print current values (useful for copying)
    print(f"Lower: [{h_min}, {s_min}, {v_min}]")
    print(f"Upper: [{h_max}, {s_max}, {v_max}]")
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()