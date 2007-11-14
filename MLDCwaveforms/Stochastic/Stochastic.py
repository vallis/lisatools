#!/usr/bin/env python

# wavearray = []; wavearray.append(NoiseWave(noisehp,noisehc,elat,elon,0)); WaveArray(wavearray)

import synthlisa

import lisaxml
import numpy
import math
import random

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
                   ('SpectralType',         'String',      'WhiteOmega', 'frequency dependence of PSD'),
                   ('PseudoRandomSeed',     '1',           0,       'seed for pseudorandom generators'),
                   ('InterpolationOrder',   '1',           1,       'interpolation order for pseudorandom noise') )
    
                   # some kind of random seed, too... some kind of sampling frequency...
    
    def __init__(self,name=''):
        super(Stochastic, self).__init__('Stochastic',name)
    
    def synthesize(self,samples,deltat,inittime):
        # set buffer length compatibly with TDI...
        bufferlen = min(256,2**int(math.log(160/deltat,2)))
        
        # set seed if not given
        if not hasattr(self,'PseudoRandomSeed') or self.PseudoRandomSeed == 0:
            self.PseudoRandomSeed = random.randint(0,2**30)
        
        # set hc seed from other seed
        random.seed(self.PseudoRandomSeed)
        seedhc = random.randint(0,2**30)
        
        sourcehp = synthlisa.WhiteNoiseSource(bufferlen,self.PseudoRandomSeed,math.sqrt(self.PowerSpectralDensity)*math.sqrt(0.5/deltat))
        sourcehc = synthlisa.WhiteNoiseSource(bufferlen,seedhc,               math.sqrt(self.PowerSpectralDensity)*math.sqrt(0.5/deltat))
        
        if self.SpectralType == 'White':
            myfilter = synthlisa.NoFilter()
        elif self.SpectralType == 'WhiteOmega':
            filtera = [1, 1.50000000000000, -6.76587301587302, -10.7738095238095,
                       20.2827380952381, 34.8402777777778, -35.3437500000000,
                       -67.0312500000000, 39.3554687500000, 85.4213169642857,
                       -28.7820870535714, -75.9643167162698, 13.3829752604167,
                       48.3846028645833, -3.25995551215278, -22.2980957031250,
                       -0.178133283342634, 7.42222013927641, 0.445333208356585,
                       -1.76197052001953, -0.166408326890734, 0.290992101033529,
                       0.0333706537882487, -0.0320993907867916, -0.00393296991075788,
                       0.00221676485879081, 0.000263114770253499, -0.0000861287117004395,
                       -8.86619091033936e-6, 1.54806507958306e-6, 
                       1.14597025371733e-7, -8.16015970139276e-9,
                       -2.40222092658754e-10]
            
            filterb = [0, -1.50000000000000, -6.76587301587302, 10.7738095238095,
                       20.2827380952381, -34.8402777777778, -35.3437500000000,
                       67.0312500000000, 39.3554687500000, -85.4213169642857,
                       -28.7820870535714, 75.9643167162698, 13.3829752604167,
                       -48.3846028645833, -3.25995551215278, 22.2980957031250,
                       -0.178133283342634, -7.42222013927641, 0.445333208356585,
                       1.76197052001953, -0.166408326890734, -0.290992101033529,
                       0.0333706537882487, 0.0320993907867916, -0.00393296991075788,
                       -0.00221676485879081, 0.000263114770253499, 0.0000861287117004395,
                       -8.86619091033936e-6, -1.54806507958306e-6,
                       1.14597025371733e-7, 8.16015970139276e-9,
                       -2.40222092658754e-10]
            
            filtera = [1, 3.0/2, 5.0/12]
            filterb = [0, 3.0/2, -5.0/12]
                
            filterb = [0.9999 * x for x in filterb]
            
            myfilter = synthlisa.IIRFilter(filtera,filterb)
        else:
            raise NotImplementedError, "Unknown SpectralType %s" % self.SpectralType
        
        interp = synthlisa.getInterpolator(self.InterpolationOrder)
        
        # synthlisa has long-standing problems with prebuffering...
        noisehp = synthlisa.InterpolatedSignal(synthlisa.SignalFilter(256,sourcehp,myfilter),interp,deltat,-inittime)
        noisehc = synthlisa.InterpolatedSignal(synthlisa.SignalFilter(256,sourcehc,myfilter),interp,deltat,-inittime)
        
        return synthlisa.NoiseWave(noisehp,noisehc,self.EclipticLatitude,self.EclipticLongitude,self.Polarization)
    
    def waveforms(self,samples,deltat,inittime):
        noise = self.synthesize(samples,deltat,inittime) 
        
        [hp,hc] = numpy.transpose(synthlisa.getobs(samples,deltat,[noise.hp,noise.hc],inittime))
        
        return (hp,hc)
    
