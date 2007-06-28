#!/usr/bin/env python

import os
import sys
import re

try:
    inputfile, outputfile = sys.argv[1:3]

    assert '.xml' in inputfile
    assert '.xml' in outputfile
except:
    print "Please give me a source and a target .xml files!"
    sys.exit(0)

basein  = inputfile[:-4]
baseout = outputfile[:-4]

binfiles = 0
while True:
    binin = '%s-%s.bin' % (basein,binfiles)
    if os.path.isfile(binin):
        binout = '%s-%s.bin' % (baseout,binfiles)
        os.system('cp %s %s' % (binin,binout))
        binfiles += 1
    else:
        break

out = open(outputfile,'w')

for line in open(inputfile):
    out.write(re.sub(os.path.basename(basein),os.path.basename(baseout),line))
