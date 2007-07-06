/*
 gcc MLDC2_sortkey.c nrutil2.c -lm -o MLDC2_sortkey
 */
/***************************  REQUIRED LIBRARIES  ***************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "nrutil2.h"
#include "sort.h"

/*********************  DEFINITION OF GLOBAL VARIABLES  *********************/

/*************  PROTOTYPE DECLARATIONS FOR INTERNAL FUNCTIONS  **************/

void KILL(char*);


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
  
  double f, theta, phi, Amp, psi, iota, phase, SNR;
  double d_f, d_theta, d_phi, d_Amp, d_psi, d_iota, d_phase;
  double Sn;
  double *freq;
  double **iparams;
  double *rA, *rE, *iA, *iE, *dA, *dE;
  double **nwr, **corr, **fmatch;
  long *indx;
  double min_nwr;
  double PI;
  char Gfile[100];
    
  FILE *ifile, *bfile1, *bfile2, *bfile3, *bfile4;

  NP = 7;
  PI = 3.141592653589793;

  if(argc!=2) KILL("to run:  ./MLDC2_windows keyfile\n");

  /*  open key file  */
  ifile = fopen(argv[1], "r");

  /*  determine number of sources injected  */ 
  NI=0;
  while(!feof(ifile)) 
  {
    NI++;
    fscanf(ifile,"%lf %lf %lf %lf %lf %lf %lf",&f, &theta, &phi, &Amp, &iota, &psi, &phase);
  }
  rewind(ifile);
  NI--;    
  printf("number of sources injected:  %i\n",NI);  


  iparams = dmatrix(1,NI,0,NP);
  freq = dvector(1,NI);
  indx = lvector(1,NI);

  /*  store injected sources in array  */
  printf("storing injected source parameters\n");
  for(i=1; i<=NI; i++)
  {
    fscanf(ifile,"%lf %lf %lf %lf %lf %lf %lf",&f, &theta, &phi, &Amp, &iota, &psi, &phase);
    iparams[i][1] = f;
    iparams[i][2] = theta;
    iparams[i][3] = phi;
    iparams[i][4] = Amp;
    iparams[i][5] = iota;
    iparams[i][6] = psi;
    iparams[i][7] = phase;
    freq[i] = f;
  }

  fclose(ifile);

  indexx(NI,freq,indx);

  bfile1 = fopen(argv[1], "w");

  for(i=1; i<=NI; i++)
  {
    f = iparams[indx[i]][1];
    theta = iparams[indx[i]][2];
    phi = iparams[indx[i]][3];
    Amp = iparams[indx[i]][4];
    iota = iparams[indx[i]][5];
    psi = iparams[indx[i]][6];
    phase = iparams[indx[i]][7];
    fprintf(bfile1,"%.12lg %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg\n", f, theta, phi, Amp, iota, psi, phase);
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

