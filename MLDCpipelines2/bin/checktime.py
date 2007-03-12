#!/usr/bin/env python

import sys
import synthlisa

a = synthlisa.readXML(sys.argv[1])
b = a.getTDITimeSeries()

print b[0]['Data'][0:2]
