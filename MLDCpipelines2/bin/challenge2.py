#!/usr/bin/env python

# some definitions...

import sys
import os
import glob
import re

def run(command):
    commandline = command % globals()
    print "--> %s" % commandline

    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

# ---------------------------------------
# STEP 0: parse parameters, set constants
# ---------------------------------------

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME",
                      version="$Id: $")

parser.add_option("-t", "--training",
                  action="store_true", dest="istraining", default=False,
                  help="include source information in output file [off by default]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=31457280.0,
                  help="total time for TDI observables (s) [default 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

# useful (later) to provide make-like capabilities

parser.add_option("-c", "--clean",
                  action="store_true", dest="clean", default=True,
                  help="delete all intermediate files (currently can't be disabled) [on by default]")

(options, args) = parser.parse_args()

if len(args) != 1:
    parser.error("You must specify the challenge name (and only one)!")

if options.seed == None:
    parser.error("You must specify the seed!")

timestep = options.timestep
duration = options.duration

# --------------------------------------------------------------------------------------
# STEP 1: create source XML parameter files, and collect all of them in Source directory
# --------------------------------------------------------------------------------------

# first empty the Source directory
run('rm -f Source/*.xml')

# standard BBH source at 1 Gpc
BBHseed = options.seed + 1
run('bin/makesource-BBH.py --seed=%(BBHseed)s --distance=1e6 --coalescTime=150 Source/BH-1.xml')
BBHseed = options.seed + 2
run('bin/makesource-BBH.py --seed=%(BBHseed)s --distance=2e6 --coalescTime=120 Source/BH-2.xml')

# ---------------------------------------
# STEP 2: create barycentric strain files
# ---------------------------------------

# first empty the Barycentric directory
run('rm -f Barycentric/*.xml Barycentric/*.bin')

# then run makebarycentric over all the files in the Source directory
for xmlfile in glob.glob('Source/*.xml'):
    baryfile = 'Barycentric/' + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
    run('bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')

# --------------------------
# STEP 3: run Synthetic LISA
# --------------------------

# first empty the TDI directory
run('rm -f TDI/*.xml TDI/*.bin')

# then run makeTDI-synthlisa over all the barycentric files in the Barycentric directory
# the results are files of TDI time series that include 
for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
    tdifile = 'TDI/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
    run('bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')

# now run noise generation...
noiseseed = options.seed - 1
noisefile = 'TDI/tdi-frequency-noise.xml'
run('bin/makeTDInoise-synthlisa.py --seed=%(noiseseed)s --duration=%(duration)s --timeStep=%(timestep)s %(noisefile)s')

# -------------------------
# STEP 4: assemble datasets
# -------------------------

import lisaxml
import numpy

# first empty the Dataset directory
run('rm -f Dataset/*.xml Dataset/*.bin')

# improve dataset metadata here

nonoisefile = 'DataSet/' + args[0] + '-frequency-nonoise.xml'
nonoise = lisaxml.lisaXML(nonoisefile,comments='No-noise dataset for challenge 2 (synthlisa version)')
nonoise.close()

withnoisefile = 'DataSet/' + args[0] + '-frequency.xml'
withnoise = lisaxml.lisaXML(withnoisefile,comments='Full dataset for challenge 2 (synthlisa version)')
withnoise.close()

keyfile = 'DataSet/' + args[0] + '-key.xml'
key = lisaxml.lisaXML(keyfile,comments='XML key for challenge 2')
key.close()

if options.istraining:
    run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-frequency.xml')
    run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
else:
    run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-frequency.xml')
    run('bin/mergeXML.py -k %(withnoisefile)s %(nonoisefile)s %(noisefile)s')

run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-frequency.xml')

# exit with success code
sys.exit(0)

# - must add noise and geometry specification
# - must see why withnoise file is empty...
# - must do some instructions