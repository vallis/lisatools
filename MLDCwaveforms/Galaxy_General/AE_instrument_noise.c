/****************************  NEEDED LIBRARIES  ****************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Constants.h"
#include "Detector.h"
#include <stdio.h>
#include <stddef.h>
#include "arrays.h"

double AE_instrument_noise(double f)
{
  //Power spectral density of the detector noise and transfer frequency
  double Sn, red, confusion_noise;
  double f1, f2;
  double A1, A2, slope;
  FILE *outfile;
  
  red = (2.0*pi*1.0e-4)*(2.0*pi*1.0e-4);

  // Calculate the power spectral density of the detector noise at the given frequency

  Sn = 16.0/3.0*pow(sin(f/fstar),2.0)*( ( (2.0+cos(f/fstar))*Sps + 2.0*(3.0+2.0*cos(f/fstar)+cos(2.0*f/fstar))*Sacc*(1.0/pow(2.0*pi*f,4)+ red/pow(2.0*pi*f,6))) / pow(2.0*L,2.0));

	
  return Sn;
}
