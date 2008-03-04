#!/usr/bin/env python

import lisaxml

import numpy

import sys
import os
import re
import math

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

execdir = os.path.dirname(os.path.abspath(sys.argv[0]))

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id:  $")
                                    
parser.add_option("-S", "--samples",
                  type="float", dest="samples", default=4194304.0,
                  help="total samples for TDI observables [default 4194304 = 2^22]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-P", "--positionNoise",
                  type="float", dest="positionNoise", default=4e-22,
                  help="proof-mass noise level [m^2 Hz^-1, default = 4e-22]")

parser.add_option("-o", "--rescalePositionNoise",
                  action="store_true", dest="rescalePositionNoise", default=False,
                  help="rescale proof-mass noise level by armlength [off by default]")

parser.add_option("-a", "--armlength",
                  type="float", dest="armlength", default=5e9,
                  help="LISA armlength [m, default = 5e9]")

parser.add_option("-l", "--libdir",
                  type="string", dest="libdir", default=None,
                  help="base directory for FFTW and GSL (e.g., /usr for /usr/lib/libfftw.a)")

(options, args) = parser.parse_args()

if len(args) < 1 or '.xml' not in args[0]:
    parser.error("You must specify an .xml output file!")

if options.seed == None:
    parser.error("You must specify a seed!")

if options.libdir == None:
    parser.error("You must specify the location of FFTW and GSL")

outputfile = args[0]
nonoisefile = re.sub('.xml','-nonoise.xml',outputfile)

# make the signal

timestep  = options.timestep
samples   = int(options.samples)
duration  = timestep * samples

armlength = options.armlength
pnoise    = options.positionNoise

libdir = options.libdir

here = os.getcwd()
os.chdir('%s/../../MLDCwaveforms/Galaxy_General' % execdir)

if not os.path.isfile('Setup'):
    run('./Compile --fftw=%(libdir)s --gsl=%(libdir)s')

run('%(execdir)s/../../MLDCwaveforms/Galaxy_General/Setup 0 0 %(armlength)s 9e-30 %(pnoise)s %(timestep)s %(samples)s')
run('./Compile --fftw=%(libdir)s --gsl=%(libdir)s')
os.chdir(here)

seed = options.seed

run('%(execdir)s/makesource-Galaxy3.py --seed=%(seed)s --general TheGalaxy.xml')

run('%(execdir)s/makeTDIsignals-Galaxy3.py TheGalaxy.xml %(nonoisefile)s LisasimGalaxy.xml')

# make the noise

rescale   = options.positionNoise and '-o' or ''
armsecs   = float(options.armlength) / 299792458.0

run('%(execdir)s/makeTDInoise-synthlisa2.py --seed=%(seed)s -T %(duration)s -d %(timestep)s -P %(pnoise)s %(rescale)s -a %(armsecs)s --LISA=Rigid %(outputfile)s')

# merge the noise

run("%(execdir)s/mergeXML.py %(outputfile)s %(nonoisefile)s")

# delete some extra stuff

run('rm TheGalaxy.xml Galaxy_%s.dat Galaxy_Bright_%s.dat' % (seed,seed))
run('rm LisasimGalaxy*')
