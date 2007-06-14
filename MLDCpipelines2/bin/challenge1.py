#!/usr/bin/env python

import os
import sys
import random
from random import randint

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

# only one argument, and it's the superglobal seed...
seed = int(sys.argv[1])

random.seed(seed)

seed111a,seed111at = randint(1,100000), randint(1,100000)
seed111b,seed111bt = randint(1,100000), randint(1,100000)
seed111c,seed111ct = randint(1,100000), randint(1,100000)

seed112,seed112t = randint(1,100000), randint(1,100000)

seed121,seed121t = randint(1,100000), randint(1,100000)
seed122,seed122t = randint(1,100000), randint(1,100000)

seed131,seed131t = randint(1,100000), randint(1,100000)
seed132,seed132t = randint(1,100000), randint(1,100000)
seed133,seed133t = randint(1,100000), randint(1,100000)
seed134,seed134t = randint(1,100000), randint(1,100000)
seed135,seed135t = randint(1,100000), randint(1,100000)

run('bin/challenge2.py            --duration=31457280 --seed=%(seed112)s  challenge1.1.2' % globals())
run('bin/challenge2.py --training --duration=31457280 --seed=%(seed112t)s challenge1.1.2' % globals())


run('bin/challenge2.py            --duration=31457280 --seed=%(seed111a)s  challenge1.1.1a' % globals())
run('bin/challenge2.py --training --duration=31457280 --seed=%(seed111at)s challenge1.1.1a' % globals())

run('bin/challenge2.py            --duration=31457280 --seed=%(seed111b)s  challenge1.1.1b' % globals())
run('bin/challenge2.py --training --duration=31457280 --seed=%(seed111bt)s challenge1.1.1b' % globals())

run('bin/challenge2.py            --duration=31457280 --seed=%(seed111c)s  challenge1.1.1c' % globals())
run('bin/challenge2.py --training --duration=31457280 --seed=%(seed111ct)s challenge1.1.1c' % globals())


run('bin/challenge2.py            --duration=31457280 --seed=%(seed121)s  challenge1.2.1' % globals())
run('bin/challenge2.py --training --duration=31457280 --seed=%(seed121t)s challenge1.2.1' % globals())

run('bin/challenge2.py            --duration=31457280 --seed=%(seed122)s  challenge1.2.2' % globals())
run('bin/challenge2.py --training --duration=31457280 --seed=%(seed122t)s challenge1.2.2' % globals())

run('bin/challenge2.py            --duration=62914560 --seed=%(seed131)s  challenge1.3.1' % globals())
run('bin/challenge2.py --training --duration=62914560 --seed=%(seed131t)s challenge1.3.1' % globals())

run('bin/challenge2.py            --duration=62914560 --seed=%(seed132)s  challenge1.3.2' % globals())
run('bin/challenge2.py --training --duration=62914560 --seed=%(seed132t)s challenge1.3.2' % globals())

run('bin/challenge2.py            --duration=62914560 --seed=%(seed133)s  challenge1.3.3' % globals())
run('bin/challenge2.py --training --duration=62914560 --seed=%(seed133t)s challenge1.3.3' % globals())

run('bin/challenge2.py            --duration=62914560 --seed=%(seed134)s  challenge1.3.4' % globals())
run('bin/challenge2.py --training --duration=62914560 --seed=%(seed134t)s challenge1.3.4' % globals())

run('bin/challenge2.py            --duration=62914560 --seed=%(seed135)s  challenge1.3.5' % globals())
run('bin/challenge2.py --training --duration=62914560 --seed=%(seed135t)s challenge1.3.6' % globals())
