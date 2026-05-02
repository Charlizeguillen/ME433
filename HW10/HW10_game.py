import pgzrun
import serial
import random

WIDTH = 800
HEIGHT = 600

airplane = Actor("airplane")

airplane.x = WIDTH / 2
airplane.y = HEIGHT - 80

# change this to your Pico port
ser = serial.Serial("/dev/tty.usbmodem101", 115200, timeout=0.01)

ax = 0
ay = 0

speed = 10
deadzone = 0.04

obstacles = []
obstacle_speed = 4
spawn_timer = 0

score = 0
game_state = "start"   # start, playing, gameover


def reset_game():
    global obstacles, score, obstacle_speed, spawn_timer

    airplane.x = WIDTH / 2
    airplane.y = HEIGHT - 80

    obstacles = []
    score = 0
    obstacle_speed = 4
    spawn_timer = 0


def update():
    global ax, ay, spawn_timer, score, obstacle_speed, game_state

    if keyboard.space:
        if game_state == "start" or game_state == "gameover":
            reset_game()
            game_state = "playing"

    if game_state != "playing":
        return

    # read IMU serial data
    try:
        while ser.in_waiting > 0:
            line = ser.readline().decode().strip()

            if line:
                values = line.split(",")

                if len(values) == 2:
                    ax = float(values[0])
                    ay = float(values[1])

    except:
        pass

    move_x = ax
    move_y = ay

    if abs(move_x) < deadzone:
        move_x = 0

    if abs(move_y) < deadzone:
        move_y = 0

    airplane.x += move_x * speed
    airplane.y += -(move_y * speed)

    # keep airplane on screen
    airplane.x = max(30, min(WIDTH - 30, airplane.x))
    airplane.y = max(30, min(HEIGHT - 30, airplane.y))

    # spawn obstacles
    spawn_timer += 1

    if spawn_timer > 45:
        obstacle = Actor("obstacle")
        obstacle.x = random.randint(30, WIDTH - 30)
        obstacle.y = -30
        obstacles.append(obstacle)
        spawn_timer = 0

    # move obstacles
    for obstacle in obstacles:
        obstacle.y += obstacle_speed

    # remove obstacles after they leave screen
    obstacles[:] = [obs for obs in obstacles if obs.y < HEIGHT + 50]

    # check collision
    for obstacle in obstacles:
        if airplane.colliderect(obstacle):
            game_state = "gameover"

    # score and difficulty
    score += 1

    if score % 500 == 0:
        obstacle_speed += 0.5


def draw():
    screen.clear()
    screen.fill((30, 140, 50))

    if game_state == "start":
        screen.draw.text("IMU Airplane GAME", center=(WIDTH/2, 200), fontsize=60, color="white")
        screen.draw.text("Tilt the IMU to move the airplane", center=(WIDTH/2, 280), fontsize=35, color="white")
        screen.draw.text("Press SPACE to begin", center=(WIDTH/2, 350), fontsize=40, color="yellow")
        return

    if game_state == "playing":
        airplane.draw()

        for obstacle in obstacles:
            obstacle.draw()

        screen.draw.text(f"Score: {score}", (10, 10), fontsize=35, color="white")
        screen.draw.text(f"ax: {ax:.2f}", (10, 50), fontsize=25, color="white")
        screen.draw.text(f"ay: {ay:.2f}", (10, 75), fontsize=25, color="white")

    if game_state == "gameover":
        airplane.draw()

        for obstacle in obstacles:
            obstacle.draw()

        screen.draw.text("GAME OVER", center=(WIDTH/2, 230), fontsize=70, color="red")
        screen.draw.text(f"Final Score: {score}", center=(WIDTH/2, 310), fontsize=45, color="white")
        screen.draw.text("Press SPACE to restart", center=(WIDTH/2, 380), fontsize=40, color="yellow")


pgzrun.go()