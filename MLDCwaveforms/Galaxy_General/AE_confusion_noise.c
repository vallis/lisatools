/****************************  NEEDED LIBRARIES  ****************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Constants.h"
#include "Detector.h"
#include <stdio.h>
#include <stddef.h>
#include "arrays.h"

double AE_confusion_noise(double f, double *carray, double *farray, int Nfit)
{
  //Power spectral density of the detector noise and transfer frequency
  double Sn;
  int j;
  double df;
  FILE *outfile;
  
    df = farray[1]-farray[0];
    Sn = -45.0;

      if((f > farray[0]) && (f < farray[Nfit-1]))
	{
	  j = (int)((f-farray[0])/df);
          Sn = carray[j] + (carray[j+1]-carray[j])/(log10(farray[j+1])-log10(farray[j]))*(log10(f)-log10(farray[j]));
	}

      Sn = pow(10.0, Sn);
	
  return Sn;
}
