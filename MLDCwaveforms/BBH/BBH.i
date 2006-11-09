%module BBH
%{
#include "BBHChallenge1.hh"
%}

// the following three lines allow the automatic conversion of a numpy array
// into a pointer to an array of doubles and the long length of the array
// each pair of these that appears in the library header file (e.g., BBHChallenge1.hh)
// needs to be named explicitly

%include numpy_typemaps.i
%typemap(in) (double *hPlus ,long hPlusLength ) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *hCross,long hCrossLength) = (double *numpyarray,long numpyarraysize);

// the following include defines all the calls that need to be wrapped by SWIG
// the include at the top serves only to provide definitions for this one;
// since it is between braces %{ %}, its content goes straight into
// the CPP wrapper file

%include "BBHChallenge1.hh"

// and here we can add some Python to the interface code

%pythoncode %{
import lisaxml
import numpy
import math

SourceClassModules = {}
SourceClassModules['BlackHoleBinary'] = 'BBH'

lisaxml.SourceClassModules['BlackHoleBinary'] = 'BBH'

# these two classes originally defined in lisaxml;
# but here we do not wish to depend on that

class XMLobject(object):
    def __init__(self):
        self.__dict__['parameters'] = []

    def __setattr__(self,attr,value):
        self.__dict__[attr] = value

        if (not attr in self.parameters) and (not '_Unit' in attr):
            self.parameters.append(attr)

class Source(XMLobject):
    def __init__(self,sourcetype,name=''):
        super(Source,self).__init__()

        # avoid calling setattr
        self.__dict__['xmltype'] = sourcetype
        self.__dict__['name'] = name

    def parstr(self,attr):
        value = getattr(self,attr)

        try:
            unit = getattr(self,attr+'_Unit')
        except AttributeError:
            unit = 'default'

        return "%s (%s)" % (value,unit)

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
%}
