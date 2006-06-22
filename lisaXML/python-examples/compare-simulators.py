#!/usr/bin/env python

from synthlisa import *
from Numeric import *

import math
import sys

if len(sys.argv) < 3:
    print "Usage: %s <synthlisa.xml> <lisasimulator.xml>" % sys.argv[0]

inputXML = readXML(sys.argv[1])
obs = inputXML.getTDITimeSeries()
inputXML.close()

inputXML = readXML(sys.argv[2])
obs2 = inputXML.getTDITimeSeries()
inputXML.close()

# print obs[0].keys()

t = obs[0]['Data'][:,0]
X = obs[0]['Data'][:,1]
Y = obs[0]['Data'][:,2]
Z = obs[0]['Data'][:,3]

t2 = obs2[0]['Data'][:,0]
X2 = obs2[0]['Data'][:,1]
Y2 = obs2[0]['Data'][:,2]
Z2 = obs2[0]['Data'][:,3]

stime = 15
patches = 64

# use Blackman windowing for spectra

mywindow = 'blackman'

# compute spectra, averaging "patches" overlapping segments of time series

sX = spect(X,stime,patches,win=mywindow)
sX2 = spect(X2,stime,patches,win=mywindow)

# go to a spectrum of phase (from frequency)
f = sX[1:,0]
sX[1:,1] = sX[1:,1] / (2 * math.pi * f)**2

# go to a spectrum of phase (from strain)
scale = 1e10 / 299792458.0
sX2[1:,1] = scale**2 * sX2[1:,1]

# get the theoretical expression

w = 2 * math.pi * f
L = 16.67

Spm = 2.5e-48 * (1 + (f/1e-4)**-2) * f**(-2)
Sop = 1.8e-37 * f**2

SX = 16.0 * sin(w*L)**2 * (2 * (1 + cos(w*L)**2) * Spm + Sop) / (2 * math.pi * f)**2

outputXML = lisaXML('tdispectrum.xml',author='Michele Vallisneri')

outputXML.TDISpectraSelfDescribed(transpose([f,sX[1:,1],sX2[1:,1],SX]),'f,Xf,Xs,Xt',encoding='Text')

outputXML.close()
