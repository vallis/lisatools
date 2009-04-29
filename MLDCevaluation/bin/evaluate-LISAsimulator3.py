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
nyquistf = 0.5/15.0

spr = "     "


## overlap maximized over the phase
## first argument is a signal second is zero phase template
def MaxInnerProd(ser1, ser2, PSD):
  size = Numeric.shape(ser1)[0]
  pdlen = size/2
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

parser = OptionParser(usage="usage: %prog [options] OutputFile BlindData.xml ChallengeNTDI_Key.xml Result1TDI.xml Result2TDI.xml ...",
                      version="$Id:  $")


parser.add_option("-p", "--maxPhase",
                  action="store_true", dest="phasemax", default=False,
                  help="maximizes over the phase on a single channel [off by default]")

parser.add_option("-g", "--Galaxy",
                  action="store_true", dest="gal", default=False,
                  help="Adding the Galaxy to the instrumental noise [off by default]")
                  
parser.add_option("-k", "--usekey",
                  action="store_true", dest="usekey", default=False,
                  help="use key file in the evaluation results as well [off by default]")


(options, args) = parser.parse_args()

#!!!! currently we support only a single source parameter file  !!!

if len(args) < 3:
    parser.error("USAGE: evaluate-syntheticLISA2.py  output file, data file, key file and at leat one input file <options>")



fileout = args[0]
Datafile = args[1]
Injfile = args[2]
Detfiles = args[3:]


sz = len(Detfiles)
if (sz > 0):
   Detfiles[0] = Detfiles[0][1:-1]
   for i in xrange(sz):
      if i !=0:
         Detfiles[i] = Detfiles[i][:-1]



# To do: Need to read quadrutures as well...
if options.phasemax :
   if ( re.search('challenge3.2', Injfile) == None ):
	print "phase maximization can be done only for 3.2 chalenge"
	sys.exit(1)


# Reading the data 

Datatdifile = lisaxml.readXML(Datafile)
tdiData = Datatdifile.getTDIObservables()[0]
Xdata = tdiData.Xs
Adata = (2.0*tdiData.Xs -tdiData.Ys - tdiData.Zs)/3.0
Edata = (tdiData.Zs - tdiData.Ys)/math.sqrt(3.0) 

sampling = tdiData.TimeSeries.Cadence
samples = Numeric.shape(Xdata)[0]

nyquistf = 0.5/sampling
print "Cadence = ", sampling, "   Nyquist freq = ", nyquistf

print "Deaing with data file:", Datafile
challname = ""
if ( re.search('challenge3.2', Datafile) != None ):
  challname = "3.2"
elif ( re.search('challenge3.3', Datafile) != None ):
  challname = "3.3"
elif ( re.search('challenge3.4', Datafile) != None ):
  challname = "3.4"
  fHigh = nyquistf
  print "challenge3.4 -> upper frequency is extended to nyquist frequency"

Specdat = synthlisa.spect(Adata,sampling,0)
fr = Specdat[1:,0]
PSDdat = Specdat[1:,1]

# computing useful number of points
pdlen3 = samples/2
freqs = Numeric.arange(0,pdlen3+1,dtype='d') * (nyquistf / pdlen3)
ind = 0
for i in xrange(pdlen3):
    if (freqs[i] > fLow and freqs[i]<= fHigh):
        ind += 1    

ind = ind*2
print "data size = ", samples
print "number of used points ", ind, "corresponds to the freq range: ", fLow, "-", fHigh

if (re.search('challenge2.2', Injfile) != None):
    Dfr = 9.
elif (re.search('challenge1.3', Injfile) != None):
    Dfr = 14.

# reading Key file
if (options.usekey):
   Injtdifile = lisaxml.readXML(Injfile)
   Injsources = Injtdifile.getLISASources()[0]
   tdi = Injtdifile.getTDIObservables()[0]
   Injtdifile.close()
   sampling = tdi.TimeSeries.Cadence
   X = tdi.Xs
   A = (2.0*tdi.Xs -tdi.Ys - tdi.Zs)/3.0
   E = (tdi.Zs - tdi.Ys)/math.sqrt(3.0)
#XX = synthlisa.spect(X, sampling, 0)
#fr = XX[1:,0]


"""
Sigfilout = open("AEsigCheck1.dat", 'w') 
for i in xrange(len(A)):
   record = str(sampling*float(i)) + spr + str(A[i]) + spr + str(E[i]) + "\n"
   Sigfilout.write(record)
Sigfilout.close()   
sys.exit(0);
"""

# Compute noise spectral density

om = 2.0 * pi * fr
L  = 16.6782

# instrument noise

Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
Sop = 1.8e-37 * fr**2

Synx = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)
Synxy = -4.0 * numpy.sin(2.0*om*L)*numpy.sin(om*L) * ( Sop + 4.*Spm )
Syn = 2.0 * (Synx - Synxy)/3.0

SnX = Synx/(4.0*L*L*om*om)
SnA = Syn/(4.0*L*L*om*om)




#Se = Sa
# expression from Phys. Rev. D 73, 122001 (2006), corrected and 
# modified to include TDI response function
# for fractional frequency fluctuations

# conversion LS to SL (incl.deriv.) -- TDI response function Michelson to X,Y,Z
Sgal = 4.*numpy.sin(om*L)**2 * (
         numpy.piecewise(fr,(fr >= 1.0e-5  ) & (fr < 1.0e-3  ),[lambda f: 10**-44.62 * f**-2.3, 0]) + \
         numpy.piecewise(fr,(fr >= 1.0e-3  ) & (fr < 10**-2.7),[lambda f: 10**-50.92 * f**-4.4, 0]) + \
         numpy.piecewise(fr,(fr >= 10**-2.7) & (fr < 10**-2.4),[lambda f: 10**-62.8  * f**-8.8, 0]) + \
         numpy.piecewise(fr,(fr >= 10**-2.4) & (fr < 10**-2.0),[lambda f: 10**-89.68 * f**-20.0,0])     )


#Sgal = (16./3.)*numpy.sin(om*L)**2 * (
#     numpy.piecewise(fr,(fr >= 1.0e-4  ) & (fr <= 4.0e-4  ),[lambda f: 2.5e-36*(f/1.0e-4)**-1.83, 0]) + \
#     numpy.piecewise(fr,(fr > 4.0e-4  ) & (fr <= 1.15e-3  ),[lambda f: 1.95e-37*(f/4.0e-4)**-2.93, 0]) + \
#     numpy.piecewise(fr,(fr > 1.15e-3  ) & (fr <= 2.0e-3  ),[lambda f: 8.86e-39*(f/1.15e-3)**-4.11, 0]) + \
#     numpy.piecewise(fr,(fr > 2.0e-3  ) & (fr <= 2.3e-3  ),[lambda f: 9.0e-40*(f/2.0e-3)**-7.09, 0]) + \
#     numpy.piecewise(fr,(fr > 2.3e-3  ) & (fr <= 2.5e-3  ),[lambda f: 3.34e-40*(f/2.3e-3)**-13.15, 0]) + \
#     numpy.piecewise(fr,(fr > 2.5e-3  ) ,[lambda f: 1.12e-40*(f/2.5e-3)**-23.0, 0]) 
#)

if (options.gal):
    SnX = SnX + Sgal
    SnA = SnA + Sgal   # for galaxy Sxy = -1/2 Sx

# clipping: check (roughly) if first null is within the computed range
highf = 0.01
highi = int(highf / fr[0])
if  fr[-1] > highf and highi > 1:
    # then find the minimum of the noise curve up to highf and clip to that minimum value
      minSn = numpy.min(SnA[1:highi])
      SnA = numpy.maximum(SnA,minSn)
      print "Clipping noise PSD to %s" % minSn


#fout = open("NewGalaxy.dat", 'w')
#for i in xrange(len(fr)):
#  record = str(fr[i]) + "    " + str(SnX[i]) + "    " + str(SnA[i]) + "\n"
#  fout.write(record)
#fout.close()
#sys.exit(0)

#foutS = open("PsdTest.dat",'w')
#for i in xrange(len(fr)):
#   record = str(fr[i]) + spr + str(PSDdat[i]) + spr + str(SnA[i]) + "\n"
#   foutS.write(record)
#foutS.close()

   

if (options.usekey):
   normA = ComputeNorm(A,sampling, SnA)
  # normX = ComputeNorm(X,sampling, SnX)
   normE = ComputeNorm(E,sampling, SnA)

   # start evaluation


fout = open(fileout, 'w')
BBH = 1
if (options.usekey and challname == "3.2"):
   # use the key file   
   rec = 80*'*' + "\n" + "using key file: "+ Injfile + "\n" + Injsources.name + "\n"+ 80*'*' + "\n"
   print rec
   fout.write(rec)
   Tc = Injsources.CoalescenceTime
   if (Tc <= 1.0368e7):
      BBH = 1
   #elif (Tc >= 1.5552e7 and Tc <= 6.2208e7):
   elif (Tc >= 1.55e7 and Tc <= 3.0e7):
      BBH = 3
   elif (Tc >= 3e7 and Tc <= 5.0e7):
      BBH = 4   
   elif (Tc >= 6.9984e7 and Tc<=7.2576e7):
      BBH = 6
   elif (Tc>=6.48e7 and  Tc < 6.7392e7 ):
      BBH = 2
   #############################################
   ## Computing chi^2 and SNR with the key file
   ##############################################
   # Computing combined log likelihood:
   
   InnerA = sampling*InnerProd(Adata, A, SnA)
   InnerE = sampling*InnerProd(Edata, E, SnA)
   normA =  sampling*InnerProd(A, A, SnA)
   normE =  sampling*InnerProd(E, E, SnA)

      #normA = ComputeNorm(A,sampling, SnA)
      #normE = ComputeNorm(E,sampling, SnA)
   rec = 80*'='+"\n"
   print rec
   fout.write(rec)
   logL = (InnerA + InnerE) - 0.5*(normA + normE)
   rec = "LogL_comb  =  " + str(logL) 
   print rec
   fout.write(rec+"\n")
   rec = "computing snr -> logL maximized over amplitude (1/DL) \n"
   print rec
   fout.write(rec)
   SNRA = InnerA/math.sqrt(normA)
   SNRE = InnerE/math.sqrt(normE)
   rec = "SNR_A = " + str(SNRA) + "   SNR_E = " + str(SNRE)
   print rec
   fout.write(rec + "\n")
   SNRcomb = (InnerA + InnerE)/math.sqrt(normA + normE)
   rec = "SNR_comb = " + str(SNRcomb)
   print rec
   fout.write(rec + "\n")
   rec = 80*'='+"\n"
   print rec
   fout.write(rec)

if (options.usekey and challname == "3.4"):   
   rec = 80*'*' + "\n" + "using key file: "+ Injfile + "\n" + Injsources.name + "\n"+ 80*'*' + "\n"
   print rec
   fout.write(rec)
   InnerA = sampling*InnerProd(Adata, A, SnA)
   InnerE = sampling*InnerProd(Edata, E, SnA)
   normA =  sampling*InnerProd(A, A, SnA)
   normE =  sampling*InnerProd(E, E, SnA)
   rec = 80*'='+"\n"
   print rec
   fout.write(rec)
   logL = (InnerA + InnerE) - 0.5*(normA + normE)
   rec = "LogL_comb  =  " + str(logL) 
   print rec
   fout.write(rec+"\n")
   rec = "computing snr -> logL maximized over amplitude (1/DL) \n"
   print rec
   fout.write(rec)
   SNRA = InnerA/math.sqrt(normA)
   SNRE = InnerE/math.sqrt(normE)
   rec = "SNR_A = " + str(SNRA) + "   SNR_E = " + str(SNRE)
   print rec
   fout.write(rec + "\n")
   SNRcomb = (InnerA + InnerE)/math.sqrt(normA + normE)
   rec = "SNR_comb = " + str(SNRcomb)
   print rec
   fout.write(rec + "\n")
   rec = 80*'='+"\n"
   print rec
   fout.write(rec)
   
SMBH = False
EMRI = False   

for userfile in Detfiles:
      # print userfile
   if ( re.search('key', userfile) == None):
       rec = 80*'*' + "\n" + 10*" " + "Dealing with " + userfile + " file... \n" 
       Dettdifile = lisaxml.readXML(userfile)
       Detsources = Dettdifile.getLISASources()[0]
       rec = rec + "source name: " + Detsources.name + "\n"
       #   foutS = open("SignalTest.dat",'w')
       #   for i in xrange(len(As)):
       #       #record = str(sampling*float(i)) + spr + str(Xs[i]) + spr + str(As[i]) + spr + str(Es[i]) + "\n"
       #       record = str(sampling*float(i)) + spr + str(Dettdi.Xf[i]) + spr + str(Dettdi.Yf[i]) + spr + str(Dettdi.Zf[i]) + "\n"
       #       foutS.write(record)
       #   foutS.close()
       fout.write(rec)
       print rec
       if (re.search('SMBH', Detsources.name)):
         src = BBH
         SMBH = True
         if (options.usekey):
            Tc = Detsources.CoalescenceTime
            if (Tc <= 1.0368e7):
               src = 1
         #elif (Tc >= 1.5552e7 and Tc <= 6.2208e7):
         #     src = 3.4
            elif (Tc >= 1.55e7 and Tc <= 3.0e7):
               src = 3
            elif (Tc >= 3e7 and Tc <= 5.0e7):
               src = 4   
          # elif (Tc >= 4.2768e7 <= 5.0544e7):
        #    src = 5   
            elif (Tc >= 6.9984e7 and Tc<=7.2576e7):
               src = 6
            elif (Tc>=6.48e7 and  Tc < 6.7392e7 ):
               src = 2
            rec = "According to Tc-classification: BBH_key #  " +  str(BBH) +  " and  BBH_src  #  " + str(src) + "\n"
            print rec
            fout.write(rec)

       if (SMBH and src != BBH):
           continue
       Dettdi =  Dettdifile.getTDIObservables()[0]
       Xs = Dettdi.Xs
       As = (2.0*Dettdi.Xs - Dettdi.Ys - Dettdi.Zs)/3.0
       Es = (Dettdi.Zs - Dettdi.Ys)/math.sqrt(3.0)
       InnerA = sampling*InnerProd(Adata, As, SnA)
       InnerE = sampling*InnerProd(Edata, Es, SnA)
       normAs =  sampling*InnerProd(As, As, SnA)
       normEs =  sampling*InnerProd(Es, Es, SnA)
       print "norms: ", normAs, normEs
       print "in prods: ", InnerA, InnerE
       logL = (InnerA + InnerE) - 0.5*(normAs + normEs)
       rec = "LogL_comb  =  " + str(logL) 
       print rec
       fout.write(rec+"\n")
       rec = "computing snr -> logL maximized over amplitude (1/DL) \n"
       print rec
       fout.write(rec)
       SNRA = InnerA/math.sqrt(normAs)
       SNRE = InnerE/math.sqrt(normEs)
       rec = "SNR_A = " + str(SNRA) + "   SNR_E = " + str(SNRE)
       print rec
       fout.write(rec + "\n")
       SNRcomb = (InnerA + InnerE)/math.sqrt(normAs + normEs)
       rec = "SNR_comb = " + str(SNRcomb)
       print rec
       if (options.usekey):
          InnerA = sampling*InnerProd(A, As, SnA)
          InnerE = sampling*InnerProd(E, Es, SnA)
          olapA = InnerA/math.sqrt(normAs*normA)
          olapE = InnerE/math.sqrt(normEs*normE)
          rec = "Overlap_A = " + str(olapA) + "   Overlap_E = " + str(olapE) + "\n"
          print rec
          fout.write(rec + "\n")



"""

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
          Xs = Dettdi.Xs
          As = (2.0*Dettdi.Xs - Dettdi.Ys - Dettdi.Zs)/3.0
          Es = (Dettdi.Zs - Dettdi.Ys)/math.sqrt(3.0)
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

          print "SNRA = ", SnrAs/normAs, "  SNRE = ", SnrEs/normEs
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

  SnrA = sampling*InnerProd(Adata, A, SnA)/normA
  SnrE = sampling*InnerProd(Edata, E, SnA)/normE
  print "SnrA = ", SnrA, "  SnrE = ", SnrE
  #print "normA = ", normA, "  normE = ", normE
  #print "innerProda = ",  sampling*InnerProd(Adata, A, Sa), "innerProde = ",  sampling*InnerProd(Edata, E, Se)
  Snr = sqrt(SnrA**2 + SnrE**2)
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
           Xs = Dettdi.Xs
           As = (2.0*Dettdi.Xs - Dettdi.Ys - Dettdi.Zs)/3.0
           Es = (Dettdi.Zs - Dettdi.Ys)/math.sqrt(3.0)
	   normXs = ComputeNorm(Xs, sampling, SnX)
           normAs = ComputeNorm(As, sampling, SnA)
           normEs = ComputeNorm(Es, sampling, SnA)
 #          Resfilout = open("AEresCheck1.dat", 'w') 
 #          for i in xrange(len(A)):
 #             record = str(sampling*float(i)) + spr + str(As[i]) + spr + str(Es[i]) + "\n"
 #             Resfilout.write(record)
 #          Resfilout.close() 
	 
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
              Nind = Injfile[-16:-15]
              injfile =  Injfile[:-17]+'_0-'+Nind+'-tdi-strain.xml'
              injfile =  Injfile[:-15]+'_0'+'-tdi-strain.xml'
              Injtdifile = lisaxml.readXML(injfile)
              tdi0 = Injtdifile.getTDIObservables()[0]
              X0 = tdi0.Xs

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
"""
   

   
 

   

          

   
   

