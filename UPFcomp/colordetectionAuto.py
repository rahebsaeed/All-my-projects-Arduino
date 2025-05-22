import cv2
import numpy as np

def auto_detect_color_range(image, mask=None):
    """Automatically detect color range from an image region"""
    if mask is None:
        mask = np.ones(image.shape[:2], dtype=np.uint8) * 255
    
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    
    # Only consider pixels where mask is white
    pixels = hsv[mask == 255]
    
    # Calculate mean and standard deviation
    mean, std = cv2.meanStdDev(pixels)
    
    # Define range as mean ± 2 standard deviations
    lower = np.maximum(0, mean - 2 * std).flatten().astype(np.uint8)
    upper = np.minimum([179, 255, 255], mean + 2 * std).flatten().astype(np.uint8)
    
    return lower, upper

# Example usage:
image = cv2.imread('color_sample.jpg')
mask = np.zeros(image.shape[:2], dtype=np.uint8)
cv2.rectangle(mask, (50, 50), (150, 150), 255, -1)  # Define region of interest

lower, upper = auto_detect_color_range(image, mask)
print("Auto-detected range:")
print("Lower:", lower)
print("Upper:", upper)