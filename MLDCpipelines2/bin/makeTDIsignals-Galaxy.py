#!/usr/bin/env python

import os

# only one argument, and it's the seed...
seed = int(sys.argv[1])

here = os.getcwd()
os.chdir('../MLDCwaveforms/Galaxy')

# compute the instrument response (takes a couple of hours)
assert(0 == os.system('./Fast_Response 1')) 
assert(0 == os.system('./Fast_XML_LS Test Data/Sources_LS.txt')) 
assert(0 == os.system('./Fast_XML_SL Test Data/Sources_SL.txt')) 

os.chdir(here)

assert(0 == os.system('cp ../MLDCwaveforms/Galaxy/XML/Test_synthLISA_noisefree.xml TDI/Test-tdi-frequency.xml'))
assert(0 == os.system('cp ../MLDCwaveforms/Galaxy/XML/Test_LISAsim_noisefree.xml   TDI/Test-tdi-frequency.xml'))

assert(0 == os.system('mkdir TDI/Binary'))
assert(0 == os.system('cp ../MLDCwaveforms/Galaxy/Binary/Test_synthLISA_noisefree.bin TDI/Binary/.'))
assert(0 == os.system('cp ../MLDCwaveforms/Galaxy/Binary/Test_LISAsim_noisefree.bin   TDI/Binary/.'))

# this should relocate the binaries...
assert(0 == os.system('bin/mergeXML.py TDI/Test-tdi-frequency.xml Source/TestGalaxy.xml'))