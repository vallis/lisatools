#!/usr/bin/env python

import sys
import lisaxml

from math import pi, cos, sin, tan, acos, atan2, sqrt

# just a simple bisection root finder

def find_root(function,lower,upper,tolerance):
    while upper - lower > tolerance:
        middle = 0.5 * (lower + upper)

        if function(upper)*function(middle) > 0.0:
            upper = middle
        else:
            lower = middle

    print "Check solve: %f" % function(upper)
    return upper

def unwrap(angle):
    while angle > 2*pi:
        angle = angle - 2*pi
    
    while angle < 0:
        angle = angle + 2*pi
    
    return angle

# begin the main code...

infile  = sys.argv[1]
outfile = sys.argv[2]

inputXML = lisaxml.readXML(infile)
outputXML = lisaxml.lisaXML(outfile,comments='Updated Apr 2007 to fix EMRI parameters, no changes to signals')

outputXML.LISAData(inputXML.getLISAgeometry())

allsources = inputXML.getLISASources()

for source in allsources:
    if source.xmltype == 'ExtremeMassRatioInspiral':
        thetas = 0.5*pi - source.EclipticLatitude
        phis   = source.EclipticLongitude
        pol    = source.Polarization

        thetak = source.PolarAngleOfSpin
        phik   = source.AzimuthalAngleOfSpin

        lam    = source.LambdaAngle
        alpha0 = source.InitialAlphaAngle

        thetal = acos(cos(thetak)*cos(lam) + sin(thetak)*sin(lam)*cos(alpha0))
    
        def cosphil(thetak,phik):
            return sin(thetak) * cos(phik) * cos(lam) + sin(phik) * sin(lam) * sin(alpha0) - cos(phik) * cos(thetak) * sin(lam) * cos(alpha0)

        def sinphil(thetak,phik):
            return sin(thetak) * sin(phik) * cos(lam) - cos(phik) * sin(lam) * sin(alpha0) - sin(phik) * cos(thetak) * sin(lam) * cos(alpha0)

        phil = atan2(sinphil(thetak,phik),cosphil(thetak,phik))

        cosi = cos(thetas) * cos(thetal) + sin(thetas) * sin(thetal) * cos(phis - phil) 

        def psibad(thetal):
            cosphisl = (cosi - cos(thetas) * cos(thetal)) / (sin(thetas) * sin(thetal))
            # we're choosing one branch here, but cosphisl may be > 1, in which case we get a nan
            sinphisl = sqrt(1 - cosphisl**2)

            return unwrap(atan2(sin(thetal) * sinphisl,
                                cos(thetas) * sin(thetal) * cosphisl - cos(thetal) * sin(thetas)))
        
        thetalnew = find_root(lambda x: psibad(x) - 0.5*pi + psibad(x) - pol,
                              0,pi,1e-5)

        philnew   = find_root(lambda x: cos(thetas)*cos(thetalnew) + sin(thetas)*sin(thetalnew)*cos(phis - x) - cosi,
                              0,2*pi,1e-5)

        thetaknew = find_root(lambda x: cos(x) * cos(lam) + sin(x) * sin(lam) * cos(alpha0) - cos(thetalnew),
                              0,pi,1e-5)
        
        phiknew   = find_root(lambda x: sinphil(thetaknew,x) / cosphil(thetaknew,x) - tan(philnew),
                              0,2*pi,1e-5)

        source.PolarAngleOfSpin = thetaknew
        source.AzimuthalAngleOfSpin = phiknew

        source.Polarization = 0

    outputXML.SourceData(source)
