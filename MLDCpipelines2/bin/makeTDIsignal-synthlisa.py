#!/usr/bin/env python

__version__='$Id$'

import lisaxml
from lisaxml.convertunit import convertUnit

import synthlisa

import numpy
import numpy.oldnumeric as Numeric
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
# Computing SNR....

# this is |\tilde{h}|^2 * dt/N
sampling = ts.Cadence
hX = synthlisa.spect(X,sampling, 0)
hY = synthlisa.spect(Y,sampling, 0)
hZ = synthlisa.spect(Z,sampling, 0)

#computing noise+Galaxy spectra
pdlen = samples/2
Sx = Numeric.zeros(pdlen+1, dtype='d')
Sopt = 1.8e-37
Spm = 2.5e-48
L=16.6782
snX = 0.0
snY = 0.0
snZ = 0.0
#fout = open("Psd.dat", 'w')
for i in xrange(pdlen):
   fr = hX[i+1,0]
   om = fr*2.0*math.pi
   #factor = 1.1e4*(8.*math.pi*L)**2*fr**4 
   factor = 1.54e4*(8.*math.pi*L)**2*fr**4 
   # instrumental noise
   Sx[i+1] = 16.*(math.sin(om*L)*fr)**2*Sopt + (32.*math.sin(om*L)*math.sin(om*L) + \
             8.*math.sin(2.*om*L)*math.sin(2.*om*L))*Spm*(1.+1.e-8/fr**2)/fr**2
   Sgal = 0.0
   # confusion noise
   if (fr>=1.e-4 and fr<1.e-3):
       Sgal = 10**-44.62*fr**-2.3 
   elif (fr>1.e-3 and fr<=10**-2.386):
       Sgal = 10**-47.62*fr**-3.3
   elif (fr>10**-2.386 and fr <=0.01):
       Sgal = 10**-69.615*fr**-12.52
#   elif (fr>1.e-3 and fr<=10.**-2.7):   # old Neil's fit
#       Sgal = 10**-50.92*fr**-4.4
#   elif (fr>10**-2.7 and fr<=10**-2.4):
#       Sgal = 10**-62.8*fr**-8.8
#   elif (fr>10**-2.4 and fr<=0.01):
#       Sgal = 10**-89.68*fr**-20.0
   Sgal *= factor
   Sx[i+1] += Sgal
   snX += hX[i+1,1]/Sx[i+1]
   snY += hY[i+1,1]/Sx[i+1]
   snZ += hZ[i+1,1]/Sx[i+1]
#   rec = str(fr) + "   " + str(Sx[i+1]) + "     " + str(Sgal) + "     " + str(Sx[i+1]+Sgal) + "\n"
#"    " + str(hX[i+1, 1]) + "     " + str(snX) + "\n"
#   fout.write(rec)

#fout.close()
snX *= 4.0
snY *= 4.0
snZ *= 4.0
SNR = snX
if(snY >SNR):
   SNR = snY
if(snZ >SNR):
   SNR = snZ

#  I rescale  SNR= 500 need to be passed over somehow...
print "SNR = ", math.sqrt(SNR)
hX[:,1] *= 500.0/math.sqrt(SNR)
#fout = open("Xcorrected.dat", 'w')
#for i in xrange(pdlen):
#   rec = str(hX[i+1,0]) + "    " + str(hX[i+1, 1]) + " \n"
#   fout.write(rec)
#fout.close()
#sys.exit(0)

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
