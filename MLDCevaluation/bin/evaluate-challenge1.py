#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
from lisaxml.convertunit import convertUnit

import sys
import math

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] KEYFILE.xml RESULTFILE.xml",
                      version="$Id: $")

parser.add_option("-b", "--preBuffer",
                  type="float", dest="prebuffer", default=900.0,
                  help="length of data computed before initial time (s) [default 900.0]")

parser.add_option("-p", "--postBuffer",
                  type="float", dest="postbuffer", default=900.0,
                  help="length of data computed after final time (s) [default 900.0]")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for waveform, not counting prebuffer (s) [default 0.0]")
                                    
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=31457280.0,
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for waveform sampling (s) [default 15]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# currently we support only a single source parameter file


if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputKeyfile,inputResfile) = args

# work only with the first source of the first file
# TO DO:  Later it should be expanded to multiple sources in result file

inputXML = lisaxml.readXML(inputKeyfile)
Injsystem = inputXML.getLISASources()[0]
inputXML.close()

inputXML = lisaxml.readXML(inputResfile)
Detsystem = inputXML.getLISASources()[0]
inputXML.close()

# print out parameters

if options.verbose:
    for p in mysystem.parameters:
        print p, ':', mysystem.parstr(p)


# Repeating steps from the "makebarycentric.py"


#Stas: do we need those lines?
if ( hasattr(Injsystem,'IntegrationStep') and
    convertUnit(Injsystem.IntegrationStep,Injsystem.IntegrationStep_Unit,'Second') != (options.timestep,'Second') ):
    print >> sys.stderr, "Overriding source IntegrationStep (%s) with our own timestep (%s s)" % (Injsystem.parstr('IntegrationStep'),
                                                                                                  options.timestep)


initialtime = options.inittime - options.prebuffer
samples = int( (options.duration + options.prebuffer + options.postbuffer) / options.timestep + 0.1 )


#### Computing Injected Waveform

(hpI0,hcI0) = Injsystem.waveforms(samples,options.timestep,initialtime)

# impose polarization on waveform

if (Injsystem.SourceType == 'BlackHoleBinary'):
  print "Generating BBH signal..."
  pol = Injsystem.Polarization
  hpI =  math.cos(2*pol) * hpI0 + math.sin(2*pol) * hcI0
  hcI = -math.sin(2*pol) * hpI0 + math.cos(2*pol) * hcI0
  Injsystem.TimeSeries = lisaxml.TimeSeries((hpI,hcI),'hp,hc')
elif (mysystem.SourceType == 'EMRI'):
  print "Generating EMRI signal..."
  Injsystem.TimeSeries = lisaxml.TimeSeries((hpI0,hcI0),'hp,hc')
  hpI = hpI0
  hcI = hcI0
elif (Injsystem.SourceiType == 'GalacticBinary'):
  print "Generating Galactic binary"
  Injssystem.TimeSeries = lisaxml.TimeSeries((hpI0,hcI0),'hp,hc')
  hpI = hpI0
  hcI = hcI0


#### Computing Detected Waveform

(hpD0,hcD0) = Detsystem.waveforms(samples,options.timestep,initialtime)

# impose polarization on waveform

if (Detsystem.SourceType == 'BlackHoleBinary'):
  print "Generating BBH signal..."
  pol = Detsystem.Polarization
  hpD =  math.cos(2*pol) * hpD0 + math.sin(2*pol) * hcD0
  hcD = -math.sin(2*pol) * hpD0 + math.cos(2*pol) * hcD0
  Detsystem.TimeSeries = lisaxml.TimeSeries((hpD,hcD),'hp,hc')
elif (mysystem.SourceType == 'EMRI'):
  print "Generating EMRI signal..."
  Detsystem.TimeSeries = lisaxml.TimeSeries((hpD0,hcD0),'hp,hc')
  hpD = hpD0
  hcD = hcD0
elif (Injsystem.SourceType == 'GalacticBinary'):
  print "Generating Galactic binary"
  Detssystem.TimeSeries = lisaxml.TimeSeries((hpI0,hcI0),'hp,hc')
  hpD = hpD0
  hcD = hcD0




# this hardcodes the standard LISA object for the MLDC...
lisa = synthlisa.CacheLISA(synthlisa.EccentricInclined(0.0,1.5*pi,-1))

# Generating TDI

## for injection
sourceInj = synthlisa.SampledWave(hpI,hc,
                               ts.Length,ts.Cadence,-ts.TimeOffset, # synthlisa prebuffer is -TimeOffset
                               1.0,synthlisa.NoFilter(),2,          # norm = 1.0, no filtering, Lagrange-2 interpolation
                               Injsystem.EclipticLatitude,
                               Injsystem.EclipticLongitude,0)       # polarization already encoded in hp,hc

tdiInj = synthlisa.TDIsignal(lisa,sourceInj)

samples = int( options.duration / options.timestep + 0.1 )

[tInj,Xinj,Yinj,Zinj] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                              [tdiInj.t,tdiInj.Xm,tdiInj.Ym,tdiInj.Zm],
                                              options.inittime))

## for detected params

sourceDet = synthlisa.SampledWave(hpI,hc,
                               ts.Length,ts.Cadence,-ts.TimeOffset, # synthlisa prebuffer is -TimeOffset
                               1.0,synthlisa.NoFilter(),2,          # norm = 1.0, no filtering, Lagrange-2 interpolation
                               Detsystem.EclipticLatitude,
                               Detsystem.EclipticLongitude,0)       # polarization already encoded in hp,hc

tdiDet = synthlisa.TDIsignal(lisa,sourceDet)

samples = int( options.duration / options.timestep + 0.1 )

[tdet,Xdet,Ydet,Zdet] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                              [tdiDet.t,tdiDet.Xm,tdiDet.Ym,tdiDet.Zm],
                                              options.inittime)) 

# computing 
sampling = ts.Cadence

hX = synthlisa.spect(Xinj - Xdet,sampling,0)
# get frequencies, skip DC
fr = hX[1:,0]

om = 2.0 * pi * fr
L  = 16.6782

# instrument noise
Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
Sop = 1.8e-37 * fr**2

Sx = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)

## maximization over the phase will be added later

snX = sqrt(2.0 * numpy.sum(hX[1:,1] / Sx))

print snX

hIx = synthlisa.spect(Xinj,sampling,0)
hDx = synthlisa.spect(Xdet,sampling,0)

normhI = sqrt(2.0 * numpy.sum(hIX[1:,1] / Sx))
normhD = sqrt(2.0 * numpy.sum(hDX[1:,1] / Sx))

olap = 0.5*(normI/normD + normD/normI - snX*snX/(normI*normD) )

print olap

# TO DO:
    #  implement maximization over phase
    #  implement A, E and chack the noise spctra...









