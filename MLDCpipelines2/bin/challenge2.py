#!/usr/bin/env python

__version__='$Id$'

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

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME...]",
                      version="$Id$")

parser.add_option("-t", "--training",
                  action="store_true", dest="istraining", default=False,
                  help="include source information in output file [off by default]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560.0,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name!")

challengename = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

seed = options.seed
timestep = options.timestep
duration = options.duration

# --------------------------------------------------------------------------------------
# STEP 1: create source XML parameter files, and collect all of them in Source directory
# --------------------------------------------------------------------------------------

# first empty the Source and Galaxy directories
run('rm -f Source/*.xml')
run('rm -f Galaxy/*.xml')

# to run CHALLENGE, a file source-parameter generation script CHALLENGE.py must sit in bin/
# it must take a single argument (the seed) and put its results in the Source subdirectory

sourcescript = 'bin/' + challengename + '.py'
if not os.path.isfile(sourcescript):
    parser.error("I need the challenge script %s!" % sourcescript)
else:
    run('python %s %s' % (sourcescript,seed))

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
run('rm -f TDI/*.xml TDI/*.bin TDI/Binary')

# then run makeTDI-synthlisa over all the barycentric files in the Barycentric directory
# the results are files of TDI time series that include 
for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
    tdifile = 'TDI/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
    run('bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')

# now run noise generation...
noiseseed = options.seed
noisefile = 'TDI/tdi-frequency-noise.xml'
run('bin/makeTDInoise-synthlisa.py --seed=%(noiseseed)s --duration=%(duration)s --timeStep=%(timestep)s %(noisefile)s')

# --------------------------
# STEP 4: run LISA Simulator
# --------------------------

# -----------------------
# STEP 5: run Fast Galaxy
# -----------------------

# hmm... are we telling everybody the seed with the filename of the galaxy?

galaxyxml = glob.glob('Galaxy/*.xml')

if galaxyxml:
    run('bin/makeTDIsignals-Galaxy.py %s' % seed)

# -------------------------
# STEP 6: assemble datasets
# -------------------------

import lisaxml
import numpy

# first empty the Dataset directory
run('rm -f Dataset/*.xml Dataset/*.bin')

# improve dataset metadata here

nonoisefile = 'Dataset/' + challengename + '-frequency-nonoise.xml'
nonoise = lisaxml.lisaXML(nonoisefile,comments='No-noise dataset for challenge 2 (synthlisa version)')
nonoise.close()

withnoisefile = 'Dataset/' + challengename + '-frequency.xml'
withnoise = lisaxml.lisaXML(withnoisefile,comments='Full dataset for challenge 2 (synthlisa version)')
withnoise.close()

keyfile = 'Dataset/' + challengename + '-key.xml'
key = lisaxml.lisaXML(keyfile,comments='XML key for challenge 2')
key.close()

if options.istraining:
    run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-frequency.xml')
    run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
else:
    run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-frequency.xml')
    run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(noisefile)s')

run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-frequency.xml')

# exit with success code
sys.exit(0)

# TO DO:
# - must add noise and geometry specification (do it with files in Template)
# - must do some instructions
# - must do "real" galaxy...
# - must do SNR