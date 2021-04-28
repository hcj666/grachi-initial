#!/usr/bin/python

import sys
import os
import matplotlib

import numpy
import matplotlib
matplotlib.use('AGG')
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from matplotlib.ticker import FormatStrFormatter

lastsecs = 240

fname = sys.argv[-1]

tdata = numpy.loadtxt(fname, delimiter=" ")
times = tdata[:, 0]
values = tdata[:, 1]

lastt = max(times)


#majorFormatter = FormatStrFormatter('%.2f')
 
fig = plt.figure(figsize=(3.5, 2.0))
plt.plot(times[times > lastt - lastsecs], values[times > lastt - lastsecs])
plt.gca().xaxis.set_major_locator( MaxNLocator(nbins = 7, prune = 'lower') )
plt.xlim([lastt - lastsecs, lastt])
#plt.ylim([lastt - lastsecs, lastt])

plt.gca().yaxis.set_major_locator( MaxNLocator(nbins = 7, prune = 'lower') )

#plt.gca().yaxis.set_major_formatter(majorFormatter)
plt.savefig(fname.replace(".dat", ".png"), format="png")


