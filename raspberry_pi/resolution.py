import time
import statistics
from picamera2 import Picamera2

# Initialize the Pi Camera 3
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"size": (1280, 720)}))
picam2.start()

# List to hold capture times
capture_times = []

# Warm-up frame (optional)
_ = picam2.capture_array()

# 1000 captures
for i in range(1000):
    start = time.time()
    frame = picam2.capture_array()
    end = time.time()
    capture_times.append(end - start)

# Compute metrics
mean_t = statistics.mean(capture_times)
max_t = max(capture_times)
min_t = min(capture_times)
var_t = statistics.variance(capture_times)
fps = 1.0 / mean_t if mean_t > 0 else 0.0

print(f"Captured 1000 frames at 1280×720")
print(f"Mean capture time: {mean_t:.4f} s")
print(f"Max capture time:  {max_t:.4f} s")
print(f"Min capture time:  {min_t:.4f} s")
print(f"Variance:          {var_t:.6f}")
print(f"Achieved FPS:      {fps:.2f}")

# Cleanup (if desired)
picam2.close()
