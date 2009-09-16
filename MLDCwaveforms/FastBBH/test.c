/* compile with gcc -O2 -o test test.c Tools.c -lm */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "Declarations.h"
#include "Constants.h"
#include "numrec.h"

int main(int argc,char *argv)
{
  int i;
  SBH_structure SBH;
  double *hp, *hc;
  double dt;
  int n;
  FILE *Out;



  SBH.Mass1 = 2906387.81661;
  SBH.Mass2 = 1056812.14434;
  SBH.CoalescenceTime = 19395657.1969;
  SBH.Distance = 1000000000.0;
  SBH.Spin1 = 0.76622880068;
  SBH.Spin2 = 0.357815817167;
  SBH.EclipticLatitude = 0.759892750778;
  SBH.InitialPolarAngleL = 2.02230588043;
  SBH.PolarAngleOfSpin1 = acos(-0.158856838338);
  SBH.PolarAngleOfSpin2 = acos(-0.482166499414);
  SBH.EclipticLongitude = 4.76236796807;
  SBH.InitialAzimuthalAngleL = 4.74885787067;
  SBH.AzimuthalAngleOfSpin1 = 3.21243381724;
  SBH.AzimuthalAngleOfSpin2 = 2.5442762228;
  SBH.PhaseAtCoalescence = 3.1710410372;
  SBH.TaperApplied = 7.0;
  SBH.AmplPNorder = 0.0;
  SBH.TimeSample = 15.0;
  SBH.Tobs = 62914560.0;
  SBH.Tpad = 900.0;
  SBH.Rmin = 6.0;
  SBH.TaperSteepness = 150.0;
  SBH.TaperQFactor = 3.0;

  n = (int)((SBH.Tobs + 2.0*SBH.Tpad)/SBH.TimeSample);

  hp = dvector(0, n-1);
  hc = dvector(0, n-1);

  SBH_Barycenter(SBH, hp, hc);

  Out = fopen("SBH.dat","w");

  for(i = 0; i < n; i++)
    { 
      fprintf(Out,"%.12e %.12e %.12e\n", (-SBH.Tpad+(double)(i)*SBH.TimeSample), hp[i], hc[i]);
    }

  fclose(Out);

  return 0;

}

