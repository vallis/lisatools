/*
 gcc bright_cut.c dfour1.c nrutil2.c AEnoise.c template.c -lm -o bright_cut
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

#include "nrutil2.h"

void template(double *params, long NP, double *hA, double *hE, long NMAX, long *hM, long *hp, double Sn);

void KILL(char*);

double AEnoise(double f);
double nwip(double *a, double *b, long N, double Sn);

/* ============================  MAIN PROGRAM  ============================ */

int main(int argc, char* argv[])
{
  /*   Variable declaration   */
  long i, j, k;
  long NA, N3, N4, N5;
  long count;
  int Bmin, Bi, binshift;
  
  long NP; 
  long NMAX, SMAX;
  long M, q;

  double *params;
  double *A, *E;
  double **hA, **hE;
  double f, Amp;
  double Sn, Sm, St;
  double SNR;
  double Acut;
  
  double powerA1, powerA2, powerA3;
  double powerE1, powerE2, powerE3;
  double x;
  
  
  FILE *source, *sfile1, *sfile2, *sfile3;
  FILE *tfile1, *tfile2, *tfile3;
  FILE *pfile1, *pfile2, *pfile3;
  FILE *test;
  if(argc!=3) KILL("./bright galaxy.dat brights.dat\n");
  
  /*  fix constants  */
  NMAX = 16384;
  NP = 7;
  
  /*  open source file  */
  source = fopen(argv[1], "r");

  /*  allocate memory  */
  params  = dvector(1,NP);
  A       = dvector(1,2*NMAX);
  E       = dvector(1,2*NMAX);
 
  /*  open output files  */
  sfile1 = fopen(argv[2], "w");
  
  /*  initialize summed quantities  */
  NA = N3 = count = 0;

  while(!feof(source))
  {
    /*  count sources scanned  */
    count++;
    
    /*  read in source parameters   */
    for(i=1; i<=NP; i++) 
    {
      fscanf(source, "%lg ", &x);
      params[i] = x;
    }
    /*  change to co-latitude  */
    params[2] = 0.5*pi - params[2];

    f  = params[1];
    Sn = AEnoise(f);
    
    /*  generate source template   */
    template(params, NP, A, E, NMAX, &M, &q, Sn);

    /*  calculate michelson noise  */
    Sm = Sn/(4.0*sin(f/fstar)*sin(f/fstar));
   
    /*  calculate amplitude cut  */
    Amp  = params[4];
    Acut = Amp*sqrt(T/Sm);

    /************************  Amplitude Cut  *********************************/ 
    if((Acut >= 1.0) || (f > 0.001))
    {
      /*  tally sources that get past Amplitude or low f cut  */
      NA++;
      
      /**************************  SNR Cut  ***********************************/
      SNR = sqrt(nwip(A, A, M, Sn) + nwip(E, E, M, Sn));

      if((SNR >= 3.0) || (f > 0.005))
      {
        /*  tally bright sources  */
        N3++;
        
        params[2] = 0.5*pi - params[2];
        /*  store sources that make the cut  */
        for(i=1; i<=NP; i++) fprintf(sfile1, "%.12g ",params[i]);
        fprintf(sfile1, "\n");
                
      }

    }

    
    /*  update terminal on progress  */
    if(count%100000 == 0) printf("%d %d %d\n",count, NA, N3);
  }//end file 'while'
  

  fclose(source);
  fclose(sfile1);
    
  /*  deallocate memory  */
  free_dvector(params,1,NP);
  free_dvector(A,1,2*NMAX);
  free_dvector(E,1,2*NMAX);
  return;
}

void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);
 
  return;
}


double nwip(double *a, double *b, long N, double Sn)
{

  int i;
  double ReA, ImA, ReB, ImB;
  double product;
  
  product = 0.0;
 
  for(i=1; i<=N; i++)
    {
      ReA = a[2*i-1];  
      ImA = a[2*i]; 
      ReB = b[2*i-1];  
      ImB = b[2*i];    
      product += ReA*ReB + ImA*ImB;
    }
 
  return 4.0*product/Sn;
  
}
