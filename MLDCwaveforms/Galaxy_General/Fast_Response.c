#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arrays.h"
#include "Constants.h"
#include "Detector.h"
#include <fftw3.h>

void spacecraft(double t,  double *x, double *y, double *z);
void convolve(long N, double *a, long M, double *b, double *cn);
void XYZ(double ***d, double f0, long q, long M, double *XLS, double *XSL, double *YLS, double *YSL, double *ZLS, double *ZSL);
void FAST_LISA(double *params, long N, long M, double *XLS, double *XSL, double *YLS, double *YSL, double *ZLS, double *ZSL);
double AEnoise(double f);
void KILL(char*);

int main(int argc,char **argv)
{

  double f, fdot, theta, phi, A, iota, psi, phase;
  char Gfile[50];
  double *params;
  double *XfLS, *YfLS, *ZfLS;
  double *XfSL, *YfSL, *ZfSL;
  double *XLS, *YLS, *ZLS;
  double *XSL, *YSL, *ZSL;
  double fonfs, Sn, Sm, Acut;
  long M, N, q;
  long i, j, k, cnt, mult;

  // used by fftw3
  fftw_complex *out;
  fftw_plan plan_reverse;
  int nc;
  double *in;

  FILE* Infile;
  FILE* Outfile;

  if(argc !=2) KILL("Fast_Response rSeed\n");

  params = dvector(0,7);

  if((T/year) <= 8.0) mult = 8;
  if((T/year) <= 4.0) mult = 4;
  if((T/year) <= 2.0) mult = 2;
  if((T/year) <= 1.0) mult = 1;

  nc = NFFT/2+1;
  in = fftw_malloc ( sizeof ( double ) * NFFT);
  out = fftw_malloc ( sizeof ( fftw_complex ) * nc );
  plan_reverse = fftw_plan_dft_c2r_1d(NFFT, out, in, FFTW_ESTIMATE);

  sprintf(Gfile, "Data/Galaxy_%s.dat", argv[1]);

  Infile = fopen(Gfile,"r");

  XfLS = dvector(0,NFFT-1);  YfLS = dvector(0,NFFT-1);  ZfLS = dvector(0,NFFT-1);
  XfSL = dvector(0,NFFT-1);  YfSL = dvector(0,NFFT-1);  ZfSL = dvector(0,NFFT-1);

  for(i=0; i<NFFT; i++)
    {
      XfLS[i] = 0.0;
      YfLS[i] = 0.0;
      ZfLS[i] = 0.0;
      XfSL[i] = 0.0;
      YfSL[i] = 0.0;
      ZfSL[i] = 0.0;
    }

  printf("Starting Simulation\n");

  cnt = 0;

    while ( !feof(Infile) )
     {
       fscanf(Infile, "%lf%lf%lf%lf%lf%lf%lf%lf\n", &f, &fdot, &theta, &phi, &A, &iota, &psi, &phase);
      
       params[0] = f;
       params[1] = fdot;
       params[2] = pi/2.0-theta;
       params[3] = phi; 
       params[4] = A; 
       params[5] = iota;
       params[6] = psi;
       params[7] = phase;

       N = 32*mult;
       if(f > 0.001) N = 64*mult;
       if(f > 0.01) N = 256*mult;
       if(f > 0.03) N = 512*mult;
       if(f > 0.1) N = 1024*mult;

       cnt++;

       if (cnt%100000 == 0) printf("%ld sources simulated\n", cnt);

       fonfs = f/fstar;

       q = (long)(f*T);

       Sn = AEnoise(f);

       /*  calculate michelson noise  */
       Sm = Sn/(4.0*sin(f/fstar)*sin(f/fstar));

       Acut = A*sqrt(T/Sm);

       M = (long)(pow(2.0,(rint(log(Acut)/log(2.0))+1.0)));

       if(M < N) M = N;
       if(M > 8192) M = 8192;

       XLS = dvector(1,2*M);  YLS = dvector(1,2*M);  ZLS = dvector(1,2*M);
       XSL = dvector(1,2*M);  YSL = dvector(1,2*M);  ZSL = dvector(1,2*M);

       FAST_LISA(params, N, M, XLS, XSL, YLS, YSL, ZLS, ZSL);

      for(i=1; i<=M; i++)
       {

        k = (q + i -1 - M/2);

	XfSL[2*k] += XSL[2*i-1];
	XfSL[2*k+1] += XSL[2*i];
	YfSL[2*k] += YSL[2*i-1];
	YfSL[2*k+1] += YSL[2*i];
	ZfSL[2*k] += ZSL[2*i-1];
	ZfSL[2*k+1] += ZSL[2*i];

	XfLS[2*k] += XLS[2*i-1];
	XfLS[2*k+1] += XLS[2*i];
	YfLS[2*k] += YLS[2*i-1];
	YfLS[2*k+1] += YLS[2*i];
	ZfLS[2*k] += ZLS[2*i-1];
	ZfLS[2*k+1] += ZLS[2*i];
       
       }


       free_dvector(XLS,1,2*M);  free_dvector(YLS,1,2*M);  free_dvector(ZLS,1,2*M);
       free_dvector(XSL,1,2*M);  free_dvector(YSL,1,2*M);  free_dvector(ZSL,1,2*M);

     }

  printf("Simulation Finished\n");

      Outfile = fopen("Data/Sources_LS.txt","w");
      sprintf(Gfile, "Galaxy_LS_%s", argv[1]);
      fprintf(Outfile,"%s\n", Gfile);
      fclose(Outfile);

      Outfile = fopen("Data/Sources_SL.txt","w");
      sprintf(Gfile, "Galaxy_SL_%s", argv[1]);
      fprintf(Outfile,"%s\n", Gfile);
      fclose(Outfile);


   out[0][0] = 0.0;
   out[0][1] = 0.0;
  for(i=1; i<NFFT/2; i++)
    {
      out[i][0] = XfLS[2*i];
      out[i][1] = -XfLS[2*i+1];
    }
   out[nc-1][0] = 0.0;
   out[nc-1][1] = 0.0;
   fftw_execute ( plan_reverse);

      sprintf(Gfile, "Binary/X_Galaxy_LS_%s.bin", argv[1]);
      Outfile = fopen(Gfile,"wb");
      for (i = 0 ; i < NFFT ; i++) fwrite(&in[i], sizeof(double), 1, Outfile);
      fclose(Outfile);

   out[0][0] = 0.0;
   out[0][1] = 0.0;
  for(i=1; i<NFFT/2; i++)
    {
      out[i][0] = YfLS[2*i];
      out[i][1] = -YfLS[2*i+1];
    }
   out[nc-1][0] = 0.0;
   out[nc-1][1] = 0.0;
   fftw_execute ( plan_reverse);

      sprintf(Gfile, "Binary/Y_Galaxy_LS_%s.bin", argv[1]);
      Outfile = fopen(Gfile,"wb");
      for (i = 0 ; i < NFFT ; i++) fwrite(&in[i], sizeof(double), 1, Outfile);
      fclose(Outfile);


   out[0][0] = 0.0;
   out[0][1] = 0.0;
  for(i=1; i<NFFT/2; i++)
    {
      out[i][0] = ZfLS[2*i];
      out[i][1] = -ZfLS[2*i+1];
    }
   out[nc-1][0] = 0.0;
   out[nc-1][1] = 0.0;
   fftw_execute ( plan_reverse);

      sprintf(Gfile, "Binary/Z_Galaxy_LS_%s.bin", argv[1]);
      Outfile = fopen(Gfile,"wb");
      for (i = 0 ; i < NFFT ; i++) fwrite(&in[i], sizeof(double), 1, Outfile);
      fclose(Outfile);


   out[0][0] = 0.0;
   out[0][1] = 0.0;
  for(i=1; i<NFFT/2; i++)
    {
      out[i][0] = XfSL[2*i];
      out[i][1] = -XfSL[2*i+1];
    }
   out[nc-1][0] = 0.0;
   out[nc-1][1] = 0.0;
   fftw_execute ( plan_reverse);

      sprintf(Gfile, "Binary/X_Galaxy_SL_%s.bin", argv[1]);
      Outfile = fopen(Gfile,"wb");
      for (i = 0 ; i < NFFT ; i++) fwrite(&in[i], sizeof(double), 1, Outfile);
      fclose(Outfile);

   out[0][0] = 0.0;
   out[0][1] = 0.0;
  for(i=1; i<NFFT/2; i++)
    {
      out[i][0] = YfSL[2*i];
      out[i][1] = -YfSL[2*i+1];
    }
   out[nc-1][0] = 0.0;
   out[nc-1][1] = 0.0;
   fftw_execute ( plan_reverse);

      sprintf(Gfile, "Binary/Y_Galaxy_SL_%s.bin", argv[1]);
      Outfile = fopen(Gfile,"wb");
      for (i = 0 ; i < NFFT ; i++) fwrite(&in[i], sizeof(double), 1, Outfile);
      fclose(Outfile);

   out[0][0] = 0.0;
   out[0][1] = 0.0;
  for(i=1; i<NFFT/2; i++)
    {
      out[i][0] = ZfSL[2*i];
      out[i][1] = -ZfSL[2*i+1];
    }
   out[nc-1][0] = 0.0;
   out[nc-1][1] = 0.0;
   fftw_execute ( plan_reverse);

      sprintf(Gfile, "Binary/Z_Galaxy_SL_%s.bin", argv[1]);
      Outfile = fopen(Gfile,"wb");
      for (i = 0 ; i < NFFT ; i++) fwrite(&in[i], sizeof(double), 1, Outfile);
      fclose(Outfile);

      free_dvector(params,0,6);

      free_dvector(XfLS,0,NFFT-1);  free_dvector(YfLS,0,NFFT-1);  free_dvector(ZfLS,0,NFFT-1);
      free_dvector(XfSL,0,NFFT-1);  free_dvector(YfSL,0,NFFT-1);  free_dvector(ZfSL,0,NFFT-1);

      fftw_free ( in );
      fftw_free ( out );
      fftw_destroy_plan ( plan_reverse ); 


    return 0;

}
	
	

void FAST_LISA(double *params, long N, long M, double *XLS, double *XSL, double *YLS, double *YSL, double *ZLS, double *ZSL)
{

  /*   Indicies   */
  int i,j,n,m;

  /*   Number of data points   */
  long q;

  /*   Gravitational Wave basis vectors   */
  double *u,*v,*k;
 
  /*   Polarization basis tensors   */
  double **eplus, **ecross;
  
  /*   Spacecraft position and separation vector   */
  double *x, *y, *z;
  double *r12, *r13, *r21, *r23, *r31, *r32;

  /*   Dot products   */
  double *kdotx, **kdotr;

  /*   Convenient quantities   */
  double **dplus, **dcross;

  /*   GW Source data   */
  double Mc, theta, phi, psi, D, iota, A, Aplus, Across, f0, fdot, phio;
  double costh, sinth, cosph, sinph, cosi, cosps, sinps;
 
  /*   Time and distance variables   */
  double *xi, t;

  /*   Gravitational wave frequency & ratio of f and transfer frequency f*  */
  double *f, *fonfs;
  
  /*   LISA response to slow terms (Real & Imaginary pieces)   */
  //Static quantities (Re and Im)
  double DPr, DPi, DCr, DCi;
  //Time varrying quantities (Re & Im) broken up into convenient segments
  double **TR, **TI, tran1r, tran1i, tran2r, tran2i;
  //Miscellaneous constants used to speed up calculations
  double arg1, arg2, sinc;

  /*   Fourier coefficients before FFT and after convolution  */
  //Time series of slowly evolving terms at each vertex   
  double *data12, *data13, *data21, *data23, *data31, *data32;
  //Fourier coefficients of slowly evolving terms (numerical)
  double *a12, *a13, *a21, *a23, *a31, *a32;
  //Fourier coefficients of rapidly evolving terms (analytical)
  double *b;             
  //Fourier coefficients of entire response (convolution)
  double *c12, *c13, *c21, *c23, *c31, *c32;
  //Package cij's into proper form for TDI subroutines
  double ***d;

  /*   Miscellaneous  */
  double xm, fstep, power;

  // used by fftw3
  fftw_complex *in, *out;
  fftw_plan plan_forward;

  /*   Allocating Arrays   */
  
  u = dvector(1,3); v = dvector(1,3); k = dvector(1,3);

  kdotx = dvector(1,3); kdotr = dmatrix(1,3,1,3);

  xi = dvector(1,3);
  
  f = dvector(1,3);
  
  fonfs = dvector(1,3);

  eplus  = dmatrix(1,3,1,3); ecross = dmatrix(1,3,1,3); 
  
  dplus  = dmatrix(1,3,1,3); dcross = dmatrix(1,3,1,3); 

  x = dvector(1,3); y = dvector(1,3); z = dvector(1,3);

  r12 = dvector(1,3); r21 = dvector(1,3); r31 = dvector(1,3);  
  r13 = dvector(1,3); r23 = dvector(1,3); r32 = dvector(1,3);
 
  TR = dmatrix(1,3,1,3); TI = dmatrix(1,3,1,3);

  data12 = dvector(1,2*N); data21 = dvector(1,2*N); data31 = dvector(1,2*N);
  data13 = dvector(1,2*N); data23 = dvector(1,2*N); data32 = dvector(1,2*N); 

  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  plan_forward = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  
  a12 = dvector(1,2*N+2); a21 = dvector(1,2*N+2); a31 = dvector(1,2*N+2);
  a13 = dvector(1,2*N+2); a23 = dvector(1,2*N+2); a32 = dvector(1,2*N+2);

  b = dvector(1,2*M+2);

  c12 = dvector(1,2*M+2); c21 = dvector(1,2*M+2); c31 = dvector(1,2*M+2);
  c13 = dvector(1,2*M+2); c23 = dvector(1,2*M+2); c32 = dvector(1,2*M+2);

  d = d3tensor(1,3,1,3,1,2*M);

   f0 = params[0];
   fdot = params[1];
   theta = params[2];
   phi = params[3]; 
   A = params[4]; 
   iota = params[5];
   psi = params[6];
   phio = params[7];

  //Calculate cos and sin of sky position, inclination, polarization
  costh = cos(theta);   sinth = sin(theta);
  cosph = cos(phi);     sinph = sin(phi);
  cosps = cos(2.*psi);  sinps = sin(2.*psi);
  cosi  = cos(iota);
  
  // Note: We don't bother to compute the amplitude evolution as it is immesurably small
  Aplus = A*(1.+cosi*cosi);
  Across = -2.0*A*cosi;

  //Calculate carrier frequency bin
  q = (long)(f0*T);

  //Calculate constant pieces of transfer functions
  DPr = Aplus*cosps;
  DPi = -Across*sinps;
  DCr = -Aplus*sinps;
  DCi = -Across*cosps;
 
		       
  /*   Tensor Crap   */
  u[1] =  costh*cosph;
  u[2] =  costh*sinph;
  u[3] = -sinth;

  v[1] =  sinph;
  v[2] = -cosph;
  v[3] =  0.;

  k[1] = -sinth*cosph;
  k[2] = -sinth*sinph;
  k[3] = -costh;


  for(i=1;i<=3;i++)
    {
      for(j=1;j<=3;j++)
	    {
         eplus[i][j]  = u[i]*u[j] - v[i]*v[j];
	     ecross[i][j] = u[i]*v[j] + v[i]*u[j];
	  
      	}
    }


  /*   Analytical Fourier transform of high frequency components   */
  for(i=1; i<=M; i++)
    {
      m = q + i-1 - M/2;
      xm = pi*(f0*T - (double)m);
      b[2*i-1] = cos(xm)*sin(xm)/xm;
      b[2*i] = sin(xm)*sin(xm)/xm;
    }


  /*   Main Loop   */
  for(n=1; n<=N; n++)
    {
      t = T*(double)(n-1)/(double)N;
	      
      //Calculate position of each spacecraft at time t
      spacecraft(t, x, y, z);

      for(i=1; i<=3; i++) 
        {
	  kdotx[i] = (x[i]*k[1]+y[i]*k[2]+z[i]*k[3])/clight;
          xi[i]    = t - kdotx[i];
          f[i]     = f0 + fdot*xi[i];
          fonfs[i] = f[i]/fstar;
        }

	     
      //Unit separation vector from spacecrafts i to j 
      r12[1] = (x[2] - x[1])/L;   r13[1] = (x[3] - x[1])/L;   r23[1] = (x[3] - x[2])/L;
      r12[2] = (y[2] - y[1])/L;   r13[2] = (y[3] - y[1])/L;   r23[2] = (y[3] - y[2])/L;
      r12[3] = (z[2] - z[1])/L;   r13[3] = (z[3] - z[1])/L;   r23[3] = (z[3] - z[2])/L;
      //Make use of symmetry
      for(i=1; i<=3; i++)
	{
	  r21[i] = -r12[i];
	  r31[i] = -r13[i];
	  r32[i] = -r23[i];
	}
	    
      
      dplus[1][2] = dplus[1][3] = dplus[2][1] = dplus[2][3] = dplus[3][1] = dplus[3][2] = 0.;
      dcross[1][2] = dcross[1][3] = dcross[2][1] = dcross[2][3] = dcross[3][1] = dcross[3][2] = 0.;
      //Convenient quantities d+ & dx
      for(i=1; i<=3; i++)
	{
	  for(j=1; j<=3; j++)
	    {
	      dplus[1][2]  += r12[i]*r12[j]*eplus[i][j];   dcross[1][2] += r12[i]*r12[j]*ecross[i][j];
	      dplus[2][3]  += r23[i]*r23[j]*eplus[i][j];   dcross[2][3] += r23[i]*r23[j]*ecross[i][j];
	      dplus[1][3]  += r13[i]*r13[j]*eplus[i][j];   dcross[1][3] += r13[i]*r13[j]*ecross[i][j];
	    }
	}
      
      dplus[2][1] = dplus[1][2];  dcross[2][1] = dcross[1][2];
      dplus[3][2] = dplus[2][3];  dcross[3][2] = dcross[2][3];
      dplus[3][1] = dplus[1][3];  dcross[3][1] = dcross[1][3];
      

      kdotr[1][2] = kdotr[1][3] = kdotr[2][1] = kdotr[2][3] = kdotr[3][1] = kdotr[3][2] = 0.;
      for(i=1; i<=3; i++)
	{
	  kdotr[1][2] += k[i]*r12[i];   kdotr[1][3] += k[i]*r13[i];   kdotr[2][3] += k[i]*r23[i];	  
	}
      kdotr[2][1] = -kdotr[1][2];
      kdotr[3][1] = -kdotr[1][3];
      kdotr[3][2] = -kdotr[2][3];

      //Calculating Transfer function
      for(i=1; i<=3; i++)
	{
	  for(j=1; j<=3; j++)
	    {
	      if(i!=j)
		{
		  arg1 = 0.5*fonfs[i]*(1 - kdotr[i][j]);
		  arg2 = pi*fdot*xi[i]*xi[i] + phio - 2.*pi*kdotx[i]*f0;
		  sinc = 0.25*sin(arg1)/arg1;
	      
		  tran1r = dplus[i][j]*DPr + dcross[i][j]*DCr;
		  tran1i = dplus[i][j]*DPi + dcross[i][j]*DCi;

		  tran2r = cos(arg1 + arg2);
		  tran2i = sin(arg1 + arg2);    
		  
		   //Real & Imaginary part of the slowly evolving signal
		  TR[i][j] = sinc*(tran1r*tran2r - tran1i*tran2i);
		  TI[i][j] = sinc*(tran1r*tran2i + tran1i*tran2r);
		}
	    }
	}
    
      data12[2*n-1] = TR[1][2];   data21[2*n-1] = TR[2][1];   data31[2*n-1] = TR[3][1];   
      data12[2*n]   = TI[1][2];   data21[2*n]   = TI[2][1];   data31[2*n]   = TI[3][1];
      data13[2*n-1] = TR[1][3];   data23[2*n-1] = TR[2][3];   data32[2*n-1] = TR[3][2];
      data13[2*n]   = TI[1][3];   data23[2*n]   = TI[2][3];   data32[2*n]   = TI[3][2];
    }

  for(n=1; n<=N; n++)
    {
      in[n-1][0] = data12[2*n-1];
      in[n-1][1] = -data12[2*n];
    }

  fftw_execute(plan_forward);

  data12[1] = out[0][0];
  data12[2] = -out[0][1];
  for(i=2; i<=N; i++)
    {
      data12[2*i-1] = out[N+1-i][0];
      data12[2*i] = -out[N+1-i][1];
    }

  for(n=1; n<=N; n++)
    {
      in[n-1][0] = data21[2*n-1];
      in[n-1][1] = -data21[2*n];
    }

  fftw_execute(plan_forward);

  data21[1] = out[0][0];
  data21[2] = -out[0][1];
  for(i=2; i<=N; i++)
    {
      data21[2*i-1] = out[N+1-i][0];
      data21[2*i] = -out[N+1-i][1];
    }

  for(n=1; n<=N; n++)
    {
      in[n-1][0] = data13[2*n-1];
      in[n-1][1] = -data13[2*n];
    }

  fftw_execute(plan_forward);

  data13[1] = out[0][0];
  data13[2] = -out[0][1];
  for(i=2; i<=N; i++)
    {
      data13[2*i-1] = out[N+1-i][0];
      data13[2*i] = -out[N+1-i][1];
    }

  for(n=1; n<=N; n++)
    {
      in[n-1][0] = data31[2*n-1];
      in[n-1][1] = -data31[2*n];
    }

  fftw_execute(plan_forward);

  data31[1] = out[0][0];
  data31[2] = -out[0][1];
  for(i=2; i<=N; i++)
    {
      data31[2*i-1] = out[N+1-i][0];
      data31[2*i] = -out[N+1-i][1];
    }


  for(n=1; n<=N; n++)
    {
      in[n-1][0] = data32[2*n-1];
      in[n-1][1] = -data32[2*n];
    }

  fftw_execute(plan_forward);

  data32[1] = out[0][0];
  data32[2] = -out[0][1];
  for(i=2; i<=N; i++)
    {
      data32[2*i-1] = out[N+1-i][0];
      data32[2*i] = -out[N+1-i][1];
    }


  for(n=1; n<=N; n++)
    {
      in[n-1][0] = data23[2*n-1];
      in[n-1][1] = -data23[2*n];
    }

  fftw_execute(plan_forward);

  data23[1] = out[0][0];
  data23[2] = -out[0][1];
  for(i=2; i<=N; i++)
    {
      data23[2*i-1] = out[N+1-i][0];
      data23[2*i] = -out[N+1-i][1];
    }

  for(i=1; i<=N; i++)
    {
      a12[i] = data12[N+i]/(double)N;  a21[i] = data21[N+i]/(double)N;  a31[i] = data31[N+i]/(double)N;
      a12[i+N] = data12[i]/(double)N;  a21[i+N] = data21[i]/(double)N;  a31[i+N] = data31[i]/(double)N;
      a13[i] = data13[N+i]/(double)N;  a23[i] = data23[N+i]/(double)N;  a32[i] = data32[N+i]/(double)N;
      a13[i+N] = data13[i]/(double)N;  a23[i+N] = data23[i]/(double)N;  a32[i+N] = data32[i]/(double)N;
    }

  a12[2*N+1] = data12[N+1]/(double)N;  a21[2*N+1] = data21[N+1]/(double)N;  a31[2*N+1] = data31[N+1]/(double)N;
  a12[2*N+2] = data12[N+2]/(double)N;  a21[2*N+2] = data21[N+2]/(double)N;  a31[2*N+2] = data31[N+2]/(double)N;
  a13[2*N+1] = data13[N+1]/(double)N;  a23[2*N+1] = data23[N+1]/(double)N;  a32[2*N+1] = data32[N+1]/(double)N;
  a13[2*N+2] = data13[N+2]/(double)N;  a23[2*N+2] = data23[N+2]/(double)N;  a32[2*N+2] = data32[N+2]/(double)N;


  /*   Convolving Fourier Coefficients */
  convolve(N, a12, M, b, c12);  convolve(N, a21, M, b, c21);  convolve(N, a31, M, b, c31);
  convolve(N, a13, M, b, c13);  convolve(N, a23, M, b, c23);  convolve(N, a32, M, b, c32);
  
  /*   Renormalize so that the time series is real   */
  for(i=1; i<=2*M; i++)
    {
      d[1][2][i] = 0.5*c12[i];  d[2][1][i] = 0.5*c21[i];  d[3][1][i] = 0.5*c31[i];
      d[1][3][i] = 0.5*c13[i];  d[2][3][i] = 0.5*c23[i];  d[3][2][i] = 0.5*c32[i];
    }

  /*   Call subroutines for synthesizing different TDI data channels  */
  
  /*   X Y Z-Channel   */
  XYZ(d, f0, q, M, XLS, XSL, YLS, YSL, ZLS, ZSL);
 

  /*   Deallocate Arrays   */

  fftw_destroy_plan(plan_forward);
  fftw_free(in);
  fftw_free(out);
  
  free_dvector(u,1,3); free_dvector(v,1,3); free_dvector(k,1,3);

  free_dvector(kdotx,1,3); free_dmatrix(kdotr,1,3,1,3);

  free_dvector(xi,1,3);
  
  free_dvector(f,1,3);
  
  free_dvector(fonfs,1,3);

  free_dmatrix(eplus,1,3,1,3); free_dmatrix(ecross,1,3,1,3); 
  
  free_dmatrix(dplus,1,3,1,3); free_dmatrix(dcross,1,3,1,3); 

  free_dvector(x,1,3); free_dvector(y,1,3); free_dvector(z,1,3);

  free_dvector(r12,1,3); free_dvector(r21,1,3); free_dvector(r31,1,3);  
  free_dvector(r13,1,3); free_dvector(r23,1,3); free_dvector(r32,1,3);

  free_dmatrix(TR,1,3,1,3); free_dmatrix(TI,1,3,1,3);

  free_dvector(data12,1,2*N); free_dvector(data21,1,2*N); free_dvector(data31,1,2*N);
  free_dvector(data13,1,2*N); free_dvector(data23,1,2*N); free_dvector(data32,1,2*N); 
  
  free_dvector(a12,1,2*N+2); free_dvector(a21,1,2*N+2); free_dvector(a31,1,2*N+2);
  free_dvector(a13,1,2*N+2); free_dvector(a23,1,2*N+2); free_dvector(a32,1,2*N+2);

  free_dvector(b,1,2*M+2);

  free_dvector(c12,1,2*M+2); free_dvector(c21,1,2*M+2); free_dvector(c31,1,2*M+2);
  free_dvector(c13,1,2*M+2); free_dvector(c23,1,2*M+2); free_dvector(c32,1,2*M+2);

  free_d3tensor(d,1,3,1,3,1,2*M);

  return;
}

/*************************************************************************************/
/*                                                                                   */
/*                                    Subroutines                                    */
/*                                                                                   */
/*************************************************************************************/ 



/*************************************************************************/
/*        Rigid approximation position of each LISA spacecraft           */
/*************************************************************************/
void spacecraft(double t, double *x, double *y, double *z)
{

  double alpha;
  double beta1, beta2, beta3;
  double sa, sb, ca, cb;
 
  alpha = 2.*pi*fm*t + kappa;

  beta1 = 0. + lambda;
  beta2 = 2.*pi/3. + lambda;
  beta3 = 4.*pi/3. + lambda;

  sa = sin(alpha);
  ca = cos(alpha);


  sb = sin(beta1);
  cb = cos(beta1);
  x[1] = AU*ca + AU*ec*(sa*ca*sb - (1. + sa*sa)*cb);
  y[1] = AU*sa + AU*ec*(sa*ca*cb - (1. + ca*ca)*sb);
  z[1] = -sq3*AU*ec*(ca*cb + sa*sb);

 
  sb = sin(beta2);
  cb = cos(beta2);
  x[2] = AU*ca + AU*ec*(sa*ca*sb - (1. + sa*sa)*cb);
  y[2] = AU*sa + AU*ec*(sa*ca*cb - (1. + ca*ca)*sb);
  z[2] = -sq3*AU*ec*(ca*cb + sa*sb);

  sb = sin(beta3);
  cb = cos(beta3);
  x[3] = AU*ca + AU*ec*(sa*ca*sb - (1. + sa*sa)*cb);
  y[3] = AU*sa + AU*ec*(sa*ca*cb - (1. + ca*ca)*sb);
  z[3] = -sq3*AU*ec*(ca*cb + sa*sb);
  
}
/*************************************************************************/





/*************************************************************************/
/*  Frequency domain convolution of slow and fast Fourier coefficients   */
/*************************************************************************/

void convolve(long N, double *a, long M, double *b, double *cn)
{
  int i,n,m;
  double *ReA, *ImA, *ReB, *ImB, *ReC, *ImC;
 
  ReA = dvector(1,N);
  ImA = dvector(1,N);
  
  ReB = dvector(1,M);
  ImB = dvector(1,M);

  ReC = dvector(1,M);
  ImC = dvector(1,M);

  for(i=1; i<=N; i++)
    {
      ReA[i] = a[2*i-1];
      ImA[i] = a[2*i];
    }

  for(i=1; i<=M; i++)
    {
      ReB[i] = b[2*i-1];
      ImB[i] = b[2*i];
    }

  for(i=1; i<=M; i++)
    {
      ReC[i] = ImC[i] = 0.;
      for(n=1; n<=N; n++)
	{
	  m = i - n + (N-M)/2;
	  if(m<1)
	    {
	      do 
		{
		  m = m + M;
		}while(m<1);
	    }
	  //printf("%i %i %i\n",i,n,m);
	  ReC[i] += ReA[n]*ReB[m] - ImA[n]*ImB[m];
	  ImC[i] += ReA[n]*ImB[m] + ImA[n]*ReB[m];
	}
    }
  
  for(i=1; i<=M; i++)
  {
    m = M/2 + i + 1;

           if(m>M)m = m-M;
           cn[2*i-1] = ReC[m];
           cn[2*i] = ImC[m];
  }
  
  
  free_dvector(ReA,1,N);
  free_dvector(ImA,1,N);
  free_dvector(ReB,1,M);
  free_dvector(ImB,1,M);
  free_dvector(ReC,1,M);
  free_dvector(ImC,1,M);

}
/*************************************************************************/

void XYZ(double ***d, double f0, long q, long M, double *XLS, double *XSL, double *YLS, double *YSL, double *ZLS, double *ZSL)
{ 
  int i;  
  double fonfs, sqT;
  double ReX, ImX, ReY, ImY, ReZ, ImZ;
  double c3, s3, c2, s2, c1, s1;
  double f,power;
  double *X, *Y, *Z;
  double phiLS, phiSL, cLS, sLS, cSL, sSL;

   X = dvector(1,2*M);  Y = dvector(1,2*M);  Z = dvector(1,2*M);

   phiLS = 2.0*pi*f0*(dt/2.0-L/clight);
   phiSL = pi/2.0-2.0*pi*f0*(L/clight);
   cLS = cos(phiLS);
   sLS = sin(phiLS);
   cSL = cos(phiSL);
   sSL = sin(phiSL);

  sqT = sqrt(T);
  for(i=1; i<=M; i++)
    {

      f = ((double)(q + i-1 - M/2))/T;
      fonfs = f/fstar;
      c3 = cos(3.*fonfs);  c2 = cos(2.*fonfs);  c1 = cos(1.*fonfs);
      s3 = sin(3.*fonfs);  s2 = sin(2.*fonfs);  s1 = sin(1.*fonfs);


      X[2*i-1] = (d[1][2][2*i-1]-d[1][3][2*i-1])*c3 + (d[1][2][2*i]-d[1][3][2*i])*s3 +
	         (d[2][1][2*i-1]-d[3][1][2*i-1])*c2 + (d[2][1][2*i]-d[3][1][2*i])*s2 +
	         (d[1][3][2*i-1]-d[1][2][2*i-1])*c1 + (d[1][3][2*i]-d[1][2][2*i])*s1 +
	         (d[3][1][2*i-1]-d[2][1][2*i-1]);
      X[2*i]   = (d[1][2][2*i]-d[1][3][2*i])*c3 - (d[1][2][2*i-1]-d[1][3][2*i-1])*s3 +
	         (d[2][1][2*i]-d[3][1][2*i])*c2 - (d[2][1][2*i-1]-d[3][1][2*i-1])*s2 +
                 (d[1][3][2*i]-d[1][2][2*i])*c1 - (d[1][3][2*i-1]-d[1][2][2*i-1])*s1 +
	         (d[3][1][2*i]-d[2][1][2*i]);
	        
      Y[2*i-1] = (d[2][3][2*i-1]-d[2][1][2*i-1])*c3 + (d[2][3][2*i]-d[2][1][2*i])*s3 +
	         (d[3][2][2*i-1]-d[1][2][2*i-1])*c2 + (d[3][2][2*i]-d[1][2][2*i])*s2+
	         (d[2][1][2*i-1]-d[2][3][2*i-1])*c1 + (d[2][1][2*i]-d[2][3][2*i])*s1+
                 (d[1][2][2*i-1]-d[3][2][2*i-1]);
      Y[2*i]   = (d[2][3][2*i]-d[2][1][2*i])*c3 - (d[2][3][2*i-1]-d[2][1][2*i-1])*s3+
	         (d[3][2][2*i]-d[1][2][2*i])*c2 - (d[3][2][2*i-1]-d[1][2][2*i-1])*s2+
	         (d[2][1][2*i]-d[2][3][2*i])*c1 - (d[2][1][2*i-1]-d[2][3][2*i-1])*s1+
	         (d[1][2][2*i]-d[3][2][2*i]);
	             
      Z[2*i-1] = (d[3][1][2*i-1]-d[3][2][2*i-1])*c3 + (d[3][1][2*i]-d[3][2][2*i])*s3+
	         (d[1][3][2*i-1]-d[2][3][2*i-1])*c2 + (d[1][3][2*i]-d[2][3][2*i])*s2+
	         (d[3][2][2*i-1]-d[3][1][2*i-1])*c1 + (d[3][2][2*i]-d[3][1][2*i])*s1+
                 (d[2][3][2*i-1]-d[1][3][2*i-1]);

      Z[2*i]   = (d[3][1][2*i]-d[3][2][2*i])*c3 - (d[3][1][2*i-1]-d[3][2][2*i-1])*s3+
	         (d[1][3][2*i]-d[2][3][2*i])*c2 - (d[1][3][2*i-1]-d[2][3][2*i-1])*s2+
	         (d[3][2][2*i]-d[3][1][2*i])*c1 - (d[3][2][2*i-1]-d[3][1][2*i-1])*s1+
	         (d[2][3][2*i]-d[1][3][2*i]);
        

      XSL[2*i-1] = 2.0*fonfs*(X[2*i-1]*cSL-X[2*i]*sSL);
      XSL[2*i] = -2.0*fonfs*(X[2*i-1]*sSL+X[2*i]*cSL);
      YSL[2*i-1] = 2.0*fonfs*(Y[2*i-1]*cSL-Y[2*i]*sSL);
      YSL[2*i] = -2.0*fonfs*(Y[2*i-1]*sSL+Y[2*i]*cSL);
      ZSL[2*i-1] = 2.0*fonfs*(Z[2*i-1]*cSL-Z[2*i]*sSL);
      ZSL[2*i] = -2.0*fonfs*(Z[2*i-1]*sSL+Z[2*i]*cSL);

      XLS[2*i-1] = (X[2*i-1]*cLS-X[2*i]*sLS);
      XLS[2*i] = -(X[2*i-1]*sLS+X[2*i]*cLS);
      YLS[2*i-1] = (Y[2*i-1]*cLS-Y[2*i]*sLS);
      YLS[2*i] = -(Y[2*i-1]*sLS+Y[2*i]*cLS);
      ZLS[2*i-1] = (Z[2*i-1]*cLS-Z[2*i]*sLS);
      ZLS[2*i] = -(Z[2*i-1]*sLS+Z[2*i]*cLS);


    }

   free_dvector(X,1,2*M);  free_dvector(Y,1,2*M);  free_dvector(Z,1,2*M);

}

void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);

 
  return;
}
