#!/usr/bin/env python

import sys
import lisaxml

from math import pi, cos, sin, tan, acos, atan2, sqrt

def RotationMatrix(theta,phi,psi):    
    return [  [cos(psi) + (cos(phi)*cos(phi))*(1 - cos(psi))*(sin(theta)*sin(theta)),
              -(cos(theta)*sin(psi)) + cos(phi)*(1 - cos(psi))*sin(phi)*(sin(theta)*sin(theta)),
              cos(phi)*(1 - cos(psi))*cos(theta)*sin(theta) + sin(phi)*sin(psi)*sin(theta)],

              [cos(theta)*sin(psi) + cos(phi)*(1 - cos(psi))*sin(phi)*(sin(theta)*sin(theta)),
              cos(psi) + (1 - cos(psi))*(sin(phi)*sin(phi))*(sin(theta)*sin(theta)),
              (1 - cos(psi))*cos(theta)*sin(phi)*sin(theta) - cos(phi)*sin(psi)*sin(theta)],

              [cos(phi)*(1 - cos(psi))*cos(theta)*sin(theta) - sin(phi)*sin(psi)*sin(theta),
              (1 - cos(psi))*cos(theta)*sin(phi)*sin(theta) + cos(phi)*sin(psi)*sin(theta),
              cos(psi) + (1 - cos(psi))*(cos(theta)*cos(theta))] ]

def Rotate(rm,v):
    return [ sum([rm[i][j] * v[j] for j in range(0,3)]) for i in range(0,3)]

def AngleToVector(theta,phi):
    return [ sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta) ]

def VectorToAngle(v):
    theta = acos(v[2])
    phi = atan2(v[1],v[0])
    
    return (theta,phi)

# begin the main code...

infile  = sys.argv[1]
outfile = sys.argv[2]

inputXML = lisaxml.readXML(infile)
outputXML = lisaxml.lisaXML(outfile,comments='Updated Apr 2007 to fix timestamps and EMRI parameters, no changes to signals')

outputXML.LISAData(inputXML.getLISAgeometry())

allsources = inputXML.getLISASources()

for source in allsources:
    if hasattr(source,'xmltype') and source.xmltype == 'ExtremeMassRatioInspiral':
        print "---- Analyzing source %s ----" % source.name
        
        thetas = 0.5*pi - source.EclipticLatitude
        phis   = source.EclipticLongitude
        pol    = source.Polarization

        # this should be fixed
        lam    = source.LambdaAngle

        tildeg = source.InitialTildeGamma
        alpha0 = source.InitialAlphaAngle
        
        # these need rotating
        thetak = source.PolarAngleOfSpin
        phik   = source.AzimuthalAngleOfSpin

        # rotate thetak, phik

        rm = RotationMatrix(thetas,phis,pol-pi/2)
        
        thetaknew, phiknew = VectorToAngle(Rotate(rm,AngleToVector(thetak,phik)))
        if phiknew < 0:
            phiknew = phiknew + 2*pi

        cosk = cos(thetas) * cos(thetak) + sin(thetas) * sin(thetak) * cos(phis - phik)
        cosknew = cos(thetas) * cos(thetaknew) + sin(thetas) * sin(thetaknew) * cos(phis - phiknew)
        print "Check cosk: %+f -> %+f" % (cosk, cosknew)

        # get thetal, phil at alpha = alpha0 (lam is constant)
        
        thetal = acos(cos(thetak)*cos(lam) + sin(thetak)*sin(lam)*cos(alpha0))
        phil = atan2(sin(thetak) * sin(phik) * cos(lam) - cos(phik) * sin(lam) * sin(alpha0) - sin(phik) * cos(thetak) * sin(lam) * cos(alpha0),
                     sin(thetak) * cos(phik) * cos(lam) + sin(phik) * sin(lam) * sin(alpha0) - cos(phik) * cos(thetak) * sin(lam) * cos(alpha0))
        
        # rotate thetal, phil
        
        thetalnew, philnew = VectorToAngle(Rotate(rm,AngleToVector(thetal,phil)))
        
        cosi = cos(thetas) * cos(thetal) + sin(thetas) * sin(thetal) * cos(phis - phil)
        cosinew = cos(thetas) * cos(thetalnew) + sin(thetas) * sin(thetalnew) * cos(phis - philnew)
        print "Check cosi: %+f -> %+f" % (cosi, cosinew)
        
        # get the new alpha0 (lam does not change...)
        
        alpha0new = acos((cos(thetalnew) - cos(thetaknew)*cos(lam))/(sin(thetaknew)*sin(lam)))
        
        nscl = sin(thetas) * sin(phik - phis) * sin(lam) * cos(alpha0) + (cosk * cos(thetak) - cos(thetas)) * sin(lam) * sin(alpha0) / sin(thetak)
        sinalpha0new = (nscl - sin(thetas) * sin(phiknew - phis) * sin(lam) * cos(alpha0new)) / ((cosk * cos(thetaknew) - cos(thetas)) * sin(lam)) * sin(thetaknew) 
        if sinalpha0new < 0:
            alpha0new = 2*pi - alpha0new
        
        # verify that gamma is invariant
        
        beta = atan2( (cos(lam) * cosi - cosk) / (sin(lam) * sqrt(1 - cosi**2)), 
                      nscl / (sin(lam) * sqrt(1 - cosi**2)) )

        nsclnew = sin(thetas) * sin(phiknew - phis) * sin(lam) * cos(alpha0new) + (cosk * cos(thetaknew) - cos(thetas)) * sin(lam) * sin(alpha0new) / sin(thetaknew)
        betanew = atan2( (cos(lam) * cosi - cosk) / (sin(lam) * sqrt(1 - cosi**2)),
                         nsclnew / (sin(lam) * sqrt(1 - cosi**2)) )
        
        tildegnew = tildeg + beta - betanew

        print "Check tgam: %+f -> %+f" % (tildeg, tildegnew)

        # write changes to file
        
        print "--- Changes to key file ---"
        print "alpha0: %+f*pi -> %+f*pi" % (alpha0/pi, alpha0new/pi)
        
        print "thetak: %+f*pi -> %+f*pi" % (thetak/pi, thetaknew/pi)
        print "phik  : %+f*pi -> %+f*pi" % (phik/pi,   phiknew/pi)
        
        print "pol   : %+f*pi -> 0" % (pol/pi)
        
        psiSL = atan2(sin(thetal) * sin(phis - phil),
                      cos(thetas) * sin(thetal) * cos(phis - phil) - cos(thetal) * sin(thetas))

        psiLS = atan2(cos(thetas) * sin(thetalnew) * cos(phis - philnew) - cos(thetalnew) * sin(thetas),
                      sin(thetalnew) * sin(phis - philnew))
                
        ccf,    scf    = cos(2*pol - 2*psiSL), sin(2*pol - 2*psiSL)
        newccf, newscf = cos(2*psiLS),         sin(2*psiLS)
        
        print "--- Final check: polarization rotation ---"
        print "cospsi: %+f -> %+f" % (ccf,newccf)
        print "sinpsi: %+f -> %+f" % (scf,newscf)
        
        source.InitialAlphaAngle = alpha0new

        source.PolarAngleOfSpin = thetaknew
        source.AzimuthalAngleOfSpin = phiknew

        source.Polarization = 0

    outputXML.SourceData(source)

tdi = inputXML.getTDIObservables()

if tdi:
    outputXML.TDIData(tdi[0])
