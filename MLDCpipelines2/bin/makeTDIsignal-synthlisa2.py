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

# this version supports "immediate" synthlisa sourcefiles

parser = OptionParser(usage="usage: %prog [options] IMMEDIATE.xml/BARYCENTRIC.xml OUTPUT.xml",
                      version="$Id$")

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
                  help="output raw phase measurements (y's and z's) in addition to TDI X, Y, Z")

parser.add_option("-O", "--observables",
                  type="string", dest="observables", default='1.5',
                  help="TDI observables: 1.5 [default], 2.0, Sagnac (experimental; not compatible with -r)")

parser.add_option("-c", "--combinedSNR",
                  action="store_true", dest="combinedSNR", default=False,
                  help="use combined snr = sqrt(2)*max{SNR_x, SNR_y, SNR_z} as SNR constrain [off by default]")

parser.add_option("-L", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

if options.duration < 10:
    options.duration = options.duration * 31457280

# for the moment, support a single input barycentric file

samples = int( options.duration / options.timestep + 0.1 )

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

inputXML = lisaxml.readXML(inputfile)
allsources = inputXML.getLISASources()
inputXML.close()

sourceobjects = []

settime = False

for waveforms in allsources:
    if hasattr(waveforms,'RequestSN') and len(allsources) > 1:
        print "Cannot process multiple sources with RequestSN."
        sys.exit(1)
    
    if hasattr(waveforms,'RequestTimeOffset') and hasattr(waveforms,'RequestDuration'):
        # modify the region where we are making TDI, but not beyond the boundaries
        # of the region requested by command-line arguments

        RequestInitTime = max(waveforms.RequestTimeOffset,options.inittime)
        RequestEndTime  = min(waveforms.RequestTimeOffset + waveforms.RequestDuration,options.inittime + options.duration)

        RequestSamples  = int( (RequestEndTime - RequestInitTime) / options.timestep)
        RequestSamples += (RequestSamples % 2)  # want an even value

        if not settime:
            options.inittime = RequestInitTime
            samples = RequestSamples
            
            settime = True
        else:
            if options.inittime != RequestInitTime or samples != RequestSamples:
                print "Found multiple, incompatible RequestTimeOffset or RequestDuration parameters."
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

# add caching?
if options.LISAmodel == 'Eccentric':
    lisa = synthlisa.EccentricInclined(0.0,1.5*pi,-1)
elif options.LISAmodel == 'Static':
    lisa = synthlisa.OriginalLISA()
elif options.LISAmodel == 'Rigid':
    lisa = synthlisa.CircularRotating(0.0,1.5*pi,-1)
else:
    parser.error("I don't recognize this LISA model!")

if len(sourceobjects) == 0:
    print "!!! Could not find any suitable sources..."
elif len(sourceobjects) == 1:
    tdi = synthlisa.TDIsignal(lisa,sourceobjects[0])
else:
    # print "(proceeding with %s sources)" % len(sourceobjects)
    tdi = synthlisa.TDIsignal(lisa,synthlisa.WaveArray(sourceobjects))

if options.rawMeasurements:
    [t,X,Y,Z,
       y123,y231,y312,y321,y132,y213,
       z123,z231,z312,z321,z132,z213] = numpy.transpose(synthlisa.getobs(samples,options.timestep,
                                                                         [tdi.t,tdi.Xm,tdi.Ym,tdi.Zm,
                                                                                tdi.y123,tdi.y231,tdi.y312,tdi.y321,tdi.y132,tdi.y213,
                                                                                tdi.z123,tdi.z231,tdi.z312,tdi.z321,tdi.z132,tdi.z213],
                                                                          options.inittime,display=options.verbose))
    obsstr = 't,Xf,Yf,Zf'
else:
    # note that choices of options.observables other than 1.5 are not compatible with debugSNR and/or RequestSN
    
    if options.observables == '1.5':
        obsset = [tdi.t,tdi.Xm,tdi.Ym,tdi.Zm]
        obsstr = 't,Xf,Yf,Zf'
    elif options.observables == '2.0':
        obsset = [tdi.t,tdi.X1,tdi.X2,tdi.X3]
        obsstr = 't,X1f,X2f,X3f'
    elif options.observables == 'Sagnac':
        obsset = [tdi.alpham,tdi.betam,tdi.gammam,tdi.zetam]
        obsstr = 'alphaf,betaf,gammaf,zetaf'
    else:
        parser.error("I don't recognize the set of TDI observables!")

    [t,X,Y,Z] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,obsset,options.inittime))

# Computing SNR....

if options.debugSNR or hasattr(allsources[0],'RequestSN'):
    # this is |\tilde{h}|^2 * dt/N
    sampling = ts.Cadence
    
    hX = synthlisa.spect(X,sampling,0)
    hY = synthlisa.spect(Y,sampling,0)
    hZ = synthlisa.spect(Z,sampling,0)
    
    # get frequencies, skip DC
    fr = hX[1:,0]
    
    om = 2.0 * pi * fr
    L  = 16.6782
    
    # instrument noise (was 2.5, 1.8)
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
    
    if options.noiseOnly:
        Sn = Sx
    else:
        Sn = Sx + Sgal
    
    # the (S/N)^2 is given by 2T times the integrated ratio
    # of the spectral densities (the factor of 2 because the spectral
    # density is one-sided); notice however that the df is 1/T,
    # so we need only to sum up the array containing the ratio,
    # and multiply by two
    
    # clipping: check (roughly) if first null is within the computed range
    highf = 0.01
    highi = int(highf / fr[0])
    if options.combinedSNR and fr[-1] > highf and highi > 1:
        # then find the minimum of the noise curve up to highf and clip to that minimum value
        minSn = numpy.min(Sn[1:highi])
        Sn = numpy.maximum(Sn,minSn)
        print "Clipping noise PSD to %s" % minSn
    
    snX = sqrt(2.0 * numpy.sum(hX[1:,1] / Sn))
    snY = sqrt(2.0 * numpy.sum(hY[1:,1] / Sn))
    snZ = sqrt(2.0 * numpy.sum(hZ[1:,1] / Sn))
    
    SNR = max(snX,snY,snZ)
    
    if options.combinedSNR:
        SNR = SNR*sqrt(2.0) 
        print "makeTDIsignal-synthlisa.py: combined SNR [sqrt(2) * max(X,Y,Z)] = %s" % SNR
    else:
        print "makeTDIsignal-synthlisa.py: maximum X,Y,Z SNR = %s" % SNR

    if options.debugSNR:
        print "makeTDIsignal-synthlisa.py: SNR X, Y, Z = %s, %s, %s" % (snX,snY,snZ)
    
        Sxy = -4.0 * numpy.sin(2.0*om*L)*numpy.sin(om*L) * ( Sop + 4.*Spm )
 
        if options.noiseOnly:
            Sa = 2.0 * (Sx - Sxy)/3.0
        else:
            Sxy_gal = -0.5*Sgal
            Sa = 2.0 * (Sx + Sgal - Sxy - Sxy_gal)/3.0
    
        hA = synthlisa.spect((2*X - Y - Z)/3.0,sampling,0)
        hE = synthlisa.spect((Z - Y)/sqrt(3.0),sampling,0)
    
        snA = sqrt(2.0 * numpy.sum(hA[1:,1] / Sa))
        snE = sqrt(2.0 * numpy.sum(hE[1:,1] / Sa))
    
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
    
    if options.rawMeasurements:
        y123 *= factor; y231 *= factor; y312 *= factor; y321 *= factor; y132 *= factor; y213 *= factor
        z123 *= factor; z231 *= factor; z312 *= factor; z321 *= factor; z132 *= factor; z213 *= factor

tdiobs = lisaxml.Observable(obsstr)
tdiobs.TimeSeries = lisaxml.TimeSeries([t,X,Y,Z],obsstr)

tdiobs.DataType = 'FractionalFrequency'
tdiobs.TimeSeries.Cadence = options.timestep
tdiobs.TimeSeries.TimeOffset = options.inittime

if options.rawMeasurements:
    tdiobsraw = lisaxml.Observable('t,y123f,y231f,y312f,y321f,y132f,y213f,z123f,z231f,z312f,z321f,z132f,z213f')     
    tdiobsraw.TimeSeries = lisaxml.TimeSeries([t,y123,y231,y312,y321,y132,y213,
                                                 z123,z231,z312,z321,z132,z213],'t,y123f,y231f,y312f,y321f,y132f,y213f,z123f,z231f,z312f,z321f,z132f,z213f')

    tdiobsraw.DataType = 'FractionalFrequency'
    tdiobsraw.TimeSeries.Cadence = options.timestep
    tdiobsraw.TimeSeries.TimeOffset = options.inittime

outputXML = lisaxml.lisaXML(outputfile)

# save the standard LISA...

lisa = lisaxml.LISA('Standard MLDC PseudoLISA')
lisa.TimeOffset      = 0; lisa.TimeOffset_Unit      = 'Second'
lisa.InitialPosition = 0; lisa.InitialPosition_Unit = 'Radian'
lisa.InitialRotation = 0; lisa.InitialRotation_Unit = 'Radian'
lisa.Armlength = 16.6782; lisa.Armlength_Unit       = 'Second'
lisa.OrbitApproximation = options.LISAmodel; lisa.OrbitApproximation_Unit = 'String'

outputXML.LISAData(lisa)

for source in allsources:
    if hasattr(source,'TimeSeries'):
        del source.TimeSeries

    outputXML.SourceData(source)

# output the TDI observables
outputXML.TDIData(tdiobs)

if 'tdiobsraw' in globals():
    outputXML.TDIData(tdiobsraw)

outputXML.close()

sys.exit(0)
