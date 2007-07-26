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

lisaxml.SourceClassModules['ExtremeMassRatioInspiral'] = 'EMRI'

class ExtremeMassRatioInspiral(lisaxml.Source):
    """Returns a MLDC source that models the waveform emitted by the
    extreme mass ratio nspiral."""

    # this is the list of parameters that will be recorded in the lisaXML SourceData sections
    # give ParameterName, DefaultUnit, DefaultValue (a string), Description

    outputlist = (('EclipticLatitude',                 'Radian',        None, 'standard ecliptic latitude'),
                  ('EclipticLongitude',                'Radian',        None, 'standard ecliptic longitude'),
                  ('Polarization',                     'Radian',        0,    'standard source polarization (degenerate for EMRIs)'),
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
                  ('Distance',                         'Parsec',        None, 'standard source distance'))

    def  __init__(self,name=''):
        super(ExtremeMassRatioInspiral, self).__init__('ExtremeMassRatioInspiral',name)
        
        self.__samples = None
        self.__deltat  = None
        self.__inittime = None
    
    def waveforms(self,samples,deltat,inittime,debug=0):
        if samples != self.__samples or deltat != self.__deltat or inittime != self.__inittime:
            self.__samples, self.__deltat, self.__inittime = samples, deltat, inittime

            self.__emri = AKWaveform(self.Spin, self.MassOfCompactObject, self.MassOfSMBH, inittime + deltat*(samples-1), deltat)
            
            self.__emri.SetSourceLocation(math.pi/2.0 - self.EclipticLatitude, self.EclipticLongitude, self.PolarAngleOfSpin, \
                                        self.AzimuthalAngleOfSpin, self.Distance)
            
            self.__emri.EvolveOrbit(inittime, self.InitialAzimuthalOrbitalFrequency, self.InitialEccentricity, self.InitialTildeGamma,\
                                  self.InitialAzimuthalOrbitalPhase, self.InitialAlphaAngle, self.LambdaAngle)
        
        hp = numpy.empty(samples,'d')
        hc = numpy.empty(samples,'d')

        # don't apply polarization here since it comes in at the level of makebarycentric.py
        # wavelen = emri.GetWaveform(self.Polarization, hp, hc)

        wavelen = self.__emri.GetWaveform(0, hp, hc, debug)

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
