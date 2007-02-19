#!/usr/bin/env python

import sys
import numpy

infile  = sys.argv[1]
outfile = sys.argv[2]

# assume same endianness

readlength = 4 * 4194304 * 8

binaryfile = open(infile,'r')
readbuffer = numpy.fromstring(binaryfile.read(readlength),'double')
binaryfile.close()

# readbuffer = readbuffer.byteswap()

readbuffer = numpy.reshape(readbuffer,(4194304,4))

readbuffer[:,0] = numpy.arange(0,62914560,15,dtype='d')

binaryfile = open(outfile,'w')
binaryfile.write(readbuffer.tostring())
binaryfile.close()
