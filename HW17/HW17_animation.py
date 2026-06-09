import serial
import matplotlib.pyplot as plt
import numpy as np

port = "/dev/cu.usbmodem1101" 
baud = 115200

ser = serial.Serial(port, baud, timeout=1)

arm_length = 1.0

plt.ion()
fig, ax = plt.subplots()

ax.set_aspect("equal")
ax.set_xlim(-1.2, 1.2)
ax.set_ylim(-1.2, 1.2)
ax.grid(True)

lever_line, = ax.plot([0, arm_length], [0, 0], linewidth=4)
pivot_dot, = ax.plot(0, 0, "o")
text = ax.text(-1.1, 1.05, "")

while True:
    try:
        line = ser.readline().decode().strip()

        if not line:
            continue

        # expected Pico format:
        # angle_deg,load_raw
        parts = line.split(",")

        if len(parts) < 2:
            continue

        angle_deg = float(parts[0])
        load_raw = float(parts[1])

        angle_rad = np.deg2rad(angle_deg)

        x = arm_length * np.cos(angle_rad)
        y = arm_length * np.sin(angle_rad)

        lever_line.set_data([0, x], [0, y])
        text.set_text(f"Angle: {angle_deg:.1f} deg\nLoad: {load_raw:.0f}")

        plt.pause(0.001)

    except KeyboardInterrupt:
        break
    except:
        continue

ser.close()