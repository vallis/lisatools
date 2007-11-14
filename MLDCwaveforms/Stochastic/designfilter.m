function Hd = designfilter(N)

% the upper limit of the frequency range is in effect
% a cut off for the filter

% design the filter to be 1/f^3 between 1e-5 and 1Hz

F = 10.^(-5:0.1:0);
F(1) = 0;

% normalize would need to normalize to 1, this should be fine already

A = sqrt(1 ./ F.^3);
A(1) = A(2);

A = A / max(A);

loglog(F,A);


d = fdesign.arbmag('N,F,A',N,F,A);
Hd = design(d,'IIR','freqsamp');
fvtool(Hd,'MagnitudeDisplay','Zero-phase','DesignMask','on')
set(gcf,'Color','white');
