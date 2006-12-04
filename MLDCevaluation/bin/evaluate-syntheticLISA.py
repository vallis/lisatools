#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
import sys
import re, math
import numpy

import synthlisa
from math import pi, sin, cos, sqrt

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] ChallengeNTDI_Key.xml Result1TDI.xml Result2TDI.xml ...",
                      version="$Id:  $")


parser.add_option("-p", "--maxPhase",
                  action="store_true", dest="phasemax", default=False,
                  help="maximizes over the phase on a single channel [off by default]")


(options, args) = parser.parse_args()

#!!!! currently we support only a single source parameter file  !!!

if len(args) < 2:
    parser.error("You must specify at least the output file and one input file!")

Injfile = args[0]
Detfiles = args[1:]

# To do: Need to read quadrutures as well...

if options.phasemax :
   if ( re.search('Challenge1.2', Injfile) == None and \
        re.search('Challenge1.1.1', Injfile) == None ):
	print "phase maximization can be done only for 1.2 and 1.1.1 chalenges"
	sys.exit(1)


# Dealing with key files (injected sources)

Injtdifile = lisaxml.readXML(Injfile)
lisa = Injtdifile.getLISAgeometry()
Injsources = Injtdifile.getLISASources()
tdi = Injtdifile.getTDIObservables()[0]
Injtdifile.close()

X = tdi.Xf
A = (2.0*tdi.Xf -tdi.Yf - tdi.Zf)/3.0
E = (tdi.Zf - tdi.Yf)/math.sqrt(3.0) 

sampling = tdi.TimeSeries.Cadence

XX = synthlisa.spect(X,sampling,0)
AA = synthlisa.spect(A,sampling,0)
EE = synthlisa.spect(E,sampling,0)

###  Computing quadratures if needed
if options.phasemax:
   injfile =  Injfile[:-18]+'_piby2-tdi-frequency.xml'
   Injtdifile = lisaxml.readXML(injfile)
   tdipiby2 = Injtdifile.getTDIObservables()[0]
   Xpiby2 = tdipiby2.Xf
   Injtdifile.close()
   injfile =  Injfile[:-18]+'_0-tdi-frequency.xml'
   Injtdifile = lisaxml.readXML(injfile)
   tdi0 = Injtdifile.getTDIObservables()[0]
   X0 = tdi0.Xf


fr = XX[1:,0]

# Compute noise spectral density

om = 2.0 * pi * fr
L  = 16.6782

# instrument noise

Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
Sop = 1.8e-37 * fr**2

Sx = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)
Sxy = -4.0 * numpy.sin(2.0*om*L)*numpy.sin(om*L) * ( Sop + 4.*Spm )
Sa = 2.0 * (Sx - Sxy)/3.0
Se = Sa


normX = sqrt(2.0 * numpy.sum(XX[1:,1] / Sx))
normA = sqrt(2.0 * numpy.sum(AA[1:,1] / Sa))
normE = sqrt(2.0 * numpy.sum(EE[1:,1] / Se))

#print Detfiles[0]
sz = len(Detfiles)
if (sz > 0):
   Detfiles[0] = Detfiles[0][1:-1]
   for i in xrange(sz):
      if i !=0:
         Detfiles[i] = Detfiles[i][:-1]

for userfile in Detfiles:
   # print userfile
    if ( re.search('Key', userfile) == None):
       print 80*'*'
       print 4*" ", "Dealing with %s file..." % userfile 
       print 80*'*'
       Dettdifile = lisaxml.readXML(userfile)
       #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       # Here I am dealing with only 1 source!!!!!
       # Nedd to expand it deal with multiple surces...
       #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       Dettdi =  Dettdifile.getTDIObservables()[0]
       Xd = Dettdi.Xf
       Ad = (2.0*Dettdi.Xf - Dettdi.Yf - Dettdi.Zf)/3.0
       Ed = (Dettdi.Zf - Dettdi.Yf)/math.sqrt(3.0) 
#       diffX = X - Xd
#       diffA = A - Ad
#       diffE = E - Ed
       diffXX = synthlisa.spect(X - Xd,sampling,0) 
       diffAA = synthlisa.spect(A - Ad,sampling,0) 
       diffEE = synthlisa.spect(E - Ed,sampling,0) 

       # Computing SNR of a difference:
       SnA =  2.0 * numpy.sum(diffAA[1:,1] / Sa) # square SNR_Adiff
       SnE =  2.0 * numpy.sum(diffEE[1:,1] / Se)
       SnDiff = sqrt( SnA + SnE )

       print "combined SNR of difference = ", SnDiff
        
       # Computing overlaps
       
       AAd = synthlisa.spect(Ad,sampling,0)
       EEd = synthlisa.spect(Ed,sampling,0)
       normAd =  sqrt(2.0 * numpy.sum(AAd[1:,1] / Sa))
       normEd =  sqrt(2.0 * numpy.sum(EEd[1:,1] / Se))
       olapA =  0.5*(normA/normAd + normAd/normA  - SnA/(normA*normAd) )
       olapE =  0.5*(normE/normEd + normEd/normE  - SnE/(normE*normEd) )
    
       print "overlap between A tdis  = ", olapA
       print "overlap between E tdis  = ", olapE

       if options.phasemax:
           diffXX0 = synthlisa.spect(X0 - Xd,sampling,0)
#           diffXXpiby2 = synthlisa.spect(Xpiby2 - Xd,sampling,0)
#	   SnXdifMax = sqrt( 2.0 * numpy.sum(diffXX0[1:,1] / Sx) + \
#	                     2.0 * numpy.sum(diffXXpiby2[1:,1] / Sx) )
#	   SnXdif = sqrt(2.0 * numpy.sum(diffXX[1:,1] / Sx)) 
#	   print "Non-maximized SNR of dX = ", SnXdif
#	   print "Maximized over the phase SNR of dX = ", SnXdifMax
        


