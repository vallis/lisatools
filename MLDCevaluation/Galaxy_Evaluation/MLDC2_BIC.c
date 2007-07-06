
/***************************  REQUIRED LIBRARIES  ***************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "IO/readxml.h"
#include "IO/xmlbinary.h"

/*********************  DEFINITION OF GLOBAL VARIABLES  *********************/

#include "Constants.h"

#define WINDOW 128

/*************  PROTOTYPE DECLARATIONS FOR INTERNAL FUNCTIONS  **************/


#include "nrutil2.h"

void template(double *params, long NP, double *hA, double *hE, long NMAX, long *hM, long *hp, double Sn);

void KILL(char*);

#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
void dfour1(double data[], unsigned long nn, int isign);
void drealft(double data[], unsigned long n, int isign);

double nwip(double *a, double *b, long N, double Sn);
double BIC(double *A, double *E, double *Sfit, int N, double Sn, long D, int Neff);
double AEnoise(double f);
void Data_Dimension(double *A, double *E, long M, int NP, double SA, double SE, int *Neff);

/* ============================  MAIN PROGRAM  ============================ */

int main(int argc, char* argv[])
{
  /*   Variable declaration   */
  int i, j, k, m, n, ii;
  int iteration;
  int imin, imax;
  int wmin, wmax;
  int source;
  int count, COUNT;
  int trigger,check;
  int D, N;
  int Nkeep, Nreject, Nbright;
  int Nold, out;
  int binshift;
  int *keep;
  int Neff;

  long NP, NW; 
  long NMAX, SMAX;
  long M, q;
  long Bmin, tBmin, sBmin;
  long Bi;

  double *params;
  double *A, *E;
  double *sA, *sE;
  double *hA, *hE;
  double *dA, *dE;
  double *wA, *wE;
  double *wAtemp, *wEtemp;
  double **bright;
  double *resolved, *rejected;
  double dBIC, dBICmax, minBIC, tempBIC;
  double *fBIC, *finalBIC;
  double *Sfit, **Sfitarray;
  double f, logf, Tobs, fix;
  double theta, phi, Amp, psi, iota, phase;
  double reA, imA, reE, imE;
  double Sn, SNR;
  double SA, SE;
  double spower, dpower, tpower, Apower, Epower;
  double sEpower, dEpower, tEpower;

  FILE *dfile, *sfile, *tfile, *outfile1, *outfile2;
  FILE *bfile1, *bfile2, *bfile3, *bfile4;
  FILE *rfile1, *rfile2, *rfile3, *rfile4;

  char ErrorMessage[100];
  char Gfile[100];

  TimeSeries *timeseries;

  if(argc!=4) KILL("to run:  ./MLDC2_BIC challenge.xml entry.dat name\n");

  /*  open data file  */
    timeseries = getTDIdata(argv[1]);

  if(timeseries->Length != NFFT)
    {
     sprintf(ErrorMessage,"Data file length %d not compatible with current simulator setting %d\n", timeseries->Length, NFFT);
     KILL(ErrorMessage);
    }

  if(fabs(timeseries->Cadence - dt) > 1.0e-8)
    {
     sprintf(ErrorMessage,"Input cadence %g not compatible with current simulator setting %g\n", timeseries->Cadence, dt);
     KILL(ErrorMessage);
    }

        Tobs = (double)(NFFT)*dt;

        /* used to convert Numerical Recipes FFT output into a strain spectral density */

    fix = sqrt(Tobs)/((double)NFFT);

  sA   = dvector(1,NFFT);
  sE   = dvector(1,NFFT);

    for(i=0; i<NFFT; i++)
      {
	sA[i+1] = (2.0*timeseries->Data[1]->data[i]- timeseries->Data[2]->data[i] - timeseries->Data[3]->data[i])/3.0;
        sE[i+1] = (timeseries->Data[3]->data[i] - timeseries->Data[2]->data[i])/sqrt(3.0);
      }

    freeTimeSeries(timeseries);

   drealft(sA, NFFT, 1);
   drealft(sE, NFFT, 1);

  for(i = 1; i <= NFFT; i++)
    {
     sA[i] *= fix;
     sE[i] *= fix;
     //printf("%e %e\n", sA[i], sE[i]);
    }


  /*  open source parameter file   */
  sfile = fopen(argv[2], "r");
  
  printf("read in data files\n");
  
  printf("length of LISA data:  %i\n",NFFT);  

  /*  determine number of sources identified  */
  SMAX=0;
  while(!feof(sfile)) 
  {
    SMAX++;
    fscanf(sfile,"%ld %lf %lf %lf %lf %lf %lf %lf",&j, &f, &theta, &phi, &Amp, &psi, &iota, &phase);
  }
  rewind(sfile);
  SMAX--;    
  printf("number of sources identified:  %i\n",SMAX);  

  NP = 7;  
  NW = (int)floor(NFFT/(2*WINDOW));
  
  printf("Total number of windows:  %i\n",NW);  
   
  /*  allocate memory  */
  bright  = dmatrix(1,SMAX,0,NP);
  keep    = ivector(1,SMAX);
  params  = dvector(1,NP);
  A	  = dvector(1,16384);
  E	  = dvector(1,16384);
  hA      = dvector(1,NFFT);
  hE      = dvector(1,NFFT);
  dA      = dvector(1,NFFT);
  dE      = dvector(1,NFFT);
  wA      = dvector(1,2*WINDOW);
  wE      = dvector(1,2*WINDOW);
  wAtemp  = dvector(1,2*WINDOW);
  wEtemp  = dvector(1,2*WINDOW);
  Sfit =  dvector(0,2);
  Sfitarray = dmatrix(1,NW,0,2);
  fBIC    = dvector(1,NW);
  finalBIC    = dvector(1,NW);

  NMAX = NFFT/2;

  /* store sources in array  */
  printf("store source parameters\n");
  for(i=1; i<=SMAX; i++)
  {
    fscanf(sfile,"%ld %lf %lf %lf %lf %lf %lf %lf",&j, &f, &theta, &phi, &Amp, &psi, &iota, &phase);
    bright[i][0] = 1.0;
    bright[i][1] = f;
    bright[i][2] = 0.5*pi-theta;
    bright[i][3] = phi;
    bright[i][4] = Amp;
    bright[i][5] = iota;
    bright[i][6] = psi;
    bright[i][7] = phase;
  }
  rewind(sfile);

  printf("generate recovered source super-template:  \n");

   for(j=1; j<=NFFT; j++) hA[j] = hE[j] = 0.0;

    for(ii=1; ii<=SMAX; ii++)
     {
      for(i=1; i<=NP; i++) params[i] = bright[ii][i];

      f = params[1];
      Sn = AEnoise(f);

      template(params, NP, A, E, 16384, &M, &q, Sn);
      
      Bi = q - M/2;
        
      for(i=1; i<=M; i++)
      {
        j = i + Bi; 
        if(j>=1 && j<=(NMAX))
        {
          hA[2*j-1] += A[2*i-1];
          hA[2*j]   += A[2*i];
          hE[2*j-1] += E[2*i-1];
          hE[2*j]   += E[2*i];
        }
      } 

     }

    tBmin = 1;
    sBmin = 1;
  
  binshift = 0;
  
  /*  form up residual  */
  printf("forming reference residual\n");
  
  for(i=1; i<=NMAX; i++)
  {
      dA[2*i-1] = sA[2*i-1] - hA[2*i-1];
      dA[2*i]   = sA[2*i]   - hA[2*i];
      dE[2*i-1] = sE[2*i-1] - hE[2*i-1];
      dE[2*i]   = sE[2*i]   - hE[2*i];
  }
 
  printf("printing initial residual file\n");
  sprintf(Gfile, "%s_initial_residual.dat", argv[3]);
  outfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_initial_residual_power.dat", argv[3]);
  outfile2 = fopen(Gfile,"w");
  for(i=1; i<=NMAX; i++)
  {
    logf = log10( (double)i/T );
   
     tpower = 0.5*log10( 2.0*( hA[2*i-1]*hA[2*i-1] + hA[2*i]*hA[2*i] ) );
     spower = 0.5*log10( 2.0*( sA[2*i-1]*sA[2*i-1] + sA[2*i]*sA[2*i] ) );
     dpower = 0.5*log10( 2.0*( dA[2*i-1]*dA[2*i-1] + dA[2*i]*dA[2*i] ) );
     tEpower = 0.5*log10( 2.0*( hA[2*i-1]*hA[2*i-1] + hA[2*i]*hA[2*i] ) );
     sEpower = 0.5*log10( 2.0*( sA[2*i-1]*sA[2*i-1] + sA[2*i]*sA[2*i] ) );
     dEpower = 0.5*log10( 2.0*( dA[2*i-1]*dA[2*i-1] + dA[2*i]*dA[2*i] ) );
     fprintf(outfile1, "%.12g %.12g %.12g %.12g %.12g\n", (double)i/T, dA[2*i-1], dA[2*i], dE[2*i-1], dE[2*i]);
     fprintf(outfile2, "%.12g %.12g %.12g %.12g %.12g %.12g %.12g\n", logf, dpower, dEpower, spower, sEpower, tpower, tEpower);

  }
  fclose(outfile1);
  fclose(outfile2);

     
  /*  windowing data  */
  printf("***   Occam Loop   ***\n");   
  Nbright = 0;
  Nold = Nkeep = SMAX;
  iteration = 0;
  m=0;
  while(m==0)
  {
  iteration ++;
  printf("iteration %i\n",iteration);
  
  source  = 0;
  for(n=1; n<=NW; n++)
  {
    
    if((n/1000)*1000 == n) printf("window:  %i\n",n);
     
    imin = (n-1)*WINDOW + 1;
    imax = imin - 1 + WINDOW;
  
    wmin  = imin - 1 + sBmin;
    wmax  = wmin - 1 + WINDOW;

    // calculate noise of central frequency bin  
    f  = (double)((n-1)*WINDOW + WINDOW/2)/T;
    Sn = AEnoise(f);



    /*  initialize arrays  */  
    j = 0;
    for(i=imin; i<=imax; i++)
    {
      j++;
      wAtemp[2*j-1] = wA[2*j-1] = dA[2*i-1];
      wAtemp[2*j]   = wA[2*j]   = dA[2*i];
      wEtemp[2*j-1] = wE[2*j-1] = dE[2*i-1];
      wEtemp[2*j]   = wE[2*j]   = dE[2*i];
    }
  
    // scan through brights file for sources within the window
    
    count = 0;
    for(i=1; i<=SMAX; i++)
      {
      q = (int)floor(bright[i][1]*T);

    if( q >= wmin && q <= wmax )
      {
       if(bright[i][0] > 0.0) 
	 {
          count++;  
          keep[count] = i;
         }
      }
      }

    COUNT = count;
    
    if(iteration==1)
     {
      Nbright += COUNT;
     }
       
    /*  calculate reference BIC for window. The number of
        parameters is just the 3 noise fit parameters. */
    minBIC  = BIC(wA, wE, Sfit, WINDOW, Sn, 0, 1);       
    fBIC[n] = minBIC;
    finalBIC[n] = minBIC;
    for(i=0; i<3; i++) Sfitarray[n][i] = Sfit[i];
      
    /*  start throwing fish back into the pond  */
    dBICmax = -1.0e22;  
    for(count=1; count<=COUNT; count++)
    {
      /*  add source back to residual  */
      for(i=1; i<=NP; i++) params[i] = bright[keep[count]][i];
      
      template(params, NP, A, E, 16384, &M, &q, Sn);
      
      Bi = q - M/2;
      binshift = Bi - wmin;
        
      for(i=1; i<=M; i++)
      {
        j = i + binshift; 
        if(j>=1 && j<=WINDOW)
        {
          wAtemp[2*j-1] += A[2*i-1];
          wAtemp[2*j]   += A[2*i];
          wEtemp[2*j-1] += E[2*i-1];
          wEtemp[2*j]   += E[2*i];
        }
      }

      /* use current noise estimate to compute Neff */
      SA = Sn*Sfitarray[n][0];
      SE = Sn*Sfitarray[n][1];

      Data_Dimension(A, E, M, NP, SA, SE, &Neff);

      // printf("%d %d\n\n", M, Neff);
        
      /*  calculate tempBIC  */
      tempBIC = BIC(wAtemp, wEtemp, Sfit, WINDOW, Sn, NP, Neff);
      dBIC = minBIC - tempBIC;

      if(dBIC > dBICmax)
      {
        dBICmax = dBIC;
        out = count;
      }
        
      /* reset residual  */
      for(i=1; i<=2*WINDOW; i++)
      {
        wAtemp[i] = wA[i];
        wEtemp[i] = wE[i];
      }
    }//end count loop
      
    /*  figure out which fish to leave in the pond  */
    if(dBICmax > -2.0)
    {
      /*  add 'worst' source back to residual  */
      for(i=1; i<=NP; i++) params[i] = bright[keep[out]][i];
            
      template(params, NP, A, E, 16384, &M, &q, Sn);
    
      Bi = q - M/2;
      binshift = Bi - sBmin;

      for(i=1; i<=M; i++)
      {
        j = i + binshift; 
        if(j>0 && j <= NMAX)
        {
          dA[2*j-1] += A[2*i-1];
          dA[2*j]   += A[2*i];
          dE[2*j-1] += E[2*i-1];
          dE[2*j]   += E[2*i];
        }
      }
      
      bright[keep[out]][0] = -1.0;
              
      /*  reset reference residual  */
      j = 0;
      for(i=imin; i<=imax; i++)
      {
        j++;
        wA[2*j-1] = dA[2*i-1];
        wA[2*j]   = dA[2*i];
        wE[2*j-1] = dE[2*i-1];
        wE[2*j]   = dE[2*i];
      } 

      /* use current noise estimate to compute Neff */
      SA = Sn*Sfitarray[n][0];
      SE = Sn*Sfitarray[n][1];

      Data_Dimension(A, E, M, NP, SA, SE, &Neff);

      // printf("%d %d\n", Neff, M);
  
     finalBIC[n] = BIC(wA, wE, Sfit, WINDOW, Sn, NP, Neff);  
     for(i=0; i<3; i++) Sfitarray[n][i] = Sfit[i];

      Nkeep--;
    }//end BIC if
  
  }//end window loop
  //printf("%d\n", Nbright);
  printf("number of sources left: %i,   number rejected: %i\n",Nkeep, Nold-Nkeep);
  if(Nold - Nkeep < 3) m = 1;
  Nold = Nkeep;
  }//end m loop
  
  printf("total number of iterations:           %li\n", iteration);
  printf("total number of recovered sources:    %li\n", Nbright);
  printf("total number of resolvable sources:   %li\n", Nkeep);

  /*  print parameters to file  */
  printf("Saving BIC and Noise fit to file\n");
  sprintf(Gfile, "%s_BIC.dat", argv[3]);
  outfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_Sn.dat", argv[3]);
  outfile2 = fopen(Gfile,"w");

  for(n=1; n<=NW; n++)
  {    
    i = (n-1)*WINDOW + 1 + WINDOW/2;
    f = ((double)i)/T;
    Sn = AEnoise(f);
    fprintf(outfile1,"%.12g %.12g\n", f, finalBIC[n]);
    fprintf(outfile2,"%.12g %.12g %.12g %.12g %.12g\n", f, Sn, Sn*Sfitarray[n][0], Sn*Sfitarray[n][1], Sn*Sfitarray[n][2]);
  }

  fclose(outfile1);
  fclose(outfile2);
  
  /*  print parameters to file  */
  printf("printing kept and rejected sources to file\n");
  sprintf(Gfile, "%s_keepers.dat", argv[3]);
  outfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_rejects.dat", argv[3]);
  outfile2 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_keepers_W1.dat", argv[3]);
  bfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_keepers_W2.dat", argv[3]);
  bfile2 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_keepers_W3.dat", argv[3]);
  bfile3 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_keepers_W4.dat", argv[3]);
  bfile4 = fopen(Gfile,"w");


  sprintf(Gfile, "%s_rejects_W1.dat", argv[3]);
  rfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_rejects_W2.dat", argv[3]);
  rfile2 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_rejects_W3.dat", argv[3]);
  rfile3 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_rejects_W4.dat", argv[3]);
  rfile4 = fopen(Gfile,"w");

  Nreject  = Nbright - Nkeep;
  resolved = dvector(1,Nkeep);
  rejected = dvector(1,Nreject);
  for(n=1; n<=SMAX; n++)
  {
    for(i=1; i<=NP; i++) params[i] = bright[n][i];
    f = params[1];
    Sn= AEnoise(f);
    q = (long)(f*T)/WINDOW+1;
    SA = Sn*Sfitarray[q][0];
    SE = Sn*Sfitarray[q][1];
    template(params, NP, A, E, 16384, &M, &q, Sn);
    SNR = sqrt(nwip(A, A, M, SA) + nwip(E, E, M, SE));
    params[2] = 0.5*pi-params[2];
    if(bright[n][0] > 0.0)
    {
      fprintf(outfile1,"%d  ", n);
      for(i=1; i<=4; i++) fprintf(outfile1,"%.12g ",params[i]);
      fprintf(outfile1,"%.12g ",params[6]);
      fprintf(outfile1,"%.12g ",params[5]);
      fprintf(outfile1,"%.12g\n",params[7]);


      /* output in focus windows specifically  */
      if(f>=0.2985e-3 && f<=0.3015e-3)
      {  
        fprintf(bfile1,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(bfile1,"%.12g ",params[i]);
        fprintf(bfile1,"%.12g ",params[6]);
        fprintf(bfile1,"%.12g ",params[5]);
        fprintf(bfile1,"%.12g\n",params[7]);
      }
      if(f>=0.9985e-3 && f<=1.0015e-3)
      {  
        fprintf(bfile2,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(bfile2,"%.12g ",params[i]);
        fprintf(bfile2,"%.12g ",params[6]);
        fprintf(bfile2,"%.12g ",params[5]);
        fprintf(bfile2,"%.12g\n",params[7]);
      }
      if(f>=2.9985e-3 && f<=3.0015e-3)
      {  
        fprintf(bfile3,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(bfile3,"%.12g ",params[i]);
        fprintf(bfile3,"%.12g ",params[6]);
        fprintf(bfile3,"%.12g ",params[5]);
        fprintf(bfile3,"%.12g\n",params[7]);
      }
      if(f>=5.9985e-3 && f<=6.0015e-3)
      {  
        fprintf(bfile4,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(bfile4,"%.12g ",params[i]);
        fprintf(bfile4,"%.12g ",params[6]);
        fprintf(bfile4,"%.12g ",params[5]);
        fprintf(bfile4,"%.12g\n",params[7]);
      }    
    
    }else
    {
      fprintf(outfile2,"%d  ", n);
      for(i=1; i<=4; i++) fprintf(outfile2,"%.12g ",params[i]);
      fprintf(outfile2,"%.12g ",params[6]);
      fprintf(outfile2,"%.12g ",params[5]);
      fprintf(outfile2,"%.12g\n",params[7]);

      /* output in focus windows specifically  */
      if(f>=0.2985e-3 && f<=0.3015e-3)
      {  
        fprintf(rfile1,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(rfile1,"%.12g ",params[i]);
        fprintf(rfile1,"%.12g ",params[6]);
        fprintf(rfile1,"%.12g ",params[5]);
        fprintf(rfile1,"%.12g\n",params[7]);
      }
      if(f>=0.9985e-3 && f<=1.0015e-3)
      {  
        fprintf(rfile2,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(rfile2,"%.12g ",params[i]);
        fprintf(rfile2,"%.12g ",params[6]);
        fprintf(rfile2,"%.12g ",params[5]);
        fprintf(rfile2,"%.12g\n",params[7]);
      }
      if(f>=2.9985e-3 && f<=3.0015e-3)
      {  
        fprintf(rfile3,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(rfile3,"%.12g ",params[i]);
        fprintf(rfile3,"%.12g ",params[6]);
        fprintf(rfile3,"%.12g ",params[5]);
        fprintf(rfile3,"%.12gn",params[7]);
      }
      if(f>=5.9985e-3 && f<=6.0015e-3)
      {  
        fprintf(rfile4,"%d  ", n);
        for(i=1; i<=4; i++) fprintf(rfile4,"%.12g ",params[i]);
        fprintf(rfile4,"%.12g ",params[6]);
        fprintf(rfile4,"%.12g ",params[5]);
        fprintf(rfile4,"%.12g\n",params[7]);
      }    

    }
  }

  fclose(outfile1);
  fclose(outfile2);
  fclose(bfile1);
  fclose(bfile2);
  fclose(bfile3);
  fclose(bfile4);
  fclose(rfile1);
  fclose(rfile2);
  fclose(rfile3);
  fclose(rfile4);

  free_dvector(resolved,1,Nkeep);
  free_dvector(rejected,1,Nreject);
  
  sprintf(Gfile, "%s_final_residual.dat", argv[3]);
  outfile1 = fopen(Gfile,"w");
  sprintf(Gfile, "%s_final_residual_power.dat", argv[3]);
  outfile2 = fopen(Gfile,"w");

  /*  print final residual  */
  printf("printing final residual\n");
  for(i=1; i<=NMAX; i++)
  {

    f = (double)i/T;
  
    Apower = 0.5*log10( 2.0*( dA[2*i-1]*dA[2*i-1] + dA[2*i]*dA[2*i] ) );
    Epower = 0.5*log10( 2.0*( dE[2*i-1]*dE[2*i-1] + dE[2*i]*dE[2*i] ) );
    
    fprintf(outfile1,"%.12g %.12g %.12g %.12g\n", f, dA[2*i-1], dA[2*i], dE[2*i-1], dE[2*i]);
    fprintf(outfile2,"%.12g %.12g %.12g\n", log10(f), Apower, Epower);

  }
  fclose(outfile1);
  fclose(outfile2);

  fclose(dfile);
  fclose(sfile);
  fclose(tfile);
  
  
  /*  deallocate memory  */
  free_dmatrix(bright,1,SMAX,0,NP);
  free_ivector(keep,1,SMAX);
  free_dvector(params,1,NP);
  free_dvector(A,1,16384);
  free_dvector(E,1,16384);
  free_dvector(sA,1,2*NMAX);
  free_dvector(sE,1,2*NMAX);
  free_dvector(hA,1,2*NMAX);
  free_dvector(hE,1,2*NMAX);
  free_dvector(dA,1,2*NMAX);
  free_dvector(dE,1,2*NMAX);
  free_dvector(wA,1,2*WINDOW);
  free_dvector(wE,1,2*WINDOW);
  free_dvector(wAtemp,1,2*WINDOW);
  free_dvector(wEtemp,1,2*WINDOW);
  free_dvector(fBIC,1,NW);}

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


double BIC(double *A, double *E, double *Sfit, int N, double Sn, long D, int Neff)
{
  double SA, SE, SAE;
  double logL, penalty;
  
  /*  calculate likelihood   */
  SA  = 0.5*nwip(A, A, N, Sn)/(double)N;
  SE  = 0.5*nwip(E, E, N, Sn)/(double)N;
  SAE = 0.5*nwip(A, E, N, Sn)/(double)N;

  Sfit[0] = SA;
  Sfit[1] = SE;
  Sfit[2] = SAE;
  
  logL    = 2.0*(double)N*log( SA*SE - SAE*SAE ) ;

  /* Note: Each source we throw back reduces the penalty, hence the minus sign
     for the source specfic terms in the penalty factor */
  penalty = 3.0*log( 4.0*(double)N ) - (double)D*log( 4.0*(double)Neff );
  
  /*  calculate model evidence using BIC  */
  return logL + penalty;
}

void dfour1(double data[], unsigned long nn, int isign)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi, swap;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}


void drealft(double data[], unsigned long n, int isign)
{
	void dfour1(double data[], unsigned long nn, int isign);
	unsigned long i,i1,i2,i3,i4,np3;
	double c1=0.5,c2,h1r,h1i,h2r,h2i;
	double wr,wi,wpr,wpi,wtemp,theta;

	theta=3.141592653589793/(double) (n>>1);
	if (isign == 1) {
		c2 = -0.5;
		dfour1(data,n>>1,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	np3=n+3;
	for (i=2;i<=(n>>2);i++) {
		i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i;
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r-data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		dfour1(data,n>>1,-1);
	}
}

void Data_Dimension(double *A, double *E, long M, int NP, double SA, double SE, int *Neff)
{

  int i, j, k;
  int Nsize;
  double ReA, ImA, ReB, ImB;
  double product;
  double SNRsq;
  
  product = 0.0;
  Nsize = 1;

  SNRsq = (nwip(A, A, M, SA) + nwip(E, E, M, SE));

   j = k = M/2;

   ReA = A[2*j-1];  
   ImA = A[2*j]; 
   ReB = E[2*j-1];  
   ImB = E[2*j];    
   product += 4.0*((ReA*ReA + ImA*ImA)/SA + (ReB*ReB + ImB*ImB)/SE);

   // find how many data points it takes to get to within D of SNR^2
   // we have 2 data points per bin per channel, and we are adding two bins
   // from each channel at each iteration, thus we add 8 data points at each
   // iteration.
  while(product < (SNRsq - (double)NP))
    {

      // printf("%d %f %f\n", Nsize, SNRsq, product);

      j += 1;
      k -= 1;
      Nsize += 2;

      ReA = A[2*j-1];  
      ImA = A[2*j]; 
      ReB = E[2*j-1];  
      ImB = E[2*j];
      product += 4.0*((ReA*ReA + ImA*ImA)/SA + (ReB*ReB + ImB*ImB)/SE);
      ReA = A[2*k-1];  
      ImA = A[2*k]; 
      ReB = E[2*k-1];  
      ImB = E[2*k];
      product += 4.0*((ReA*ReA + ImA*ImA)/SA + (ReB*ReB + ImB*ImB)/SE);
      
    }

  *Neff = Nsize;
 
}
