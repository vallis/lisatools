#!/usr/bin/env python

import sys
import os

# only one argument, and it's the seed...
seed = int(sys.argv[1])

here = os.getcwd()
os.chdir('../MLDCwaveforms/Galaxy')

# run the Test code

# no need to run Galaxy_Maker, we have Data/Galaxy_1.dat already...
assert(0 == os.system('./Galaxy_key 1 Test'))

os.chdir(here)

assert(0 == os.system('cp ../MLDCwaveforms/Galaxy/XML/Test_key.xml Galaxy/TestGalaxy.xml'))

# assert(0 == os.system('./Galaxy_Maker %s 1' % (seed+4))
# assert(0 == os.system('./Galaxy_Key BackgroundGalaxy %s' % (seed+4))
