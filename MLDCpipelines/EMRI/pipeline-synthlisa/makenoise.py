#!/usr/bin/env python

from synthlisa import *
from Numeric import *

import math
import sys

if len(sys.argv) < 2:
    print "Usage: %s <challenge-name>" % sys.argv[0]

lisa = EccentricInclined(0.0,1.5*math.pi,-1)

proofnoise = [SumSignal(stdproofnoise(lisa,100,2.5e-48,2),
                        stdproofnoisepink(lisa,1000,2.5e-48,1.0e-4,2))
              for i in range(6)]

shotnoise =  [stdopticalnoise(lisa,15,1.8e-37,4) for i in range(6)]

lasernoise = [NoSignal() for i in range(6)]

tdinoise = TDInoise(lisa,proofnoise,shotnoise,lasernoise)

outputXML = lisaXML(sys.argv[1] + '-noise.xml',author='Michele Vallisneri',
                                               comments='Geometry/noise file for challenge ' + sys.argv[1])

outputXML.NoiseData(tdinoise,'TDI noises')

outputXML.close()
