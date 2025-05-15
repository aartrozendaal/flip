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

# Load Haar cascade classifier
haar_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_upperbody.xml")

# Detection function using Haar
def get_closest_person_coordinates():
    frame = picam2.capture_array()
    gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)

    # Detect upper bodies
    bodies = haar_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(60, 60))

    people = []
    for (x, y, w, h) in bodies:
        area = w * h
        x1, y1, x2, y2 = x, y, x + w, y + h
        people.append({'box': (x1, y1, x2, y2), 'area': area})

    global count_exactly_x_people
    if len(people) == number_of_people:
        count_exactly_x_people += 1

    if not people:
        return None, 0

    # Find the closest person (largest area)
    largest = max(people, key=lambda p: p['area'])
    x1, y1, x2, y2 = largest['box']
    center_x = int((x1 + x2) / 2)
    top_y = int(y1)

    return (center_x, top_y), len(people), area

while True:
    coordinates, num_people, area = get_closest_person_coordinates()
    if coordinates:
        print(f"Largest Detected Person Coordinates: {coordinates}")
    else:
        print("No person detected.")

    print(f"People Detected: {num_people}")
    print(f"Size: {area}")
