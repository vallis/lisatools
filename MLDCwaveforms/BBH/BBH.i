// BBH.i, file required for swig

%module BBH
%{
#include "Macros.hh"
#include "Constants.hh"
#include "BBHChallenge1.hh"
%}

%include "BBHChallenge1.hh"

// Mapping std::vector<double> to DVector

%include "std_vector.i"
namespace std {
    %template(DVector) vector<double>;
}

// Python interface...

%pythoncode %{
import lisaxml
import numpy

import math

SourceClassModules = {}

SourceClassModules['BlackHoleBinary'] = 'BBH'

class BlackHoleBinary(lisaxml.Source):
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
    
    def waveforms(self,samples,deltat,inittime):
        bbh = BBHChallenge1(self.Mass1,self.Mass2)
        
        bbh.SetInitialOrbit(self.CoalescenceTime,self.InitialAngularOrbitalPhase)

        bbh.ComputeInspiral(inittime,deltat,deltat*samples)
        bbh.SetObserver(-self.Inclination,self.Distance)
        
        hPlus  = DVector()
        hCross = DVector()
        
        bbh.ComputeWaveform(self.TruncationTime,self.TaperApplied,hPlus,hCross)
        
        hp = numpy.zeros(samples,'d')
        hc = numpy.zeros(samples,'d')
        
        wavelen = len(hPlus)
        
        # copy into numpy arrays; pad with zeros
        
        hp[0:wavelen] = hPlus[0:wavelen]
        hc[0:wavelen] = hCross[0:wavelen]
        
        hp[wavelen:samples] = 0.0
        hc[wavelen:samples] = 0.0        
        
        # free temporary array storage
        
        hPlus.resize(0)
        hCross.resize(0)
        
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
%}
