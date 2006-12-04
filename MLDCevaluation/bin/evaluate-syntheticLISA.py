#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
import sys
import re, math

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

# To do: Need to read quadrutures as well...

Injtdifile = lisaxml.readXML(Injfile)

lisa = Injtdifile.getLISAgeometry()

Injsources = Injtdifile.getLISASources()

tdi = Injtdifile.getTDIObservables()[0]

Injtdifile.close()

X = tdi.Xf

A = (2.0*tdi.Xf -tdi.Yf - tdi.Zf)/3.0

E = (tdi.Zf - tdi.Yf)/math.sqrt(3.0) 


#To do: here I'll compute noise spectral density

#print Detfiles[0]
sz = len(Detfiles)
if (sz > 0):
   Detfiles[0] = Detfiles[0][1:-1]
   for i in xrange(sz):
      if i !=0:
         Detfiles[i] = Detfiles[0][:-1]
         

for userfile in Detfiles:
    print userfile
#    if ( re.search('Key', userfile) == None): Uncomment thois line later
       Dettdifile = lisaxml.readXML(userfile)
       Dettdi =  Dettdifile.getTDIObservables()[0]
       
    




