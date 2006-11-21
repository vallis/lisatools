#!/usr/bin/env python

import os
import sys

# only one argument, and it's the seed...
seed = int(sys.argv[1])

# standard BBH sources...
assert(0 == os.system('bin/makesource-BBH.py --seed=%s --distance=1e6 --coalescTime=150 Source/BH-1.xml' % (seed+1)))
assert(0 == os.system('bin/makesource-BBH.py --seed=%s --distance=2e6 --coalescTime=120 Source/BH-2.xml' % (seed+2)))

# standard EMRI source...
assert(0 == os.system('bin/makesource-EMRI.py --seed=%s --distance=1e6 Source/EMRI-1.xml' % (seed+3)))

# now the Galaxy!
assert(0 == os.system('bin/makesource-Galaxy.py %s' % seed))
