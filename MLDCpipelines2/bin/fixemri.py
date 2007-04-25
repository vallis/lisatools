#!/usr/bin/env python

import sys
import lisaxml

from math import pi, cos, sin, tan, acos, atan2, sqrt

# just a simple bisection root finder

def sign(x):
    # note, this will return nan for a nan
    return x and x/abs(x)

def brackets(function,lower,upper,steps=100):
    delta = (upper - lower) / steps
    
    bracks = []
    
    signlof = sign(function(lower))
    for i in range(1,steps+1):
        signhif = sign(function(lower + delta*i))
        
        # this test will (rightly) fail if either sign is nan
        # could also use signlof * signhif < 0 without defining sign
        if signlof == -signhif:
            bracks.append((lower + delta*(i-1),lower+delta*i))
        
        signlof = signhif
    
    return bracks

def bracket_find_root(function,lower,upper,tolerance,debug=0):
    bracks = brackets(function,lower,upper)
    
    if debug:
        print "Bracketing..."

    sols = []

    for brack in bracks:
        solx = find_root(function,brack[0],brack[1],tolerance,debug)
        solf = function(solx)

        if debug:
            print "Sol: ", solx, solf

        # another way to check for nan
        if abs(solf) >= 0:
            if abs(solf) < 1e-6:
                sols.append(solx)

    if debug:
        print "...solutions: ", sols

    return sols

def find_root(function,lower,upper,tolerance,debug=0):    
    while upper - lower > tolerance:
        middle = 0.5 * (lower + upper)
        
        if function(upper)*function(middle) > 0.0:
            upper = middle
        else:
            lower = middle
    
    if debug:
        print "Check solve: ", function(upper)

    return upper

def unwrap(angle,period):
    while angle > 0.5*period:
        angle = angle - period
    
    while angle < -0.5*period:
        angle = angle + period
    
    return angle

# begin the main code...

infile  = sys.argv[1]
outfile = sys.argv[2]

inputXML = lisaxml.readXML(infile)
outputXML = lisaxml.lisaXML(outfile,comments='Updated Apr 2007 to fix EMRI parameters, no changes to signals')

outputXML.LISAData(inputXML.getLISAgeometry())

allsources = inputXML.getLISASources()

dochecks = 1

for source in allsources:
    if source.xmltype == 'ExtremeMassRatioInspiral':
        print "---- Analyzing source %s ----" % source.name
        
        thetas = 0.5*pi - source.EclipticLatitude
        phis   = source.EclipticLongitude
        pol    = source.Polarization

        tildeg = source.InitialTildeGamma

        thetak = source.PolarAngleOfSpin
        phik   = source.AzimuthalAngleOfSpin

        lam    = source.LambdaAngle
        alpha0 = source.InitialAlphaAngle

        # --- SOLVING for the initial thetal and phil

        # Eq. (22a) of gwdaw-mldc2 proc, assuming 0 < thetal < pi
        thetal = acos(cos(thetak)*cos(lam) + sin(thetak)*sin(lam)*cos(alpha0))

        # Eqs. (22b-c) of gwdaw-mldc2 proc, preserve sign information
        phil = atan2(sin(thetak) * sin(phik) * cos(lam) - cos(phik) * sin(lam) * sin(alpha0) - sin(phik) * cos(thetak) * sin(lam) * cos(alpha0),
                     sin(thetak) * cos(phik) * cos(lam) + sin(phik) * sin(lam) * sin(alpha0) - cos(phik) * cos(thetak) * sin(lam) * cos(alpha0))

        # --- SOLVING for thetalnew and philnew
    
        # Invariant from Eq. (25) of gwdaw-mldc2 proc; used to eliminate sin/cos(phis - phil)
        #   in solving for psi
        cosi = cos(thetas) * cos(thetal) + sin(thetas) * sin(thetal) * cos(phis - phil)

        # The old instantaneous polarization; this is pi/2 - psi of Eq. (32), gwdaw-mldc2
        def psibad(thetal,sgn=1):
            # return nan if we're asked to divide by zero
            if thetal == 0:
                return sqrt(-1)
             
            # build cos(phis - phil) from cosi
            cosphisl = (cosi - cos(thetas) * cos(thetal)) / (sin(thetas) * sin(thetal))
            
            # cosi doesn't tell us the sign of sin(phis - phil); we use what's passed as argument
            #   but set it to one by default
            sinphisl = sgn * sqrt(1 - cosphisl**2)

            # This is the old definition, arguments reversed w.r.t. Eq. (32), gwdaw-mldc2
            return atan2(sin(thetal) * sinphisl,
                         cos(thetas) * sin(thetal) * cosphisl - cos(thetal) * sin(thetas))

        # This returns a pair (thetalnew,philnew) as a function of the sign of sin(phis - phil)
        def thetalphilnew(sgn=1):
            # Solve for psibad(thetalnew,philnew), modulus pi:
            #   assume positive sign for sin(phis - philnew); we know the sign of sin(phis - phil)
            thetalnew = bracket_find_root(lambda x: unwrap(psibad(x,sgn) - 0.5*pi - psibad(thetal,sign(sin(phis - phil))) + pol,pi),
                                          0,pi,1e-12)

            if len(thetalnew) > 1:
                raise ValueError, "Found more than one psibad solutions!"
            elif len(thetalnew) == 0:
                raise ValueError, "Cannot find any solution for psibad!"
            else:
                thetalnew = thetalnew[0]

            philnew = phis - acos( (cosi - cos(thetas) * cos(thetalnew)) / (sin(thetas) * sin(thetalnew)) )

            # enforce positive sign for sin(phis - philnew)
            if sign(sin(phis - philnew)) == -sgn:
                philnew = 2*phis - philnew
            
            # enforce [0,2pi] range for philnew
            if philnew < 0:
                philnew = philnew + 2*pi
        
            if dochecks == 1:
                print "--- Checks (tl',pl') ---"
                print "  sign sin(phis - philnew): ", sign(sin(phis - philnew))
                print "  psi equation: ", unwrap(psibad(thetalnew,sgn) - 0.5*pi - psibad(thetal,sign(sin(phis - phil))) + pol,pi)
                print "  cosi: %f -> %f" % (cosi, cos(thetas)*cos(thetalnew) + sin(thetas)*sin(thetalnew)*cos(phis - philnew))
        
            return (thetalnew,philnew)
        
        (thetalnew1, philnew1) = thetalphilnew(1)
        (thetalnew2, philnew2) = thetalphilnew(-1)
        
        # --- SOLVING for thetaknew and phiknew

        # Invariant from Eq. (23) of gwdaw-mldc2 proc
        cosk = cos(thetas) * cos(thetak) + sin(thetas) * sin(thetak) * cos(phis - phik)

        def falpha0(thetalnew,philnew,sgn):
            # this solves for thetaknew as a function of alpha0; there are two branches
            def fthetak(alpha0,sgn=1):
                return acos( ( cos(lam)*cos(thetalnew) + sgn*sqrt(cos(alpha0)**2 * sin(lam)**2 *
                               (cos(lam)**2 - cos(thetalnew)**2 + cos(alpha0)**2 * sin(lam)**2)) ) / (cos(lam)**2 + cos(alpha0)**2 * sin(lam)**2) )

            # use Eqs. (22b-c) to solve for sin/cos(phik - phil)
            def fcosphilk(thetaknew,alpha0):
                return (sin(thetaknew)*cos(lam) - cos(thetaknew)*sin(lam)*cos(alpha0)) / sin(thetalnew)
            def fsinphilk(alpha0):
                return -sin(lam)*sin(alpha0) / sin(thetalnew)

            cosphisl = cos(phis - philnew)
            sinphisl = sin(phis - philnew)

            alpha0new = bracket_find_root(lambda a: ( cos(thetas) * cos(fthetak(a,sgn)) + sin(thetas) * sin(fthetak(a,sgn)) *
                                                        ( cosphisl * fcosphilk(fthetak(a,sgn),a) - sinphisl * fsinphilk(a) ) - cosk ),
                                          0,2*pi,1e-12)

            sols = []
            for alpha0 in alpha0new:
                thetaknew = fthetak(alpha0,sgn)
                                
                phiknew = philnew - atan2(fsinphilk(alpha0),fcosphilk(thetaknew,alpha0))
                if phiknew < 0:
                    phiknew = phiknew + 2*pi
                
                sols.append( (alpha0,thetaknew,phiknew,thetalnew,philnew) )

            return sols

        sols = falpha0(thetalnew1,philnew1,1) + falpha0(thetalnew1,philnew1,-1) + falpha0(thetalnew2,philnew2,1) + falpha0(thetalnew2,philnew2,-1)

        # choose the best solution (defined as the one for which the transformation thetak, phik -> thetal, phil has the least error)

        thesol = None

        for sol in sols:
            (alpha0new,thetaknew,phiknew,thetalnew,philnew) = sol
                        
            checkthetalnew = acos(cos(thetaknew)*cos(lam) + sin(thetaknew)*sin(lam)*cos(alpha0new))
            checkphilnew = atan2(sin(thetaknew) * sin(phiknew) * cos(lam) - cos(phiknew) * sin(lam) * sin(alpha0new) - sin(phiknew) * cos(thetaknew) * sin(lam) * cos(alpha0new),
                                 sin(thetaknew) * cos(phiknew) * cos(lam) + sin(phiknew) * sin(lam) * sin(alpha0new) - cos(phiknew) * cos(thetaknew) * sin(lam) * cos(alpha0new))
            if checkphilnew < 0: checkphilnew = checkphilnew + 2*pi
            checkcosk = cos(thetas) * cos(thetaknew) + sin(thetas) * sin(thetaknew) * cos(phis - phiknew)

            checkall = (checkthetalnew - thetalnew)**2 + (checkphilnew - philnew)**2 + (checkcosk - cosk)**2

            if thesol == None or checkall < oldcheckall:
                thesol = sol
                oldcheckall = checkall

            if dochecks == 1:
                print "--- Checks (a', tl',pl',tk',pk') ---"
                print "  a', tk', pk', tl', pl': %f, %f, %f, %f, %f %s" % (alpha0new,thetaknew,phiknew,thetalnew,philnew,checkall < 1e-6 and "*" or "")
                print "  l'->k->l': %f -> %f, %f -> %f" % (thetalnew,checkthetalnew,philnew,checkphilnew)        
                print "  cosk: %f -> %f" % (cosk, checkcosk)

        (alpha0new,thetaknew,phiknew,thetalnew,philnew) = thesol
        
        # now find changes to gamma

        nscl = sin(thetas) * sin(phik - phis) * sin(lam) * cos(alpha0) + (cosk * cos(thetak) - cos(thetas)) * sin(lam) * sin(alpha0) / sin(thetak)

        sinbeta = (cos(lam) * cosi - cosk) / (sin(lam) * sqrt(1 - cosi**2))
        cosbeta = nscl / (sin(lam) * sqrt(1 - cosi**2))

        beta = atan2(sinbeta,cosbeta)

        nsclnew = sin(thetas) * sin(phiknew - phis) * sin(lam) * cos(alpha0new) + (cosk * cos(thetaknew) - cos(thetas)) * sin(lam) * sin(alpha0new) / sin(thetaknew)

        cosbetanew = nsclnew / (sin(lam) * sqrt(1 - cosi**2))

        betanew = atan2(sinbeta,cosbetanew)
        
        tildegnew = tildeg + beta - betanew

        if dochecks == 1:
            print "--- Checks (gamma) ---"
            print "  gamma: %f -> %f" % (tildeg + beta,tildegnew + betanew)
        
        # write changes to file
        
        print "--- Changes to key file ---"
        print "alpha0: %+f*pi -> %+f*pi" % (alpha0/pi, alpha0new/pi)
        print "tildeg: %+f*pi -> %+f*pi" % (tildeg/pi, tildegnew/pi)
        print "thetak: %+f*pi -> %+f*pi" % (thetak/pi, thetaknew/pi)
        print "phik  : %+f*pi -> %+f*pi" % (phik/pi,   phiknew/pi)
        print "pol   : %+f*pi -> 0" % (pol/pi)
        
        psiSL = atan2(sin(thetal) * sin(phis - phil),
                      cos(thetas) * sin(thetal) * cos(phis - phil) - cos(thetal) * sin(thetas))

        psiLS = atan2(cos(thetas) * sin(thetalnew) * cos(phis - philnew) - cos(thetalnew) * sin(thetas),
                      sin(thetalnew) * sin(phis - philnew))
                
        ccf,    scf    = cos(2*pol - 2*psiSL), sin(2*pol - 2*psiSL)
        newccf, newscf = cos(2*psiLS),         sin(2*psiLS)
        
        print "--- Polarization rotation ---"
        print "cospsi: %+f    -> %+f" % (ccf,newccf)
        print "sinpsi: %+f    -> %+f" % (scf,newscf)
        
        source.InitialTildeGamma = tildegnew
        source.InitialAlphaAngle = alpha0new

        source.PolarAngleOfSpin = thetaknew
        source.AzimuthalAngleOfSpin = phiknew

        source.Polarization = 0

    outputXML.SourceData(source)
