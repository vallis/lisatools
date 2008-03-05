#!/usr/bin/env python

__version__='$Id: $'

import pyRXP
import sys
import os

def recursedelete(tree,filedir):
    if tree[0] == 'Stream' and ('Remote' in tree[1]['Type']):
        for child in tree[2]:
            if type(child) == str:
                datafile = child.strip()
                
                if os.path.isfile(datafile):
                    os.system('rm %s' % datafile)
                elif os.path.isfile(filedir + datafile):
                    os.system('rm %s' % filedir + datafile)
    else:
        if tree[2] != None:
            for child in tree[2]:
                if type(child) == tuple:
                    recursedelete(child,filedir)


p = pyRXP.Parser()

errors = 0

for filename in sys.argv[1:]:
    try:
        tree = p(open(filename).read())
    except:
        print "!!! Cannot open file %s, or it is malformed XML/XSIL. I will skip it." % filename
        errors += 1
        continue
    
    recursedelete(tree,os.path.dirname(os.path.abspath(filename)) + '/')
    os.system('rm %s' % filename)
    
sys.exit(errors)
