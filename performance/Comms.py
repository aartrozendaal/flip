import serial
import time

ser = serial.Serial('/dev/ttyACM0', 115200)  # Adjust to your Arduino port
time.sleep(2)  # Allow time for initialization


def send_data(num_faces, face_x, face_y, face_size,
              num_bodies, body_x, body_y, body_size,
              sound_direction, emotion):
    data = f"{num_faces},{face_x},{face_y},{face_size}," \
           f"{num_bodies},{body_x},{body_y},{body_size}," \
           f"{sound_direction},{emotion}\n"
    ser.write(data.encode())


# Example usage
while True:
    num_faces, face_x, face_y, face_size = 0,0,0,0
    num_bodies,body_x,body_y,body_size = 0,0,0,0
    
    sound_direction = 0
    emotion = 0
    
    #print(f"Faces: {num_faces} at ({face_x}, {face_y}) size: {face_size} | "
      #f"Bodies: {num_bodies} at ({body_x}, {body_y}) size: {body_size} | "
      #f"Sound Direction: {sound_direction} | Emotion: {emotion}")

    send_data(num_faces, face_x, face_y, face_size,
          num_bodies, body_x, body_y, body_size,
          sound_direction, emotion)
    