import cv2
import numpy as np
from picamera2 import Picamera2
from ultralytics import YOLO
import serial
import time
import statistics

# === Setup ===

# Initialize the Pi Camera 3
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (1280, 720)}))  # HD resolution
picam2.start()

# Arduino Serial
ser = serial.Serial('/dev/ttyACM0', 9600)  # Adjust to your Arduino port
time.sleep(2)  # Allow time for initialization

# Load YOLOv8n model
model = YOLO('yolov8n.pt')

# Body detection counter variable
number_of_bodies = 1
count_exactly_x_bodies = 0
loop_times = []
num_loops = 1000


# === Detection Function ===

def get_closest_body_coordinates():
    frame = picam2.capture_array()
    img = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    results = model(img)[0]

    people = []
    for box in results.boxes:
        cls = int(box.cls[0])
        if cls == 0:  # 'person' class in COCO
            x1, y1, x2, y2 = box.xyxy[0].cpu().numpy()
            area = (x2 - x1) * (y2 - y1)
            people.append({'box': (x1, y1, x2, y2), 'area': area})

    num_people = len(people)

    global count_exactly_x_bodies
    if num_people == number_of_bodies:
        count_exactly_x_bodies += 1

    largest_body_coords = None
    if people:
        largest = max(people, key=lambda p: p['area'])
        x1, y1, x2, y2 = largest['box']
        center_x = int((x1 + x2) / 2)
        top_y = int(y1)  # top of the head
        largest_body_coords = (center_x, top_y)
        ser.write(f"{center_x},{top_y}\n".encode())

    return largest_body_coords, num_people


# === Main Loop ===

overall_start = time.perf_counter()

for i in range(num_loops):
    print(f"\n--- Loop {i+1}/{num_loops} ---")

    start_time = time.perf_counter()
    coordinates, num_people = get_closest_body_coordinates()
    end_time = time.perf_counter()

    loop_duration = end_time - start_time
    loop_times.append(loop_duration)

    # Uncomment for live output
    # if coordinates:
    #     print(f"Largest Body Coordinates: {coordinates}")
    # else:
    #     print("No body detected.")
    # print(f"Bodies Detected: {num_people}")
    # print(f"Loop Time: {loop_duration:.3f} seconds")

overall_end = time.perf_counter()
total_time = overall_end - overall_start

# === Stats Summary ===

mean_time = statistics.mean(loop_times)
variance_time = statistics.variance(loop_times)
max_time = max(loop_times)
min_time = min(loop_times)

print("\n=== Performance Summary ===")
print(f"Total Time for {num_loops} Loops: {total_time:.2f} seconds")
print(f"Mean Loop Time: {mean_time:.4f} seconds")
print(f"Variance: {variance_time:.6f}")
print(f"Maximum Loop Time: {max_time:.4f} seconds")
print(f"Minimum Loop Time: {min_time:.4f} seconds")
print(f"Average FPS: {1 / mean_time:.2f}")
print(f"Number of times exactly {number_of_bodies} body(s) were detected: {count_exactly_x_bodies}")
