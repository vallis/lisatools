import lisaxml
import numpy
import math

lisaxml.SourceClassModules['GalacticBinary'] = 'GalacticBinary'

class GalacticBinary(lisaxml.Source):
    """Returns a MLDC source that models the waveform emitted by a
    monochromatic galactic binary."""
    
    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description
    
    outputlist = ( ('EclipticLatitude',           'Radian',    None, 'standard ecliptic latitude'),
                   ('EclipticLongitude',          'Radian',    None, 'standard ecliptic longitude'),
                   ('Polarization',               'Radian',    None, 'standard source polarization'),
                   ('Frequency',                  'Hertz',     None, 'GW frequency'),
                   ('InitialPhase',               'Radian',    None, 'GW phase at t = 0'),
                   ('Inclination',                'Radian',    None, 'standard source inclination'),
                   ('Amplitude',                  '1',         None, 'dimensionless GW amplitude') )
    
    def __init__(self,name=''):
        super(GalacticBinary, self).__init__('GalacticBinary',name)
    
    def waveforms(self,samples,deltat,inittime):
        t = numpy.arange(inittime,inittime + deltat*samples,deltat,'d')    
        phase = 2.0 * math.pi * self.Frequency * t + self.InitialPhase
        
        Ap = self.Amplitude * (1.0 + math.cos(self.Inclination)**2)
        Ac = -2.0 * self.Amplitude * math.cos(self.Inclination)        
        
        hp = Ap * numpy.cos(phase) 
        hc = Ac * numpy.sin(phase);
        
        return (hp,hc)
    
