#!/usr/bin/env python

import sys, math
import numpy
import lisaxml2 as lisaxml
import synthlisa

from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] TDIFILE.xml [COMPARISONFILE.xml ...]",
                      version="$Id:  $")

parser.add_option("-g", "--includeGalaxy",
                  action="store_true", dest="includeGalaxy", default=False,
                  help="include galaxy")

parser.add_option("-T", "--useT",
                  action="store_true", dest="useT", default=False,
                  help="use the T TDI combination in addition to A and E")

parser.add_option("-m", "--minimumFrequency",
                  type="float", dest="minimumFrequency", default=1e-5,
                  help="minimum integration frequency (default 1e-5 Hz)")

parser.add_option("-M", "--maximumFrequency",
                  type="float", dest="maximumFrequency", default=1e-2,
                  help="maximum integration frequency (default 0.01 Hz)")

parser.add_option("-N", "--useNyquist",
                  action="store_true", dest="useNyquist", default=False,
                  help="use Nyquist as maximum integration frequency (false by default)")

parser.add_option("-C", "--noClipping",
                  action="store_true", dest="noClipping", default=False,
                  help="disable clipping of noise PSD (false by default)")

parser.add_option("-o", "--outputIntegrand",
                  type="string", dest="outputIntegrand", default=None,
                  help="output A, E, and T normalized SNR integrands (give filename; experimental)")

(options, args) = parser.parse_args()

class memoize:
    def __init__(self, function):
        self.function = function
        self.memoized = {}
    
    def __call__(self, *args):
        f = args[0]
        key = (f[0],len(f))
        
        try:
            return self.memoized[key]
        except KeyError:
            self.memoized[key] = self.function(f)
            return self.memoized[key]
    


# clipping: if the first null is within the computed range, find the minimum
# of the noise curve up to highf and clip to that minimum value

def simpleclip(fr,Sn):
    highf = 0.01
    highi = int(highf / fr[0])
    
    if fr[-1] > highf and highi > 1:
        minSn = numpy.min(Sn[1:highi])
        Sn[:] = numpy.maximum(Sn,minSn)

def aetclip(fr,Sa,St):
    Sa[:] = numpy.maximum(Sa,1e-38 * fr**4)    
    St[:] = numpy.maximum(St,numpy.maximum(3e-47,1e-37 * fr**4))


@memoize
def lisanoise(fr):
    L = 16.6782
    x = 2.0 * math.pi * fr * L
    
    Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
    Sop = 1.8e-37 * fr**2
    
    Sx  = 16.0 * numpy.sin(x)**2 * (2.0 * (1.0 + numpy.cos(x)**2) * Spm + Sop)
    Sxy = -4.0 * numpy.sin(2.0*x) * numpy.sin(x) * (Sop + 4.0*Spm)
    
    St  = 16.0 * (1.0 - numpy.cos(x)) * numpy.sin(x)**2 * Sop + \
          128.0 * numpy.sin(0.5*x)**4 * numpy.sin(x)**2 * Spm
    
    Sa  = 2.0 * (Sx - Sxy)/3.0
    
    if not options.noClipping:
        aetclip(fr,Sa,St)
    
    if not options.includeGalaxy:
        return Sa, St
    else:
        Sgal = (2.0*x)**2 * 4.0 * numpy.sin(x)**2 * (
            numpy.piecewise(fr,(fr >= 1.0e-5  ) & (fr < 1.0e-3  ),[lambda f: 10**-44.62 * f**-2.3, 0]) + \
            numpy.piecewise(fr,(fr >= 1.0e-3  ) & (fr < 10**-2.7),[lambda f: 10**-50.92 * f**-4.4, 0]) + \
            numpy.piecewise(fr,(fr >= 10**-2.7) & (fr < 10**-2.4),[lambda f: 10**-62.8  * f**-8.8, 0]) + \
            numpy.piecewise(fr,(fr >= 10**-2.4) & (fr < 10**-2.0),[lambda f: 10**-89.68 * f**-20.0,0])     )
        
        # assuming no Galaxy noise in T
        return Sa + Sgal/3.0, St
    


class resprod(object):
    pass

class tdifile:
    fLow  = options.minimumFrequency
    fHigh = options.maximumFrequency
    
    def __init__(self,file):
        # use only the first source in each file
        tdidata = lisaxml.readXML(arg).getTDIObservables()[0]
        
        A = (2.0*tdidata.Xf - tdidata.Yf - tdidata.Zf) / 3.0
        E = (tdidata.Zf - tdidata.Yf) / math.sqrt(3.0)
        T = (tdidata.Xf + tdidata.Yf + tdidata.Zf) / math.sqrt(3.0)
        
        self.dt = tdidata.TimeSeries.Cadence
        if options.useNyquist:
            self.fHigh = 0.5/self.dt
        
        self.len = len(A)
        
        self.At = numpy.fft.fft(A)
        self.Et = numpy.fft.fft(E)
        self.Tt = numpy.fft.fft(T)
        
        self.df = 1.0 / (self.dt * self.len)
        self.nyquistf = 0.5 / self.dt
        
        self.f = numpy.concatenate( (
            numpy.linspace(0.0,self.nyquistf,self.len/2+1),
            numpy.linspace(-(self.nyquistf-self.df),self.df,self.len/2-1)
        ) )
    
    def product(self,other):
        assert self.dt  == other.dt
        assert self.len == other.len
        
        lowi  = max(1,self.fLow/self.df)
        highi = min(self.len/2,self.fHigh/self.df)
        
        sn, st = lisanoise(self.f[lowi:highi])
        
        intA = 4.0 / (self.len**2 * self.df) * numpy.conj(self.At[lowi:highi]) * other.At[lowi:highi] / sn
        intE = 4.0 / (self.len**2 * self.df) * numpy.conj(self.Et[lowi:highi]) * other.Et[lowi:highi] / sn
        intT = 4.0 / (self.len**2 * self.df) * numpy.conj(self.Tt[lowi:highi]) * other.Tt[lowi:highi] / st
        
        if options.outputIntegrand:
            # ints = numpy.concatenate((self.f[lowi:highi],sn,st))
            
            tdistr = 'f,Are,Aim,Ere,Eim,Tre,Tim'
            tdiobs = lisaxml.Observable(tdistr)
            tdiobs.FrequencySeries = lisaxml.FrequencySeries([self.f[lowi:highi],
                                                              intA.real,intA.imag,
                                                              intE.real,intE.imag,
                                                              intT.real,intT.imag],tdistr)
            
            tdiobs.DataType = 'FractionalFrequency'
            tdiobs.FrequencySeries.Cadence      = self.df
            tdiobs.FrequencySeries.Cadence_Unit = 'Hertz'
            tdiobs.FrequencySeries.FrequencyOffset      = self.f[lowi]
            tdiobs.FrequencySeries.FrequencyOffset_Unit = 'Hertz'
            
            outputXML = lisaxml.lisaXML(options.outputIntegrand)
            outputXML.TDIData(tdiobs)
            outputXML.close()            
            
        innerA = numpy.sum(intA); innerE = numpy.sum(intE); innerT = numpy.sum(intT)
        
        res = resprod()
        res.A = innerA.real; res.E = innerE.real; res.T = innerT.real
        
        return res
    
    def dosnr(self):
        res = self.product(self)
        self.snrA, self.snrE, self.snrT = map(math.sqrt,(res.A,res.E,res.T))
        
        self.snrAE = math.sqrt(self.snrA**2 + self.snrE**2)        
        self.snrAET = math.sqrt(self.snrA**2 + self.snrE**2 + self.snrT**2)
    


def tabprint(tup):
    for t in tup:
        print t,
        
    print

sigs = [tdifile(arg) for arg in args]

temp = sigs[0]
temp.dosnr()

if len(sigs) == 1:
    if options.useT:
        tabprint([temp.snrA,temp.snrE,temp.snrT,temp.snrAET])
    else:
        tabprint([temp.snrA,temp.snrE,temp.snrAET])
else:
    for sig in sigs[1:]:
        sig.dosnr()
        prod = sig.product(temp)
        
        if options.useT:
            tabprint( [ prod.A/temp.snrA, prod.E/temp.snrE, prod.T/temp.snrT,   # SNRs
                        (prod.A + prod.E + prod.T) / temp.snrAET,               # collective SNR
                        prod.A / (temp.snrA * sig.snrA),                        # correlations
                        prod.E / (temp.snrE * sig.snrE),
                        prod.T / (temp.snrT * sig.snrT) ] )
        else:
            tabprint( [ prod.A/temp.snrA, prod.E/temp.snrE,
                        (prod.A + prod.E) / temp.snrAE,
                        prod.A / (temp.snrA * sig.snrA),
                        prod.E / (temp.snrE * sig.snrE) ] )
