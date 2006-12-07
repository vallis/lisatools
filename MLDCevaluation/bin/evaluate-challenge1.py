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

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME [Challenge1.1.1 or Challenge1.1.2 or ...]",
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

##### I : create barycentric waveforms from the key file

## If it is challenge 1.1.1 or 1.2.1 or 1.2.2 we want to do phase maximization

xmlKey = ['Key/'+challengename+"_Key.xml"]
if(challengename in ['Challenge1.1.1a', 'Challenge1.1.1b', 'Challenge1.1.1c', 'Challenge1.2.1', 'Challenge1.2.2']):
    print "Creating Keyfile with zero and pi/2 phases"
    key = xmlKey[0]
    run("bin/create_quadratures.py %(key)s")
    xmlKey.append('Key/'+challengename+'_Key_0.xml')
    xmlKey.append( 'Key/'+challengename+'_Key_piby2.xml')

for key in xmlKey:    
  baryfile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(key))
  run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(key)s %(baryfile)s')

##### II : creating barycentric for user specified params

sources = "Source/" + challengename + '/' + "/*xml"
for xmlfile in glob.glob(sources):
    baryfile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
    run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')

#### III : creating TDI files for all barycentric files

#barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
#for xmlfile in glob.glob(barycentric):
#    tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
#    run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')


#### IV : call evaluation script

tdis = glob.glob('TDI/'+challengename+'/*.xml')
keyTdi = 'TDI/'+challengename+'/'+challengename+'_Key-tdi-frequency.xml'
#for xmlfile in glob.glob(tdis):
run('bin/evaluate-syntheticLISA.py --maxPhase %(keyTdi)s %(tdis)s')
