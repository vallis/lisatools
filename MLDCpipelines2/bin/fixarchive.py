#!/usr/bin/env python

import sys
import os
import re

def run(s):
    os.system(s % globals())

if len(sys.argv) < 2:
    print "Use %s ARCHIVE.tar.gz to replace the archive, fixing the time axis and removing stray nonoise files" % sys.argv[0]
    sys.exit(0)

archive = sys.argv[1]

if '.tar.gz' in archive:
    challengename = os.path.basename(re.sub('\.tar\.gz','',archive))
    backup = re.sub('\.tar\.gz','-backup.tar.gz',archive)
else:
    print "Please give filename of tar-gzipped archive"
    sys.exit(0)

# untar and make a copy
run('mkdir %(challengename)s')
run('tar zxvf %(archive)s -C %(challengename)s')
run('mv %(archive)s %(backup)s')

# fix the primary binary
run('fixbinary.py %(challengename)s/%(challengename)s-0.bin %(challengename)s/%(challengename)s-0.bin')

# remove the spurious binary if present
if os.path.isfile('%(challengename)s/%(challengename)s-nonoise-0.bin' % globals()) and ('nonoise' not in challengename):
    run('rm %(challengename)s/%(challengename)s-nonoise-0.bin')

# tar up the directory and remove it
run('tar zcvf %(archive)s %(challengename)s')
run('rm -rf %(challengename)s')
