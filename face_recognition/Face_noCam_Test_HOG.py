import cv2
import numpy as np
from picamera2 import Picamera2
from cvzone import FaceDetectionModule
import serial
import time
import statistics

# Initialize the Pi Camera 3
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (1280, 720)}))  # HD resolution
picam2.start()

ser = serial.Serial('/dev/ttyACM0', 9600)  # Adjust to your Arduino port
time.sleep(2)  # Allow time for initialization

# Face detection counter variable
number_of_faces = 2  # Set how many faces you want to track
count_exactly_x_faces = 0  # Counter for how many times exactly X faces are detected
loop_times = []
num_loops = 1000 # number of loops performed

# Setup HOG + SVM face detector
hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

def get_closest_face_coordinates():
    frame = picam2.capture_array()
    img = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

    # Detect people (we treat them as faces here for the purpose of using HOG+SVM)
    rects, _ = hog.detectMultiScale(img, winStride=(8, 8), padding=(16, 16), scale=1.05)

    global count_exactly_x_faces
    if len(rects) == number_of_faces:
        count_exactly_x_faces += 1

    largest_face_coords = None
    if len(rects) > 0:
        largest_area = 0
        for (x, y, w, h) in rects:
            area = w * h
            if area > largest_area:
                largest_area = area
                center_x = x + w // 2
                center_y = y + h // 2
                largest_face_coords = (center_x, center_y)
        ser.write(f"{center_x},{center_y}\n".encode())

    return largest_face_coords, len(rects)


overall_start = time.perf_counter()

for i in range(num_loops):
    print(f"\n--- Loop {i+1}/{num_loops} ---")

    start_time = time.perf_counter()
    coordinates, num_faces = get_closest_face_coordinates()
    end_time = time.perf_counter()

    loop_duration = end_time - start_time
    loop_times.append(loop_duration)

   # if coordinates:
 #      print(f"Largest Face Coordinates: {coordinates}")
 #   else:
   #     print("No face detected.")

#     print(f"Faces Detected: {num_faces}")
#     print(f"Loop Time: {loop_duration:.3f} seconds")

overall_end = time.perf_counter()
total_time = overall_end - overall_start

# Calculate stats
mean_time = statistics.mean(loop_times)
variance_time = statistics.variance(loop_times)
max_time = max(loop_times)
min_time = min(loop_times)

# Final summary
print("\n=== Performance Summary ===")
print(f"Total Time for {num_loops} Loops: {total_time:.2f} seconds")
print(f"Mean Loop Time: {mean_time:.4f} seconds")
print(f"Variance: {variance_time:.6f}")
print(f"Maximum Loop Time: {max_time:.4f} seconds")
print(f"Minimum Loop Time: {min_time:.4f} seconds")
print(f"Average FPS: {1 / mean_time:.2f}")
print(f"Number of times exactly {number_of_faces} face(s) were detected: {count_exactly_x_faces}")
