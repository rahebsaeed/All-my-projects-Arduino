import os
import cv2
import numpy as np
from math import cos, sin, radians

def rotate_image(image, angle):
    """Rotate image by specified angle while keeping the full image visible"""
    height, width = image.shape[:2]
    image_center = (width/2, height/2)
    
    # Get rotation matrix
    rotation_mat = cv2.getRotationMatrix2D(image_center, angle, 1.0)
    
    # Calculate new bounding dimensions
    abs_cos = abs(rotation_mat[0,0])
    abs_sin = abs(rotation_mat[0,1])
    
    bound_w = int(height * abs_sin + width * abs_cos)
    bound_h = int(height * abs_cos + width * abs_sin)
    
    # Adjust rotation matrix to center
    rotation_mat[0, 2] += bound_w/2 - image_center[0]
    rotation_mat[1, 2] += bound_h/2 - image_center[1]
    
    # Perform rotation
    rotated_image = cv2.warpAffine(image, rotation_mat, (bound_w, bound_h))
    return rotated_image

def process_images(input_folder, output_folder):
    """Process all images in input folder and save rotated versions to output folder"""
    # Create output folder if it doesn't exist
    os.makedirs(output_folder, exist_ok=True)
    
    # Process each image in input folder
    for filename in os.listdir(input_folder):
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.tiff')):
            image_path = os.path.join(input_folder, filename)
            image = cv2.imread(image_path)
            
            if image is not None:
                # Save original image first
                base_name = os.path.splitext(filename)[0]
                original_output = os.path.join(output_folder, f"{base_name}_0.jpg")
                cv2.imwrite(original_output, image)
                
                # Generate rotated versions
                for angle in range(15, 181, 30):  # 15, 30, ..., 180
                    rotated = rotate_image(image, angle)
                    rotated_output = os.path.join(output_folder, f"{base_name}_{angle}.jpg")
                    cv2.imwrite(rotated_output, rotated)
                
                print(f"Processed {filename}")

if __name__ == "__main__":
    input_folder = "img1"  # Folder containing your original images
    output_folder = "augmented_images1"  # Folder to save augmented images
    
    process_images(input_folder, output_folder)
    print(f"All images processed. Augmented images saved to {output_folder}")