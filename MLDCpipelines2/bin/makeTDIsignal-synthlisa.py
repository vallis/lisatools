#!/usr/bin/env python

__version__='$Id$'

import lisaxml
from lisaxml.convertunit import convertUnit

import synthlisa

import numpy
import numpy.oldnumeric as Numeric
import sys

from math import pi, sin, cos, sqrt

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

parser.add_option("-n", "--noiseOnly",
		  action="store_true", dest="noiseOnly", default=False,
		  help="compute SNR using instrumental noise only [off by default]")

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
lisa = synthlisa.CacheLISA(synthlisa.EccentricInclined(0.0,1.5*pi,-1))

tdi = synthlisa.TDIsignal(lisa,source)

samples = int( options.duration / options.timestep + 0.1 )

[t,X,Y,Z] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                              [tdi.t,tdi.Xm,tdi.Ym,tdi.Zm],
                                              options.inittime))

# Computing SNR....

# this is |\tilde{h}|^2 * dt/N
sampling = ts.Cadence

hX = synthlisa.spect(X,sampling,0)
hY = synthlisa.spect(Y,sampling,0)
hZ = synthlisa.spect(Z,sampling,0)

# get frequencies, skip DC
fr = hX[1:,0]

om = 2.0 * pi * fr
L  = 16.6782

# instrument noise
Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
Sop = 1.8e-37 * fr**2

Sx = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)

# expression from Phys. Rev. D 73, 122001 (2006), corrected and modified to include TDI response function
# for fractional frequency fluctuations

# conversion LS to SL (incl.deriv.) -- TDI response function Michelson to X,Y,Z
Sgal = (2.0 * L)**2 * (2*pi*fr)**2 * 4.0 * numpy.sin(om*L)**2 * (
         numpy.piecewise(fr,(fr >= 1.0e-4  ) & (fr < 1.0e-3  ),[lambda f: 10**-44.62 * f**-2.3, 0]) + \
         numpy.piecewise(fr,(fr >= 1.0e-3  ) & (fr < 10**-2.7),[lambda f: 10**-50.92 * f**-4.4, 0]) + \
         numpy.piecewise(fr,(fr >= 10**-2.7) & (fr < 10**-2.4),[lambda f: 10**-62.8  * f**-8.8, 0]) + \
         numpy.piecewise(fr,(fr >= 10**-2.4) & (fr < 10**-2.0),[lambda f: 10**-89.68 * f**-20.0,0])     )

Sn = Sx + Sgal
if options.noiseOnly:
   Sn = Sx
    

#fout = open("PsdN.dat", 'w')
#spr  = "     "
#for i in xrange(samples/2):
#    rec =  str(fr[i]) + spr + str(Sx[i]) + spr + str(Sgal[i]) + spr + str(Sn[i]) + "\n"
#    fout.write(rec)
#fout.close()


# the (S/N)^2 is given by 2T times the integrated ratio
# of the spectral densities (the factor of 2 because the spectral
# density is one-sided); notice however that the df is 1/T,
# so we need only to sum up the array containing the ratio,
# and multiply by two

snX = sqrt(2.0 * numpy.sum(hX[1:,1] / Sn))
snY = sqrt(2.0 * numpy.sum(hY[1:,1] / Sn))
snZ = sqrt(2.0 * numpy.sum(hZ[1:,1] / Sn))

SNR = max(snX,snY,snZ)

print "makeTDIsignal-synthlisa.py: maximum X,Y,Z SNR = %s" % SNR

if hasattr(waveforms,'RequestSN'):
    ReqSN = waveforms.RequestSN

    factor = ReqSN / SNR
    print "makeTDIsignal-synthlisa.py: satisfying RequestSN=%s by rescaling by %s" % (ReqSN,factor)

    ts.Arrays[0] *= factor
    ts.Arrays[1] *= factor   

    if hasattr(waveforms, 'Distance'):
        waveforms.Distance /= factor
    elif hasattr(waveforms, 'Amplitude'):
        waveforms.Amplitude *= factor 

    # remove the RequestSN param from the source
    del waveforms.RequestSN

    outputXML = lisaxml.lisaXML(inputfile, author='makeTDIsignal-synthlisa.py, MV/SB 20061129')
    outputXML.SourceData(waveforms)
    outputXML.close()

    X *= factor
    Y *= factor
    Z *= factor

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

# remove the hp,hc arrays and save source parameters
del waveforms.TimeSeries
outputXML.SourceData(waveforms)

# output the TDI observables
outputXML.TDIData(tdiobs)

outputXML.close()

sys.exit(0)
