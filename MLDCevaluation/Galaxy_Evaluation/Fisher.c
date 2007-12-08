#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "nrutil2.h"
#include "Constants.h"


void LISA_PREP(double *params, long *output, double Sn);
void FAST_LISA(double *params, long Np, long N, long M, double *A, double *E);
double AEnoise(double f);
double nwip(double *a, double *b, long N, double Sn);

void Fisher(double *params, long NP, long NMAX, double **gamma, double **metric)
{
   int n,i,j, k;
   
   long N, M, *output;
   
   double eps, f;
   double Sn;
   double *x;
   double *Ap, *Ep;
   double *Am, *Em;
   double **dAdx, **dEdx;
   double *gvec;
   double *A, *E;
   double mag;
   
   
   /*  Allocate memory  */
   output = lvector(1,3);
   
   x = dvector(1,NP);

   f = params[1];
   Sn= AEnoise(f);

   //determines array sizes for FAST_LISA.c and central bin
   LISA_PREP(params, output, Sn);
   N  = output[1];   M  = output[2];  
   //printf("%i %i %i %i\n",N,M,NP,NMAX);
   //cap NS and MS to operate within range of FAST_LISA.c
   if(M<N) M = N;
   if(M>NMAX) M = NMAX;

   A = dvector(1,2*M);  
   E = dvector(1,2*M);
   Ap = dvector(1,2*M);  
   Ep = dvector(1,2*M);
   Am = dvector(1,2*M);  
   Em = dvector(1,2*M);
   
   gvec = dvector(1,NP);
   dAdx = dmatrix(1,NP,1,2*M);
   dEdx = dmatrix(1,NP,1,2*M);   

   FAST_LISA(params, NP, N, M, A, E);

   mag = nwip(A, A, M, Sn) + nwip(E, E, M, Sn);
   
   //Infinitesimal change in parameter for differentiation
   eps = 1.0e-6; 

   params[1] *= T;
   params[4] = log(params[4]);

   /*   loop through each parameter and calculate dh/dx[i]   */
   for(i=1; i<=NP; i++)
   {
      for(j=1; j<=NP; j++) x[j] = params[j];


      //  Perturb parameter x_i forward in eps
      x[i] += eps;
      
      x[1] /= T;
      x[4] = exp(x[4]);

      //  Generate A & E channel of forward perturbed waveform
      FAST_LISA(x, NP, N, M, Ap, Ep);

      //  Reset parameters   
      for(j=1; j<=NP; j++) x[j] = params[j];      
      
      //  Perturb parameter x_i backward in eps
      x[i] -= eps;
      
      x[1] /= T;
      x[4] = exp(x[4]);

      //  Generate A & E channel of backward perturbed waveform
      FAST_LISA(x, NP, N, M, Am, Em);
      
      //  Calculate h,i via numerical central differencing
      for(n=1; n<=M; n++)
      {
         dAdx[i][2*n-1] = 0.5*(Ap[2*n-1] - Am[2*n-1])/eps;
         dAdx[i][2*n]   = 0.5*(Ap[2*n]   - Am[2*n])/eps;
	 dEdx[i][2*n-1] = 0.5*(Ep[2*n-1] - Em[2*n-1])/eps;
         dEdx[i][2*n]   = 0.5*(Ep[2*n]   - Em[2*n])/eps;
      }
   }
   
   
   /*  Calculate ij element of Fisher via (h,i|h,j)  */
   for(i=1; i<=NP; i++)
   {
      for(k=1; k<=(2*M); k++)
       {
	 Ap[k] = dAdx[i][k];
	 Ep[k] = dEdx[i][k];
       }

      gvec[i] = nwip(A, Ap, M, Sn) + nwip(E, Ep, M, Sn);

      for(j=i; j<=NP; j++)
      {

      for(k=1; k<=(2*M); k++)
       {
	 Am[k] = dAdx[j][k];
	 Em[k] = dEdx[j][k];
       }

         gamma[i][j] = nwip(Am, Ap, M, Sn) + nwip(Em, Ep, M, Sn);
      }
   }

   // printf("\n test %e \n\n", (gamma[1][4]-gvec[1]*gvec[4]/mag)/mag);

   for(i=1; i<=NP; i++)
   {
      for(j=1; j<i; j++)
      {
	gamma[i][j] = gamma[j][i];
      }
   }

   for(i=1; i<=NP; i++)
   {
      for(j=1; j<=NP; j++)
      {
	metric[i][j] = (gamma[i][j] - gvec[i]*gvec[j]/mag)/mag;
      }
   }
 
   params[1] /= T;
   params[4] = exp(params[4]);

  
   /*  Free allocated memory  */
   free_lvector(output,1,3);
   free_dvector(x,1,NP);   
   free_dvector(gvec,1,NP);
   free_dvector(A,1,2*M);  
   free_dvector(E,1,2*M);
   free_dvector(Ap,1,2*M);  
   free_dvector(Ep,1,2*M);
   free_dvector(Am,1,2*M);  
   free_dvector(Em,1,2*M);   
   free_dmatrix(dAdx,1,NP,1,2*M);
   free_dmatrix(dEdx,1,NP,1,2*M); 
 
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
