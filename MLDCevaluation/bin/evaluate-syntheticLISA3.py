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

parser = OptionParser(usage="""usage: %prog [options] OutputFile BlindData.xml ChallengeNTDI_Key.xml Result1TDI.xml Result2TDI.xml ...
!!!! IMPORTANT !!!! if you do not use key file, please provide a dummy xml file anyway""",
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

if len(args) < 4:
    parser.error("""USAGE: evaluate-syntheticLISA3.py output file, data file, key file and at least one input file <options>
    !!!! IMPORTANT !!!! if you do not use key file, please provide a dummy xml file, it won't be used""")

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

#print "Detfiles"
#print Detfiles

# To do: Need to read quadrutures as well...
if options.phasemax :
   if ( re.search('challenge3.2', Datafile) == None ):
	print "phase maximization can be done only for 3.2 chalenge"
	sys.exit(1)


challname = ""
if ( re.search('challenge3.2', Datafile) == None ):
  challname = "3.2"
elif ( re.search('challenge3.3', Datafile) == None ):
  challname = "3.3"
elif ( re.search('challenge3.4', Datafile) == None ):
  challname = "3.4"
     
# Reading the data 

Datatdifile = lisaxml.readXML(Datafile)
tdiData = Datatdifile.getTDIObservables()[0]
Xdata = tdiData.Xf
Adata = (2.0*tdiData.Xf -tdiData.Yf - tdiData.Zf)/3.0
Edata = (tdiData.Zf - tdiData.Yf)/math.sqrt(3.0) 

sampling = tdiData.TimeSeries.Cadence
samples = Numeric.shape(Xdata)[0]


Specdat = synthlisa.spect(Adata,sampling,0)
fr = Specdat[1:,0]
PSDdat = Specdat[1:,1]


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


"""
EMRIchal = False

if(re.search('challenge1.3', Injfile) != None):
    EMRIchal = True
elif(re.search('challenge1B.3', Injfile) != None):
    EMRIchal = True


if (re.search('challenge2.2', Injfile) != None):
    Dfr = 9.
elif (EMRIchal):
    Dfr = 14.
"""
# reading Key file

if (options.usekey):
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
   

"""
Sigfilout = open("AEsigCheck1.dat", 'w') 
for i in xrange(len(A)):
   record = str(sampling*float(i)) + spr + str(A[i]) + spr + str(E[i]) + "\n"
   Sigfilout.write(record)
Sigfilout.close()   
sys.exit(0);
"""
"""
if (EMRIchal):
   IndX = len(A)
   for i in xrange(len(A)-1, 1, -1):
      if (A[i] != 0.0):
         IndX = i
         print "IndX = ", IndX 
      break
   A[IndX-20:] = 0.0
   E[IndX-20:] = 0.0
"""

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


Sgal = (2.0*L*om)**2 * 4.0 * numpy.sin(om*L)**2 * (
          numpy.piecewise(fr,(fr >= 1.0e-4  ) & (fr < 1.0e-3  ),[lambda f: 10**-44.62 * f**-2.3, 0]) + \
          numpy.piecewise(fr,(fr >= 1.0e-3  ) & (fr < 10**-2.7),[lambda f: 10**-50.92 * f**-4.4, 0]) + \
          numpy.piecewise(fr,(fr >= 10**-2.7) & (fr < 10**-2.4),[lambda f: 10**-62.8  * f**-8.8, 0]) + \
          numpy.piecewise(fr,(fr >= 10**-2.4) & (fr < 10**-2.0),[lambda f: 10**-89.68 * f**-20.0,0])     )


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

if (options.usekey):
  normX = ComputeNorm(X,sampling, SnX)
  normE = ComputeNorm(E,sampling, SnA)


# start evaluation


fout = open(fileout, 'w')

if (options.usekey):
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
       Xs = Dettdi.Xf
       As = (2.0*Dettdi.Xf - Dettdi.Yf - Dettdi.Zf)/3.0
       Es = (Dettdi.Zf - Dettdi.Yf)/math.sqrt(3.0)
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
          
          #sys.exit(0)
       
    
   
 

   
   

   

          

   
   

