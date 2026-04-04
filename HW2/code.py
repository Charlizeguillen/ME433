import time
import board
import pwmio
from adafruit_motor import servo

# Create PWM output on GP16 at 50 Hz for servo control
pwm = pwmio.PWMOut(board.GP16, duty_cycle=0, frequency=50)

# Create servo object
my_servo = servo.Servo(pwm, min_pulse=1000, max_pulse=2000)

while True:
    # Sweep from 0 to 180 degrees
    for angle in range(0, 181, 10):
        my_servo.angle = angle
        time.sleep(0.05)

    # Sweep back from 180 to 0 degrees
    for angle in range(180, -1, -10):
        my_servo.angle = angle
        time.sleep(0.05)
