import sys, os
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

filename="data/wind_field.dat"
data = pd.read_csv(filename,sep = ' ')
data_t = data[data["t"]==100]

fig = plt.figure()
ax = fig.gca(projection='3d')
plt.xlabel('x')
plt.ylabel('y')
plt.title('Updraft velocity[m/s]', fontsize=15)
cs=ax.plot_trisurf(data_t["x"], data_t["y"], data_t["updraft"], cmap=cm.jet, linewidth=0.2)

plt.show()

