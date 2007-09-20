#! /usr/bin/octave -q
%%
%% Copyright (C) 2007 John T Whelan, Reinhard Prix
%%
%%  This program is free software; you can redistribute it and/or modify
%%  it under the terms of the GNU General Public License as published by
%%  the Free Software Foundation; either version 2 of the License, or
%%  (at your option) any later version.
%%
%%  This program is distributed in the hope that it will be useful,
%%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%%  GNU General Public License for more details.
%%
%%  You should have received a copy of the GNU General Public License
%%  along with with program; see the file COPYING. If not, write to the 
%%  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
%%  MA  02111-1307  USA
%%


## given two input arguments: a result-file and a key-file, try to estimate the errors

ARGV = argv;

days = 86400;
weeks = 7 * days;
years = 365 * days;

Tobs = 2 * years;

if ( nargin < 2 || nargin > 3 )
  error ("Usage: compareResults2Key_pipeline2.m <res-file> <key-file> [ <coincMismatch> ] \n\n");
endif
if ( nargin < 3 )
  coincMismatch = 1.0;
else
  coincMismatch = str2num(ARGV{3});
endif
resFname = ARGV{1};
keysFname = ARGV{2};

## ========== Function definitions ==================================================
function Amu = get_Amu ( Amplitude, Inclination, Polarization, InitialPhase )
  ## compute A1 - A4 from MLDC amplitude parameters 

  cosi = - cos ( Inclination );
  Aplus = Amplitude * ( 1 + cosi^2 );
  Across = 2 * Amplitude * cosi;

  psi = pi/2 - Polarization ;
  phi0 = InitialPhase + pi; ## Mystery sign flip

  cosphi = cos(phi0);
  sinphi = sin(phi0);
  cos2psi = cos(2*psi);
  sin2psi = sin(2*psi);


  ## A^1 = A_+ \cos\phi_0 \cos 2\psi - A_\times \sin\phi_0 \sin 2\psi
  A1 = Aplus * cosphi * cos2psi - Across * sinphi * sin2psi;

  ## A^2 =A_+ \cos\phi_0 \sin 2\psi + A_\times \sin\phi_0 \cos 2\psi
  A2 = Aplus * cosphi * sin2psi + Across * sinphi * cos2psi;

  ## A^3 = -A_+ \sin\phi_0 \cos 2\psi - A_\times \cos\phi_0 \sin 2\psi
  A3 = - Aplus * sinphi * cos2psi - Across * cosphi * sin2psi;

  ## A^4 = -A_+\sin\phi_0 \sin 2\psi + A_\times \cos\phi_0 \cos 2\psi
  A4 = -Aplus * sinphi * sin2psi + Across * cosphi * cos2psi;

  Amu = [A1, A2, A3, A4 ];

endfunction

%% --------------------------------------------------
function ret = readCandidatesFromKey ( fname )
  ## read key candidates in 'intermediate candidates' format

  dat = load(fname);

  ## format used in writing (writeIntermediateCandidate())
  ##ret = fprintf (fid,  "%15.13f   %-14.11f   %13.11f   %8.4f  	%9g %9g %9g %9g       %g %g %g %g \n",
  ##             candi.Freq,  candi.Delta, candi.Alpha, candi.twoF, 
  ##             candi.A1, candi.A2, candi.A3, candi.A4, 
  ##		 candi.Ad, candi.Bd, candi.Cd, candi.Sinv_Tsft );
  numCols = columns ( dat );
  numRows = rows ( dat );

  if ( numCols != 12 )
    error ("Incomplete key-file , cols = %d != 12 \n", numCols );
  endif
  
  ret = cell ( 1, numRows );	## malloc beforehand is much faster

  for i = 1:numRows
    cand.Freq  = dat(i,1);
    cand.Delta = dat(i,2);
    cand.Alpha = dat(i,3);

    cand.twoF  = dat(i,4);

    cand.A1    = dat(i,5);
    cand.A2    = dat(i,6);
    cand.A3    = dat(i,7);
    cand.A4    = dat(i,8);

    cand.As2   = cand.A1^2 + cand.A2^2 + cand.A3^2 + cand.A4^2;

    cand.Ad    = dat(i,9);
    cand.Bd    = dat(i,10);
    cand.Cd    = dat(i,11);
    cand.Sinv_Tsft = dat(i,12);
  
    ret{i} = cand;
  endfor

  return;
endfunction ## readCandidatesFromKey()
%% --------------------------------------------------

function ret = readMLDC2Candidates ( fname )
  ## read 'MLDC2' candidates in final-result format

  dat = load(fname);

  ##  i, candList{i}.Freq,  candList{i}.Delta, candList{i}.Alpha, Amplitude, Polarization, Inclination, InitialPhase
  numRows = rows ( dat );
  numCols = columns ( dat );
  if ( numCols != 8 )
    error ("Illegal number of columns (%d) in MLDC2 results file\n", numCols );
  endif

  ret = cell ( 1, numRows );	## malloc beforehand is much faster

  for i = 1:numRows
    cand.Freq  = dat(i,2);
    cand.Delta = dat(i,3);
    cand.Alpha = dat(i,4);

    Amp = dat(i,5);
    Pol = dat(i,6);
    Incl= dat(i,7);
    Phase0 = dat(i,8);

    h0   = 2 * Amp;
    cosi = cos ( pi - Incl );

    Ap = 0.5 * h0 * ( 1 + cosi^2 );
    Ac = h0 * cosi;
    cand.As2 = Ap^2 + Ac^2;

    Amu = get_Amu ( Amp, Incl, Pol, Phase0 );

    cand.A1 = Amu(1);
    cand.A2 = Amu(2);
    cand.A3 = Amu(3);
    cand.A4 = Amu(4);
  
    ret{i} = cand;
  endfor

  return;

endfunction ## readMLDC2Candidates()

## --------------------------------------------------
function ret = sortCandListbyAs2 ( candList )
  ## return a cand-list sorted by descending As2

  numCands = length(candList);

  As2 = zeros(1,numCands);
  for i = 1:numCands;
    As2(i) = candList{i}.As2;
  endfor
  
  [els, sortedInds] = sort ( As2, "descend" );
  ret = candList ( sortedInds );

endfunction ## sortCandListbyAs2()

## --------------------------------------------------
function canonicalCoords = convertAstronomical2Canonical ( astronomicalCoords )
  ## this is the inverse function to 'convertCanonical2Astronomical()'
  ## astronomical: { Freq, Alpha, Delta }
  ## canonical: { om0, kX, kY }, where
  ## om0 = 2 pi Freq
  ## kX/Y = - 2 pi (Vorb/c) Freq nX/Y
  ## nX = cos(Delta) cos(Alpha), nY = cos(Delta) sin(Alpha) , nZ = sin(Delta) = sqrt(1 - nX^2 - nY^2)
  ## inversion: Alpha = atan2( kY, kX ), Delta = asin ( nZ )

  ## taken from UnitsConstants.m
  ## AU_SI	= 1.4959787066e11;	%% Astronomical unit, m 
  ## C_SI 	= 299792458; 		%% Speed of light in vacuo, m s^-1 
  ## YRSID_SI	= 31558149.8;		%% Sidereal year (1994), s 
  ## Vorb       = 2 * pi * AU_SI / YRSID_SI 
  ## --> Vorb =  2.9785e+04;
  ## twoPiVorb_C= (2 * pi * Vorb / C_SI); %% 2pi Vorb/c
  twoPiVorb_C = 0.00062424;

  Freq  = astronomicalCoords.Freq;
  Alpha = astronomicalCoords.Alpha;
  Delta = astronomicalCoords.Delta;
  
  om0 = 2 * pi * Freq;

  nX = cos(Alpha) .* cos(Delta);
  nY = sin(Alpha) .* cos(Delta);

  kX = - twoPiVorb_C * Freq .* nX;
  kY = - twoPiVorb_C * Freq .* nY;

  canonicalCoords.om0 = om0;
  canonicalCoords.kX  = kX;
  canonicalCoords.kY  = kY;

  return;

endfunction ## convertCanonical2Astronmical()


## --------------------------------------------------
function [bname, dirname ] = basename ( path, ext )
  ## split given path into the basename (without 'ext') + dirname 

  ## lose the extension and path and use as results-directory
  pos1 = rindex (path, "/");

  if ( exist ("ext") && (length(ext) > 0) )
    pos2 = rindex (path, ext);
  else
    pos2 = length(path) + 1;
  endif

  bname = substr ( path, pos1 + 1, pos2 - pos1 - 1 );
  dirname = substr ( path, 1, pos1 - 1 );
  
  return;

endfunction ## basename()

## --------------------------------------------------
function mis = computeDopplerMismatchCanonical ( canon0, canoni, gij )
  ## compute metric mismatch between canon0 and each candidate in canoni
  ## assumes they've alread been converted by convertAstronomical2Canonical

  global debug;

  if ( length(canon0) == 0 )
    mis = -1;
    return;
  endif

  dom0 = canoni.om0 - canon0.om0;
  dkX  = canoni.kX  - canon0.kX;
  dkY  = canoni.kY  - canon0.kY;

  mis = gij(1,1) .* dom0.^2 + 2 * gij(1,2) .* dom0 .* dkX + 2 * gij(1,3) .* dom0 .* dkY	\
                                + gij(2,2) .* dkX.^2      + 2 * gij(2,3) .* dkX .* dkY 	\
                                                              + gij(3,3) .* dkY.^2 ;

  return;

endfunction ## computeDopplerMismatchCanonical()


## --------------------------------------------------
function writeIntermediateCandidate ( fid, candi )
  ## write params for 'intermediate candidates'
  if ( !isfield ( candi, "Freq" ) || !isfield ( candi, "Alpha" ) || !isfield (candi, "Delta") )
    error ("Required fields 'Freq', 'Alpha', 'Delta' missing in input structure.\n");
  endif

  have_twoF  = isfield (candi, "twoF");
  have_Amu   = isfield (candi, "A1");
  have_Mmunu = isfield (candi, "Ad" );

  ## ----- first line: write header-line ----- 
  if ( ftell(fid) == 0 )	
    fprintf (fid, "%%%% -*- truncate-lines: t; fill-column: 180 -*-\n");	## little emacs-hooks for better viewing
    fprintf (fid, "%%%%         Freq            Lat            Long");
    if ( have_twoF || have_Amu || have_Mmunu )
      fprintf (fid, "                2F");
    endif
    if ( have_Amu || have_Mmunu )
      fprintf (fid, "                A1              A2              A3              A4");
    endif
    if ( have_Mmunu )
      fprintf (fid, "           Ad         Bd         Cd   Sinv_Tsft");
    endif
    fprintf (fid, "\n");
  endif

  ## ----- write data-lines from this candidate-struct [can have array-entries ] ----- 
  for i = 1:length(candi.Freq)
    ## basic candidate-info: must exist always
    fprintf (fid,  "%15.13f %+14.11f %15.11f",
	     candi.Freq(i),  candi.Delta(i), candi.Alpha(i) );

    ## write 2F if given or if field needs to be filled
    twoF_format = "   %15.4f";
    if ( have_twoF ) twoF = candi.twoF(i); else twoF = NaN; endif
    if ( have_twoF || have_Amu || have_Mmunu )
      fprintf ( fid, twoF_format, twoF )
    endif

    ## write Amus if given or if the fields need to be filled 
    Amu_format = "   % 15.9g % 15.9g % 15.9g % 15.9g";
    if ( have_Amu ) Amu = [ candi.A1(i), candi.A2(i), candi.A3(i), candi.A4(i) ]; else Amu = [NaN, NaN, NaN, NaN]; endif
    if ( have_Amu || have_Mmunu )
      fprintf (fid, Amu_format, Amu );
    endif

    ## write Mmunu if given 
    Mmunu_format = "   % 10.6g % 10.6g % 10.6g %10.6g";
    if ( have_Mmunu )
      fprintf (fid, Mmunu_format, candi.Ad(i), candi.Bd(i), candi.Cd(i), candi.Sinv_Tsft(i) );
    endif
    
    fprintf (fid, "\n");
  endfor

  return;

endfunction ## writeIntermediateCandidate()

function Mmunu = getMmunu4Cand ( cand )
  %% compute the amplitude-metric M_mu_nu for candidate (which contains Ad, Bd, Cd, Sinv_Tsft)

  Mmunu = [ cand.Ad, cand.Cd,       0,       0; 
	    cand.Cd, cand.Bd,       0,       0;
	          0,       0, cand.Ad, cand.Cd;
	          0,       0, cand.Cd, cand.Bd; ];

  Mmunu *= 0.5 * cand.Sinv_Tsft;

  return;

endfunction

## ========== MAIN CODE ==================================================

res = readMLDC2Candidates ( resFname );
printf ("%d candidates in results\n", length(res) );

keys = readCandidatesFromKey ( keysFname );
printf ("%d sources in key\n", length(keys) );

## sort candidates and keys by decreasing 2F value
printf ("Sorting candidates by As2 ...");
res = sortCandListbyAs2(res);
printf ("done\n");
printf ("Sorting keys by As2...");
keys = sortCandListbyAs2(keys);
printf ("done\n");

numRes = length ( res );
numKeys = length ( keys );


## get flat LISA metric for comparison
##gij = getLISAMetric ( Tobs, codesDir );

gij = [ 
       3.3151e+14,    1.1133e+11,   2.5244e+13;
       1.1133e+11,    1.2605e+13,   7.8191e+07;
       2.5244e+13,    7.8191e+07,   1.2622e+13;
       ];	## hardcoded for standalone evaluation

myBlank = zeros(1, numKeys);

isFound = myBlank;
isMatched = myBlank;

key_Freqs = myBlank;
key_Alphas = myBlank;
key_Deltas = myBlank;
printf ("Preparing key-arrays ... ");
for ii = 1:numKeys
  key_Freqs(ii) = keys{ii}.Freq;
  key_Alphas(ii) = keys{ii}.Alpha;
  key_Deltas(ii) = keys{ii}.Delta;
endfor
printf ("done.\nConverting keys to canonical coords...");
keysArray.Freq = key_Freqs;
keysArray.Alpha = key_Alphas;
keysArray.Delta = key_Deltas;
keysCanon = convertAstronomical2Canonical ( keysArray );
printf ("done\n");

[ resBname, resDirName  ] = basename ( resFname );

matchfname = strcat ( "." , "/MATCHED_", resBname );
falsefname = strcat ( "." , "/FALSE_", resBname );

if ( ( matchfid = fopen ( matchfname, "wb" )) == -1 )
  error ("Failed to open match file '%s' for writing!\n\n", matchfname );
endif

if ( ( falsefid = fopen ( falsefname, "wb" )) == -1 )
  error ("Failed to open false file '%s' for writing!\n\n", falsefname );
endif

digs = num2str(length(num2str(numRes)));

##coincname = ["./COINC_" resBname];
##if ( ( [myerr, mymsg] = mkdir(coincname) ) == -1 )
##  if ( !isdir(coincname) )
##    error ("Failed to create directory '%s'!\n\nError was '%s'\n\n",
##	   coincname, myerr );
##  endif
##endif

matchedMax = zeros(1,numRes);
numMatched = 0;

for ii = 1:numRes
  myRes.Freq = res{ii}.Freq;
  myRes.Alpha = res{ii}.Alpha;
  myRes.Delta = res{ii}.Delta;
  resCanon = convertAstronomical2Canonical ( myRes );
  mis = computeDopplerMismatchCanonical ( resCanon, keysCanon, gij );
  matchInd = find( mis <= coincMismatch );
  isMatched(matchInd) = ones(size(matchInd));
  unFoundInd = find( !isFound(matchInd) );
  if ( length(unFoundInd) > 0 )
    myFoundInd = matchInd(unFoundInd(1));
    isFound(myFoundInd) = 1;
    thisKey = keys{myFoundInd};
    Mmunu = getMmunu4Cand ( thisKey );
    Amu = [ res{ii}.A1, res{ii}.A2, res{ii}.A3, res{ii}.A4 ];
    res{ii}.twoF = Amu * Mmunu * Amu';
    res{ii}.Ad = thisKey.Ad; res{ii}.Bd = thisKey.Ad; res{ii}.Cd = thisKey.Cd; res{ii}.Sinv_Tsft = thisKey.Sinv_Tsft;
    writeIntermediateCandidate( matchfid, res{ii} );
    writeIntermediateCandidate( matchfid, thisKey );
    fprintf (matchfid, "\n");
    ##myfname = sprintf ( [coincname "/COINC_%0." digs "d_" resBname], ii );
    ##if ( ( myfid = fopen ( myfname, "wb" )) == -1 )
    ##error ("Failed to open coinc file '%s' for writing!\n\n", myfname );
    ##endif
    ##writeIntermediateCandidate( myfid, res{ii});
    ##fprintf (myfid, "\n");
    ##for jj = matchInd
    ##writeIntermediateCandidate( myfid, keys{jj});
    ##endfor
    ##fclose ( myfid );
  else
    ## for false-alarms we need to "estimate" the two-F value
    ## by using a 'wrong' Mmunu matrix: take the first one from key
    firstKey = keys{1};
    MmunuWrong = getMmunu4Cand ( firstKey );
    res{ii}.Ad = firstKey.Ad; res{ii}.Bd = firstKey.Ad; res{ii}.Cd = firstKey.Cd; res{ii}.Sinv_Tsft = firstKey.Sinv_Tsft;
    Amu = [ res{ii}.A1, res{ii}.A2, res{ii}.A3, res{ii}.A4 ];
    res{ii}.twoF = Amu * MmunuWrong * Amu';

    writeIntermediateCandidate( falsefid, res{ii});
  endif
endfor %% ii < numRes

fclose ( falsefid );
fclose ( matchfid );

missfname = strcat ( "." , "/MISSED_", resBname );
if ( ( missfid = fopen ( missfname, "wb" )) == -1 )
  error ("Failed to open missed sources file '%s' for writing!\n\n", missfname );
endif

fortyfname = strcat ( "." , "/MISSED40_", resBname );
if ( ( fortyfid = fopen ( fortyfname, "wb" )) == -1 )
  error ("Failed to open missed sources file '%s' for writing!\n\n", fortyfname );
endif

hidfname = strcat ( "." , "/HIDDEN_", resBname );
if ( ( hidfid = fopen ( hidfname, "wb" )) == -1 )
  error ("Failed to open hidden sources file '%s' for writing!\n\n", missfname );
endif

missind = find ( !isMatched );
hidind = find ( (!isFound) & isMatched );

numFound = sum(isFound);

printf ("%d sources found\n", numFound);
printf ("%d sources hidden\n", length(hidind));
printf ("%d sources unhidden but missed\n", length(missind));
printf ("%d false alarms\n", numRes - numFound);

printf ("\nWriting missed and hidden candidate files ...");
for jj = missind
  writeIntermediateCandidate( missfid, keys{jj} );
  if ( keys{jj}.twoF > 40 )
    writeIntermediateCandidate( fortyfid, keys{jj} );
  endif
endfor

for jj = hidind
  writeIntermediateCandidate( hidfid, keys{jj} );
endfor

fclose ( missfid );
fclose ( hidfid );

printf ("done.\n");

%% -------------------- error-bars --------------------
printf ("Estimating errors ... ");

## taken from UnitsConstants.m
## AU_SI	= 1.4959787066e11;	%% Astronomical unit, m 
## C_SI 	= 299792458; 		%% Speed of light in vacuo, m s^-1 
## YRSID_SI	= 31558149.8;		%% Sidereal year (1994), s 
## Vorb       = 2 * pi * AU_SI / YRSID_SI 
## --> Vorb =  2.9785e+04;
## twoPiVorb_C= (2 * pi * Vorb / C_SI); %% 2pi Vorb/c
twoPiVorb_C = 0.00062424;

gIJ = inverse ( gij );

sigmafname = strcat ( "." , "/SIGMA_", resBname );
if ( ( sigmafid = fopen ( sigmafname, "wb" )) == -1 )
  error ("Failed to open sigma file '%s' for writing!\n\n", sigmafname );
endif

for ii = 1:numRes
  %% error bars on Doppler params
  GammaIJ = gIJ / res{ii}.twoF; % fisher matrix (not subtracting 4)
  cosAlpha = cos(res{ii}.Alpha);
  sinAlpha = sin(res{ii}.Alpha);
  cosDelta = cos(res{ii}.Delta);
  sinDelta = sin(res{ii}.Delta);
  Freq = res{ii}.Freq;

  sigma.Freq = sqrt(GammaIJ(1,1)) / (2*pi);

  dAlpha_dK = [  sinAlpha / (twoPiVorb_C*Freq*cosDelta) ; 
	       - cosAlpha / (twoPiVorb_C*Freq*cosDelta) ];
  sigma.Alpha = sqrt ( transpose(dAlpha_dK) * GammaIJ(2:3,2:3) * dAlpha_dK );

#   dDelta_dLambda = [ cosDelta / (2*pi*Freq*sinDelta) ;\ 
# 		     cosAlpha / (twoPiVorb_C*Freq*sinDelta) ;\
# 		     sinAlpha / (twoPiVorb_C*Freq*sinDelta) ];
#   sigma.Delta = sqrt ( transpose(dDelta_dLambda) * GammaIJ * dDelta_dLambda );
  dCosDelta_dLambda = [ - cosDelta / (2*pi*Freq) ; 
 		        - cosAlpha / (twoPiVorb_C*Freq) ;
 		        - sinAlpha / (twoPiVorb_C*Freq) ];

  sigmaCosDelta = sqrt ( transpose(dCosDelta_dLambda) * GammaIJ * dCosDelta_dLambda ); 

  sigma.Delta = acos(cosDelta-sigmaCosDelta) - abs(res{ii}.Delta);

  %% error bars on Amplitude params
  Mmunu = getMmunu4Cand ( res{ii} );
  MMUNU = inverse(Mmunu);
  sigma.A1 = sqrt(MMUNU(1,1));
  sigma.A2 = sqrt(MMUNU(2,2));
  sigma.A3 = sqrt(MMUNU(3,3));
  sigma.A4 = sqrt(MMUNU(4,4));

  %% Placeholders so all rows have same # of cols
  sigma.Sinv_Tsft = res{ii}.Sinv_Tsft ;
  sigma.Ad = res{ii}.Ad;
  sigma.Bd = res{ii}.Bd;
  sigma.Cd = res{ii}.Cd;

  writeIntermediateCandidate( sigmafid, res{ii});
  writeIntermediateCandidate( sigmafid, sigma);
  fprintf( sigmafid, "\n" );
endfor %% ii < numRes

fclose ( sigmafid );

printf ("done.\n\n");

