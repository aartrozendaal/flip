import cv2
import numpy as np
from picamera2 import Picamera2
import serial
import time
import statistics

# Initialize the Pi Camera 3
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (1280, 720)}))  # HD resolution
picam2.start()

# Initialize serial communication to Arduino
ser = serial.Serial('/dev/ttyACM0', 9600)  # Adjust to your Arduino port
time.sleep(2)  # Allow time for initialization

# Initialize HOG + SVM people detector
hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

# Detection config
number_of_people = 1
count_exactly_x_people = 0
loop_times = []
num_loops = 1000

def get_closest_person_coordinates():
    frame = picam2.capture_array()
    img = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

    # Detect people using HOG+SVM
    boxes, weights = hog.detectMultiScale(img, winStride=(8, 8), padding=(8, 8), scale=1.05)

    people = []
    for (x, y, w, h), weight in zip(boxes, weights):
        area = w * h
        x1, y1, x2, y2 = x, y, x + w, y + h
        people.append({'box': (x1, y1, x2, y2), 'area': area})

    global count_exactly_x_people
    if len(people) == number_of_people:
        count_exactly_x_people += 1

    if not people:
        return None, 0

    # Find closest (largest area)
    largest = max(people, key=lambda p: p['area'])
    x1, y1, x2, y2 = largest['box']
    center_x = int((x1 + x2) / 2)
    top_y = int(y1)

    ser.write(f"{center_x},{top_y}\n".encode())

    return (center_x, top_y), len(people)

# Main loop
overall_start = time.perf_counter()

for i in range(num_loops):
    print(f"\n--- Loop {i+1}/{num_loops} ---")

    start_time = time.perf_counter()
    coordinates, num_people = get_closest_person_coordinates()
    end_time = time.perf_counter()

    loop_duration = end_time - start_time
    loop_times.append(loop_duration)

    if coordinates:
        print(f"Largest Person Coordinates: {coordinates}")
    else:
        print("No person detected.")

    print(f"People Detected: {num_people}")
    print(f"Loop Time: {loop_duration:.3f} seconds")

# Summary
overall_end = time.perf_counter()
total_time = overall_end - overall_start

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
print(f"Number of times exactly {number_of_people} person(s) were detected: {count_exactly_x_people}")
