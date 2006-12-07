#!/usr/bin/env python

__version__='$Id: $'

# keep it clean!

import lisaxml
import BBH

# import sys
# import getopt
# import math
# import random

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] INPUT.txt OUTPUT.xml",
                      version="$Id: $")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 2:
    parser.error("You must specify the input text file and the output XML file!")
    
# get the name of the output file

inTXTfile = args[0]
outXMLfile = args[1]

if '.xml' in outXMLfile:
    outXMLroot = outXMLfile[:-4]
else:
    outXMLroot = outXMLfile

srcfilenum = 0

for line in open(inTXTfile,'r'):
    try:
        pars = map(float,line.split())
    
        mysystem = BBH.BlackHoleBinary('BlackHoleBinary %s' % srcfilenum)

        mysystem.EclipticLatitude  = pars[0]
        mysystem.EclipticLongitude = pars[1]
        mysystem.Polarization      = pars[2]
        mysystem.Mass1             = pars[3]
        mysystem.Mass2             = pars[4]
        mysystem.CoalescenceTime   = pars[5]
        mysystem.InitialAngularOrbitalPhase = pars[6]
        mysystem.Distance          = pars[7]
        mysystem.Inclination       = pars[8]

        if len(pars) > 9:
            print "--> Ignoring extra params %s in line \n    %s" % (pars[8:],line),

        mysystem.IntegrationStep   = 15.0

        mysystem.TruncationTime    = 0.0
        mysystem.TaperApplied      = 7.0

        if options.verbose:
            print "--> System %s:" % srcfilenum
            for p in mysystem.parameters:
                print '   ', p, ':', mysystem.parstr(p)

        outputXML = lisaxml.lisaXML(outXMLroot + str(srcfilenum) + '.xml')
        outputXML.SourceData(mysystem,name=('BBH binary %s' % srcfilenum))
        outputXML.close()

        srcfilenum += 1
    except:
        if line.split():
            print "--> Hm, some problems in processing line \n    %s    I'll drop it!" % line

        pass

print "Successfully processed %s BBH sources" % srcfilenum
