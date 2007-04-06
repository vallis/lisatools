// EMRI.i, required by swig
// requires GSL
// SVN_ID = "$Id$"

%module EMRI
%{
#include "AKWaveform.hh"
%}

%include numpy_typemaps.i
%typemap(in) (double *hPlus ,long hPlusLength ) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *hCross,long hCrossLength) = (double *numpyarray,long numpyarraysize);

%include "cpointer.i"
%pointer_class(double, doublep);

%include "AKWaveform.hh"

%pythoncode %{
import lisaxml
import numpy
import math

SourceClassModules = {}
SourceClassModules['ExtremeMassRatioInspiral'] = 'EMRI'
lisaxml.SourceClassModules['ExtremeMassRatioInspiral'] = 'EMRI'

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



class ExtremeMassRatioInspiral(lisaxml.Source):
    """Returns a MLDC source that models the waveform emitted by the
    extreme mass ratio nspiral."""

    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description


    outputlist = (('EclipticLatitude',                 'Radian',        None, 'standard ecliptic latitude'),
                  ('EclipticLongitude',                'Radian',        None, 'standard ecliptic longitude'),
                  ('Polarization',                     'Radian',        None, 'standard source polarization'),
                  ('PolarAngleOfSpin',                 'Radian',        None, 'polar angle of MBH spin'),
                  ('AzimuthalAngleOfSpin',             'Radian',        None, 'azimuthal direction of MBH spin'),
                  ('Spin',                             'MassSquared',   None, 'magnitude of (specific) spin (S) of MBH'),
                  ('MassOfCompactObject',              'SolarMass',     None, 'mass of the compact object'),
                  ('MassOfSMBH',                       'SolarMass',     None, 'mass of the MBH'),
                  ('InitialAzimuthalOrbitalFrequency', 'Hertz',         None, 'initial value of orbital azimuthal frequency'),
                  ('InitialAzimuthalOrbitalPhase',     'Radian',        None, 'initial azimuthal orbital phase'),
                  ('InitialEccentricity',              'Unit',          None, 'initial orbital eccentricity'),
                  ('InitialTildeGamma',                'Radian',        None, 'initial position of pericenter, as angle between LxS and pericenter'),
                  ('InitialAlphaAngle',                'Radian',        None, 'initial azimuthal direction of L (in the orbital plane)'),
                  ('LambdaAngle',                      'Radian',        None, 'angle between L and S'),
                  ('Distance',                         'Parsec',        None, 'standard source distance')
            )

    def  __init__(self,name=''):
        super(ExtremeMassRatioInspiral, self).__init__('ExtremeMassRatioInspiral',name)
    
    def waveforms(self,samples,deltat,inittime):
       
       emri = AKWaveform(self.Spin, self.MassOfCompactObject, self.MassOfSMBH, inittime + deltat*(samples-1), deltat)

       emri.SetSourceLocation(math.pi/2.0 - self.EclipticLatitude, self.EclipticLongitude, self.PolarAngleOfSpin, \
                              self.AzimuthalAngleOfSpin, self.Distance)
			      
       emri.EvolveOrbit(inittime, self.InitialAzimuthalOrbitalFrequency, self.InitialEccentricity, self.InitialTildeGamma,\
                        self.InitialAzimuthalOrbitalPhase, self.InitialAlphaAngle, self.LambdaAngle)

       hp = numpy.empty(samples,'d')
       hc = numpy.empty(samples,'d')

       # don't apply polarization here since it comes in at the level of makebarycentric.py
       # wavelen = emri.GetWaveform(self.Polarization, hp, hc)

       wavelen = emri.GetWaveform(0, hp, hc)

       hp[wavelen:] = 0.0
       hc[wavelen:] = 0.0

       return (hp,hc)
       
# utility function

def EMRIEstimateInitialOrbit(spin,mu,MBHmass, Tend,e_lso,gamma_lso,phi_lso,alpha_lso, lam_lso): 
   """Estimates the initial (t=0) orbital parameters for the orbit
specified at the plunge by e_lso, gamma_lso, alpha_lso, phi_lso, lambda, duration Tend."""
       
   ak = AKWaveform(spin, mu, MBHmass, 3.e8, 15.0)

   ak.EstimateInitialParams(Tend, e_lso, gamma_lso, phi_lso, alpha_lso, lam_lso)

   nut = doublep()
   et = doublep()
   gamt = doublep()
   pht = doublep()
   alt = doublep()

   ak.GetOrbitalParamsAt0(nut, et, gamt, pht, alt)

   return[nut.value(), et.value(), gamt.value(), pht.value(), alt.value()]

%}
