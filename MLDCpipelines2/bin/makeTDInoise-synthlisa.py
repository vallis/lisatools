#!/usr/bin/env python

__version__='$Id$'

import lisaxml
from lisaxml.convertunit import convertUnit

import synthlisa

import numpy

import sys
import math

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id$")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for TDI observables (s) [default 0.0]")
                                    
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=31457280.0,
                  help="total time for TDI observables (s) [default 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# for the moment, support a single input barycentric file

if len(args) < 1:
    parser.error("You must specify an output file!")

outputfile = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

# set the noise seed
synthlisa.setglobalseed(options.seed)

# hardcode LISA and Noise to the MLDC standards
lisa = synthlisa.EccentricInclined(0.0,1.5*math.pi,-1)

proofnoise = [synthlisa.SumSignal(synthlisa.stdproofnoise(lisa,100,2.5e-48,2),
                                  synthlisa.stdproofnoisepink(lisa,1000,2.5e-48,1.0e-4,2))
              for i in range(6)]

shotnoise =  [synthlisa.stdopticalnoise(lisa,15,1.8e-37,4) for i in range(6)]

lasernoise = [synthlisa.NoSignal() for i in range(6)]

tdi = synthlisa.TDInoise(lisa,proofnoise,shotnoise,lasernoise)

samples = int( options.duration / options.timestep + 0.1 )

[t,X,Y,Z] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                              [tdi.t,tdi.Xm,tdi.Ym,tdi.Zm],
                                              options.inittime))

tdiobs = lisaxml.Observable('t,Xf,Yf,Zf')

tdiobs.DataType = 'FractionalFrequency'

tdiobs.TimeSeries = lisaxml.TimeSeries([t,X,Y,Z],'t,Xf,Yf,Zf')
tdiobs.TimeSeries.Cadence = options.timestep
tdiobs.TimeSeries.TimeOffset = options.inittime

outputXML = lisaxml.lisaXML(outputfile)

# save the standard LISA...
lisa = lisaxml.LISA('Standard MLDC PseudoLISA')
lisa.TimeOffset      = 0; lisa.TimeOffset_Unit      = 'Second'
lisa.InitialPosition = 0; lisa.InitialPosition_Unit = 'Radian'
lisa.InitialRotation = 0; lisa.InitialRotation_Unit = 'Radian'
lisa.Armlength = 16.6782; lisa.Armlength_Unit       = 'Second'
outputXML.LISAData(lisa)

outputXML.TDIData(tdiobs)

outputXML.close()

sys.exit(0)
