%module BBH
%{
#include "BBHChallenge1.hh"
#include "SpinBBHWaveform.hh"
%}

// the following three lines allow the automatic conversion of a numpy array
// into a pointer to an array of doubles and the long length of the array
// each pair of these that appears in the library header file (e.g., BBHChallenge1.hh)
// needs to be named explicitly

%include numpy_typemaps.i
%typemap(in) (double *hPlus ,long hPlusLength ) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *hCross,long hCrossLength) = (double *numpyarray,long numpyarraysize);

# for the CoherentWave interface

%typemap(in) (double *phic,long phicLength ) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *fc , long fcLength   ) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *a   ,long aLength    ) = (double *numpyarray,long numpyarraysize);

// the following include defines all the calls that need to be wrapped by SWIG
// the include at the top serves only to provide definitions for this one;
// since it is between braces %{ %}, its content goes straight into
// the CPP wrapper file

%include "BBHChallenge1.hh"
%include "SpinBBHWaveform.hh"

// and here we can add some Python to the interface code

%pythoncode %{
import lisaxml
import numpy
import math

lisaxml.SourceClassModules['BlackHoleBinary'] = 'BBH'

lisaxml.SourceClassModules['SpinBlackHoleBinary'] = 'BBH'

# generic CoherentWave

class CoherentWave(object):
    def __init__(self):
        # derived classes should define compute(self,samples,deltat,zerotime)
        # and use this tuple to track the last request
        self.lastcomputed = (0,0,0)    
    
    def amp(self,samples,deltat,zerotime):
        self.compute(samples,deltat,zerotime)
        return self.a
    
    def phi(self,samples,deltat,zerotime):
        self.compute(samples,deltat,zerotime)
        return self.phic
    
    def f(self,samples,deltat,zerotime):
        self.compute(samples,deltat,zerotime)    
        return self.fc
    
    def hp(self,samples,deltat,zerotime):
        self.compute(samples,deltat,zerotime)
        return self.hpc
    
    def hc(self,samples,deltat,zerotime):
        self.compute(samples,deltat,zerotime)
        return self.hcc
    
    def coherentwaveforms(self,samples,deltat,zerotime):
        self.compute(samples,deltat,zerotime)
        
        return ( self.hp(samples,deltat,zerotime) * self.amp(samples,deltat,zerotime) *
                                                    numpy.cos(self.phi(samples,deltat,zerotime)),
                 self.hc(samples,deltat,zerotime) * self.amp(samples,deltat,zerotime) *
                                                    numpy.sin(self.phi(samples,deltat,zerotime)) )
    

class BlackHoleBinary(lisaxml.Source,CoherentWave):
    """Returns a MLDC source that models the waveform emitted by the
    quasistationary circular inspiral of a binary."""
    
    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description
    
    outputlist = ( ('EclipticLatitude',           'Radian',    None, 'standard ecliptic latitude'),
                   ('EclipticLongitude',          'Radian',    None, 'standard ecliptic longitude'),
                   ('Polarization',               'Radian',    None, 'standard source polarization'),
                   ('Inclination',                'Radian',    None, 'standard source inclination'),
                   ('Distance',                   'Parsec',    None, 'standard source distance'),
                   ('Mass1',                      'SolarMass', None, 'mass of 1st binary component'),
                   ('Mass2',                      'SolarMass', None, 'mass of 2nd binary component'),
                   ('CoalescenceTime',            'Second',    None, 'absolute time of final coalescence'),
                   ('InitialAngularOrbitalPhase', 'Radian',    None, 'orbital phase at t = 0 ???'), # not quite!
                   ('TruncationTime',             'Second',    '0',  'truncation time removed from end ???'),
                   ('TaperApplied',               'TotalMass', '7',  'tapering parameter') )
    
    def __init__(self,name=''):
        super(BlackHoleBinary, self).__init__('BlackHoleBinary',name)
        
        # for CoherentWave (avoid adding to Source.parameters)
        # TO DO: I think this is redundant...
        self.__dict__['lastcomputed'] = (0,0,0)
        
        # pre-define variables here so that they will not be included in parameters when they are set
        self.phic = self.fc = self.a = None
    
    def waveforms(self,samples,deltat,inittime):
        bbh = BBHChallenge1(self.Mass1,self.Mass2)
        
        bbh.SetInitialOrbit(self.CoalescenceTime,self.InitialAngularOrbitalPhase)
        
        # note that the last parameter ComputeInspiral is the final time...
        # so to get "samples" values we need to step back one deltat
         
        bbh.ComputeInspiral(inittime,deltat,inittime + deltat*(samples-1))
        
        # note that BBHChallenge uses a "ThetaInclination", not a "IotaInclination"
        
        bbh.SetObserver(math.pi-self.Inclination,self.Distance)
        
        hp = numpy.empty(samples,'d')
        hc = numpy.empty(samples,'d')
        
        wavelen = bbh.ComputeWaveform(self.TruncationTime,self.TaperApplied,hp,hc)
        
        hp[wavelen:] = 0.0
        hc[wavelen:] = 0.0
        
        return (hp,hc)
    
    # utility functions
    
    def PNBinaryCoalescenceTime(m1,m2,f0,phi0=0):
        """Return the estimated coalescence time (seconds) for PN binary with
        masses m1, m2 (solar masses), and initial GW frequency f0 (Hertz)."""
        
        bbh = BBHChallenge1(m1,m2)
        
        omega0 = math.pi * f0
        bbh.SetInitialOrbit(omega0,phi0)
        
        return bbh.EstimateTc(omega0)
    
    def PNBinaryInitialFrequency(m1,m2,tc):
        """Returns the initial angular orbital frequency (omega0) at t=0 for
        a PN binary with masses m1,m2 and coalescence time tc (seconds)."""

        bbh = BBHChallenge1(m1,m2)
        return bbh.EstimateFreq0(tc)
    
    # CoherentWave interface
    
    def compute(self,samples,deltat,inittime):
        if self.lastcomputed != (samples,deltat,inittime):
            self.lastcomputed = (samples,deltat,inittime) 
            
            bbh = BBHChallenge1(self.Mass1,self.Mass2)
            
            bbh.SetInitialOrbit(self.CoalescenceTime,self.InitialAngularOrbitalPhase)
            
            # note that the last parameter ComputeInspiral is the final time...
            # so to get "samples" values we need to step back one deltat
            
            bbh.ComputeInspiral(inittime,deltat,inittime + deltat*(samples-1))
            
            # note that BBHChallenge uses a "ThetaInclination", not a "IotaInclination"
            
            bbh.SetObserver(math.pi-self.Inclination,self.Distance)
            
            self.phic = numpy.empty(samples,'d')
            self.fc   = numpy.empty(samples,'d')
            self.a    = numpy.empty(samples,'d')
            
            wavelen = bbh.ComputeCoherent(self.TruncationTime,self.TaperApplied,self.phic,self.fc,self.a)
            
            self.phic[wavelen:] = 0.0
            self.fc[wavelen:]   = 0.0
            
            print "Phase on first sample is", self.phic[0]
            
            # TO DO: where is the return statement?
    
    
    def hp(self,samples,deltat,zerotime):
        return (1.0 + math.cos(math.pi - self.Inclination)**2)
    
    def hc(self,samples,deltat,zerotime):
        return 2.0 * math.cos(math.pi - self.Inclination)
    

class SpinBlackHoleBinary(lisaxml.Source):
    """Returns a MLDC source that models the waveform emitted by the
	 spinning black hole binaries."""
    
    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description
    
    outputlist = (('EclipticLatitude',                 'Radian',        None, 'standard ecliptic latitude'),
                  ('EclipticLongitude',                'Radian',        None, 'standard ecliptic longitude'),
                  ('PolarAngleOfSpin1',                'Radian',        None, 'initial  polar angle of the first spin'),
                  ('PolarAngleOfSpin2',                'Radian',        None, 'initial  polar angle of the second spin'),
                  ('AzimuthalAngleOfSpin1',            'Radian',        None, 'initial azimuthal direction of first spin'),
                  ('AzimuthalAngleOfSpin2',            'Radian',        None, 'initial azimuthal direction of second spin'),
                  ('Spin1',                            'MassSquared',   None, 'magnitude of (specific) of first spin'),
                  ('Spin2',                            'MassSquared',   None, 'magnitude of (specific) of second spin'),
                  ('Mass1',                            'SolarMass',     None, 'mass of the first object'),
                  ('Mass2',                            'SolarMass',     None, 'mass of the secondary object'),
                  ('CoalescenceTime',                  'Second',        None, 'time of coalescence'),
                  ('PhaseAtCoalescence',               'Radian',        None, 'GW phase at coalescence (excluding modulation)'),
                  ('InitialPolarAngleL',               'Radian',        None, 'initial polar angle of the angular orbital momentum L_N'),
                  ('InitialAzimuthalAngleL',           'Radian',        None, 'initial azimuthal direction of L_N'),
                  ('Distance',                         'Parsec',        None, 'standard source distance'),
                  ('TaperApplied',                     'TotalMass',     '7',  'tapering parameter'),
                  ('AmplPNorder',                      'Unit',          '0',  'Post-Newtonian order of the amplitude corrections, phase is hardcoded to 2PN'))
    
    def  __init__(self,name=''):
        super(SpinBlackHoleBinary, self).__init__('SpinBlackHoleBinary',name)

        # pre-define variables here if used later, so that they will not be included in parameters when they are set
    
    def waveforms(self,samples,deltat,inittime):
        sbbh = SpinBBHWaveform(self.Mass1,self.Mass2,self.Spin1,self.Spin2,deltat)      
        sbbh.SetObserver(0.5*math.pi - self.EclipticLatitude, self.EclipticLongitude, self.Distance)        
  
        sbbh.ComputeInspiral(inittime,
                             self.CoalescenceTime,self.PhaseAtCoalescence,
                             self.InitialPolarAngleL,self.InitialAzimuthalAngleL,
                             self.PolarAngleOfSpin1,self.AzimuthalAngleOfSpin1,
                             self.PolarAngleOfSpin2,self.AzimuthalAngleOfSpin2,
                             inittime + deltat*(samples-1))
        

        hp = numpy.empty(samples,'d')
        hc = numpy.empty(samples,'d')

        wavelen = sbbh.ComputeWaveform(int(self.AmplPNorder), self.TaperApplied, hp, hc)

        hp[wavelen:] = 0.0
        hc[wavelen:] = 0.0

        return (hp,hc)   
    
%}
