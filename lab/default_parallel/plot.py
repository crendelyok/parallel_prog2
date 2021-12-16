#!/bin/python3

import numpy  as np
import matplotlib.pyplot as plt

data = np.loadtxt("time_results")


#prc = data[:, 0]
#time = data[:, 1]

first_time = time[0]
acceleration = first_time / time
efficienty = acceleration / workers

#plt.plot(workers, acceleration)
#plt.plot(workers, efficienty)

#plt.show()
