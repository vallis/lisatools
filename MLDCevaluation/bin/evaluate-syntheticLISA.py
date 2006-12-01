#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
import sys

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] ChallengeNTDI_Key.xml Result1TDI.xml Result2TDI.xml ...",
                      version="$Id:  $")


parser.add_option("-p", "--maxPhase",
                  action="store_true", dest="keyonly", default=True,
                  help="maximizes over the phase on a single channel [on by default]")


(options, args) = parser.parse_args()

# currently we support only a single source parameter file

if len(args) < 2:
    parser.error("You must specify at least the output file and one input file!")

Injfile = args[0]
Detfiles = args[1:]


Injtdifile = lisaxml.readXML(Injfile)

lisa = Injtdifile.getLISAgeometry()

Injsources = Injtdifile.getLISASources()

tdi = Injtdifile.getTDIObservables()[0]

Injtdifile.close()

for userfile in Injfiles:
    Dettdifile = lisaxml.readXML(userfile)
    Injtdi =  Dettdifile.getTDIObservables()[0]




