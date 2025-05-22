from djitellopy import Tello
import time

tello = Tello()

tello.connect()
print(tello.get_battery())

tello.takeoff()

# Define waypoints (example - replace with your actual coordinates)
waypoints = [
    (0, 100, 80),  # x, y, z (cm) - First yellow square
    (150, 100, 80), # x, y, z - Second yellow square
    (150, 250, 80), # x, y, z - Third yellow square
    (300, 250, 80)  # x, y, z - Fourth yellow square
]

speed = 60  # cm/s (adjust as needed)

for waypoint in waypoints:
    x, y, z = waypoint
    tello.go(x, y, z, speed)
    time.sleep(5)  # Give the drone time to reach the waypoint
                       # Adjust this time as needed

tello.land()