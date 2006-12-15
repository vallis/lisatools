#!/usr/bin/env python

__version__='$Id$'

# some definitions...

import sys
import os
import glob
import re
from distutils.dep_util import newer, newer_group

import lisaxml
import numpy

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

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME...",
                      version="$Id$")

parser.add_option("-t", "--training",
                  action="store_true", dest="istraining", default=False,
                  help="include source information in output file [off by default]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560.0,
#                  type="float", dest="duration", default=3932160.0,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-m", "--make",
                  action="store_true", dest="makemode", default=False,
                  help="run in make mode (use already-generated source key files and intermediate products)")

parser.add_option("-S", "--synthlisa",
                  action="store_true", dest="synthlisaonly", default=False,
                  help="run only Synthetic LISA")

parser.add_option("-L", "--lisasim",
                action="store_true", dest="lisasimonly", default=False,
                help="run only the LISA Simulator")

# add options to disable the LISA Simulator or Synthetic LISA

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name!")

challengename = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

if options.synthlisaonly:
    lisasimdir = None
else:
    import lisasimulator

    # if options.duration == 31457280:
    if options.duration == 3932160:
        lisasimdir = lisasimulator.lisasim1yr
    elif options.duration == 62914560:
        lisasimdir = lisasimulator.lisasim2yr
    else:
        parser.error("I can only run the LISA Simulator for one or two years (2^21 or 2^22 s)!")

if options.lisasimonly:
    dosynthlisa = False
else:
    dosynthlisa = True

seed = options.seed
timestep = options.timestep
duration = options.duration

# --------------------------------------------------------------------------------------
# STEP 1: create source XML parameter files, and collect all of them in Source directory
# --------------------------------------------------------------------------------------

# first empty the Source and Galaxy directories
if (not makemode):
    run('rm -f Source/*.xml')
    run('rm -f Galaxy/*.xml')

    # to run CHALLENGE, a file source-parameter generation script CHALLENGE.py must sit in bin/
    # it must take a single argument (the seed) and put its results in the Source subdirectory
    # if makesource-Galaxy.py is called, it must be with the UNALTERED seed, which is used
    # later to call makeTDIsignals-Galaxy.py

    sourcescript = 'bin/' + challengename + '.py'
    if not os.path.isfile(sourcescript):
        parser.error("I need the challenge script %s!" % sourcescript)
    else:
        run('python %s %s' % (sourcescript,seed))

# ---------------------------------------
# STEP 2: create barycentric strain files
# ---------------------------------------

# first empty the Barycentric directory
if (not makemode):
    run('rm -f Barycentric/*.xml Barycentric/*.bin')

# then run makebarycentric over all the files in the Source directory
for xmlfile in glob.glob('Source/*.xml'):
    baryfile = 'Barycentric/' + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
    if (not makemode) or newer(xmlfile,baryfile):
        run('bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')

# check if any of the source files requests an SN; in this case we'll need to run synthlisa!

if os.system('grep -q RequestSN Source/*.xml') == 0 and dosynthlisa == False:
    parser.error("Ah, at least one of your Source files has a RequestSN field; then I MUST run synthlisa to adjust SNRs.")

# --------------------------
# STEP 3: run Synthetic LISA
# --------------------------

# first empty the TDI directory
if (not makemode):
    run('rm -f TDI/*.xml TDI/*.bin TDI/Binary')

if dosynthlisa:
    # then run makeTDI-synthlisa over all the barycentric files in the Barycentric directory
    # the results are files of TDI time series that include the source info
    # these calls have also the result of rescaling the barycentric files to satisfy any RequestSN that
    # they may carry
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        tdifile = 'TDI/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
        if (not makemode) or newer(xmlfile,tdifile):
            run('bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')

    # now run noise generation...
    noiseseed = options.seed
    noisefile = 'TDI/tdi-frequency-noise.xml'
    # note that we're not checking whether the seed is the correct one
    if (not makemode) or (not os.path.isfile(noisefile)):
        run('bin/makeTDInoise-synthlisa.py --seed=%(noiseseed)s --duration=%(duration)s --timeStep=%(timestep)s %(noisefile)s')

# --------------------------
# STEP 4: run LISA Simulator
# --------------------------

# some of this may later be moved inside makeTDInoise-lisasim.py and makeTDIsignal-lisasim.py

if lisasimdir:
    here = os.getcwd()
    
    # signal generation
    
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        xmlname = re.sub('\-barycentric.xml','',os.path.basename(xmlfile))

        tdifile = 'TDI/' + xmlname + '-tdi-strain.xml'
        # this is not perfect; it would be better to read the binary filename from the XML...
        binfile = 'Barycentric/' + xmlname + '-barycentric-0.bin'

        if (not makemode) or newer(xmlfile,tdifile):
            xmldest = lisasimdir + '/XML/' + xmlname + '_Barycenter.xml'
            bindest = lisasimdir + '/XML/' + xmlname + '-barycentric-0.bin'

            run('cp %s %s' % (xmlfile,xmldest))
            run('cp %s %s' % (binfile,bindest))

            os.chdir(lisasimdir)

            # run LISA simulator...

            run('./GWconverter %s' % xmlname)
            run('./Vertex1Signals')
            run('./Vertex2Signals')
            run('./Vertex3Signals')

            run('echo "%s" > sources.txt' % xmlname)
            run('./Package %s sources.txt 0' % xmlname)

            outxml = 'XML/' + xmlname + '.xml'
            outbin = 'Binary/' + xmlname + '.bin'

            # create an empty XML file...
            sourcefileobj = lisaxml.lisaXML(here + '/' + tdifile)
            sourcefileobj.close()
            # then add the TDI data from the LISA Simulator output
            run('%s/bin/mergeXML.py %s %s %s' % (here,here + '/' + tdifile,here + '/Template/StandardLISA.xml',outxml))
            # then add the modified Source data included in the Barycentric file
            run('%s/bin/mergeXML.py -k %s %s' % (here,here + '/' + tdifile,here + '/' + xmlfile))

            # what other temporary files to remove?

            run('rm %s' % outxml)
            run('rm %s' % outbin)

            run('rm %s' % xmldest)
            run('rm %s' % bindest)
    
            os.chdir(here)
    
    # noise generation

    slnoisefile = 'TDI/tdi-strain-noise.xml'

    if (not makemode) or (not os.path.isfile(slnoisefile))::
        os.chdir(lisasimdir)

        noiseseed = options.seed
        run('./Noise_Maker %(noiseseed)s')

        run('rm sources.txt; touch sources.txt')
        run('./Package noise-only sources.txt %(noiseseed)s')

        # remove temporary files
        run('rm Binary/AccNoise*.dat Binary/ShotNoise*.dat Binary/XNoise*.bin Binary/YNoise*.bin Binary/ZNoise*.bin')
        run('rm Binary/M1Noise*.bin Binary/M2Noise*.bin Binary/M3Noise*.bin')
        
        noisefileobj = lisaxml.lisaXML(here + '/' + slnoisefile)
        noisefileobj.close()
        run('%s/bin/mergeXML.py %s %s %s' % (here,here + '/' + slnoisefile,here + '/Template/StandardLISA.xml',
                                                                           lisasimdir + '/XML/noise-only.xml'))

        run('rm XML/noise-only.xml')
        run('rm Binary/noise-only.bin')

        os.chdir(here)

# -----------------------
# STEP 5: run Fast Galaxy
# -----------------------

# hmm... are we telling everybody the seed with the filename of the galaxy?

if os.path.isfile('Galaxy/Galaxy.xml'):
    if (not makemode) or ('Galaxy/Galaxy.xml','TDI/Galaxy-tdi-frequency.xml') or newer('Galaxy/Galaxy.xml','TDI/Galaxy-tdi-strain.xml')):
        run('bin/makeTDIsignals-Galaxy.py %s' % seed)

# -------------------------
# STEP 6: assemble datasets
# -------------------------

# first empty the Dataset directory
run('rm -f Dataset/*.xml Dataset/*.bin')

# improve dataset metadata here

# do synthlisa first

if dosynthlisa:
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
        if glob.glob('TDI/*-tdi-frequency.xml'):
            run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-frequency.xml')
        run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
    else:
        if glob.glob('TDI/*-tdi-frequency.xml'):
            run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-frequency.xml')
        run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(noisefile)s')

    # need to add noise data here

    if glob.glob('TDI/*-tdi-frequency.xml'):
        run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-frequency.xml')

# next do LISA Simulator

if lisasimdir:
    nonoisefile = 'Dataset/' + challengename + '-strain-nonoise.xml'
    nonoise = lisaxml.lisaXML(nonoisefile,comments='No-noise dataset for challenge 2 (lisasim version)')
    nonoise.close()

    withnoisefile = 'Dataset/' + challengename + '-strain.xml'
    withnoise = lisaxml.lisaXML(withnoisefile,comments='Full dataset for challenge 2 (lisasim version)')
    withnoise.close()

    keyfile = 'Dataset/' + challengename + '-key.xml'
    key = lisaxml.lisaXML(keyfile,comments='XML key for challenge 2')
    key.close()

    if options.istraining:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-strain.xml')
        run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(slnoisefile)s')
    else:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-strain.xml')
        run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(slnoisefile)s')

    if not dosynthlisa:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-strain.xml')

# ------------------------
# STEP 7: package datasets
# ------------------------

# exit with success code
sys.exit(0)

# TO DO:
# - must add noise and geometry specification (do it with files in Template)
# - must do some instructions
