#!/usr/bin/env python

import os
import sys
import random

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

# two arguments, the first is the seed, the second may be True for training
seed = int(sys.argv[1])
istraining = sys.argv[2]

random.seed(seed)

#               source name    ecl lat         ecl long        f training         f challenge    
verification = {'RXJ0806'   : (0.2697997832  , 2.100953649   , 0.006220276603   , 0.005907385444) ,
                'V407Vul'   : (0.4353820782  , 5.147481878   , 0.00351250011    , 0.00367987089)  ,
                'ESCet'     : (-0.1642146356 , 0.4258413339  , 0.003220611916   , 0.00332835297)  ,
                'AMCVn'     : (0.6567450961  , 2.97249322    , 0.001944144722   , 0.00198757488)  ,
                'HPLib'     : (-0.2481858196 , 4.101821899   , 0.001813236627   , 0.001777716317) ,
                'EIPsc'     : (0.1129174697  , 6.205956271   , 0.0005194805195  , 0.0005211327983),
                'bltOPEN7'  : (0.1996953268  , 4.757415      , 0.002095374315   , 0.002004086634) ,
                'bltOPEN8'  : (-0.6154346732 , 4.13981       , 0.001839178549   , 0.001767880534) ,
                'bltOPEN9'  : (-0.1422176732 , 4.649333      , 0.001910351046   , 0.001874145362) ,
                'bltOPEN10' : (0.01476832679 , 4.756822      , 0.006334684838   , 0.006544361142) ,
                'bltOPEN11' : (0.7497998268  , 5.174382      , 0.003208731343   , 0.003250185775) ,
                'bltOPEN12' : (0.1200253268  , 4.780427      , 0.003502950886   , 0.00329556522)  ,
                'bltOPEN13' : (-0.7670126732 , 4.047208      , 0.002926858537   , 0.003062759803) ,
                'bltOPEN14' : (-0.3583376732 , 4.46843       , 0.002706457364   , 0.002489978296) ,
                'bltOPEN15' : (-0.2166256732 , 4.607265      , 0.002506720518   , 0.002355954312) ,
                'bltOPEN16' : (-0.2676646732 , 4.50488       , 0.004841122821   , 0.004974343896) ,
                'bltOPEN17' : (-0.06588667321, 4.663938      , 0.003375130955   , 0.003190094085) ,
                'bltOPEN18' : (0.04105132679 , 4.748964      , 0.004139974183   , 0.004146559486) ,
                'bltOPEN19' : (-0.04352567321, 4.749986      , 0.007870408994   , 0.008505086786) ,
                'bltOPEN20' : (-0.01221367321, 4.728495      , 0.002455096892   , 0.002351154304) }

for key in verification:
    requestsn = random.randint(10,20)
    
    sourcename = key
    sourcelat  = verification[key][0]
    sourcelon  = verification[key][1]    
    
    if istraining == 'True':
        sourcef = verification[key][2]
    else:
        sourcef = verification[key][3]
    
    run('bin/makesource-GalacticBinary.py --seed=%(seed)s --amplitude=1e-21 --latitude=%(sourcelat)s --longitude=%(sourcelon)s --requestSN=%(requestsn)s --centerf=%(sourcef)s --deltaf=0.0 --sourceName="%(sourcename)s" Source/GB-1.1.2-%(sourcename)s.xml' % globals())

seed += len(verification)