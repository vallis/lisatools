import numpy
import math

import sys
if 'lisaxml2' in sys.modules:
	import lisaxml2 as lisaxml
else:
	import lisaxml

lisaxml.SourceClassModules['GalacticBinary'] = 'GalacticBinary'

class GalacticBinary(lisaxml.Source):
    """Returns a MLDC source that models the waveform emitted by a
    monochromatic galactic binary."""
    
    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description
    
    # TO DO: added FrequencyDerivative, but will it be written out even if not defined?
    
    outputlist = ( ('Frequency',                  'Hertz',     None, 'GW frequency'),
                   ('FrequencyDerivative',        'Hertz/s',   0,    'GW frequency derivative'),
                   ('EclipticLatitude',           'Radian',    None, 'standard ecliptic latitude'),
                   ('EclipticLongitude',          'Radian',    None, 'standard ecliptic longitude'),
                   ('Amplitude',                  '1',         None, 'dimensionless GW amplitude'),
                   ('Inclination',                'Radian',    None, 'standard source inclination'),
                   ('Polarization',               'Radian',    None, 'standard source polarization'),
                   ('InitialPhase',               'Radian',    None, 'GW phase at t = 0') )
    
    def __init__(self,name='',init=None):
        super(GalacticBinary, self).__init__('GalacticBinary',name)
        
        # try to initialize the parameters from "init"
        if isinstance(init,(tuple,list)) and len(init) == len(self.outputlist):
            for i,value in enumerate(init):
                setattr(self,self.outputlist[i][0],value)
        elif isinstance(init,dict):
            for par in self.outputlist:
                if par[0] in init:
                    setattr(self,par[0],init[par[0]])
                elif par[2] != None:
                    setattr(self,par[0],par[2])
    
    def waveforms(self,samples,deltat,inittime):
        # would be better to use linspace, but should test it
        t = numpy.arange(inittime,inittime + deltat*samples,deltat,'d')
        
        if hasattr(self,'FrequencyDerivative') and self.FrequencyDerivative != 0:
            phase = 2.0 * math.pi * (self.Frequency * t + 0.5 * self.FrequencyDerivative * t**2) + self.InitialPhase
        else:
            phase = 2.0 * math.pi * self.Frequency * t + self.InitialPhase
        
        Ap = self.Amplitude * (1.0 + math.cos(self.Inclination)**2)
        Ac = -2.0 * self.Amplitude * math.cos(self.Inclination)
        
        hp = Ap * numpy.cos(phase) 
        hc = Ac * numpy.sin(phase)
        
        return (hp,hc)
    
    def synthesize(self,samples,deltat,inittime):
        if hasattr(self,'FrequencyDerivative') and self.FrequencyDerivative != 0:
            raise NotImplementedError, "synthlisa's SimpleBinary object cannot deal with FrequencyDerivative"
        
        import synthlisa
        return synthlisa.GalacticBinary(self.Frequency,self.FrequencyDerivative,
                                        self.EclipticLatitude,self.EclipticLongitude,self.Amplitude,
                                        self.Inclination,self.Polarization,self.InitialPhase)
    
