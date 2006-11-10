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

parser = OptionParser(usage="usage: %prog [options] BARYCENTRIC.xml OUTPUT.xml",
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

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# for the moment, support a single input barycentric file

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

inputXML = lisaxml.readXML(inputfile)
waveforms = inputXML.getLISASources()[0]
inputXML.close()

if options.verbose:
    for p in waveforms.parameters:
        print p, ':', waveforms.parstr(p)

if not hasattr(waveforms,'TimeSeries'):
    print "Script %s needs to be fed barycentric-strain files!" % sys.argv[0]
    sys.exit(1)

ts = waveforms.TimeSeries

# check that everything is in units that we can understand

try:
    assert ts.Cadence_Unit == 'Second'
    assert ts.TimeOffset_Unit == 'Second'

    assert waveforms.EclipticLatitude_Unit == 'Radian'
    assert waveforms.EclipticLongitude_Unit == 'Radian'
except:
    print "Script %s cannot understand units used for source parameters!" % sys.argv[0]
    sys.exit(1)

# lisaxml uses reshape to make the column arrays as views
# this does not play well with SampledWave (although it should make a copy if an
# array is not contiguous...); while we fix that, we won't lose too much with a copy

hp = ts.Arrays[0].copy()
hc = ts.Arrays[1].copy()
    
source = synthlisa.SampledWave(hp,hc,
                               ts.Length,ts.Cadence,-ts.TimeOffset, # synthlisa prebuffer is -TimeOffset
                               1.0,synthlisa.NoFilter(),2,          # norm = 1.0, no filtering, Lagrange-2 interpolation
                               waveforms.EclipticLatitude,
                               waveforms.EclipticLongitude,0)       # polarization already encoded in hp,hc

# this hardcodes the standard LISA object for the MLDC...
lisa = synthlisa.CacheLISA(synthlisa.EccentricInclined(0.0,1.5*math.pi,-1))

tdi = synthlisa.TDIsignal(lisa,source)

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

# remove the hp,hc arrays and save source parameters
del waveforms.TimeSeries
outputXML.SourceData(waveforms)

# output the TDI observables
outputXML.TDIData(tdiobs)

outputXML.close()

sys.exit(0)
