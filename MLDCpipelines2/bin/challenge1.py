#!/usr/bin/env python

import os
import sys
import random
from random import randint
from optparse import OptionParser

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

parser = OptionParser(usage="usage: %prog [options] SEED...",
                      version="$Id: $")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

(options, args) = parser.parse_args()

nproc = options.nproc
seed = args[0]

#print "seed = ", seed
#print "nproc = ", nproc

# only one argument, and it's the superglobal seed...
#seed = int(sys.argv[1])

random.seed(seed)

def twoseeds():
    return randint(1,1000000), randint(1,1000000)

seed111a,seed111at = twoseeds()
seed111b,seed111bt = twoseeds()
seed111c,seed111ct = twoseeds()

seed112,seed112t = twoseeds()
seed113,seed113t = twoseeds()
seed114,seed114t = twoseeds()
seed115,seed115t = twoseeds()

seed121,seed121t = twoseeds()
seed122,seed122t = twoseeds()

seed131,seed131t = twoseeds()
seed132,seed132t = twoseeds()
seed133,seed133t = twoseeds()
seed134,seed134t = twoseeds()
seed135,seed135t = twoseeds()


run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed111a)s  challenge1B.1.1a' % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed111at)s challenge1B.1.1a' % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed111b)s  challenge1B.1.1b' % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed111bt)s challenge1B.1.1b' % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed111c)s  challenge1B.1.1c' % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed111ct)s challenge1B.1.1c' % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed112)s   challenge1B.1.2'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed112t)s  challenge1B.1.2'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed113)s   challenge1B.1.3'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed113t)s  challenge1B.1.3'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed114)s   challenge1B.1.4'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed114t)s  challenge1B.1.4'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed115)s   challenge1B.1.5'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed115t)s  challenge1B.1.5'  % globals())
                                                                                                 
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed121)s   challenge1B.2.1'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed121t)s  challenge1B.2.1'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=31457280 --seed=%(seed122)s   challenge1B.2.2'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=31457280 --seed=%(seed122t)s  challenge1B.2.2'  % globals())
                                                                                                 
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=62914560 --seed=%(seed131)s   challenge1B.3.1'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=62914560 --seed=%(seed131t)s  challenge1B.3.1'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=62914560 --seed=%(seed132)s   challenge1B.3.2'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=62914560 --seed=%(seed132t)s  challenge1B.3.2'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=62914560 --seed=%(seed133)s   challenge1B.3.3'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=62914560 --seed=%(seed133t)s  challenge1B.3.3'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=62914560 --seed=%(seed134)s   challenge1B.3.4'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=62914560 --seed=%(seed134t)s  challenge1B.3.4'  % globals())
                                                                                                 
run('%(mydir)s/challenge.py -P %(nproc)s            --duration=62914560 --seed=%(seed135)s   challenge1B.3.5'  % globals())
run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=62914560 --seed=%(seed135t)s  challenge1B.3.5'  % globals())
