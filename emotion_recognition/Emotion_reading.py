import cv2
import numpy as np
from picamera2 import Picamera2
from cvzone import FaceDetectionModule
import serial
import time
import statistics
from fer import FER
import csv
from collections import Counter


# Initialize emotion detector
emotion_detector = FER(mtcnn=False)
saved_debug_images =0 # Top of the script

# Initialize the Pi Camera 3
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (1280, 720)}))  # HD resolution
picam2.start()

# Initialize face detector
face_detector = FaceDetectionModule.FaceDetector()

def get_closest_face_coordinates():
    frame = picam2.capture_array()
    img = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    img_copy = img.copy()  # Prevent drawing on the original
    _, list_faces = face_detector.findFaces(img_copy)

    global count_exactly_x_faces
    if len(list_faces) == number_of_faces:
        count_exactly_x_faces += 1

    largest_face_coords = None
    face_crop = None
    margin = 100  # Padding around the face — adjust as needed

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

                # Add padding to bounding box
                x1 = max(x - margin, 0)
                y1 = max(y - margin, 0)
                x2 = min(x + w + margin, img.shape[1])
                y2 = min(y + h + margin, img.shape[0])

                face_crop = img[y1:y2, x1:x2]  # Padded crop

        ser.write(f"{center_x},{center_y}\n".encode())

    return largest_face_coords, len(list_faces), face_crop


def detect_emotion(face_img):
    try:
        # Ensure face is in RGB format
        #rgb_face = cv2.cvtColor(face_img, cv2.COLOR_BGR2RGB)

        # Resize to something FER likes (optional but helps)
        resized = cv2.resize(face_img, (224, 224))

        # Detect emotion
        result = emotion_detector.detect_emotions(resized)
        global saved_debug_images
        # In the emotion detection block:
        if saved_debug_images < 5:
            debug_img = resized#cv2.cvtColor(resized, cv2.COLOR_RGB2BGR)
            cv2.imwrite(f"debug_face_{i}.jpg", debug_img)
            saved_debug_images += 1
        
        if result and "emotions" in result[0]:
            emotions = result[0]["emotions"]
            dominant_emotion = max(emotions, key=emotions.get)
            return dominant_emotion, True
        else:
            return "none", False

    except Exception as e:
        print(f"Emotion detection error: {e}")
        return "error", False

# === MAIN LOOP ===


while True:
    
    # Face Detection
    coordinates, num_faces, face_crop = get_closest_face_coordinates()

    emotion = "none"
        if face_crop is not None:
            emotion, success = detect_emotion(face_crop)

            if success:
                print(f"Detected Emotion: {emotion}")
