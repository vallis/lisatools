#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
from lisaxml.convertunit import convertUnit

import sys
import math
import os
import glob
import re

import synthlisa


import numpy
import numpy.oldnumeric as Numeric

from math import pi, sin, cos, sqrt

def run(command):
    commandline = command % globals()
    print "--> %s" % commandline

    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)



# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME [Challenge1.2.1 or Challenge1.2.2 or Challenge1B.2.1, ...]",
                      version="$Id: $")

parser.add_option("-b", "--preBuffer",
                  type="float", dest="prebuffer", default=900.0,
                  help="length of data computed before initial time (s) [default 900.0]")

parser.add_option("-p", "--postBuffer",
                  type="float", dest="postbuffer", default=900.0,
                  help="length of data computed after final time (s) [default 900.0]")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for waveform, not counting prebuffer (s) [default 0.0]")
                                    
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=31457280.0,
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for waveform sampling (s) [default 15]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name: Challenge1.1.1/Challenege1.1.2/...")

challengename = args[0]
timestep = options.timestep
duration = options.duration


##### 0 : transform all ascii data to xml

# to be added later


##### I : creating barycentric for user specified params

sources = "Source/" + challengename + '/' + "/*xml"
for xmlfile in glob.glob(sources):
    if (re.search('key', xmlfile) == None):
       baryfile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
       run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')

#### III : creating TDI files for all (non-key) barycentric files using synthetic LISA

barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
for xmlfile in glob.glob(barycentric):
    if (re.search('key', xmlfile) == None):
       tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
       run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')


#### IV : creating TDI files using LISA simulator

here = os.getcwd()

import lisasimulator
lisasimdir = lisasimulator.lisasim1yr
barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
for xmlfile in glob.glob(barycentric):
    print "working with ", xmlfile
    xmlname = re.sub('\-barycentric.xml','',os.path.basename(xmlfile))
    #tdifile = '/TDI/' + xmlname + '-tdi-strain.xml'
    tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(xmlfile))
    binfile = 'Barycentric/' + challengename + '/'+ xmlname + '-barycentric-0.bin'
   
    xmldest = lisasimdir + '/XML/' + xmlname + '_Barycenter.xml'
    bindest = lisasimdir + '/XML/' + xmlname + '-barycentric-0.bin'

    run('cp %s %s' % (xmlfile,xmldest))
    run('cp %s %s' % (binfile,bindest))
    os.chdir(lisasimdir)
  
    # run LISA simulator

    run('./GWconverter %s' % xmlname)
    run('./Vertex1Signals')
    run('./Vertex2Signals')
    run('./Vertex3Signals')

    run('echo "%s" > sources.txt' % xmlname)
    run('./Package %s sources.txt 0' % xmlname)

    outxml = 'XML/' + xmlname + '.xml'
    outbin = 'Binary/' + xmlname + '.bin'
    here2 = here+'/../MLDCpipelines2'
    # create an empty XML file...
    sourcefileobj = lisaxml.lisaXML(here + '/' + tdifile)
    sourcefileobj.close()

    run('%s/bin/mergeXML.py %s %s %s' % (here+'/../MLDCpipelines2',here + '/' + tdifile,\
                  here + '/../MLDCpipelines2/Template/StandardLISA.xml',outxml))
            # then add the modified Source data included in the Barycentric file
   
    # remove temporary files

    run('rm %s' % outxml)
    run('rm %s' % outbin)

    run('rm %s' % xmldest)
    run('rm %s' % bindest)
    
    os.chdir(here)


### preparation
if(challengename == "Challenge1B.2.1"):
  datadir = "challenge1B.2.1"
if(challengename == "Challenge1B.2.2"):
    datadir = "challenge1B.2.2"


#### V : call evaluation script for synthetic LISA data

tdis = glob.glob('TDI/'+challengename+'/*frequency.xml')
keyTdi = 'TDI/'+challengename+'/'+challengename+'-key-tdi-frequency.xml'
dataTdi = 'Data/'+ datadir+ '-frequency/*.xml'
#for xmlfile in glob.glob(tdis):
run('bin/evaluate-syntheticLISA.py --maxPhase %(dataTdi)s %(keyTdi)s %(tdis)s')

### VI : call evaluation script for LISA simulator

tdis = glob.glob('TDI/'+challengename+'/*strain.xml')
keyTdi = 'TDI/'+challengename+'/'+challengename+'-key-tdi-strain.xml'
dataTdi = 'Data/'+challengename+ '-strain/*.xml'
#for xmlfile in glob.glob(tdis):
run('bin/evaluate-LISAsimulator.py --maxPhase %(dataTdi)s %(keyTdi)s %(tdis)s')


