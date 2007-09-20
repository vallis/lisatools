#!/usr/bin/python
##
## Copyright (C) 2007 John T Whelan, Reinhard Prix
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with with program; see the file COPYING. If not, write to the 
##  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
##  MA  02111-1307  USA
##

from pylab import *
text.usetex = True
from matplotlib import rc
rc('text', usetex=True)
from scipy import unwrap
from scipy.special import erfinv
from scipy.special import erf
from optparse import *


# ----- Begin script
usage = usage = """\
skyAngle.py [options]

  Make a plot of angle errors

  ----------------------------------------------------------------------------
  --cand-file:   Not actually read in, but its name is used to deduce
                 the names of other files  
  --out-prefix:  Prefix for generated figures
  --extra-title: Extra text for title of figures
  --min-freq:    Lower frequency for plots
  --max-freq:    Upper frequency for plots
  ----------------------------------------------------------------------------   
"""

##############################################################################
parser = OptionParser( usage=usage, version= "%prog CVS $Id: skyAngle.py,v 1.2 2007/09/13 12:45:05 whelan Exp $")

# used letters:
# fiot
# F

parser.add_option("-i","--cand-file",action="store",type="string",
                  dest="resname");

parser.add_option("-o","--out-prefix",action="store",type="string",
                  dest="outname");

parser.add_option("-t","--extra-title",action="store",type="string",
                  dest="xtit");

parser.add_option("-f","--min-freq",action="store",type="float",
                  dest="minfreq");

parser.add_option("-F","--max-freq",action="store",type="float",
                  dest="maxfreq");

(opts,args) = parser.parse_args();
resname = opts.resname;

try:
    slind = resname.rindex('/');
    resdir = resname[:slind];
    bname = resname[(slind+1):];
except ValueError:
    resdir = '.';
    bname = resname;

if ( opts.outname ):
    outname = opts.outname;
else:
    try:
        dind = bname.rindex('.dat');
        outname = bname[:dind];
    except ValueError:        
        outname = bname;

if ( opts.xtit ):
    xtit = opts.xtit + ' ';
else:
    xtit = '';

if ( opts.minfreq ):
    minfreq = opts.minfreq;
else:
    minfreq = 0;

if ( opts.maxfreq ):
    maxfreq = opts.maxfreq;
else:
    maxfreq = 20;

# ----- challenge 2.1
matched = load(resdir + '/MATCHED_' + bname, comments="%");

matchedCand = matched[arange(0,shape(matched)[0],2),:];
matchedKey = matched[arange(1,shape(matched)[0],2),:];

matchedInBand = find( ( matchedCand[:,0] >= minfreq/1e3 )
                      & ( matchedCand[:,0] <= maxfreq/1e3 ) );

sys.stdout.write( "%d found sources with %.3fmHz < %.3fmHz\n"
                  % (shape(matchedInBand)[0],minfreq,maxfreq) );

matchedCand = matchedCand[matchedInBand,:];
matchedKey = matchedKey[matchedInBand,:];

freqsmHz = matchedCand[:,0]*1e3;

figure(figsize=(5,3));
axes([0.125,0.15,0.95-0.125,0.9-0.15]);
dSky = arccos(cos(matchedCand[:,1])*cos(matchedKey[:,1])
                  * cos(matchedCand[:,2]-matchedKey[:,2])
                  + sin(matchedCand[:,1])*sin(matchedKey[:,1]));
semilogy(freqsmHz,1e3*dSky,'k.',ms=3);
hemind = find (matchedCand[:,1]*matchedKey[:,1] < 0);
semilogy(freqsmHz[hemind],1e3*dSky[hemind],'ro',mfc='None',mec='r',ms=3);
title (xtit + "Challenge 2.1 SkyPos Errors");
xlabel(r'$f \rm{(mHz)}$');
ylabel(r'$\phi_{\rm{sky}} \rm{(mrad)}$');
grid(1);
legend(['sky mismatch','hemisphere flip'],numpoints=1,loc='upper right');
xlim(minfreq, maxfreq);
# ylim(0.5, 1000);
tickvals = [1, 3, 10, 30, 100, 300, 1000];
ticklabs = map(lambda val:"%d"%val, tickvals);
yticks(tickvals,ticklabs);
savefig(outname + 'skyAngle.eps');
savefig(outname + 'skyAngle.png');
