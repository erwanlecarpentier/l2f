from __future__ import division
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys
from mpl_toolkits.mplot3d import Axes3D
from scipy import *
from pylab import *
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d

## File reading
trajectory_path = "data/state.dat"
trajectory_data = np.loadtxt(trajectory_path,dtype=float)
thermal_data_path = "config/thermal_scenario.csv"
thermal_data_buffer = pd.read_csv(thermal_data_path,sep = ';')
thermal_data = thermal_data_buffer[thermal_data_buffer["t_birth"]==0]

## Trajectory plot
x = trajectory_data[:,0]
y = trajectory_data[:,1]
fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(x,y,color='#6699ff')
ax.set_xlim((-1300, 1300))
ax.set_ylim((-1300, 1300))
ax.set_xlabel('x')
ax.set_ylabel('y')

## Thermals plot
size = len(thermal_data["x"])
for i in range(0,size):
    x_th=linspace(thermal_data["x"][i],thermal_data["x"][i],1000)
    y_th=linspace(thermal_data["y"][i],thermal_data["y"][i],1000)
    ax.scatter(x_th,y_th,color='#ff6600')

## Border plot
limit_circle = Circle((0, 0), 1200, color='black', fill=False)
ax.add_patch(limit_circle)

plt.show()

