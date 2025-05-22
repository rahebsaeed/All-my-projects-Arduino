from ultralytics import YOLO

# Load a model
model = YOLO("yolov8n.pt")  # make sure this file exists in your working directory

# Train the model
results = model.train(
    data="D:/Arduino/UPFcomp/training/roboflow/data.yaml",  # absolute path
    epochs=100,
    imgsz=640,
    batch=16,
    name="roboflow_yolov8"
)