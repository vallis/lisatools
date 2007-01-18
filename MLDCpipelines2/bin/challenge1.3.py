#!/usr/bin/env python

__version__ = '$Id: $'

# some definitions...

import sys
import os
import glob
import re
import time
from distutils.dep_util import newer, newer_group

import lisaxml
import numpy
import random

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

parser = OptionParser(usage="usage: %prog [options]",
                      version="$Id:  $")

parser.add_option("-t", "--training",
                  action="store_true", dest="istraining", default=False,
                  help="include source information in output file [off by default]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=2.0*oneyear,
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

# add options to disable the LISA Simulator or Synthetic LISA

(options, args) = parser.parse_args()

if options.seed == None:
    parser.error("You must specify the seed!")

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
   
    # generating 5 EMRI sources

    # 2 with SMBH around 1e6, 2 with SMBH around 5e6, 1 with SMBH around 1e7

    requestsn = random.randint(40,110)
    run('bin/makesource-EMRI.py --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1e6 --sourceName="EMRI-1" Source/EMRI-1.xml' % (seed+1, requestsn))

    requestsn = random.randint(40,110)
    run('bin/makesource-EMRI.py --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1e6 --sourceName="EMRI-2" Source/EMRI-2.xml' % (seed+2, requestsn))

    requestsn = random.randint(40,110)
    run('bin/makesource-EMRI.py --seed=%s --distance=1e9 --requestSN=%s --massSMBH=5e6 --sourceName="EMRI-3" Source/EMRI-3.xml' % (seed+3, requestsn))

    requestsn = random.randint(40,110)
    run('bin/makesource-EMRI.py --seed=%s --distance=1e9 --requestSN=%s --massSMBH=5e6 --sourceName="EMRI-4" Source/EMRI-4.xml' % (seed+4,requestsn))

    requestsn = random.randint(40,110)
    run('bin/makesource-EMRI.py --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1e7 --sourceName="EMRI-5" Source/EMRI-5.xml' % (seed+5,requestsn))

    
   
   
step2time = time.time()
print "step1: time = ", step2time -step1time


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
print "step2: time = ", step3time -step2time

# --------------------------
# STEP 3: run Synthetic LISA
# --------------------------

# first empty the TDI directory
if (not makemode):
    run('rm -f TDI/*.xml TDI/*.bin TDI/Binary')

run('cp Template/lisa-xml.xsl Dataset/.')
run('cp Template/lisa-xml.css Dataset/.')
if options.istraining:
    globalseed = ', source seed = %s,  noise seed = %s' % (seed,seednoise)
else:
    globalseed = ''

if dosynthlisa:
    # then run makeTDI-synthlisa over all the barycentric files in the Barycentric directory
    # the results are files of TDI time series that include the source info
    # these calls have also the result of rescaling the barycentric files to satisfy any RequestSN that
    # they may carry
    ind = 1
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        sourcename = re.sub('-barycentric\.xml$','',os.path.basename(xmlfile))
	print sourcename
        tdifile = 'TDI/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
        noisefile = 'TDI/tdi-frequency-noise'+str(ind)+'.xml'
        if (not makemode) or newer(xmlfile,tdifile):
            run('bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s --noiseOnly %(xmlfile)s %(tdifile)s')
     # now run noise generation for each source
     # note that we're not checking whether the seed is the correct one
	if (not makemode) or (not os.path.isfile(noisefile)):
            run('bin/makeTDInoise-synthlisa.py --seed=%s --duration=%s --timeStep=%s %s' % (seednoise + ind, duration, timestep, noisefile))
     # add signal to noise
        if options.istraining:
           nonoisefile   = 'Dataset/'+ "Training_" + sourcename + '-frequency-nonoise-training.xml'
           withnoisefile = 'Dataset/' + "Training_" + sourcename + '-frequency-training.xml'
           keyfile       = 'Dataset/' + sourcename + '-key-training.xml'
#           nonoisetar    = challengename + '-frequency-nonoise-training.tar.gz'
#           withnoisetar  = challengename + '-frequency-training.tar.gz'
        else:
           nonoisefile   = 'Dataset/' + "Blind_" + sourcename + '-frequency-nonoise.xml'
           withnoisefile = 'Dataset/' + "Blind_" + sourcename + '-frequency.xml'
           keyfile       = 'Dataset/' + sourcename + '-key.xml'
#           nonoisetar    = challengename + '-frequency-nonoise.tar.gz'
#           withnoisetar  = challengename + '-frequency.tar.gz'

      # create empty files

        lisaxml.lisaXML(nonoisefile,\
                        author="MLDC Task Force",\
                        comments='No-noise dataset for challenge 1.3 (synthlisa version)' + globalseed).close()

        lisaxml.lisaXML(withnoisefile,\
                        author="MLDC Task Force",\
                        comments=sourcename+' dataset for challenge 1.3 (synthlisa version)' + globalseed).close()

        lisaxml.lisaXML(keyfile,\
                        author="MLDC Task Force",\
                        comments='XML key for challenge 1.3' + globalseed).close()

    # add signals and noise to the with-noise file

        if options.istraining:
            run('bin/mergeXML.py %(nonoisefile)s %(tdifile)s')
            run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
        else:
            run('bin/mergeXML.py -n %(nonoisefile)s %(tdifile)s')
            run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
	    
        ind += 1
       

step4time = time.time()
print "step3: time = ", step4time -step3time



# --------------------------
# STEP 4: run LISA Simulator
# --------------------------

# some of this may later be moved inside makeTDInoise-lisasim.py and makeTDIsignal-lisasim.py

if lisasimdir:
    here = os.getcwd()
    
    # signal generation
    ind = 1 
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        xmlname = re.sub('\-barycentric.xml','',os.path.basename(xmlfile))
	print xmlname
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

        slnoisefile = 'TDI/tdi-strain-noise' + str(ind) +'.xml'

        if (not makemode) or (not os.path.isfile(slnoisefile)):
            os.chdir(lisasimdir)

            run('./Noise_Maker %s' % (seednoise + ind))

            run('rm -f sources.txt; touch sources.txt')
            run('./Package noise-only sources.txt %s' % (seednoise + ind))
	    

        # remove temporary files
            run('rm -f Binary/AccNoise*.dat Binary/ShotNoise*.dat Binary/XNoise*.bin Binary/YNoise*.bin Binary/ZNoise*.bin')
            run('rm -f Binary/M1Noise*.bin Binary/M2Noise*.bin Binary/M3Noise*.bin')
        
            noisefileobj = lisaxml.lisaXML(here + '/' + slnoisefile)
            noisefileobj.close()
            run('%s/bin/mergeXML.py %s %s %s' % (here,here + '/' + slnoisefile,here + '/Template/StandardLISA.xml', lisasimdir + '/XML/noise-only.xml'))

            run('rm -f XML/noise-only.xml')
            run('rm -f XML/noise-only.bin')
            run('rm -f Data/*.txt')

        os.chdir(here)
        
 # add signal to noise
        if options.istraining:
           nonoisefile   = 'Dataset/'+ "Training_" + sourcename + '-strain-nonoise-training.xml'
           withnoisefile = 'Dataset/' + "Training_" + sourcename + '-strain-training.xml'
           keyfile       = 'Dataset/' + sourcename + '-key-training.xml'
#           nonoisetar    = challengename + '-frequency-nonoise-training.tar.gz'
#           withnoisetar  = challengename + '-frequency-training.tar.gz'
        else:
           nonoisefile   = 'Dataset/' + "Blind_" + sourcename + '-strain-nonoise.xml'
           withnoisefile = 'Dataset/' + "Blind_" + sourcename + '-strain.xml'
           keyfile       = 'Dataset/' + sourcename + '-key.xml'
#           nonoisetar    = challengename + '-frequency-nonoise.tar.gz'
#           withnoisetar  = challengename + '-frequency.tar.gz'

      # create empty files

        lisaxml.lisaXML(nonoisefile,\
                        author="MLDC Task Force",\
                        comments='No-noise dataset for challenge 1.3 (lisasim version)' + globalseed).close()

        lisaxml.lisaXML(withnoisefile,\
                        author="MLDC Task Force",\
                        comments=sourcename+' dataset for challenge 1.3 (lisasim version)' + globalseed).close()


    # add signals and noise to the with-noise file

        if options.istraining:
            run('bin/mergeXML.py %(nonoisefile)s %(tdifile)s')
            run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
        else:
            run('bin/mergeXML.py -n %(nonoisefile)s %(tdifile)s')
            run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
	    
        ind += 1


step5time = time.time()
print "step4: time = ", step5time -step4time

# ------------------------
# STEP 5: package datasets
# ------------------------
os.chdir('Dataset')

nonoisefile   = os.path.basename(nonoisefile)
withnoisefile = os.path.basename(withnoisefile)

synLisapack = 'Challenge1.3_SynLISA.tar.gz'
Lisasimpack = 'Challenge1.3_LISAsim.tar.gz'

run('tar zcf %s lisa-xml.xsl lisa-xml.css *EMRI*frequency*' % (synLisapack) )
run('tar zcf %s lisa-xml.xsl lisa-xml.css *EMRI*strain*' % (Lisasimpack) )
   
 
#    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (nonoisetar,  nonoisefile,  re.sub('\.xml','-*.bin',nonoisefile  )))
#    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (withnoisetar,withnoisefile,re.sub('\.xml','-*.bin',withnoisefile)))    

os.chdir('..')

run('rm Dataset/lisa-xml.xsl')
run('rm Dataset/lisa-xml.css')

step6time = time.time()
print "step5: time = ", step6time -step5time



endtime = time.time()

print
print "--> Completed generating %s" % challengename 
print "    Total time         : %s" % timestring(endtime   - step0time)
#print "    Sources time       : %s" % timestring(step3time - step1time)
#print "    Synthetic LISA time: %s" % timestring(step4time - step3time)
#print "    LISA Simulator time: %s" % timestring(step5time - step4time)
#print "    Fast Galaxy time   : %s" % timestring(step6time - step5time)

# exit with success code
sys.exit(0)

# TO DO:
# - must add noise and geometry specification (do it with files in Template)
# - must do some instructions
