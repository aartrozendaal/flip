import cv2
import numpy as np
from picamera2 import Picamera2
from cvzone import FaceDetectionModule
import serial
import time
import statistics
from deepface import DeepFace
import csv

csv_filename = "Emotion_noCam_Test_Log.csv"
with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow([
        "Loop", "FaceTime(s)", "EmotionTime(s)", "TotalTime(s)",
        "NumFaces", "FaceX", "FaceY", "Emotion"
    ])

# Initialize the Pi Camera 3
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (1280, 720)}))  # HD resolution
picam2.start()

ser = serial.Serial('/dev/ttyACM0', 9600)  # Adjust to your Arduino port
time.sleep(2)  # Allow time for initialization

# Initialize face detector
face_detector = FaceDetectionModule.FaceDetector()

# Face detection counter variable
number_of_faces = 1  # Set how many faces you want to track
count_exactly_x_faces = 0  # Counter for how many times exactly X faces are detected
loop_times = []
num_loops = 1000 # number of loops performed

emotion_results = []
face_times = []
emotion_times = []
combined_times = []

def get_closest_face_coordinates():
    frame = picam2.capture_array()
    img = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    img, list_faces = face_detector.findFaces(img)

    # Update global count if exactly X faces were seen
    global count_exactly_x_faces
    if len(list_faces) == number_of_faces:
        count_exactly_x_faces += 1

    largest_face_coords = None
    face_crop = None
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
                face_crop = img[y:y+h, x:x+w]  # crop the largest face
        ser.write(f"{center_x},{center_y}\n".encode())

    return largest_face_coords, len(list_faces), face_crop

# Main loop with 100 iterations

def detect_emotion(face_img):
    try:
        result = DeepFace.analyze(face_img, actions=['emotion'], enforce_detection=False)
        dominant_emotion = result[0]['dominant_emotion']
        return dominant_emotion, True
    except Exception as e:
        #print(f"Emotion detection error: {e}")
        return "error", False
    
overall_start = time.perf_counter()

with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow([
        "Loop", "FaceTime(s)", "EmotionTime(s)", "TotalTime(s)",
        "NumFaces", "FaceX", "FaceY", "Emotion"
    ])

    for i in range(num_loops):
        print(f"\n--- Loop {i+1}/{num_loops} ---")

        loop_start = time.perf_counter()

        # Face Detection
        face_start = time.perf_counter()
        coordinates, num_faces, face_crop = get_closest_face_coordinates()
        face_end = time.perf_counter()

        face_time = face_end - face_start
        face_times.append(face_time)

        # Emotion Detection
        emotion = "none"
        emotion_time = 0
        success = False

        if face_crop is not None:
            emotion_start = time.perf_counter()
            emotion, success = detect_emotion(face_crop)
            emotion_end = time.perf_counter()
            emotion_time = emotion_end - emotion_start
        else:
            emotion_time = 0

        emotion_times.append(emotion_time)
        emotion_results.append(emotion)

        if success:
            print(f"Detected Emotion: {emotion}")
            try:
                ser.write(f"{emotion}\n".encode())
            except Exception as e:
                print(f"Serial write error: {e}")

        # Combine timings
        loop_end = time.perf_counter()
        loop_duration = loop_end - loop_start
        loop_times.append(loop_duration)
        combined_times.append(face_time + emotion_time)

        # Extract coordinates or default
        face_x, face_y = coordinates if coordinates else (-1, -1)

        # Write to CSV
        writer.writerow([
            i+1, face_time, emotion_time, loop_duration,
            num_faces, face_x, face_y, emotion
        ])

overall_end = time.perf_counter()
total_time = overall_end - overall_start



# Calculate stats
mean_time = statistics.mean(loop_times)
variance_time = statistics.variance(loop_times)
max_time = max(loop_times)
min_time = min(loop_times)

def print_stats(name, times):
    print(f"{name} - Mean: {statistics.mean(times):.4f}s | "
          f"Variance: {statistics.variance(times):.6f} | "
          f"Max: {max(times):.4f}s | Min: {min(times):.4f}s")

print("\n=== Performance Breakdown ===")
print_stats("Face Detection Time", face_times)
print_stats("Emotion Detection Time", emotion_times)
print_stats("Combined Processing Time", combined_times)

# Emotion Summary
print("\n=== Emotion Summary ===")
from collections import Counter
emotion_counter = Counter(emotion_results)
for emotion, count in emotion_counter.items():
    print(f"{emotion}: {count} times")