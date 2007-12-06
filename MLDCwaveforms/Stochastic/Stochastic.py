#!/usr/bin/env python

# wavearray = []; wavearray.append(NoiseWave(noisehp,noisehc,elat,elon,0)); WaveArray(wavearray)

import numpy
import math
import random

import synthlisa

# define BandIntFilter workaround for versions of synthlisa that do not have it

if not hasattr(synthlisa,'BandIntFilter'):
    def BandIntFilter(deltat,flow,fhi):
        r0 = math.pi * flow * deltat
        r1 = math.pi * fhi  * deltat

        alpha0 =  (1.0 + r1) / (1.0 + r0)
        alpha1 = -(1.0 - r1) / (1.0 + r0)
        beta1  =  (1.0 - r0) / (1.0 + r0)
        
        return synthlisa.IIRFilter([alpha0,alpha1],[0.0,beta1])
    
    synthlisa.BandIntFilter = BandIntFilter

import lisaxml

lisaxml.SourceClassModules['Stochastic'] = 'Stochastic'

class Stochastic(lisaxml.Source): 
    """Returns a MLDC source that models the waveform emitted by a
    monochromatic galactic binary. Requires synthlisa."""
    
    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description
    
    outputlist = ( ('EclipticLatitude',     'Radian',      None,    'standard ecliptic latitude'),
                   ('EclipticLongitude',    'Radian',      None,    'standard ecliptic longitude'),
                   ('Polarization',         'Radian',      0,       'standard source polarization'),
                   ('PowerSpectralDensity', '(f/Hz)^n/Hz', None,    'PSD normalization at 1 Hz'),
                   ('SpectralSlope',        '1',           -3,      'slope of PSD'),
                   ('Flow',                 'Hertz',       1e-5,    'minimum noise frequency'),
                   ('Fknee',                'Hertz',       1e-2,    'maximum noise frequency'),
                   ('PseudoRandomSeed',     '1',           0,       'seed for pseudorandom generators'),
                   ('InterpolationOrder',   '1',           1,       'interpolation order for pseudorandom noise') )
    
                   # some kind of random seed, too... some kind of sampling frequency...
    
    dopolarization = True
    
    def __init__(self,name=''):
        super(Stochastic, self).__init__('Stochastic',name)
    
    def synthesize(self,samples,deltat,inittime):
        if inittime < 0:
            prebuf = -inittime
        else:
            prebuf = 900.0
        
        # set seed if not given
        if not hasattr(self,'PseudoRandomSeed') or self.PseudoRandomSeed == 0:
            self.PseudoRandomSeed = random.randint(0,2**30)
        
        # set hc seed from other seed
        seedhp = self.PseudoRandomSeed
        random.seed(seedhp); seedhc = random.randint(0,2**30)
        
        noisehp = self.makealphanoise(deltat,prebuf,self.PowerSpectralDensity,-self.SpectralSlope,
                                      self.Flow,self.Fknee,self.InterpolationOrder,seedhp)
        noisehc = self.makealphanoise(deltat,prebuf,self.PowerSpectralDensity,-self.SpectralSlope,
                                      self.Flow,self.Fknee,self.InterpolationOrder,seedhc)
                
        interp = synthlisa.getInterpolator(self.InterpolationOrder)
        
        return synthlisa.NoiseWave(noisehp,noisehc,self.EclipticLatitude,self.EclipticLongitude,-self.Polarization)
    
    def waveforms(self,samples,deltat,inittime):
        noise = self.synthesize(samples,deltat,inittime) 
        
        [hp,hc] = numpy.transpose(synthlisa.getobsc(samples,deltat,[noise.hp,noise.hc],inittime))
        
        return (hp,hc)
    
    def makealphanoise(self,deltat,prebuf,psd,alpha,flow,fhi,interplen=1,seed=0,order=None):
        nyquistf = 0.5/deltat
        norm = math.sqrt(psd) * math.sqrt(nyquistf) / fhi**(alpha/2.0)
        sigbuffer = min(256,2**int(math.log(160/deltat,2)))
                
        wlow = 2 * math.pi * flow
        whi  = 2 * math.pi * fhi
        
        if order == None:
            order = int(1.5 * (math.log10(whi) - math.log10(wlow)) + 1)
        
        deltap = (math.log10(whi) - math.log10(wlow)) / order
        
        logp0 = math.log10(wlow) + 0.5 * (1.0 - 0.5 * alpha) * deltap
        
        logp = [logp0 + i * deltap for i in range(0,order)]
        logz = [logpi + 0.5 * alpha * deltap for logpi in logp]
        
        thenoise = synthlisa.WhiteNoiseSource(sigbuffer,0)
        
        for i in range(0,order):
            newbandfilter = synthlisa.BandIntFilter(deltat,10**logp[i]/(2 * math.pi),10**logz[i]/(2 * math.pi))
            thenoise = synthlisa.SignalFilter(sigbuffer,thenoise,newbandfilter)
        
        interp = synthlisa.getInterpolator(interplen)
        
        return synthlisa.InterpolatedSignal(thenoise,interp,deltat,prebuf,norm)
    

