#!/usr/bin/env python

import lisaxml
import sys
import re, math
import numpy

import synthlisa
from math import pi, sin, cos, sqrt

import numpy.oldnumeric as Numeric
import numpy.fft as FFT

from optparse import OptionParser

parser = OptionParser(usage="Usage: %s <synthlisa.xml> <lisacode.xml> <lisasimulator.xml>", 
                      version="06.11.09,  Antoine Petiteau$")


parser.add_option("-o", "--output",
                  type="string", dest="output", default="OutCompareSim.txt",
                  help="Output file name [default OutCompareSim.txt]")                              
                  
parser.add_option("-S", "--simulators",
                  type="string", dest="simulators", default="sl,ls,lc",
                  help="data form which simulators (a comma separated list including sl, ls, and lc) [default: all of them]")

parser.add_option("-x", "--tdidata",
                  type="string", dest="tdidata", default="X,Y,Z",
                  help="tdi data (a comma separated list including X, Y, and Z) [default: all of them]")  
 
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display details [off by default]")

(options, args) = parser.parse_args()

UsedSim = []
if(re.search("sl", options.simulators) != None):
    UsedSim.append('SL') 
if(re.search("lc", options.simulators) != None):
    UsedSim.append('LC')
if(re.search("ls", options.simulators) != None):
    UsedSim.append('LS')


if len(args) < len(UsedSim):
    parser.error("Not enought data files compared to the number of simulator specified.")


tdiGens = []
if(re.search("X", options.tdidata) != None):
    tdiGens.append('X') 
if(re.search("Y", options.tdidata) != None):
    tdiGens.append('Y')
if(re.search("Z", options.tdidata) != None):
    tdiGens.append('Z')



for tdiG in tdiGens:
    print "--> For TDI ", tdiG , ":"
    X = []
    for Sim in xrange(len(args)):
        Datatdifile = lisaxml.readXML(args[Sim])
        tdiData = Datatdifile.getTDIObservables()[0]
        if (Sim == 0):
            stime = tdiData.TimeSeries.Cadence
        else:
            if (stime != tdiData.TimeSeries.Cadence):
                print "All the file should have the same time step."
                sys.exit()
        if ( (UsedSim[Sim] == 'SL') or (UsedSim[Sim] == 'LC') ):
            if (options.verbose):
                if (UsedSim[Sim] == 'SL'):
                    print " --> Read synthlisa data : ", args[Sim], " ..."
                else:
                    print " --> Read lisacode data : ", args[Sim], " ..."
            if(tdiG == 'X'):
                X.append(tdiData.Xf)
            if(tdiG == 'Y'):
                X.append(tdiData.Yf)
            if(tdiG == 'Z'):
                X.append(tdiData.Zf)
        else:
            print " --> Read lisasim data : ", args[Sim], " ..."
            if(tdiG == 'X'):
                X.append(tdiData.Xs)
            if(tdiG == 'Y'):
                X.append(tdiData.Ys)
            if(tdiG == 'Z'):
                X.append(tdiData.Zs)

        
    print " --> time step =" , stime , "s"
    patches = 64

    # use Blackman windowing for spectra

    mywindow = 'blackman'

    # compute spectra, averaging "patches" overlapping segments of time series
    if (options.verbose):
        print " --> Compute spectrums ..."
    sX = []
    for iSim in xrange(len(X)):
        sX.append(synthlisa.spect(X[iSim],stime,patches,win=mywindow))

    if (options.verbose):
        print " --> Normalized spectrums ..."
    f = sX[0][1:,0]
    for iSim in xrange(len(sX)):
        if ( (UsedSim[iSim] == 'SL') or (UsedSim[iSim] == 'LC') ):
            # go to a spectrum of phase (from frequency)
            sX[iSim][1:,1] = sX[iSim][1:,1] / (2 * math.pi * f)**2
        else:
            # go to a spectrum of phase (from strain)
            scale = 1e10 / 299792458.0
            sX[iSim][1:,1] = scale**2 * sX[iSim][1:,1]


    # Record output data
    if(options.output[len(options.output)-4] == '.'):
        outfilename = options.output[0:len(options.output)-4] + '-' + tdiG + options.output[len(options.output)-4:len(options.output)]
    else:
        outfilename = options.output + '-' + tdiG

    if (options.verbose):
        print " --> Record", tdiG, "spectrum in", outfilename, "..."
    
    fout = open(outfilename, 'w')

    rec = "#t"
    for Sim in UsedSim:
        rec = rec + " " + Sim
    rec = rec + '\n'
    fout.write(rec)

    #print f
    #print sX[0][:,1]
    #print sX[1][:,1]
    #print sX[2][:,1]

    for i in xrange(len(f)):
        rec = str(f[i])
        for iSim in xrange(len(sX)):
            rec = rec + " " + str(sX[iSim][i,1])
        rec = rec + '\n'
        fout.write(rec)

    fout.close()

