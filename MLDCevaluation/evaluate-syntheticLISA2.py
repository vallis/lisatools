#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
import sys
import re, math
import numpy

import synthlisa
from math import pi, sin, cos, sqrt

import numpy.oldnumeric as Numeric
import numpy.fft as FFT


fHigh = 1.e-2
fLow = 1.e-5

spr = "     "


## overlap maximized over the phase
## first argument is a signal second is zero phase template
def MaxInnerProd(ser1, ser2, PSD):
  size = Numeric.shape(ser1)[0]
  pdlen = size/2
  nyquistf = 0.5/15.0   #   !!! hardcoded !!!!
  freqs = Numeric.arange(0,pdlen+1,dtype='d') * (nyquistf / pdlen)
  if(Numeric.shape(ser2)[0] != size):
     print "size of time series must be the same"
     sys.exit(1)
  if(Numeric.shape(PSD)[0] != pdlen):
     print "wrong size of psd: ", pdlen, Numeric.shape(PSD)
     sys.exit(1)
  fourier1 = FFT.fft(ser1)
  fourier2 = FFT.fft(ser2)
  prod1 = Numeric.zeros(pdlen+1, dtype='d')
  prod2 = Numeric.zeros(pdlen+1, dtype='d')
  prod1[0] = 0.0
  prod1[1:pdlen] = numpy.multiply(fourier1[1:pdlen],numpy.conjugate(fourier2[1:pdlen])) + numpy.multiply(fourier1[-1:pdlen:-1],numpy.conjugate(fourier2[-1:pdlen:-1]))
  prod1[pdlen] = fourier1[pdlen]*fourier2[pdlen]
  prod2[0] = 0.0
  prod2[1:pdlen] = numpy.multiply(fourier1[1:pdlen],numpy.conjugate(fourier2[1:pdlen]*1.j)) + numpy.multiply((fourier1[-1:pdlen:-1]),numpy.conjugate(fourier2[-1:pdlen:-1]*(-1.j)))
  prod2[pdlen] = fourier1[pdlen]*fourier2[pdlen]
  Numeric.divide(prod1[1:], PSD, prod1[1:]) 
  Numeric.divide(prod2[1:], PSD, prod2[1:]) 
  olap0 = 0.0
  olappiby2 = 0.0
  for i in xrange(pdlen):
      if (freqs[i] > fLow and freqs[i]<= fHigh):
           olap0 += prod1[i]
	   olappiby2 += prod2[i]
  olap0 = 2.0*olap0/float(size)
  olappiby2 = 2.0*olappiby2/float(size)
#  olap0 =  2.0*(numpy.sum(prod1[1:]))/float(size) #it must be scaled by dt
#  olappiby2 =  2.0*(numpy.sum(prod2[1:]))/float(size) #it must be scaled by dt
  print "angle of maxim. = ", math.atan(olappiby2/olap0)
  return sqrt(olap0**2 + olappiby2**2) 


def InnerProd(ser1, ser2, PSD):
  size = Numeric.shape(ser1)[0]
  pdlen = size/2
  nyquistf = 0.5/15.0   #   !!! hardcoded !!!!
  freqs = Numeric.arange(0,pdlen+1,dtype='d') * (nyquistf / pdlen)
  if(Numeric.shape(ser2)[0] != size):
     print "size of time series must be the same"
     sys.exit(1)
  if(Numeric.shape(PSD)[0] != pdlen):
     print "wrong size of psd: ", pdlen, Numeric.shape(PSD)
     sys.exit(1)
  fourier1 = FFT.fft(ser1)
  fourier2 = FFT.fft(ser2)
  prod = Numeric.zeros(pdlen+1, dtype='d')
  prod[0] = 0.0
  prod[1:pdlen] = numpy.multiply(fourier1[1:pdlen],numpy.conjugate(fourier2[1:pdlen])) + numpy.multiply(fourier1[-1:pdlen:-1],numpy.conjugate(fourier2[-1:pdlen:-1]))
  prod[pdlen] = fourier1[pdlen]*fourier2[pdlen]
  Numeric.divide(prod[1:], PSD, prod[1:]) 
  olap0 = 0.0
  for i in xrange(pdlen):
      if (freqs[i] > fLow and freqs[i]<= fHigh):
           olap0 += prod[i]
  olap0 = 2.0*olap0/float(size)
 # olap0 =  2.0*(numpy.sum(prod[1:]))/float(size) #it must be scaled by dt
  return  olap0

def ComputeNorm(ser, sampling, PSD):
     size = Numeric.shape(ser)[0]
     if(Numeric.shape(PSD)[0] != size/2):
         print "wrong size of psd: ", pdlen, Numeric.shape(PSD)
         sys.exit(1)
     ser2 = synthlisa.spect(ser, sampling,0)
     norm = 0.0
     indx = 0
     #fout = open("psdTest.dat", 'w')
     for i in xrange(size/2-1):
        if (ser2[i][0] > fLow and ser2[i][0]<= fHigh):
	    norm += ser2[i][1]/PSD[i]
#	    record = str(ser2[i][0]) + spr + str(PSD[i]) + spr + str(2.0*norm) + "\n"
#	    fout.write(record)
	    indx = indx + 1
     norm = sqrt(2.0*norm)
#    fout.close()     
     #print "indx = ", indx
     #norm = sqrt(2.0 *numpy.sum(ser2[1:,1]/PSD))
     return norm



# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] BlindData.xml ChallengeNTDI_Key.xml Result1TDI.xml Result2TDI.xml ...",
                      version="$Id:  $")


parser.add_option("-p", "--maxPhase",
                  action="store_true", dest="phasemax", default=False,
                  help="maximizes over the phase on a single channel [off by default]")

parser.add_option("-g", "--Galaxy",
                  action="store_true", dest="gal", default=False,
                  help="Adding the Galaxy to the instrumental noise [off by default]")


(options, args) = parser.parse_args()

#!!!! currently we support only a single source parameter file  !!!

if len(args) < 3:
    parser.error("USAGE: evaluate-syntheticLISA2.py  data file, key file and at leat one input file <options>")

Datafile = args[0]
Injfile = args[1]
Detfiles = args[2:]


sz = len(Detfiles)
if (sz > 0):
   Detfiles[0] = Detfiles[0][1:-1]
   for i in xrange(sz):
      if i !=0:
         Detfiles[i] = Detfiles[i][:-1]



# To do: Need to read quadrutures as well...
if options.phasemax :
   if ( re.search('challenge2.2', Injfile) == None ):
	print "phase maximization can be done only for 2.2 chalenge"
	sys.exit(1)


# Reading the data 

Datatdifile = lisaxml.readXML(Datafile)
tdiData = Datatdifile.getTDIObservables()[0]
Xdata = tdiData.Xf
Adata = (2.0*tdiData.Xf -tdiData.Yf - tdiData.Zf)/3.0
Edata = (tdiData.Zf - tdiData.Yf)/math.sqrt(3.0) 

samples = Numeric.shape(Xdata)[0]

# computing useful number of points
pdlen3 = samples/2
nyquistf = 0.5/15.0   #   !!! hardcoded !!!!
freqs = Numeric.arange(0,pdlen3+1,dtype='d') * (nyquistf / pdlen3)
ind = 0
for i in xrange(pdlen3):
    if (freqs[i] > fLow and freqs[i]<= fHigh):
        ind += 1    

ind = ind*2
print "data size = ", samples
print "number of used points ", ind 

if (re.search('challenge2.2', Injfile) != None):
    Dfr = 9.
elif (re.search('challenge1.3', Injfile) != None):
    Dfr = 14.

# reading Key file

Injtdifile = lisaxml.readXML(Injfile)
Injsources = Injtdifile.getLISASources()[0]
tdi = Injtdifile.getTDIObservables()[0]
Injtdifile.close()
sampling = tdi.TimeSeries.Cadence
X = tdi.Xf
A = (2.0*tdi.Xf -tdi.Yf - tdi.Zf)/3.0
E = (tdi.Zf - tdi.Yf)/math.sqrt(3.0)
#XX = synthlisa.spect(X, sampling, 0)
#fr = XX[1:,0]


Specdat = synthlisa.spect(Adata,sampling,0)
fr = Specdat[1:,0]
PSDdat = Specdat[1:,1] 
"""
Sigfilout = open("AEsigCheck1.dat", 'w') 
for i in xrange(len(A)):
   record = str(sampling*float(i)) + spr + str(A[i]) + spr + str(E[i]) + "\n"
   Sigfilout.write(record)
Sigfilout.close()   
sys.exit(0);
"""

if (re.search('challenge1.3', Injfile) != None):
   IndX = len(A)
   for i in xrange(len(A)-1, 1, -1):
      if (A[i] != 0.0):
         IndX = i
         print "IndX = ", IndX 
      break
   A[IndX-20:] = 0.0
   E[IndX-20:] = 0.0


# Compute noise spectral density

om = 2.0 * pi * fr
L  = 16.6782

# instrument noise

Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
Sop = 1.8e-37 * fr**2

Sx = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)
Sxy = -4.0 * numpy.sin(2.0*om*L)*numpy.sin(om*L) * ( Sop + 4.*Spm )
Sa = 2.0 * (Sx - Sxy)/3.0
#Se = Sa
# expression from Phys. Rev. D 73, 122001 (2006), corrected and 
# modified to include TDI response function
# for fractional frequency fluctuations

# conversion LS to SL (incl.deriv.) -- TDI response function Michelson to X,Y,Z
Sgal = (16./3.)*numpy.sin(om*L)**2 * (2.*L*om)**2 * (
     numpy.piecewise(fr,(fr >= 1.0e-4  ) & (fr <= 4.0e-4  ),[lambda f: 2.5e-36*(f/1.0e-4)**-1.83, 0]) + \
     numpy.piecewise(fr,(fr > 4.0e-4  ) & (fr <= 1.15e-3  ),[lambda f: 1.95e-37*(f/4.0e-4)**-2.93, 0]) + \
     numpy.piecewise(fr,(fr > 1.15e-3  ) & (fr <= 2.0e-3  ),[lambda f: 8.86e-39*(f/1.15e-3)**-4.11, 0]) + \
     numpy.piecewise(fr,(fr > 2.0e-3  ) & (fr <= 2.3e-3  ),[lambda f: 9.0e-40*(f/2.0e-3)**-7.09, 0]) + \
     numpy.piecewise(fr,(fr > 2.3e-3  ) & (fr <= 2.5e-3  ),[lambda f: 3.34e-40*(f/2.3e-3)**-13.15, 0]) + \
     numpy.piecewise(fr,(fr > 2.5e-3  ) ,[lambda f: 1.12e-40*(f/2.5e-3)**-23.0, 0]) 
)


SnX = Sx
SnA = Sa
if (options.gal):
    SnX = Sx + Sgal
    SnA = Sa + Sgal   # for galaxy Sxy = -1/2 Sx

"""
foutS = open("PsdTest.dat",'w')
for i in xrange(len(fr)):
   record = str(fr[i]) + spr + str(PSDdat[i]) + spr + str(SnA[i]) + "\n"
   foutS.write(record)
foutS.close()

sys.exit(0)
"""   


normA = ComputeNorm(A,sampling, SnA)
normX = ComputeNorm(X,sampling, SnX)
normE = ComputeNorm(E,sampling, SnA)



if (re.search('challenge1.3', Injfile) != None):
   print "evaluating EMRIs.... ", Injfile
   # Dealing with key files (injected sources)
   MBH = Injsources.MassOfSMBH
   if (MBH >= 0.95e7):
       chal = 1
   elif (MBH >= 4.75e6 and MBH <= 5.25e6):
       chal = 2
   elif (MBH <=1.05e6):
       chal = 4
   
   #############################################
   ## Computing chi^2 and SNR with the key file
   ##############################################

   diffAkAd = synthlisa.spect(Adata - A,sampling,0) 
   diffEkEd = synthlisa.spect(Edata - E,sampling,0) 

   # Computing combined SNR of a difference:
   SnrA =  2.0 * numpy.sum(diffAkAd[1:,1] / SnA) # square SNR_Adiff
   SnrE =  2.0 * numpy.sum(diffEkEd[1:,1] / SnA)
   SnrA = ComputeNorm(Adata-A, sampling, SnA)**2
   SnrE = ComputeNorm(Edata-E, sampling, SnA)**2
   SnrDiff = sqrt( SnrA**2 + SnrE**2 )

   xi2 = SnrDiff/(samples - Dfr)
   chi2 = 0.5*(SnrA + SnrE)/(ind - Dfr)
   #Computing combined SNR

   SnrA = sampling*InnerProd(Adata, A, SnA)
   SnrE = sampling*InnerProd(Edata, E, SnA)
   print "SnrA = ", SnrA/normA, "  SnrE = ", SnrE/normE
   #print "normA = ", normA, "  normE = ", normE
   #print "innerProda = ",  sampling*InnerProd(Adata, A, Sa), "innerProde = ",  sampling*InnerProd(Edata, E, Se)
   Snr = (SnrA + SnrE)/sqrt(normA*normA + normE*normE)
   print 80*'='
   print "using key file we get"
   #print "xi = ", xi2
   print "chi^2 = ", chi2
   print "combined SNR = ", Snr
   print 80*'='
   ###########################################
   ##   Dealing with result files
   ###########################################
   for userfile in Detfiles:
    print userfile
    if ( re.search('key', userfile) == None):
       print 80*'*'
       print 4*" ", "Dealing with %s file..." % userfile 
       print 80*'*'
       Dettdifile = lisaxml.readXML(userfile)
       Detsources = Dettdifile.getLISASources()[0]
       MBH = Detsources.MassOfSMBH
       if (MBH >= 0.95e7):
         src = 1
       elif (MBH >= 4.75e6 and MBH <= 5.25e6):
         src = 2
       elif (MBH <=1.05e6):
         src = 4
       if (src == chal):
          Dettdi =  Dettdifile.getTDIObservables()[0]
          Xs = Dettdi.Xf
          As = (2.0*Dettdi.Xf - Dettdi.Yf - Dettdi.Zf)/3.0
          Es = (Dettdi.Zf - Dettdi.Yf)/math.sqrt(3.0)
          IndX = len(As)
          for i in xrange(len(As)-1, 1, -1):
            if (As[i] != 0.0):
               IndX = i
               break
          As[IndX-25:] = 0.0
          Es[IndX-25:] = 0.0
	#  foutT = open("TruncTest.dat", 'w')
	#  for i in xrange(len(As)):
	##     record = str(i) + spr + str(As[i]) + spr + str(Es[i]) + "\n"
	#     foutT.write(record)
	#  foutT.close()
	  normXs = ComputeNorm(Xs, sampling, SnX)
          normAs = ComputeNorm(As, sampling, SnA)
          normEs = ComputeNorm(Es, sampling, SnA)
	 
	  ### computing chi^2

          diffAsAd = synthlisa.spect(Adata - As,sampling,0) 
          diffEsEd = synthlisa.spect(Edata - Es,sampling,0) 

          # Computing combined SNR of a difference:
          SnrA =  2.0 * numpy.sum(diffAsAd[1:,1] / SnA) # square SNR_Adiff
          SnrE =  2.0 * numpy.sum(diffEsEd[1:,1] / SnA)
          SnrA = ComputeNorm(Adata-As, sampling, SnA)**2
          SnrE = ComputeNorm(Edata-Es, sampling, SnA)**2
          SnrDiff = sqrt( SnrA**2 + SnrE**2 )

          chi2 = 0.5*(SnrA + SnrE)/(ind - Dfr)
          SnrAdif = ComputeNorm(A-As, sampling, SnA)**2
          SnrEdif = ComputeNorm(E-Es, sampling, SnA)**2
 
          d_chi2 = 0.5*(SnrAdif + SnrEdif)/Dfr

	  print "chi^2 = ", chi2
          print "d_chi^2 = ", d_chi2, SnrAdif/Dfr, SnrEdif/Dfr
          #Computing combined SNR

          SnrAs = sampling*InnerProd(Adata, As, SnA)
          SnrEs = sampling*InnerProd(Edata, Es, SnA)

          Snrs = (SnrAs + SnrEs)/sqrt(normAs*normAs + normEs*normEs)
          print "SNRA = ", SnrAs/normAs, "SNRE = ", SnrEs/normEs
          print "combined SNR  = ", Snrs
        
          # Computing overlaps
       
          olapA = sampling*InnerProd(A,As,SnA)/(normA*normAs)
          olapE = sampling*InnerProd(E,Es,SnA)/(normE*normEs)
    
          print "overlap between A tdis  = ", olapA
          print "overlap between E tdis  = ", olapE
          print "         "
elif (re.search('challenge2.2', Injfile) != None):
  print "evaluating BBHs.... ", Injfile 
  Tc = Injsources.CoalescenceTime
  if (Tc <= 1.03e7):
       BBH = 1
  elif (Tc >= 1.5e7 and Tc <= 2.e7):
       BBH = 4
  elif (Tc >= 4.e7 and Tc<=5.e7):
       BBH = 5
  elif (Tc>=6.e7 and  Tc < 6.7e7 ):
       BBH = 2
  #############################################
  ## Computing chi^2 and SNR with the key file
  ##############################################

  diffAkAd = synthlisa.spect(Adata - A,sampling,0) 
  diffEkEd = synthlisa.spect(Edata - E,sampling,0) 

  # Computing combined SNR of a difference:
  SnrA =  2.0 * numpy.sum(diffAkAd[1:,1] / SnA) # square SNR_Adiff
  SnrE =  2.0 * numpy.sum(diffEkEd[1:,1] / SnA)
  SnrA = ComputeNorm(Adata-A, sampling, SnA)**2
  SnrE = ComputeNorm(Edata-E, sampling, SnA)**2
  SnrDiff = sqrt( SnrA**2 + SnrE**2 )

  xi2 = SnrDiff/(samples - Dfr)
  chi2 = 0.5*(SnrA + SnrE)/(ind - Dfr)
  #Computing combined SNR

  SnrA = sampling*InnerProd(Adata, A, SnA)
  SnrE = sampling*InnerProd(Edata, E, SnA)
  print "SnrA = ", SnrA/normA, "  SnrE = ", SnrE/normE
  #print "normA = ", normA, "  normE = ", normE
  #print "innerProda = ",  sampling*InnerProd(Adata, A, Sa), "innerProde = ",  sampling*InnerProd(Edata, E, Se)
  Snr = (SnrA + SnrE)/sqrt(normA*normA + normE*normE)
  print 80*'='
  print "using key file we get"
  #print "xi = ", xi2
  print "chi^2 = ", chi2
  print "combined SNR = ", Snr
  print 80*'='

  ###########################################
  ##   Dealing with result files
  ###########################################
  for userfile in Detfiles:
   # print userfile
   if ( re.search('key', userfile) == None):
       print 80*'*'
       print 4*" ", "Dealing with %s file..." % userfile 
       print 80*'*'
       Dettdifile = lisaxml.readXML(userfile)
       Detsources = Dettdifile.getLISASources()[0]
       Tc = Detsources.CoalescenceTime
       if (Tc <= 1.03e7):
           src = 1
       elif (Tc >= 1.5e7 and Tc <= 2.e7):
           src = 4
       elif (Tc >= 4.e7 and Tc<=5.e7):
           src = 5
       elif (Tc>=6.e7 and Tc < 6.7e7):
           src = 2
       print "BBH # ", BBH, " src # ", src 
       if (src == BBH):
           Dettdi =  Dettdifile.getTDIObservables()[0]
           Xs = Dettdi.Xf
           As = (2.0*Dettdi.Xf - Dettdi.Yf - Dettdi.Zf)/3.0
           Es = (Dettdi.Zf - Dettdi.Yf)/math.sqrt(3.0)
	   normXs = ComputeNorm(Xs, sampling, SnX)
           normAs = ComputeNorm(As, sampling, SnA)
           normEs = ComputeNorm(Es, sampling, SnA)
      #     Resfilout = open("AEresCheck1.dat", 'w') 
      #     for i in xrange(len(A)):
      ##        record = str(sampling*float(i)) + spr + str(As[i]) + spr + str(Es[i]) + "\n"
      #        Resfilout.write(record)
       #    Resfilout.close() 
	 
	   ### computing chi^2

           diffAsAd = synthlisa.spect(Adata - As,sampling,0) 
           diffEsEd = synthlisa.spect(Edata - Es,sampling,0) 

           # Computing combined SNR of a difference:
           SnrA =  2.0 * numpy.sum(diffAsAd[1:,1] / SnA) # square SNR_Adiff
           SnrE =  2.0 * numpy.sum(diffEsEd[1:,1] / SnA)
           SnrA = ComputeNorm(Adata-As, sampling, SnA)**2
           SnrE = ComputeNorm(Edata-Es, sampling, SnA)**2
           SnrDiff = sqrt( SnrA**2 + SnrE**2 )

           chi2 = 0.5*(SnrA + SnrE)/(ind - Dfr)
           SnrAdif = ComputeNorm(A-As, sampling, SnA)**2
           SnrEdif = ComputeNorm(E-Es, sampling, SnA)**2
 
           d_chi2 = 0.5*(SnrAdif + SnrEdif)/Dfr

   	   print "chi^2 = ", chi2
           print "d_chi^2 = ", d_chi2, SnrAdif/Dfr, SnrEdif/Dfr
           #Computing combined SNR
 
           SnrAs = sampling*InnerProd(Adata, As, SnA)
           SnrEs = sampling*InnerProd(Edata, Es, SnA)

           Snrs = (SnrAs + SnrEs)/sqrt(normAs*normAs + normEs*normEs)

           print "SNRA = ", SnrAs/normAs, " SNRE = ", SnrEs/normEs
           print "combined SNR  = ", Snrs
        
           # Computing overlaps
       
           olapA = sampling*InnerProd(A,As,SnA)/(normA*normAs)
           olapE = sampling*InnerProd(E,Es,SnA)/(normE*normEs)
    
           print "overlap between A tdis  = ", olapA
           print "overlap between E tdis  = ", olapE

           if options.phasemax:
              Nind = Injfile[-19:-18]
              injfile =  Injfile[:-20]+'_0-'+Nind+'-tdi-frequency.xml'
              injfile =  Injfile[:-18]+'_0'+'-tdi-frequency.xml'
              Injtdifile = lisaxml.readXML(injfile)
              tdi0 = Injtdifile.getTDIObservables()[0]
              X0 = tdi0.Xf

              normX0 = ComputeNorm(X0, sampling, SnX)

#	   olapTest = sampling*MaxInnerProd(Xpiby2, X0, Sx)/normXX0
#	   print "max olap between quadratures: ", olapTest, normXX0
#	   olap1 = sampling*InnerProd(X0, X0, Sx)/normXX0
#	   olap2 = sampling*InnerProd(X0, Xpiby2, Sx)/normXX0
#           print "test ;", olap1, olap2

	      quadr = sampling*MaxInnerProd(Xs, X0,  SnX)
   	      SnrXdifMin = (normXs**2 + normX0**2 - 2.0*quadr)/(normX0 * normXs)
	   
	      SnrXdiff = (normXs**2 + normX**2 - 2.0*sampling*InnerProd(Xs, X, SnX))/(normX * normXs)
	   
	      print "Non-minimized SNR of dX = ", SnrXdiff
	      print "Minimized over the phase SNR of dX = ", SnrXdifMin
	      print "Maximized overlap = ", quadr/(normX0 * normXs)

   

   
   

   

          

   
   

