#!/usr/bin/env python

__version__='$Id: $'

# keep it clean!

import lisaxml
import Stochastic
import healpix

import sys
import getopt
import math
import random
import re

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: $")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-p", "--PSD",
                  type="float", dest="PSD", default=1e-47,
                  help="total power spectral density (strain, adimensional) [default = 5e-48]")

parser.add_option("-r", "--randomizePSD",
                  type="float", dest="randomizePSD", default=0.0,
                  help="fractional random displacement in total power spectral density (as a fraction of PSD) [default = 0]")

parser.add_option("-R", "--pixelRefinement",
                  type="int", dest="refine", default=2,
                  help="healpix refinement level")

parser.add_option("-n", "--sourceName",
                  type="string", dest="sourceName", default="GW stochastic background",
                  help='name of source [defaults to "GW stochastic background"]')

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="divide sources between nproc files to run on parallel CPUs [default 1]")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 1:
    parser.error("You must specify the output file!")
    
if options.seed == None:
    parser.error("You must specify the seed!")
    
if options.PSD == None:
    parser.error("You must specify the PSD!")

if len(args) < 1:
    parser.error("I need the output file!")

# get the name of the output file

if options.nproc == 1:
    outputfilenames = [args[0]]
else:
    outputfilenames = [re.sub('.xml','',args[0]) + ('-%s.xml' % str(i)) for i in range(options.nproc)]

outputfiles = [lisaxml.lisaXML(filename,author='M. Vallisneri') for filename in outputfilenames]

npixels = healpix.nside2npix(2**options.refine)

random.seed(options.seed)

PSD = options.PSD * (1.0 + random.uniform(-options.randomizePSD,options.randomizePSD))

whichfile = 0

for i in range(npixels):
    mysystem = Stochastic.Stochastic(options.sourceName + ' pseudosource #%s' % i)

    mysystem.EclipticLatitude, mysystem.EclipticLongitude = healpix.pix2ang_ring(2**options.refine,i)
    mysystem.Polarization = 0

    # Check normalization: divide by the number of pixels, or by solid angle / (4 pi)?
    # The 2 in the denominator is for the two polarizations
    
    mysystem.PowerSpectralDensity = PSD / (2*npixels)
    mysystem.SpectralSlope = -3
    mysystem.Flow  = 1e-5
    mysystem.Fknee = 1e-2

    mysystem.PseudoRandomSeed = random.randint(0,2**30)

    mysystem.InterpolationOrder = 2
    
    if options.verbose:
        for p in mysystem.parameters:
            print p, ':', mysystem.parstr(p)
    
    outputfiles[whichfile].SourceData(mysystem)
    
    whichfile = (whichfile + 1) % options.nproc

for outputXML in outputfiles:
    outputXML.close()
