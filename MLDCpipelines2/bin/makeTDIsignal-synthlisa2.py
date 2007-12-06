#!/usr/bin/env python

__version__='$Id: makeTDIsignal-synthlisa.py 368 2007-04-25 01:44:03Z vallisneri $'

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

# this version supports "immediate" synthlisa sourcefiles

parser = OptionParser(usage="usage: %prog [options] IMMEDIATE.xml/BARYCENTRIC.xml OUTPUT.xml",
                      version="$Id: makeTDIsignal-synthlisa.py 368 2007-04-25 01:44:03Z vallisneri $")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for TDI observables (s) [default 0.0]")
                                    
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560.0,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-n", "--noiseOnly",
                  action="store_true", dest="noiseOnly", default=False,
                  help="compute SNR using instrumental noise only [off by default]")

parser.add_option("-D", "--debugSNR",
                  action="store_true", dest="debugSNR", default=False,
                  help="show more SNR data [off by default]")

parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) instead of TDI X, Y, Z")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# for the moment, support a single input barycentric file

samples = int( options.duration / options.timestep + 0.1 )

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

if options.debugSNR and options.rawMeasurements:
    parser.error('Options --debugSNR and --rawMeasurements are incompatible!')

(inputfile,outputfile) = args

inputXML = lisaxml.readXML(inputfile)
allsources = inputXML.getLISASources()
inputXML.close()

sourceobjects = []

for waveforms in allsources:
    if hasattr(waveforms,'RequestSN') and len(waveforms) > 1:
        print "Cannot process multiple sources with RequestSN field."
        sys.exit(1)
        
    if hasattr(waveforms,'RequestSN') and options.rawMeasurements:
        print "Currently the RequestSN source parameter and the --rawMeasurements option are incompatible."
        sys.exit(1)
    
    if hasattr(waveforms,'TimeSeries'):
        ts = waveforms.TimeSeries

        try:
            assert ts.Cadence_Unit == 'Second'
            assert ts.TimeOffset_Unit == 'Second'

            assert waveforms.EclipticLatitude_Unit == 'Radian'
            assert waveforms.EclipticLongitude_Unit == 'Radian'
        except:
            # improve debugging here
            print "Script %s cannot understand units used for source parameters!" % sys.argv[0]
            sys.exit(1)

        hp = ts.Arrays[0].copy()
        hc = ts.Arrays[1].copy()
    
        source = synthlisa.SampledWave(hp,hc,
                                       ts.Length,ts.Cadence,-ts.TimeOffset, # synthlisa prebuffer is -TimeOffset
                                       1.0,synthlisa.NoFilter(),2,          # norm = 1.0, no filtering, Lagrange-2 interpolation
                                       waveforms.EclipticLatitude,
                                       waveforms.EclipticLongitude,0)       # polarization already encoded in hp,hc
    elif hasattr(waveforms,'synthesize'):
        # careful with the inittime here... for sure it works when inittime = 0, but in other situations?
        # it's the usual synthlisa problem with buffers and prebuffers (really offsets)
        source = waveforms.synthesize(samples,options.timestep,options.inittime - 900.0)
    else:
        print "!!! Cannot find hp/hc table or synthesize source %s" % source.name
        sys.exit(1)

    sourceobjects.append(source)

lisa = synthlisa.CacheLISA(synthlisa.EccentricInclined(0.0,1.5*pi,-1))

if len(sourceobjects) == 0:
    print "!!! Could not find any suitable sources..."
elif len(sourceobjects) == 1:
    tdi = synthlisa.TDIsignal(lisa,sourceobjects[0])
else:
    # print "(proceeding with %s sources)" % len(sourceobjects)
    tdi = synthlisa.TDIsignal(lisa,synthlisa.WaveArray(sourceobjects))

if options.rawMeasurements:
    [t,y123,y231,y312,y321,y132,y213,
       z123,z231,z312,z321,z132,z213] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                                                          [tdi.t,tdi.y123,tdi.y231,tdi.y312,tdi.y321,tdi.y132,tdi.y213,
                                                                                 tdi.z123,tdi.z231,tdi.z312,tdi.z321,tdi.z132,tdi.z213],
                                                                          options.inittime))    
else:
    [t,X,Y,Z] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                                  [tdi.t,tdi.Xm,tdi.Ym,tdi.Zm],
                                                  options.inittime))

# Computing SNR....

if options.debugSNR or hasattr(allsources[0],'RequestSN'):
    # this is |\tilde{h}|^2 * dt/N
    sampling = ts.Cadence
    
    hX = synthlisa.spect(X,sampling,0)
    hY = synthlisa.spect(Y,sampling,0)
    hZ = synthlisa.spect(Z,sampling,0)
    
    minf = hX[1,0]
    lowi = int(1e-5 / minf)
    higi = int(9e-3 / minf)
    
    # get frequencies, skip DC
    fr = hX[lowi:higi,0]
    
    om = 2.0 * pi * fr
    L  = 16.6782
    
    # instrument noise (was 2.5, 1.8)
    Spm = 2.54e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
    Sop = 1.76e-37 * fr**2
    
    Sx = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)
    
    # expression from Phys. Rev. D 73, 122001 (2006), corrected and modified to include TDI response function
    # for fractional frequency fluctuations
    
    # conversion LS to SL (incl.deriv.) -- TDI response function Michelson to X,Y,Z
    Sgal = (2.0 * L)**2 * (2*pi*fr)**2 * 4.0 * numpy.sin(om*L)**2 * (
             numpy.piecewise(fr,(fr >= 1.0e-4  ) & (fr < 1.0e-3  ),[lambda f: 10**-44.62 * f**-2.3, 0]) + \
             numpy.piecewise(fr,(fr >= 1.0e-3  ) & (fr < 10**-2.7),[lambda f: 10**-50.92 * f**-4.4, 0]) + \
             numpy.piecewise(fr,(fr >= 10**-2.7) & (fr < 10**-2.4),[lambda f: 10**-62.8  * f**-8.8, 0]) + \
             numpy.piecewise(fr,(fr >= 10**-2.4) & (fr < 10**-2.0),[lambda f: 10**-89.68 * f**-20.0,0])     )
    
    if options.noiseOnly:
        Sn = Sx
    else:
        Sn = Sx + Sgal
    
    # the (S/N)^2 is given by 2T times the integrated ratio
    # of the spectral densities (the factor of 2 because the spectral
    # density is one-sided); notice however that the df is 1/T,
    # so we need only to sum up the array containing the ratio,
    # and multiply by two
    
    snX = sqrt(2.0 * numpy.sum(hX[lowi:higi,1] / Sn))
    snY = sqrt(2.0 * numpy.sum(hY[lowi:higi,1] / Sn))
    snZ = sqrt(2.0 * numpy.sum(hZ[lowi:higi,1] / Sn))
    
    SNR = max(snX,snY,snZ)
    
    print "makeTDIsignal-synthlisa.py: maximum X,Y,Z SNR = %s" % SNR

    if options.debugSNR:
        print "makeTDIsignal-synthlisa.py: SNR X, Y, Z = %s, %s, %s" % (snX,snY,snZ)
    
        Sxy = -4.0 * numpy.sin(2.0*om*L)*numpy.sin(om*L) * ( Sop + 4.*Spm )
        Sa = 2.0 * (Sx - Sxy)/3.0
    
        hA = synthlisa.spect((2*X - Y - Z)/3.0,sampling,0)
        hE = synthlisa.spect((Z - Y)/sqrt(3.0),sampling,0)
    
        snA = sqrt(2.0 * numpy.sum(hA[lowi:higi,1] / Sa))
        snE = sqrt(2.0 * numpy.sum(hE[lowi:higi,1] / Sa))
    
        print "makeTDIsignal-synthlisa.py: SNR A, E, total = %s, %s, %s" % (snA,snE,sqrt(snA**2 + snE**2))
    
    # note:
    # A = (2X - Y - Z) / 3
    # E = (Z - Y) / sqrt(3)

if hasattr(allsources[0],'RequestSN'):
    waveforms = allsources[0]
    
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

# TODO need to fix also mergeXML

if options.rawMeasurements:
    tdiobs = lisaxml.Observable('t,y123f,y231f,y312f,y321f,y132f,y213f,z123f,z231f,z312f,z321f,z132f,z213f')     
    tdiobs.TimeSeries = lisaxml.TimeSeries([t,y123,y231,y312,y321,y132,y213,
                                              z123,z231,z312,z321,z132,z213],'t,y123f,y231f,y312f,y321f,y132f,y213f,z123f,z231f,z312f,z321f,z132f,z213f')
else:
    tdiobs = lisaxml.Observable('t,Xf,Yf,Zf')
    tdiobs.TimeSeries = lisaxml.TimeSeries([t,X,Y,Z],'t,Xf,Yf,Zf')


tdiobs.DataType = 'FractionalFrequency'
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

for source in allsources:
    if hasattr(source,'TimeSeries'):
        del source.TimeSeries

    outputXML.SourceData(source)

# output the TDI observables
outputXML.TDIData(tdiobs)

outputXML.close()

sys.exit(0)
