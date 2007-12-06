#!/usr/bin/env python

import numpy
import math
import random

import synthlisa

import lisaxml2 as lisaxml

lisaxml.SourceClassModules['PseudoRandomNoise'] = 'PseudoRandomNoise'

class PseudoRandomNoise(lisaxml.XSILobject): 
    """Returns a synthLISA noise object. Requires synthlisa."""
    
    outputlist = ( ('PowerSpectralDensity', '(f/Hz)^n/Hz', None,    'PSD normalization at 1 Hz'),
                   ('SpectralType',         'String',      None,    'WhitePhase,WhiteFrequency,WhiteAcceleration,PinkAcceleration'),
                   ('Fknee',                'Hertz',       1e-4,    'knee frequency for PinkAcceleration'),
                   ('PseudoRandomSeed',     '1',           0,       'seed for pseudorandom generators'),
                   ('GenerationTimeStep',   'Second',      1,       'Timestep of generation'),
                   ('InterpolationOrder',   '1',           1,       'interpolation order for pseudorandom noise') )
    
    def __init__(self,name=''):
        super(PseudoRandomNoise, self).__init__('Noise',name)
        
        self.SourceType = 'PseudoRandomNoise'
    
    # in a future version it would be good to take into account synthesize parameteres
    def synthesize(self):
        prebuffer = 8.0 * 17.0 + 2.0 * self.GenerationTimeStep
        
        if not hasattr(self,'PseudoRandomSeed') or self.PseudoRandomSeed == 0:
            self.PseudoRandomSeed = random.randint(0,2**30)
        
        psd = self.PowerSpectralDensity
        
        if self.SpectralType == 'PinkAcceleration':
            seed1 = self.PseudoRandomSeed
            random.seed(seed1); seed2 = random.randint(0,2**30)
            
            comp1 = synthlisa.PowerLawNoise(self.GenerationTimeStep,prebuffer,self.PowerSpectralDensity,-2.0,
                                            self.InterpolationOrder,seed1)
            # the 10 here is hackish... original had 100 and 1000
            comp2 = synthlisa.PowerLawNoise(10*self.GenerationTimeStep,prebuffer,
                                            self.PowerSpectralDensity * self.Fknee**2,-4.0,
                                            self.InterpolationOrder,seed2)
            
            return synthlisa.SumSignal(comp1,comp2)
        if self.SpectralType   == 'WhiteAcceleration':
            slope = -2.0
        elif self.SpectralType == 'WhitePhase':
            slope = 2.0
        elif self.SpectralType == 'WhiteFrequency':
            slope = 0.0
        else:
            raise NotImplementedError, "Unknown noise spectral shape"
        
        return synthlisa.PowerLawNoise(self.GenerationTimeStep,prebuffer,self.PowerSpectralDensity,slope,
                                       self.InterpolationOrder,self.PseudoRandomSeed)
    
