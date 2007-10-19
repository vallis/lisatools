/****************************  NEEDED LIBRARIES  ****************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Constants.h"
#include <stdio.h>
#include <stddef.h>
#include "nrutil2.h"

double AEnoise(double f)
{
  //Power spectral density of the detector noise and transfer frequency
  double Sn, red, confusion_noise;
  double f1, f2;
  double A1, A2, slope;
  FILE *outfile;
  
  red = (2.0*pi*1.0e-4)*(2.0*pi*1.0e-4);

  confusion_noise = 0.0;

  // Calculate the power spectral density of the galactic background at the given frequency
  if((f > 1.0e-4) && (f <= 2.5e-4))
    {
      f1 = 1.0e-4;
      f2 =  2.5e-4;
      A1 = 1.65e-39;
      A2 = 2.0e-39;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 2.5e-4) && (f <= 4.5e-4))
    {
      f1 = 2.5e-4;
      f2 =  4.5e-4;
      A1 = 2.0e-39;
      A2 = 1.5e-39;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 4.5e-4) && (f <= 1.0e-3))
    {
      f1 = 4.5e-4;
      f2 =  1.0e-3;
      A1 = 1.5e-39;
      A2 = 7.0e-40;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 1.0e-3) && (f <= 1.5e-3))
    {
      f1 = 1.0e-3;
      f2 =  1.5e-3;
      A1 = 7.0e-40;
      A2 = 3.5e-40;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 1.5e-3) && (f <= 2.0e-3))
    {
      f1 = 1.5e-3;
      f2 =  2.0e-3;
      A1 = 3.5e-40;
      A2 = 1.5e-40;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 2.0e-3) && (f <= 2.2e-3))
    {
      f1 = 2.0e-3;
      f2 =  2.2e-3;
      A1 = 1.5e-40;
      A2 = 9.2e-41;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 2.2e-3) && (f <= 2.3e-3))
    {
      f1 = 2.2e-3;
      f2 =  2.3e-3;
      A1 = 9.2e-41;
      A2 = 4.1e-41;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 2.3e-3) && (f <= 2.5e-3))
    {
      f1 = 2.3e-3;
      f2 =  2.5e-3;
      A1 = 4.1e-41;
      A2 = 9.1e-42;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }
  if((f > 2.5e-3))
    {
      f1 = 2.5e-3;
      f2 =  3.0e-3;
      A1 = 9.1e-42;
      A2 = 5.5e-43;
      slope = (log10(A2)-log10(A1))/(log10(f2)-log10(f1));
     confusion_noise = A1*pow((f/f1),slope);
    }

  // Calculate the power spectral density of the detector noise at the given frequency

  Sn = 16.0/3.0*pow(sin(f/fstar),2.0)*( ( (2.0+cos(f/fstar))*Sps + 2.0*(3.0+2.0*cos(f/fstar)+cos(2.0*f/fstar))*Sacc*(1.0/pow(2.0*pi*f,4)+ red/pow(2.0*pi*f,6))) / pow(2.0*L,2.0)) + confusion_noise;

  // Sn = confusion_noise;
	
  return Sn;
}
