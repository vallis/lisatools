/*
 gcc MLDC3_sort.c nrutil2.c -lm -o MLDC3_sort
 */
/***************************  REQUIRED LIBRARIES  ***************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "nrutil2.h"
#include "sort.h"

#include "Constants.h"

/*********************  DEFINITION OF GLOBAL VARIABLES  *********************/

/*************  PROTOTYPE DECLARATIONS FOR INTERNAL FUNCTIONS  **************/

void KILL(char*);
double AEnoise(double f);

/* ============================  MAIN PROGRAM  ============================ */

int main(int argc, char* argv[])
{
  /*   Variable declaration   */
  int i, j, k, m, n, J;
  int NP;
  int *count, **bin_pop;
  int source, source_match;
  int i_match, j_match, k_match;
  int cc5, cc6, cc7, cc8, cc9;
  int binshift;
  
  long NI, NR;
  long q, ip, rp, M, Bi;
  
  double f, fdot, theta, phi, Amp, psi, iota, phase, SNR;
  double d_f, d_theta, d_phi, d_Amp, d_psi, d_iota, d_phase;
  double Sn, Sm;
  double x;
  double *freq;
  double **iparams;
  double *rA, *rE, *iA, *iE, *dA, *dE;
  double **nwr, **corr, **fmatch;
  long *indx;
  long *name;
  double min_nwr;
  char Gfile[100];
    
  FILE *ifile, *bfile1, *bfile2, *bfile3, *bfile4;

  NP = 8;
    
  if(argc!=2) KILL("to run:  ./MLDC3_sort file\n");

  /*  open key file  */
  ifile = fopen(argv[1], "r");

  /*  determine number of sources injected  */ 
  NI=0;
  while(!feof(ifile)) 
  {
    NI++;
    fscanf(ifile,"%lf %lf %lf %lf %lf %lf %lf %lf",&f, &fdot, &theta, &phi, &Amp, &iota, &psi, &phase);
  }
  rewind(ifile);
  NI--;    
  printf("number of sources:  %i\n",NI);  


  iparams = dmatrix(1,NI,0,NP);
  freq = dvector(1,NI);
  indx = lvector(1,NI);

  /*  store injected sources in array  */
  printf("storing source parameters\n");
  for(i=1; i<=NI; i++)
  {
   fscanf(ifile,"%lf %lf %lf %lf %lf %lf %lf %lf",&f, &fdot, &theta, &phi, &Amp, &iota, &psi, &phase);
    if(isnan(Amp) != 0)
      {
	Sn = AEnoise(f);
        Sm = Sn/(4.0*sin(f/fstar)*sin(f/fstar));
        Amp  = 10.0*sqrt(Sm/T);
      }
    if(isnan(psi) != 0) psi = 0.0;
    if(isnan(iota) != 0) iota = pi/2.0;
    if(isnan(phase) != 0) phase = 0.0;
    iparams[i][1] = f;
    iparams[i][2] = fdot;
    iparams[i][3] = theta;
    iparams[i][4] = phi;
    iparams[i][5] = Amp;
    iparams[i][6] = iota;
    iparams[i][7] = psi;
    iparams[i][8] = phase;
    freq[i] = f;
  }

  fclose(ifile);

  indexx(NI,freq,indx);

  bfile1 = fopen(argv[1], "w");


  for(i=1; i<=NI; i++)
  {
    f = iparams[indx[i]][1];
    fdot = iparams[indx[i]][2];
    theta = iparams[indx[i]][3];
    phi = iparams[indx[i]][4];
    Amp = iparams[indx[i]][5];
    iota = iparams[indx[i]][6];
    psi = iparams[indx[i]][7];
    phase = iparams[indx[i]][8]; 
    fprintf(bfile1,"%.12lg  %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg\n", f, fdot, theta, phi, Amp, iota, psi, phase);
  }

  fclose(bfile1);

}

/* ==========================  END MAIN PROGRAM  ========================== */


void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);
 
  return;
}

double AEnoise(double f)
{
  //Power spectral density of the detector noise and transfer frequency
  double Sn, red, confusion_noise;
  FILE *outfile;
  
  red = (2.0*pi*1.0e-4)*(2.0*pi*1.0e-4);

  confusion_noise = 0.0;

  // Calculate the power spectral density of the galactic background at the given frequency

  if((f > 1.0e-4) && (f <= 4.0e-4)) confusion_noise = 2.5e-36*pow((f/1.0e-4),-1.83);
  if((f > 4.0e-4) && (f <= 1.15e-3)) confusion_noise = 1.95e-37*pow((f/4.0e-4),-2.93);
  if((f > 1.15e-3) && (f <= 2.0e-3)) confusion_noise = 8.86e-39*pow((f/1.15e-3),-4.11);
  if((f > 2.0e-3) && (f <= 2.3e-3)) confusion_noise = 9.0e-40*pow((f/2.0e-3),-7.09);
  if((f > 2.3e-3) && (f <= 2.5e-3)) confusion_noise = 3.34e-40*pow((f/2.3e-3),-13.15);
  if((f > 2.5e-3)) confusion_noise = 1.12e-40*pow((f/2.5e-3),-23.0);
  // Calculate the power spectral density of the detector noise at the given frequency

  Sn = 16.0/3.0*pow(sin(f/fstar),2.0)*( ( (2.0+cos(f/fstar))*Sps + 2.0*(3.0+2.0*cos(f/fstar)+cos(2.0*f/fstar))*Sacc*(1.0/pow(2.0*pi*f,4)+ red/pow(2.0*pi*f,6))) / pow(2.0*L,2.0) + confusion_noise);
	
  return Sn;
}
