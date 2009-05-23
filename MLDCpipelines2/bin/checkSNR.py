#!/usr/bin/env python

import sys, math
import numpy
import lisaxml, synthlisa

from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] TDIFILE.xml [COMPARISONFILE.xml ...]",
                      version="$Id:  $")

parser.add_option("-g", "--includeGalaxy",
                  action="store_true", dest="includeGalaxy", default=False,
                  help="include galaxy")

parser.add_option("-m", "--minimumFrequency",
                  type="float", dest="minimumFrequency", default=1e-5,
                  help="minimum integration frequency (default 1e-5 Hz)")

parser.add_option("-M", "--maximumFrequency",
                  type="float", dest="maximumFrequency", default=1e-2,
                  help="maximum integration frequency (default 0.01 Hz)")

parser.add_option("-o", "--outputIntegrand",
                  type="string", dest="outputIntegrand", default=None,
                  help="output A and E normalized SNR integrands (give filename; experimental)")

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
    


@memoize
def lisanoise(fr):
    om = 2.0 * math.pi * fr
    L  = 16.6782
    
    Spm = 2.5e-48 * (1.0 + (fr/1.0e-4)**-2) * fr**(-2)
    Sop = 1.8e-37 * fr**2
    
    Sx  = 16.0 * numpy.sin(om*L)**2 * (2.0 * (1.0 + numpy.cos(om*L)**2) * Spm + Sop)
    Sxy = -4.0 * numpy.sin(2.0*om*L) * numpy.sin(om*L) * (Sop + 4.0*Spm)
    
    Sa = 2.0 * (Sx - Sxy)/3.0
    
    if not options.includeGalaxy:
        return Sa
    else:
        Sgal = (2.0*L*om)**2 * 4.0 * numpy.sin(om*L)**2 * (
            numpy.piecewise(fr,(fr >= 1.0e-5  ) & (fr < 1.0e-3  ),[lambda f: 10**-44.62 * f**-2.3, 0]) + \
            numpy.piecewise(fr,(fr >= 1.0e-3  ) & (fr < 10**-2.7),[lambda f: 10**-50.92 * f**-4.4, 0]) + \
            numpy.piecewise(fr,(fr >= 10**-2.7) & (fr < 10**-2.4),[lambda f: 10**-62.8  * f**-8.8, 0]) + \
            numpy.piecewise(fr,(fr >= 10**-2.4) & (fr < 10**-2.0),[lambda f: 10**-89.68 * f**-20.0,0])     )  
            
        return Sa + Sgal/3.0


class tdifile:
    fLow  = options.minimumFrequency
    fHigh = options.maximumFrequency
    
    def __init__(self,file):
        # use only the first source in each file
        tdidata = lisaxml.readXML(arg).getTDIObservables()[0]
        
        A = (2.0*tdidata.Xf - tdidata.Yf - tdidata.Zf) / 3.0
        E = (tdidata.Zf - tdidata.Yf) / math.sqrt(3.0)
        
        self.dt = tdidata.TimeSeries.Cadence
        self.len = len(A)
        
        self.At = numpy.fft.fft(A)
        self.Et = numpy.fft.fft(E)
        
        self.df = 1.0 / (self.dt * self.len)
        self.nyquistf = 0.5 / self.dt
        
        self.f = numpy.concatenate( (
            numpy.linspace(0.0,self.nyquistf,self.len/2+1),
            numpy.linspace(-(self.nyquistf-self.df),self.df,self.len/2-1)
        ) )
    
    def innerproduct(self,other):
        assert self.dt  == other.dt
        assert self.len == other.len
        
        lowi  = max(1,self.fLow/self.df)
        highi = min(self.len/2,self.fHigh/self.df)
        
        sn = lisanoise(self.f[lowi:highi])
        
        if options.outputIntegrand:
            intA = 4.0 / (self.len**2 * self.df) * numpy.conj(self.At[lowi:highi]) * other.At[lowi:highi] / sn
            intE = 4.0 / (self.len**2 * self.df) * numpy.conj(self.Et[lowi:highi]) * other.Et[lowi:highi] / sn
            
            ints = numpy.concatenate((intA,intE))
            ints.tofile(options.outputIntegrand)
            
            innerA = numpy.sum(intA); innerE = numpy.sum(intE)
        else:
            innerA = 4.0 / (self.len**2 * self.df) * numpy.sum(numpy.conj(self.At[lowi:highi]) * other.At[lowi:highi] / sn)
            innerE = 4.0 / (self.len**2 * self.df) * numpy.sum(numpy.conj(self.Et[lowi:highi]) * other.Et[lowi:highi] / sn)
        
        return innerA.real, innerE.real
    
    def snr(self):
        snrA, snrE = map(math.sqrt,self.innerproduct(self))
        snrAE = math.sqrt(snrA**2 + snrE**2)
        
        return snrA, snrE, snrAE
    

def tabprint(tup):
    for t in tup:
        print t,
        
    print

fs = [tdifile(arg) for arg in args]

snrs0 = fs[0].snr()

if len(fs) == 1:
    tabprint(snrs0)
else:
    for f in fs[1:]:
        snrs = f.snr()
        prod = f.innerproduct(fs[0])
    
        tabprint( ( prod[0]/snrs0[0], prod[1]/snrs0[1], # SNR_A, SNR_E
                    (prod[0]+prod[1])/snrs0[2],         # SNR_AE
                    prod[0]/(snrs0[0] * snrs[0]),       # overlap_A
                    prod[1]/(snrs0[1] * snrs[1]) ) )    # overlap_E
