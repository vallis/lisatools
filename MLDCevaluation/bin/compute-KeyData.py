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

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME [Challenge1.3 or Challenge2.2]", version="$Id: $")


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
                  type="float", dest="duration", default=62914560.0,
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 62914560 = 2^22 * 15]")


parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for waveform sampling (s) [default 15]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name: Challenge1.3/Challenege2.2")

challengename = args[0]
timestep = options.timestep
duration = options.duration

KeysChal1_3 = ["Key/challenge1.3.1-key.xml", "Key/challenge1.3.2-key.xml","Key/challenge1.3.4-key.xml"]

if (challengename == "Challenge1.3"):
   for xmlKey in KeysChal1_3:
       baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
       run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
       tdiKeyFile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryKeyFile))
       run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryKeyFile)s %(tdiKeyFile)s')


if (challengename == "Challenge2.2"):
       xmlKey1 = "Key/challenge2.2BBH-key.xml"
       run("bin/create_quadratures.py %s" % xmlKey1)
       for xmlKey in [xmlKey1, "Key/challenge2.2BBH-key_0.xml"]:
            baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
            run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
       barycentric = 'Barycentric/'+challengename+'/*key-*barycentric.xml'
       for baryxmlfile in glob.glob(barycentric):
            tdiKeyFile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryxmlfile))
            run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryxmlfile)s %(tdiKeyFile)s')

