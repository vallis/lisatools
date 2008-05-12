#!/usr/bin/env python

__version__='$Id:  $'

import lisaxml

import numpy

import math
import sys
import re
import wave

from optparse import OptionParser


def triang(windowsize,symmetric = 1):
	halfkernel = numpy.arange(windowsize) / (1.0 * windowsize)	
	
	kernel = numpy.zeros(2 * windowsize + 1)
	
	kernel[0:windowsize] = halfkernel
	kernel[windowsize]   = 1.0
	kernel[-windowsize:] = halfkernel[::-1]

	if symmetric:
		return kernel
	else:
		return kernel[:-1]


def normalize(sound,cadence,timewindow = 0.1):
	length = len(sound)
	
	windowsize = int(timewindow * cadence)
	
	# chop to a multiple of windowsize
	
	newlength = windowsize * int(length/windowsize)
	newsound  = numpy.zeros(newlength,'d')
	
	normlength = int(length/windowsize)

	norms = numpy.zeros(normlength+1,'d')
	norms[0] = norms[-1] = 0.0

	kernel = triang(windowsize,0)

	# normalize

	for i in range(1,normlength):
		ir = (i-1)*windowsize
		er = (i+1)*windowsize

		norm = 4.0 * numpy.sum(numpy.abs(sound[ir:er])) / (2 * windowsize)

		if norm != 0.0:
			newsound[ir:er] += kernel * sound[ir:er] / norm
	
	# clip
	
	newsound[newsound >  1.0] =  1.0
	newsound[newsound < -1.0] = -1.0
	
	# renormalize
	
	newsound[:] /= numpy.max(numpy.abs(newsound[:]))
	
	return newsound


# begin parsing arguments

parser = OptionParser(usage="usage: %prog [options] TDIFILE.xml WAVEFILE.wav ...",
                      version="$Id:  $")

(options, args) = parser.parse_args()

if len(args) < 1 or len(args) > 2:
    parser.error("You must specify one input file (XML), or an input file (XML) and an output file (WAV)!")

inputfile = args[0]

if len(args) == 2:
	outputfile = args[1]
else:
	outputfile = re.sub('\.xml','.wav',inputfile)

inputxml = lisaxml.readXML(inputfile)
tdi = inputxml.getTDIObservables()[0]
inputxml.close()

DataType = tdi.DataType
Length   = tdi.TimeSeries.Length
Cadence  = tdi.TimeSeries.Cadence

if tdi.DataType == 'FractionalFrequency':
	A = (2.0*tdi.Xf - tdi.Yf - tdi.Zf) / 3.0
	E = (tdi.Zf - tdi.Yf) / math.sqrt(3.0)
elif tdi.DataType == 'Strain':
	A = (2.0*tdi.Xs - tdi.Ys - tdi.Zs) / 3.0
	E = (tdi.Zs - tdi.Ys) / math.sqrt(3.0)
else:
    raise

# standard frame rates: 44100, 22050, or 11025

# 2**21 samples (1 year @ 15 s) @ 44.1 kHz maps 1 year to 47 s
# 1e-4 Hz -> 66 Hz, 1e-2 -> 6615 Hz (probably OK)

wavefreq = 44100

print "Mapping 1e-4 -> %s, 1e-1 -> %s Hz" % (wavefreq*1.0e-4*Cadence,wavefreq*1.0e-1*Cadence)

windowsize = int(0.1 * wavefreq)

# normalize both channels independently

A = normalize(A,wavefreq)
E = normalize(E,wavefreq)

# create integer buffer and fill it

intbuffer = numpy.empty(2*len(A),numpy.short)

intbuffer[::2]  = 32768.0 * A[:] 
intbuffer[1::2] = 32768.0 * E[:]

outputwave = wave.open(outputfile,'w')

outputwave.setnchannels(2)
outputwave.setsampwidth(2)
outputwave.setnframes(Length)

outputwave.setframerate(wavefreq)

outputwave.writeframes(intbuffer.tostring())

outputwave.close()

sys.exit(0) 
