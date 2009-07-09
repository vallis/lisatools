#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <fftw3.h>
#include "Constants.h" 
#include "Declarations.h"

double *dvector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
    double *v=0;

    v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
    if (!v) fprintf(stderr,"allocation failure in dvector()");
    return v-nl+NR_END;
}

void free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}

void Cusp_Barycenter(Cusp_structure Cusp, double *hp, double *hc, int n)
{ 
 
  double f, c2psi, s2psi;
  // char Gfile[50]; 
  // double Tobs, fix, t, fmax, Dt;
  double Tburst, T0;
  double ts, B; // delt
  long N; // M, N, q
  long i, j, back; // cnt, mult
  // FILE* Infile; 
  // FILE* Outfile; 

 // used by fftw3
  fftw_complex *out;
  fftw_plan plan_reverse;
  int nc;
  double *in;

  if(((double)(Cusp.Samples)-pow(2.0,rint(log((double)(Cusp.Samples))/log(2.0)))) > 1.0e-10)
    {
      KILL("Number of Samples is not a power of 2\n");
    }

  N = 2*Cusp.Samples;   // have to double observation time to allow for Tpad.

  c2psi = cos(2.0*Cusp.Polarization);
  s2psi = sin(2.0*Cusp.Polarization);

  Tburst = (double)(N)/Cusp.Cadence;

  T0 = Tburst/4.0;
  ts = T0 + Cusp.CentralTime;

  back = N/4 - (int)(Cusp.Tpad*Cusp.Cadence);

  // printf("%d\n", back);

  nc = N/2+1;
  in = fftw_malloc ( sizeof ( double ) * N);
  out = fftw_malloc ( sizeof ( fftw_complex ) * nc );
  plan_reverse = fftw_plan_dft_c2r_1d(N, out, in, FFTW_ESTIMATE);
  // this is a FFTW_BACKWARD transform

  out[0][0] = 0.0;
  out[0][1] = 0.0;
 
  for(i=1; i<= (nc-1); i++) 
    { 
      f = (double)(i)/Tburst; 
      B = Cusp.Amplitude*pow((1+pow((FLOW/f),2.0)),-4.0) * pow(f,-4.0/3.0);
      if(f >= Cusp.MaximumFrequency) B *= exp(1.0-f/Cusp.MaximumFrequency);
      out[i][0] = -B*cos(-TPI*f*ts);
      out[i][1] = -B*sin(-TPI*f*ts);
    } 

  out[nc-1][1] = 0.0;
 
  fftw_execute ( plan_reverse);
 
  // Normalization: FFTW performs an unnormalized DFT. Thus the continuous Fourier transform
  // is given by H(f_k) = Dt H_k = Dt F(h_i), where F is the FFTW forward DFT; and the inverse Fourier
  // transform is given by h(t_i) = h_i = 1/N F^-1(H_k), where F^-1 is the FFTW backward DFT.
  // Since in the above we're setting B equal to expression (16) of CQG 25, 184026 (2008),
  // which is a continuous Fourier transform, it follows that we need to multiply the backward FFT
  // by 1 / (Dt N), as suggested by Antoine (MV 090708)
   
  for(i=0; i< n; i++) 
    { 
      hp[i] = 0.0;
      hc[i] = 0.0;
      j = back + i;
      if((j >= 0) && (j < N)) {
	      hp[i] =  in[j]*Cusp.Cadence*c2psi/(double)(N);
          hc[i] = -in[j]*Cusp.Cadence*s2psi/(double)(N);
      }
    } 
 

  fftw_free ( in );
  fftw_free ( out );
  fftw_destroy_plan ( plan_reverse ); 
 
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
