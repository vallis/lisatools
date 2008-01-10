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

parser = OptionParser(usage="usage: %prog [options] BARYCENTRIC.xml OUTPUT.xml",
                      version="$Id: $")

parser.add_option("-d", "--lisasimDir",
                  type="string", dest="lisasimdir", default=None,
                  help="location of the LISA Simulator executable for the correct signal length [will try to guess]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560,
                  help="total time for TDI observables (s), will be overridden by LISA Simulator installation if given [default 62914560 = 2^22 * 15]")

(options, args) = parser.parse_args()

# try to find the LISA Simulator...

if options.lisasimdir == None:
    try:
        import lisasimulator
    except:
        parser.error("You must provide the location of the LISA Simulator!")
    
    if options.duration == 31457280:
        lisasimdir = lisasimulator.lisasim1yr
    elif options.duration == 62914560:
        lisasimdir = lisasimulator.lisasim2yr
    else:
        parser.error("I can only run the LISA Simulator for one or two years (2^21 or 2^22 s)!")
    
    if not os.path.isdir(lisasimdir):
        parser.error("You must provide the location of the LISA Simulator!")
else:
    lisasimdir = os.path.abspath(options.lisasimdir)

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

if (inputfile[-4:] != '.xml') or (outputfile[-4:] != '.xml'):
    parser.error("Sorry to be pedantic, but the files should end in .xml!")

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

# run LISA simulator to create source

xmlfile = os.path.abspath(inputfile)
binfile = re.sub('.xml','-0.bin',xmlfile)

# xmlname = re.sub('\-barycentric.xml','',os.path.basename(inputfile))
tdifile = os.path.abspath(outputfile)

if '-barycentric.xml' in xmlfile:
    xmlname = re.sub('\-barycentric.xml','',os.path.basename(xmlfile))
else:
    xmlname = re.sub('.xml','',os.path.basename(xmlfile))

xmldest = workdir + '/XML/' + xmlname + '_Barycenter.xml'
bindest = workdir + '/XML/' + os.path.basename(binfile)

run('cp %s %s' % (xmlfile,xmldest),quiet=True)
run('cp %s %s' % (binfile,bindest),quiet=True)

os.chdir(workdir)

# run LISA simulator...

run('./GWconverter %s' % xmlname)
run('./Vertex1Signals')
run('./Vertex2Signals')
run('./Vertex3Signals')

run('echo "%s" > sources.txt' % xmlname)
run('./Package %s sources.txt 0' % xmlname)

outxml = 'XML/' + xmlname + '.xml'
outbin = 'XML/' + xmlname + '.bin'

# create an empty XML file...
sourcefileobj = lisaxml.lisaXML(tdifile)
sourcefileobj.close()

# then add the TDI data from the LISA Simulator output
# (this will need changing when running challenge2.py in a workdir)
run('%s/mergeXML.py %s %s %s' % (execdir,tdifile,execdir + '/../Template/StandardLISA.xml',outxml),quiet=True)

# then add the modified Source data included in the Barycentric file
# (this will need changing when running challenge2.py in a workdir)
run('%s/mergeXML.py -k %s %s' % (execdir,tdifile,xmlfile),quiet=True)

os.chdir(here)

# ...remove the tmp dir

run('rm -rf %s' % workdir,quiet=True)

sys.exit(0)
