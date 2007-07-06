/*
 gcc MLDC2_windows.c -lm -o MLDC2_windows
 */
/***************************  REQUIRED LIBRARIES  ***************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>


/*********************  DEFINITION OF GLOBAL VARIABLES  *********************/

#include "Constants.h"

/*************  PROTOTYPE DECLARATIONS FOR INTERNAL FUNCTIONS  **************/

void KILL(char*);


/* ============================  MAIN PROGRAM  ============================ */

int main(int argc, char* argv[])
{
  /*   Variable declaration   */
  int i, j, k, m, n, J;
  int NP, NMAX;
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
  double *params;
  double **rparams, **iparams;
  double *rA, *rE, *iA, *iE, *dA, *dE;
  double **nwr, **corr, **fmatch;
  double min_nwr;
  char Gfile[100];
    
  FILE *ifile, *bfile1, *bfile2, *bfile3, *bfile4;
    
  if(argc!=2) KILL("to run:  ./MLDC2_windows keyfile\n");

  /*  open key file  */
  ifile = fopen(argv[1], "r");

  sprintf(Gfile, "%s-1", argv[1]);
  bfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s-2", argv[1]);
  bfile2 = fopen(Gfile,"w");
  sprintf(Gfile, "%s-3", argv[1]);
  bfile3 = fopen(Gfile,"w");
  sprintf(Gfile, "%s-4", argv[1]);
  bfile4 = fopen(Gfile,"w");

  while(!feof(ifile)) 
  {
    fscanf(ifile,"%lg %lg %lg %lg %lg %lg %lg",&f, &theta, &phi, &Amp, &psi, &iota, &phase);

      /* output in focus windows specifically  */
      if(f>=0.2985e-3 && f<=0.3015e-3)
      {  
	fprintf(bfile1,"%.12lg %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg\n", f, theta, phi, Amp, psi, iota, phase);
      }
      if(f>=0.9985e-3 && f<=1.0015e-3)
      {  
	fprintf(bfile2,"%.12lg %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg\n", f, theta, phi, Amp, psi, iota, phase);
      }
      if(f>=2.9985e-3 && f<=3.0015e-3)
      {  
	fprintf(bfile3,"%.12lg %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg\n", f, theta, phi, Amp, psi, iota, phase);
      }
      if(f>=5.9985e-3 && f<=6.0015e-3)
      {  
	fprintf(bfile4,"%.12lg %.10lg %.10lg %.10lg %.10lg %.10lg %.10lg\n", f, theta, phi, Amp, psi, iota, phase);
      }    

  }

  fclose(bfile1);
  fclose(bfile2);
  fclose(bfile3);
  fclose(bfile4);

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

