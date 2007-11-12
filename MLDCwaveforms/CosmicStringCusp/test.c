/* compile with gcc -O2 -o test test.c Cusp.c -lm lfftw3*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "Declarations.h"
#include "Constants.h"

int main(int argc,char *argv)
{
  int i;
  Cusp_structure Cusp;
  double *hp, *hc;
  double dt;
  int n;
  FILE *Out;

  Cusp.Amplitude = 1.0e-10;
  Cusp.CentralTime = 1.0e6;
  Cusp.Polarization = 2.31;
  Cusp.EclipticLatitude = 0.7598;
  Cusp.EclipticLongitude = 4.7623;
  Cusp.MaximumFrequency = 0.1;
  Cusp.Samples =  2097152;
  Cusp.Cadence = 1.0;
  Cusp.Tpad = 900.0;

  n = 2*(int)(Cusp.Tpad*Cusp.Cadence) + Cusp.Samples;

  hp = dvector(0, n-1);
  hc = dvector(0, n-1);

  Cusp_Barycenter(Cusp, hp, hc, n);

  Out = fopen("Cusp.dat","w");

  for(i = 0; i < n; i++)
    { 
      fprintf(Out,"%.12e %.12e %.12e\n", (-Cusp.Tpad+(double)(i)/Cusp.Cadence), hp[i], hc[i]);
    }

  fclose(Out);

  free_dvector(hp, 0, n-1);
  free_dvector(hc, 0, n-1);

  return 0;

}



