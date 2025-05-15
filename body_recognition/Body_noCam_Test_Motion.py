import cv2
import numpy as np
from picamera2 import Picamera2
import serial
import time
import statistics

# Initialize the Pi Camera
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (640, 480)}))
picam2.start()

# Serial communication
ser = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(2)

# Background subtractor for motion
bg_subtractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40)

# Settings
number_of_people = 1
count_exactly_x_people = 0
loop_times = []
num_loops = 1000

# Optional: color range for skin tones in HSV
lower_skin = np.array([0, 30, 60], dtype=np.uint8)
upper_skin = np.array([20, 150, 255], dtype=np.uint8)

def get_motion_coordinates():
    frame = picam2.capture_array()
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Apply background subtraction
    fg_mask = bg_subtractor.apply(frame)

    # Optional: Mask by color (e.g., skin detection)
    skin_mask = cv2.inRange(hsv, lower_skin, upper_skin)

    # Combine motion + skin detection
    combined_mask = cv2.bitwise_and(fg_mask, skin_mask)

    # Clean up the mask
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
    combined_mask = cv2.morphologyEx(combined_mask, cv2.MORPH_OPEN, kernel)

    # Find contours (moving blobs)
    contours, _ = cv2.findContours(combined_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    people = []

    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 5000:  # Minimum area to count as a person
            x, y, w, h = cv2.boundingRect(cnt)
            x1, y1, x2, y2 = x, y, x + w, y + h
            people.append({'box': (x1, y1, x2, y2), 'area': area})

    global count_exactly_x_people
    if len(people) == number_of_people:
        count_exactly_x_people += 1

    if not people:
        return None, 0

    # Find the closest/largest blob
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
    coordinates, num_people = get_motion_coordinates()
    end_time = time.perf_counter()

    loop_duration = end_time - start_time
    loop_times.append(loop_duration)

    if coordinates:
        print(f"Largest Moving Blob Coordinates: {coordinates}")
    else:
        print("No movement detected.")

    print(f"Moving Objects Detected: {num_people}")
    print(f"Loop Time: {loop_duration:.3f} seconds")

# Performance Summary
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
