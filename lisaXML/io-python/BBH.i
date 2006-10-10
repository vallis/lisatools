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
import synthlisa
import numpy.oldnumeric as Numeric
import math

def PNBinary(m1,m2,tc,phi0,dist,theta,elat,elon,pol,timeoffset=0,deltat=10,truncTime=0,taper=7,maxDuration=31459080):
    """Return a synthlisa Wave object that models the waveform emitted by
the quasistationary circular inspiral of a binary. Parameters are as
follows:

- m1, m2 are the binary masses in units of Solar Mass;

- tc is the time of coalescence (from timeoffset) in Seconds

- dist is the distance in Parsec;

- theta is the angle to the observer (pi - the standard inclination
  as defined for galactic binaries) in radians;

- elat, elon, and pol are the standard ecliptic latitude, longitude,
  and polarization;

- timeoffset is the time at which the waveform begins;
  
- deltat (by default = 10 s) is the requested sampling timestep in
  seconds;
  
- truncTime (seconds) is the truncation time removed from the end
  of the waveform;

- taper is the tapering parameter (see BBHChallenge1.cc)."""
   
 #   print 20*'*'
 #   print "         Input parameters     "
 #   print 20*'*'
 #   print "mass1 = ", m1
 #   print "mass2 = ", m2
 #   print "coalesc.time = ", tc
 #   print "initial phase= ", phi0
 #   print "distance = ", dist
 #   print "theta = ", theta 
 #   print "equat latit = ", elat
 #   print "equat long = ",elon
 #   print "polarization angle= ",pol
 #   print "timeoffset = ",timeoffset
 #   print "deltat = " ,deltat
 #   print "truncation time = ",truncTime
 #   print "taper = ",taper
 #   print "maxDuration = ",maxDuration
 #   print 20*'*'
 
    bbh = BBHChallenge1(m1,m2)
   
    bbh.SetInitialOrbit(tc, phi0)

    bbh.ComputeInspiral(timeoffset, deltat, maxDuration)
    bbh.SetObserver(theta,dist)
    
    hPlus  = DVector()
    hCross = DVector()

    bbh.ComputeWaveform(truncTime,taper,hPlus,hCross)

    wavelen = len(hPlus)

    hp = Numeric.zeros(wavelen,'d')
    hc = Numeric.zeros(wavelen,'d')

    hp[0:wavelen] = hPlus[0:wavelen]
    hc[0:wavelen] = hCross[0:wavelen]

    hPlus.resize(0)
    hCross.resize(0)

    # set waveform to begin at time timeoffset
    # use Lagrange-2 interpolation by default

    ret = synthlisa.SampledWave(hp,hc,wavelen,deltat,-timeoffset,1.0,
                                None,2,elat,elon,pol)

   # ret.CoalescenceTime = timeoffset + deltat * wavelen
    ret.CoalescenceTime = tc

    ret.sampledxmlargs = ret.xmlargs
    ret.sampledxmltype = ret.xmltype

    ret.xmlargs = [m1,m2,tc,phi0,dist,theta,elat,elon,pol,timeoffset,deltat,truncTime,taper]
    ret.xmltype = 'PNBinary'

    return ret
    
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

# define XML interface

synthlisa.argumentList['PNBinary'] = ( ('Mass1','SolarMass',None),
                                       ('Mass2','SolarMass',None),
                                       ('CoalescenceTime','Second',None),
                                       ('InitialAngularOrbitalPhase','Radian',None),
                                       ('Distance','Parsec',None),
                                       ('ThetaInclination','Radian',None),
                                       ('EclipticLatitude','Radian',None),
                                       ('EclipticLongitude','Radian',None),
                                       ('SLPolarization','Radian',None),
                                       ('TimeOffset','Second','0'),
                                       ('IntegrationStep','Second','10'),
                                       ('TruncationTime','Second','0'),
                                       ('TaperApplied','TotalMass','7'),
                                       ('MaxDuration','Second','31459080'))

synthlisa.outputList['PNBinary'] = ( ('EclipticLatitude','Radian',None),
                                     ('EclipticLongitude','Radian',None),
                                     ('Polarization','Radian',None),
                                     ('TimeOffset','Second',None),
                                     ('Mass1','SolarMass',None),
                                     ('Mass2','SolarMass',None),
                                     ('CoalescenceTime','Second',None),
                                     ('InitialAngularOrbitalPhase','Radian',None),
                                     ('Distance','Parsec',None),
                                     ('Inclination','Radian',None),
                                     ('IntegrationStep','Second',None),
                                     ('TruncationTime','Second','0'),
                                     ('TaperApplied','TotalMass','7'),
                                     ('MaxDuration','Second','31459080'))

synthlisa.ObjectToXML['PNBinary'] = 'BlackHoleBinary'

synthlisa.XMLToObject['PNBinary'] = ('PNBinary',PNBinary)
synthlisa.XMLToObject['BlackHoleBinary'] = ('PNBinary',PNBinary)

synthlisa.minimumParameterSet['BlackHoleBinary'] = synthlisa.makeminimum(['Mass1',
                                                                          'Mass2',
                                                                          'CoalescenceTime',
                                                                          'InitialAngularOrbitalPhase',
                                                                          'Distance',
                                                                          'Inclination'])

synthlisa.optionalParameterSet['BlackHoleBinary'] = synthlisa.makeoptional([('TimeOffset',('0','Second')),
                                                                            ('IntegrationStep',('10','Second')),
                                                                            ('TruncationTime',('0','Second')),
                                                                            ('TaperApplied',('7','TotalMass')),
                                                                            ('MaxDuration',('31459080','Second'))])
%}
