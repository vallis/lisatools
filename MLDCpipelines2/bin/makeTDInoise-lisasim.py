#!/usr/bin/env python

__version__='$Id: $'

import sys
import glob
import os
import tempfile
import re
from distutils.dep_util import newer

import lisaxml

def run(command,quiet = False):
    commandline = command % globals()
    
    try:
        if not quiet:
            print "----> %s" % commandline
            assert(os.system(commandline) == 0)
        else:
            assert(os.system(commandline + ' > /dev/null') == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

execdir = os.path.dirname(os.path.abspath(sys.argv[0]))

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: $")

parser.add_option("-n", "--seedNoise",
                  type="int", dest="seednoise", default=None,
                  help="noise-specific seed (int) [required]")

parser.add_option("-D", "--lisasimDir",
                  type="string", dest="lisasimdir", default=None,
                  help="location of the LISA Simulator executable for the correct signal length [will try to guess]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560,
                  help="total time for TDI observables (s), will be overridden by LISA Simulator installation if given [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s), will be overridden by LISA Simulator installation if given [default 15]")

(options, args) = parser.parse_args()

# try to find the LISA Simulator...

if options.seednoise == None:
    parser.error("You must give the noise seed!")

seednoise = options.seednoise

if options.lisasimdir == None:
    try:
        import lisasimulator
    except:
        parser.error("You must provide the location of the LISA Simulator!")
    
    if options.duration == 31457280:
        lisasimdir = lisasimulator.lisasim1yr
    elif options.duration == 62914560 and options.timestep == 15.0:
        lisasimdir = lisasimulator.lisasim2yr
    elif options.duration == 62914560 and options.timestep == 1.875:
        lisasimdir = lisasimulator.lisasim2yrhigh
    else:
        parser.error("I can only run the LISA Simulator for one year at 15 s, or two years at 15 or 1.875 s!")
    
    if not os.path.isdir(lisasimdir):
        parser.error("You must provide the location of the LISA Simulator!")
else:
    lisasimdir = os.path.abspath(options.lisasimdir)

if len(args) != 1:
    parser.error("Beyond the options, you must specify only the output file!")

outputfile = args[0]

# --- make a virtual LISAsim environment

here = os.getcwd()

workdir = os.path.abspath(tempfile.mkdtemp(dir='.'))
os.chdir(workdir)

execfiles = ['GWconverter','InstrumentNoise','Noise_Maker','Package',
             'Vertex1Noise','Vertex1Signals','Vertex1Times',
             'Vertex2Noise','Vertex2Signals','Vertex2Times',
			 'Vertex3Noise','Vertex3Signals','Vertex3Times']

for f in execfiles:
    run('ln -s %s/%s ./%s' % (lisasimdir,f,f),quiet=True)

run('mkdir Binary',quiet=True)
run('mkdir Data',quiet=True)
run('mkdir XML',quiet=True)

binfiles = ( glob.glob('%s/Binary/Vert[1-3]_*.dat' % lisasimdir) +
             glob.glob('%s/Binary/UnVc[XYZ]*.dat' % lisasimdir) +
             ['%s/Binary/Setup.dat' % lisasimdir] )

for b in binfiles:
    run('ln -s %s Binary/%s' % (b,os.path.basename(b)),quiet=True)

os.chdir(here)
tdifile = os.path.abspath(outputfile)

# make noise!

os.chdir(workdir)

run('./Noise_Maker %(seednoise)s')

run('rm -f sources.txt; touch sources.txt',quiet=True)
run('./Package noise-only sources.txt %(seednoise)s')
outxml = os.path.abspath('XML/noise-only.xml')

# remove temporary files
run('rm -f Binary/AccNoise*.dat Binary/ShotNoise*.dat Binary/XNoise*.bin Binary/YNoise*.bin Binary/ZNoise*.bin',quiet=True)
run('rm -f Binary/M1Noise*.bin Binary/M2Noise*.bin Binary/M3Noise*.bin',quiet=True)

noisefileobj = lisaxml.lisaXML(tdifile)
noisefileobj.close()
run('%s/mergeXML.py %s %s %s' % (execdir,tdifile,execdir + '/../Template/StandardLISA.xml',outxml),quiet=True)

run('rm -f XML/noise-only.xml',quiet=True)
run('rm -f XML/noise-only.bin',quiet=True)
run('rm -f Data/*.txt',quiet=True)

os.chdir(here)

# ...remove the tmp dir

run('rm -rf %s' % workdir,quiet=True)

sys.exit(0)
