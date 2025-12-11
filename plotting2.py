# Authors: Nate Seibold, Daniel Nalepa
# Date: 12/12/2025
# EE5990 Optoelectronics Final
# ChatGPT Used to Assist with Code

# import required libraries
import serial
import time
import math
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# establish serial port connection
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)

# define dicitonary for distance values
points = {}

# create a polar plot using MatPlotLib
fig = plt.figure(figsize=(6,6), facecolor='black')  # black background
ax = fig.add_subplot(111, polar=True, facecolor='black')

# create small green points
line, = ax.plot([], [], 'go', markersize=4)  # green points

ax.set_theta_zero_location('N')
ax.set_theta_direction(-1)

# set ranging based on ToF max distance
ax.set_rlim(0, 3000)

# customize grid & circles to match "radar aesthetic"
ax.grid(color='darkgreen', linestyle='--', linewidth=0.5)
ax.set_yticklabels([])  # remove radial labels
ax.set_xticklabels([])  # remove angle labels

# sweeping radar line effect
sweep_line, = ax.plot([], [], color='lime', linewidth=1, alpha=0.3)

# define function for updating values
def update(frame):
    raw = ser.readline().decode('utf-8').strip() # recieve incoming line, format and save

    if not raw: # if there is no new line...
        return line, sweep_line # pass on previous data

    try:
        angle_str, dist_str = raw.split(',') # seperate new line
        angle_deg = int(float(angle_str)) # convert angle to int

        dist_val = float(dist_str) # convert distance to float
        if dist_val < 0: # if distance out of range
            dist_val = 3000 # set to max

        points[angle_deg] = (math.radians(angle_deg), dist_val) # append new data to dictionary

        # extract data from dictionary into array
        angle_rads = []
        dists = []
        for angle_rad, dist in points.values():
            angle_rads.append(angle_rad)
            dists.append(dist)

        line.set_data(angle_rads, dists)

        # update sweeping line
        sweep_line.set_data([math.radians(angle_deg), math.radians(angle_deg)],
                            [0, 3000])

    except Exception:
        pass

    # pass back data points
    return line, sweep_line

# run plotting animation and search for new data
ani = FuncAnimation(fig, update, interval=5, cache_frame_data=False)
plt.show(block=True)

