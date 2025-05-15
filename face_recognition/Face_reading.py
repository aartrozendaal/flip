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

# Initialize face detector
face_detector = FaceDetectionModule.FaceDetector()

def get_closest_face_coordinates():
    frame = picam2.capture_array()
    img = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    img, list_faces = face_detector.findFaces(img)

    # Update global count if exactly X faces were seen
    global count_exactly_x_faces
    if len(list_faces) == number_of_faces:
        count_exactly_x_faces += 1

    largest_face_coords = None
    if list_faces:
        largest_area = 0
        for face in list_faces:
            x, y, w, h = face["bbox"]
            area = w * h
            if area > largest_area:
                largest_area = area
                center_x = x + w // 2
                center_y = y + h // 2
                largest_face_coords = (center_x, center_y)
        ser.write(f"{center_x},{center_y}\n".encode())

    return largest_face_coords, len(list_faces), largest_area

while true:

    coordinates, num_faces = get_closest_face_coordinates()



