#!/usr/bin/env python

__version__ = '$Id$'

# some definitions...

import sys
import os
import glob
import re
import time
from distutils.dep_util import newer, newer_group

import lisaxml
import numpy

def timestring(lapse):
    hrs  = int(lapse/3600)
    mins = int((lapse - hrs*3600)/60)
    secs = int((lapse - hrs*3600 - mins*60))
    
    return "%sh%sm%ss" % (hrs,mins,secs)

def run(command):
    commandline = command % globals()
    print "--> %s" % commandline

    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

step0time = time.time()

# ---------------------------------------
# STEP 0: parse parameters, set constants
# ---------------------------------------

oneyear = 31457280

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME...",
                      version="$Id$")

parser.add_option("-t", "--training",
                  action="store_true", dest="istraining", default=False,
                  help="include source information in output file [off by default]")

# note that the default value is handled below
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=None,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-n", "--seedNoise",
                  type="int", dest="seednoise", default=None,
                  help="noise-specific seed (int) [will use global seed (-s/--seed) if not given]")

parser.add_option("-m", "--make",
                  action="store_true", dest="makemode", default=False,
                  help="run in make mode (use already-generated source key files and intermediate products; needs same seeds)")

parser.add_option("-S", "--synthlisa",
                  action="store_true", dest="synthlisaonly", default=False,
                  help="run only Synthetic LISA")

parser.add_option("-L", "--lisasim",
                  action="store_true", dest="lisasimonly", default=False,
                  help="run only the LISA Simulator")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name!")

challengename = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

# this sets the default dataset duration if not specified, with a special case for apparent challenge-1 datasets
# (but not for 1.3.X EMRIs, which are two years)
if options.duration == None:
    if 'challenge1' in challengename and (not 'challenge1.3' in challengename):
        print("--> It looks like you're generating a challenge-1 dataset, so I will set the duration to 2^21 * 15 (you can override this with -T).")
        options.duration = oneyear
    else:
        options.duration = 2.0*oneyear

if options.synthlisaonly:
    lisasimdir = None
else:
    import lisasimulator

    if options.duration == 31457280:
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

if options.seednoise == None:
    seednoise = seed
else:
    seednoise = options.seednoise

timestep = options.timestep
duration = options.duration
makemode = options.makemode

step1time = time.time()

# --------------------------------------------------------------------------------------
# STEP 1: create source XML parameter files, and collect all of them in Source directory
# --------------------------------------------------------------------------------------

# first empty the Source and Galaxy directories
# makemode won't regenerate sources, so at least step 1 must be completed
# before running in make mode

if (not makemode):
    run('rm -f Source/*.xml')
    run('rm -f Galaxy/*.xml Galaxy/*.dat')

    # to run CHALLENGE, a file source-parameter generation script CHALLENGE.py must sit in bin/
    # it must take a single argument (the seed) and put its results in the Source subdirectory
    # if makesource-Galaxy.py is called, it must be with the UNALTERED seed, which is used
    # later to call makeTDIsignals-Galaxy.py

    sourcescript = 'bin/' + challengename + '.py'
    if not os.path.isfile(sourcescript):
        parser.error("I need the challenge script %s!" % sourcescript)
    else:
        run('python %s %s' % (sourcescript,seed))

step2time = time.time()

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

step3time = time.time()

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
            # if we are not generating a Galaxy, don't include its confusion noise in the evaluation of SNR
            if os.path.isfile('Galaxy/Galaxy.xml'):
                run('bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')
            else:
                run('bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')

    # now run noise generation...
    noisefile = 'TDI/tdi-frequency-noise.xml'
    # note that we're not checking whether the seed is the correct one
    if (not makemode) or (not os.path.isfile(noisefile)):
        run('bin/makeTDInoise-synthlisa.py --seed=%(seednoise)s --duration=%(duration)s --timeStep=%(timestep)s %(noisefile)s')

step4time = time.time()

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
            outbin = 'XML/' + xmlname + '.bin'

            # create an empty XML file...
            sourcefileobj = lisaxml.lisaXML(here + '/' + tdifile)
            sourcefileobj.close()
            # then add the TDI data from the LISA Simulator output
            run('%s/bin/mergeXML.py %s %s %s' % (here,here + '/' + tdifile,here + '/Template/StandardLISA.xml',outxml))
            # then add the modified Source data included in the Barycentric file
            run('%s/bin/mergeXML.py -k %s %s' % (here,here + '/' + tdifile,here + '/' + xmlfile))

            run('rm -f %s' % outxml)
            run('rm -f %s' % outbin)

            run('rm -f %s' % xmldest)
            run('rm -f %s' % bindest)
    
            run('rm -f Binary/*.bin')
            run('rm -f Binary/GW*.dat')
            run('rm -f Binary/SourceParameters.dat')
            run('rm -f Data/*.txt')
    
            os.chdir(here)
    
    # noise generation

    slnoisefile = 'TDI/tdi-strain-noise.xml'

    if (not makemode) or (not os.path.isfile(slnoisefile)):
        os.chdir(lisasimdir)

        run('./Noise_Maker %(seednoise)s')

        run('rm -f sources.txt; touch sources.txt')
        run('./Package noise-only sources.txt %(seednoise)s')

        # remove temporary files
        run('rm -f Binary/AccNoise*.dat Binary/ShotNoise*.dat Binary/XNoise*.bin Binary/YNoise*.bin Binary/ZNoise*.bin')
        run('rm -f Binary/M1Noise*.bin Binary/M2Noise*.bin Binary/M3Noise*.bin')
        
        noisefileobj = lisaxml.lisaXML(here + '/' + slnoisefile)
        noisefileobj.close()
        run('%s/bin/mergeXML.py %s %s %s' % (here,here + '/' + slnoisefile,here + '/Template/StandardLISA.xml',
                                                                           lisasimdir + '/XML/noise-only.xml'))

        run('rm -f XML/noise-only.xml')
        run('rm -f XML/noise-only.bin')
        run('rm -f Data/*.txt')

        os.chdir(here)

step5time = time.time()

# -----------------------
# STEP 5: run Fast Galaxy
# -----------------------

# hmm... are we telling everybody the seed with the filename of the galaxy?
# no, I think lisaxml later changes the name, doesn't it?

if os.path.isfile('Galaxy/Galaxy.xml'):
    if (not makemode) or (newer('Galaxy/Galaxy.xml','TDI/Galaxy-tdi-frequency.xml') or newer('Galaxy/Galaxy.xml','TDI/Galaxy-tdi-strain.xml')):
        run('bin/makeTDIsignals-Galaxy.py %s' % seed)

step6time = time.time()

# -------------------------
# STEP 6: assemble datasets
# -------------------------

# ignore make mode here, we'll always repackage everything

# first empty the Dataset directory (do not remove tar.gz)
run('rm -f Dataset/'+challengename+'*.xml Dataset/'+challengename+'*.bin Dataset/'+challengename+'*.txt')

# improve dataset metadata here

# do synthlisa first

if options.istraining:
    globalseed = ', source seed = %s,  noise seed = %s' % (seed,seednoise)
else:
    globalseed = ''

run('cp Template/lisa-xml.xsl Dataset/.')
run('cp Template/lisa-xml.css Dataset/.')

if dosynthlisa:
    # set filenames

    if options.istraining:
        nonoisefile   = 'Dataset/' + challengename + '-frequency-nonoise-training.xml'
        withnoisefile = 'Dataset/' + challengename + '-frequency-training.xml'

        nonoisetar    = challengename + '-frequency-nonoise-training.tar.gz'
        withnoisetar  = challengename + '-frequency-training.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key-training.xml'
    else:
        nonoisefile   = 'Dataset/' + challengename + '-frequency-nonoise.xml'
        withnoisefile = 'Dataset/' + challengename + '-frequency.xml'

        nonoisetar    = challengename + '-frequency-nonoise.tar.gz'
        withnoisetar  = challengename + '-frequency.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key.xml'

    # create empty files

    lisaxml.lisaXML(nonoisefile,
                    author="MLDC Task Force",
                    comments='No-noise dataset for challenge 2 (synthlisa version)' + globalseed).close()

    lisaxml.lisaXML(withnoisefile,
                    author="MLDC Task Force",
                    comments='Full dataset for challenge 2 (synthlisa version)' + globalseed).close()

    lisaxml.lisaXML(keyfile,
                    author="MLDC Task Force",
                    comments='XML key for challenge 2' + globalseed).close()

    # add signals and noise to the no-noise and with-noise files
    # omit keys if we're not doing training

    if options.istraining:
        if glob.glob('TDI/*-tdi-frequency.xml'):
            run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-frequency.xml')

        run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
    else:
        if glob.glob('TDI/*-tdi-frequency.xml'):
            run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-frequency.xml')

        run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(noisefile)s')

    # create the key with all source info

    if glob.glob('TDI/*-tdi-frequency.xml'):
        run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-frequency.xml')

    # now do some tarring up, including XSL and CSS files from Template

    os.chdir('Dataset')

    nonoisefile   = os.path.basename(nonoisefile)
    withnoisefile = os.path.basename(withnoisefile)

    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (nonoisetar,  nonoisefile,  re.sub('\.xml','-[0-9].bin',nonoisefile  )))
    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (withnoisetar,withnoisefile,re.sub('\.xml','-[0-9].bin',withnoisefile)))    

    os.chdir('..')

# next do LISA Simulator

if lisasimdir:
    # set filenames

    if options.istraining:
        nonoisefile   = 'Dataset/' + challengename + '-strain-nonoise-training.xml'
        withnoisefile = 'Dataset/' + challengename + '-strain-training.xml'

        nonoisetar    = challengename + '-strain-nonoise-training.tar.gz'
        withnoisetar  = challengename + '-strain-training.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key-training.xml'
    else:
        nonoisefile   = 'Dataset/' + challengename + '-strain-nonoise.xml'
        withnoisefile = 'Dataset/' + challengename + '-strain.xml'

        nonoisetar    = challengename + '-strain-nonoise.tar.gz'
        withnoisetar  = challengename + '-strain.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key.xml'
    
    # create empty files
    
    lisaxml.lisaXML(nonoisefile,
                    author = 'MLDC task force',
                    comments='No-noise dataset for challenge 2 (lisasim version)' + globalseed).close()

    lisaxml.lisaXML(withnoisefile,
                    author = 'MLDC task force',
                    comments='Full dataset for challenge 2 (lisasim version)' + globalseed).close()

    # add signals and noise

    if options.istraining:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-strain.xml')
        run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(slnoisefile)s')
    else:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-strain.xml')
        run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(slnoisefile)s')

    # do key file, but only if synthlisa has not run, otherwise it will be erased

    if not dosynthlisa:
        lisaxml.lisaXML(keyfile,comments='XML key for challenge 2' + globalseed).close()

        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-strain.xml')

    # now do some tarring up, including XSL and CSS files from Template

    os.chdir('Dataset')

    nonoisefile   = os.path.basename(nonoisefile)
    withnoisefile = os.path.basename(withnoisefile)

    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (nonoisetar,  nonoisefile,  re.sub('\.xml','-[0-9].bin',nonoisefile  )))
    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (withnoisetar,withnoisefile,re.sub('\.xml','-[0-9].bin',withnoisefile)))    

    os.chdir('..')

run('rm Dataset/lisa-xml.xsl')
run('rm Dataset/lisa-xml.css')

step7time = time.time()

# ------------------------
# STEP 7: package datasets
# ------------------------

endtime = time.time()

print
print "--> Completed generating %s" % challengename 
print "    Total time         : %s" % timestring(endtime   - step0time)
print "    Sources time       : %s" % timestring(step3time - step1time)
print "    Synthetic LISA time: %s" % timestring(step4time - step3time)
print "    LISA Simulator time: %s" % timestring(step5time - step4time)
print "    Fast Galaxy time   : %s" % timestring(step6time - step5time)

# exit with success code
sys.exit(0)

# TO DO:
# - must add noise and geometry specification (do it with files in Template)
# - must do some instructions
