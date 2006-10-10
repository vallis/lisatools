/****************************************************************************/
/*                                                                          */
/*                 Galactic Binary Template Matching Code                   */
/*                                                                          */
/*                 Authors: Neil J. Cornish & Jeff Crowder                  */
/*                                                                          */
/*                 Notes: This code is provided as a building               */
/*                 block for the analysis of the Mock LISA Data             */
/*                 Challenge data sets. If you use any part of              */
/*                 this code in any work that results in a                  */
/*                 publication, please add a note to that effect            */
/*                 in your acknowledgements section.                        */
/*                                                                          */
/****************************************************************************/


/***************************  REQUIRED LIBRARIES  ***************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "nrutil.h"
#include "Constants.h"

#include "io-C/readxml.h"
#include "io-C/xmlbinary.h"


#define NRANSI
#define TINY 1.0e-10
#define NMAX 200
#define GET_PSUM \
					for (j=1;j<=ndim;j++) {\
					for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];\
					psum[j]=sum;}
#define SWAP2(a,b) {swap=(a);(a)=(b);(b)=swap;}

void LowFrequency(double Tobs, double* Parameters, long fb, double* ChR, double* ChI, int Vertex);
double Noise(double);
void KILL(char*);
void get_SNR_logL(double *Parameters, double* SNR, double* dlogL, long Nrec, long Bi, double* Ch1R, double* Ch1I, double* Ch2R, double* Ch2I, double Tobs);
void dfour1(double data[], unsigned long nn, int isign);
void drealft(double data[], unsigned long n, int isign);

/* ============================  MAIN PROGRAM  ============================ */
int main(int argc, char* argv[])
{
	/* Time of observation */
	double Tobs;
	/* Total number of Fourier coefficients */
	long Nrec;
	/* Initial frequency bin that contains the source */
	long Bi, Bf;
	/* frequencies */
	double fmid, fmin, fmax, f_c;
	/* Fourier coefficients for the response with noise */
	double *Ch1R, *Ch1I, *Ch2R, *Ch2I;
	/* individual source parameters */
	double SNR, A, f, theta, phi, psi, iota, iphase;
	/* lists of source parameters */
	double *Parameters, *Params, *Pfit;
	/* log likelihood */
	double logL, logLmax, SNRmax;
	/* Bandwidth */
	double Bw;

        double f_start, f_end;

	long fb, Nfc, Of, a_move;
	long Bsi;
	/* counters */
	int i, j, k, l, m, ii, jj, kk, ll, mm, cnt;

	/* File pointers */
	FILE *Infile1, *Infile2, *Sfile, *tfile, *xfile, *record_file, *file_pointer;

        double *ATDI, *ETDI;
        double fix;
        long NFFT;

        TimeSeries *timeseries;

	if(argc!=4) KILL("MLDC_galactic [xml file name] [start frequency (Hz)] [end frequency (Hz)]\n");

	/* read in xml data file */

        timeseries = getTDIdata(argv[1]);

         printf("'%s': %d x %d doubles (every %g seconds)\n ",timeseries->Name,
                                                        timeseries->Records,
                                                        timeseries->Length,
                                                        timeseries->Cadence);

        printf("data in '%s'\n",timeseries->FileName);

        NFFT = timeseries->Length;

        Tobs = (double)(NFFT)*(timeseries->Cadence);

        /* used to convert Numerical Recipes FFT output into a strain spectral density */
        fix = sqrt(Tobs)/((double)NFFT);

	f_start = atof(argv[2]);
        f_end = atof(argv[3]);

	Bi = (long)(f_start*Tobs);

        Bf = (long)(f_end*Tobs);
        if(Bf > NFFT/2-1) Bf = NFFT/2-1;

        Nrec = Bf-Bi;
	
	/* Dynamically allocate memory for the various Fourier coefficients */
	Ch1R = dvector(0,Nrec-1);
        Ch2R = dvector(0,Nrec-1);
        Ch1I = dvector(0,Nrec-1);
        Ch2I = dvector(0,Nrec-1);

        Parameters = dvector(0,6);
        Params = dvector(0,6);
        Pfit = dvector(0,6);

 /* Dynamically allocate the A and E signal arrays */

  ATDI = (double*) malloc ((int)NFFT*sizeof(double));
  ETDI = (double*) malloc ((int)NFFT*sizeof(double));

  if (!ATDI || !ETDI) KILL("Out of memory.\n");

    for(i=0; i<NFFT; i++)
      {
	ATDI[i] = (2.0*timeseries->Data[1]->data[i]- timeseries->Data[2]->data[i] - timeseries->Data[3]->data[i])/3.0;
        ETDI[i] = (timeseries->Data[3]->data[i] - timeseries->Data[2]->data[i])/sqrt(3.0);
      }

    freeTimeSeries(timeseries);

  /* Calculate FFTs. */
   drealft(ATDI-1, NFFT, 1);
   drealft(ETDI-1, NFFT, 1);

 /* convert Numerical Recipes FFT output into a strain spectral density */

  for(i = 0; i < NFFT; i++)
    {
     ATDI[i] *= fix;
     ETDI[i] *= fix;
    }

  /* load into internal arrays and account for different FFT convention */
  for(i = Bi; i < Bf; i++)
    {
      Ch1R[i-Bi] = -ATDI[2*i];
      Ch1I[i-Bi] = ATDI[2*i+1];
      Ch2R[i-Bi] = -ETDI[2*i];
      Ch2I[i-Bi] = ETDI[2*i+1];
    }

  free(ATDI);
  free(ETDI);


  /* set template parameters (currently set to Training 1.1.1.b values) */

    f = 0.002909689776;
    A = 3.995025829e-23;
    theta = pi/2.0-0.03834232679; /* codes use co-latitude */
    phi = 4.742862;
    psi = 5.097536;
    iota = 1.413846;
    iphase = 1.740975;


       /* would be easier to use a Parameter structure if you like those things */
        Parameters[0] = A;
        Parameters[1] = f;
        Parameters[2] = theta;
        Parameters[3] = phi;
        Parameters[4] = psi;
        Parameters[5] = iota;
        Parameters[6] = iphase;


        get_SNR_logL(Parameters, &SNR, &logL, Nrec, Bi, Ch1R, Ch1I, Ch2R, Ch2I, Tobs);
 

        printf("\nTemplate gives an SNR of %f\n", SNR);
        printf("Template improves (+ve is good) the log likelihood by %f\n\n", logL);

        printf("Commencing Grid Search\n");


	/* here is a quick grid search in the neighborhood of the true parameters,
           we expect the noise to shift the best fit values away from the true
           parameters. The grid has 5^3*3^4 = 10125 points */

        logLmax = -1000.0;

        cnt = 0;

        for(i = -1; i <= 1; i++)
         {  
	   Params[0] = Parameters[0]*(1.0+0.1*(double)(i));
         for(j = -2; j <= 2; j++)
          {  
	    Params[1] = Parameters[1]+0.1*(double)(j)/Tobs;
         for(k = -2; k <= 2; k++)
          {  
	    Params[2] = Parameters[2]+0.01*(double)(k);
         for(l = -2; l <= 2; l++)
          {  
	    Params[3] = Parameters[3]+0.01*(double)(l);
         for(m = -1; m <= 1; m++)
          { 
	    Params[4] = Parameters[4]+0.05*(double)(m);
         for(ii = -1; ii <= 1; ii++)
          { 
	    Params[5] = Parameters[5]+0.05*(double)(ii);
         for(jj = -1; jj <= 1; jj++)
          { 
	    Params[6] = Parameters[6]+0.05*(double)(jj);
           
            get_SNR_logL(Params, &SNR, &logL, Nrec, Bi, Ch1R, Ch1I, Ch2R, Ch2I, Tobs);   

            cnt++;

            if(logL > logLmax)
	      {
		logLmax = logL;
                SNRmax = SNR;
                for(mm = 0; mm<7; mm++) Pfit[mm] = Params[mm];
              }  
	  }}}}}}}

        printf("Grid Search over %d templates complete\n\n",cnt); 

        printf("Best fit has SNR = %f and improves log likelihood by %f\n\n", SNRmax, logLmax);
        printf("Best fit parameters are:\n");
        printf("A = %e\n", Pfit[0]);
        printf("f = %.10f\n", Pfit[1]);
        printf("theta = %f\n", pi/2.0-Pfit[2]);
        printf("phi = %f\n", Pfit[3]);
        printf("psi = %f\n", Pfit[4]);
        printf("iota = %f\n", Pfit[5]);
        printf("phase = %f\n\n", Pfit[6]);           

	free_dvector(Parameters,0,6);
	free_dvector(Params,0,6);
	free_dvector(Pfit,0,6);

	free_dvector(Ch1R,0,Nrec-1);
        free_dvector(Ch2R,0,Nrec-1);
        free_dvector(Ch1I,0,Nrec-1);
        free_dvector(Ch2I,0,Nrec-1);

	return 0;
}


/***************************  INTERNAL FUNCTIONS  ***************************/

void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1); 
  return;
}

void LowFrequency(double Tobs, double* Parameters, long fb, double* ChR, double* ChI, int Vertex)
{
         double A, f, theta, phi, psi, iota, phase;
	/* Real and imaginary parts of the Doppler modulation */
	double *dR, *dI;
	/* Real and imaginary parts of the Doppler+amplitude modulations */
	double *sR, *sI;
	/* Real and imaginary parts of the total modulations */
	double *tR, *tI;
	/* Modulation index */
	double beta;
	/* Initial and final frequency bins for the Fourier snippet and Carrier frequency bin*/
	long Bi, Bf, q;
	/* Polarization amplitudes */
	double Ap, Ac;
	/* x_n */
	double xn;
	/* doppler comb width (in 1/year harmonics) */
	long fdop;
	double yij;
	/* Real and imaginary coefficients of D_plus and D_cross */
	double DPR[5], DPI[5], DCR[5], DCI[5];
	/* Real and imaginary parts of the Fourier cofficients for the antenna beam pattern factors */
	double pR[5], pI[5], cR[5], cI[5];
	/* Fourier coefficients */
	double apR[9], apI[9], acR[9], acI[9];
	double aR, aI;
        double fstar, fix, cf, sf;
	/* Lambda prime */
	double lambdap;
	/* Loop indexes */
	long i, j;

	double sine_theta, cosine_theta, sine_2theta, cosine_2theta, sine_phi_kappa_2lambdap, cosine_phi_kappa_2lambdap, sine_2phi_4kappa_2lambdap, cosine_2phi_4kappa_2lambdap, sine_phi_3kappa_2lambdap, cosine_phi_3kappa_2lambdap, cosine_2psi, sine_2psi, cosine_phase, sine_phase, sqrt_3;

	/* Set the value of lambda */
	if (Vertex == 1) lambdap = lambda;
	else if (Vertex == 2) lambdap = lambda + pi/4.0;
	else if (Vertex == 3) lambdap = lambda - pi/4.0;
	else KILL("A nonexistent vertex was entered.\n");

        /* would be easier to use a Parameter structure if you like those things */
        A = Parameters[0];
        f = Parameters[1];
        theta = Parameters[2];
        phi = Parameters[3];
        psi = Parameters[4];
        iota = Parameters[5];
        phase = Parameters[6];

	/* Calculate the polarization amplitudes */
	Ap = A*(1.0+pow(cos(iota),2.0));		Ac = -2.0*A*cos(iota);

        fstar = c/(2.0*pi*L);

        fix = 0.5*sqrt(Tobs);

	/* Calculate the modulation index */
	beta = 2.0*pi*f*(AU/c)*sin(theta);

	fdop = (long)((double)(fb)*year/Tobs);
	if(fdop < 6) fdop = 6;

	dR = dvector(0, 2*fdop);
	dI = dvector(0, 2*fdop); 
	sR = dvector(0, 2*fdop);
	sI = dvector(0, 2*fdop);
	tR = dvector(0, 2*fb);
	tI = dvector(0, 2*fb);

	/* Calculate the frequency bin that contains the carrier frequency */
	q = (long)(rint(f*Tobs));

	/* Caculate the initial and final bin numbers */
	Bi = q - fb;
	Bf = q + fb;

	/* The values Bi and Bf tell us where the source sits in terms of frequency bins.  Each bin is 1/Tobs Hz wide.  Bi tells us where the source starts and Bf tells us where the source ends.  Over 98% of the power in the source lies between Bi and Bf.  To put together simulation with multiple bins one needs to reference Bi and Bf in order to know where to place the Fourier snippet in the full frequency record. */

	/* Calculate the real and imaginary parts of D_plus and D_cross for each harmonic */
	sine_theta = sin(theta);
	cosine_theta = cos(theta);
	sine_2theta = sin(2.0*theta);
	cosine_2theta = cos(2.0*theta);
	sine_phi_kappa_2lambdap = sin(phi+kappa-2.0*lambdap);
	cosine_phi_kappa_2lambdap = cos(phi+kappa-2.0*lambdap);
	sine_2phi_4kappa_2lambdap = sin(2.0*phi-4.0*kappa+2.0*lambdap);
	cosine_2phi_4kappa_2lambdap = cos(2.0*phi-4.0*kappa+2.0*lambdap);
	sine_phi_3kappa_2lambdap = sin(phi-3.0*kappa+2.0*lambdap);
	cosine_phi_3kappa_2lambdap = cos(phi-3.0*kappa+2.0*lambdap);
	sqrt_3 = sqrt(3.0);
	
	DPR[4] = (sqrt_3/128.0)*(3.0+cosine_2theta)*sine_2phi_4kappa_2lambdap;
	DPR[3] = (3.0/32.0)*sine_2theta*sine_phi_3kappa_2lambdap;
	DPR[2] = -(9.0*sqrt_3/32.0)*pow(sine_theta,2)*sin(2.0*(kappa-lambdap));
	DPR[1] = (9.0/32.0)*sine_2theta*sine_phi_kappa_2lambdap;
	DPR[0] = -(9.0*sqrt_3/64.0)*(3.0+cosine_2theta)*sin(2.0*(phi-lambdap));

	DPI[4] = (sqrt(3.0)/128.0)*(3.0+cosine_2theta)*cosine_2phi_4kappa_2lambdap;
	DPI[3] = (3.0/32.0)*sine_2theta*cosine_phi_3kappa_2lambdap;
	DPI[2] = (9.0*sqrt_3/32.0)*pow(sine_theta,2)*cos(2.0*(kappa-lambdap));
	DPI[1] = -(9.0/32.0)*sine_2theta*cosine_phi_kappa_2lambdap;
	DPI[0] = 0.0;

	DCR[4] = -(sqrt_3/32.0)*cosine_theta*cosine_2phi_4kappa_2lambdap;
	DCR[3] = -(3.0/16.0)*sine_theta*cosine_phi_3kappa_2lambdap;
	DCR[2] = 0.0;
	DCR[1] = -(9.0/16.0)*sine_theta*cosine_phi_kappa_2lambdap;
	DCR[0] = (9.0*sqrt_3/16.0)*cosine_theta*cos(2.0*(phi-lambdap));

	DCI[4] = (sqrt_3/32.0)*cosine_theta*sine_2phi_4kappa_2lambdap;
	DCI[3] = (3.0/16.0)*sine_theta*sine_phi_3kappa_2lambdap;
	DCI[2] = 0.0;
	DCI[1] = -(9.0/16.0)*sine_theta*sine_phi_kappa_2lambdap;
	DCI[0] = 0.0;


	cosine_2psi = cos(2.0*psi);
	sine_2psi = sin(2.0*psi);

	/* Calculate the Fourier coefficients for the antenna beam pattern functions */
	for (i = 0 ; i <= 4 ; i++)
	{
		pR[i] = Ap*0.5*(cosine_2psi*DPR[i] - sine_2psi*DCR[i]);
		pI[i] = Ap*0.5*(cosine_2psi*DPI[i] - sine_2psi*DCI[i]);

		cR[i] = Ac*0.5*(sine_2psi*DPR[i] + cosine_2psi*DCR[i]);
		cI[i] = Ac*0.5*(sine_2psi*DPI[i] + cosine_2psi*DCI[i]);
	}

	for (i = -fdop ; i <= fdop ; i++)
	{
		dR[i+fdop] = jn(i,beta)*cos((pi/2.0-phi)*(double)i);	dI[i+fdop] = jn(i,beta)*sin((pi/2.0-phi)*(double)i);
		sR[i+fdop] = 0.0;		sI[i+fdop] = 0.0;
	}

	for (i = -fb ; i <= fb ; i++)
	{
		tR[i+fb] = 0.0;			tI[i+fb] = 0.0;
	}

	/* Convert the amplitude modulations into the range -4 to 4, at the same time multiplying by exp(i phase).  For the cross polarization we also include the 3 pi/2 factor that reflects the sine vs cosine nature of the two terms. */

	cosine_phase = cos(phase);
	sine_phase = sin(phase);

	for (i = 0 ; i < 4 ; i++)
	{
		apR[i] = pR[4-i]*cosine_phase + pI[4-i]*sine_phase;
		apI[i] = -pI[4-i]*cosine_phase + pR[4-i]*sine_phase;
		acR[i] = cR[4-i]*sine_phase - cI[4-i]*cosine_phase;
		acI[i] = -cI[4-i]*sine_phase - cR[4-i]*cosine_phase;
	}

	for (i = 0; i <= 4; i++)
	{
		apR[i+4] = pR[i]*cosine_phase - pI[i]*sine_phase;
		apI[i+4] = pI[i]*cosine_phase + pR[i]*sine_phase;
		acR[i+4] = cR[i]*sine_phase  +  cI[i]*cosine_phase;
		acI[i+4] = cI[i]*sine_phase  -  cR[i]*cosine_phase;
	}

	/* Convolve the amplitude and Doppler modulation */
	for (i = -4; i <= 4; i++) for (j = -fdop; j <= fdop; j++) if( (i+j >= -fdop) && (i+j <= fdop) )
	{
		sR[i+j+fdop] += (apR[i+4]+acR[i+4])*dR[j+fdop]-(apI[i+4]+acI[i+4])*dI[j+fdop];
		sI[i+j+fdop] += (apI[i+4]+acI[i+4])*dR[j+fdop]+(apR[i+4]+acR[i+4])*dI[j+fdop];
	}

	/* Multiply together the modulation results with the exp(2 pi i f t) barycenter phase and convert to a discrete FFT of duration Tobs */
	for (j = -fb; j <= fb; j++)
	{
		for (i = -fdop; i <= fdop; i++)
		{
			yij = Tobs*(f+fm*(double)(i))-(double)(q+j);

			if(fabs(yij) < 1.0e-9)
			{
				aR = 1.0;		aI = 0.0;
			}
			else
			{
				aR = sin(pi*yij)*cos(pi*yij)/(pi*yij);		aI = sin(pi*yij)*sin(pi*yij)/(pi*yij);
			}

			tR[j+fb] += sR[i+fdop]*aR-sI[i+fdop]*aI;
			tI[j+fb] += sI[i+fdop]*aR+sR[i+fdop]*aI;
		}
	}

	/* convert to MLDC style A, E */

	for (i = 0 ; i < 2*fb+1 ; i++)
	{
	  xn = 2.0*(double)(Bi+i)/(Tobs*fstar);

          cf = 1.0-cos(xn);
          sf = sin(xn);

	  ChR[i] = fix*(tR[i]*cf-tI[i]*sf);
	  ChI[i] = fix*(tI[i]*cf+tR[i]*sf);

        }

	free_dvector(dR, 0, 2*fdop);
	free_dvector(dI, 0, 2*fdop); 
	free_dvector(sR, 0, 2*fdop);
	free_dvector(sI, 0, 2*fdop);
	free_dvector(tR, 0, 2*fb);
	free_dvector(tI, 0, 2*fb);

	return;
}


double Noise(double f)
{
	/* Power spectral density of the detector noise and transfer frequency*/
	double Sn, fstar, red;

	fstar = c/(2.0*pi*L);

        red = (2.0*pi*1.0e-4)*(2.0*pi*1.0e-4);

	/* Calculate the power spectral density of the detector noise at the given frequency */
	Sn = 2.0*(1.0-cos(2.0*f/fstar))*( 4.0*Sps + 8.0*(1.0+pow(cos(f/fstar),2))*Sacc*(1.0/pow(2.0*pi*f,4)+ red/pow(2.0*pi*f,6))) / pow(2.0*L,2);

	return Sn;
}

void get_SNR_logL(double *Parameters, double* SNR, double* dlogL, long Nrec, long Bi, double* Ch1R, double* Ch1I, double* Ch2R, double* Ch2I, double Tobs)
{
	/* Fourier coefficients for the total responses */
	double *T1R, *T2R, *T1I, *T2I;
	/* source parameters */
	double A, f, theta, phi, psi, iota, iphase;
	/* loop variables */
	long i, j, k;
	double power, Sn, Bw;
        double t1, t2, t3, t4;
	long q, fb, Nfc, BTi, BTf;
        FILE *signal, *template;

	        f = Parameters[1];

		/* Calculate the template bandwidth */
		Bw = 2.0*(5.0 + 2.0*pi*f*AU/c)*fm;

		/* Calculate the number of bins in the half-bandwidth */
		fb = (long)(floor(Bw*Tobs));

		/* Calculate the total number of Fourier coefficients in the template */
		Nfc = (2*fb+1);

		/* Calculate the frequency bin that contains the carrier frequency */
		q = (long)(rint(f*Tobs));

		/* Caculate the initial and final bin numbers */
		BTi = q - fb;
		BTf = q + fb;

		/* Dynamically allocate memory for the various Fourier coefficients */
		T1R = dvector(0,Nfc-1);
                T2R = dvector(0,Nfc-1);
                T1I = dvector(0,Nfc-1);
                T2I = dvector(0,Nfc-1);

		/* Generate the templates for the A and E channels */

		LowFrequency(Tobs, Parameters, fb, T1R, T1I, 1);
		LowFrequency(Tobs, Parameters, fb, T2R, T2I, 2);

                t1 = 0.0;
                t2 = 0.0;
                t3 = 0.0;
                t4 = 0.0;

                /* signal = fopen("signal.dat","w");
		   template = fopen("template.dat","w"); */

		for (i = 0; i < Nfc; i++)
		  {
                    f = ((double)(i+BTi)+0.5)/Tobs;
                    Sn = Noise(f);
                    j = BTi-Bi+i;
                    if((j>=0) && (j<Nrec))
		      {
                        /* fprintf(signal,"%e %e %e %e %e\n", f, Ch1R[j], Ch1I[j], Ch2R[j], Ch2I[j]);
			   fprintf(template,"%e %e %e %e %e\n", f, T1R[i], T1I[i], T2R[i], T2I[i]); */
			t1 += 4.0*((Ch1R[j]*T1R[i]+Ch1I[j]*T1I[i]) + (Ch2R[j]*T2R[i]+Ch2I[j]*T2I[i]))/Sn;
                        t2 += 4.0*((T1R[i]*T1R[i]+T1I[i]*T1I[i]) + (T2R[i]*T2R[i]+T2I[i]*T2I[i]))/Sn;
			t3 += 4.0*((Ch1R[j]*Ch1R[j]+Ch1I[j]*Ch1I[j]) + (Ch2R[j]*Ch2R[j]+Ch2I[j]*Ch2I[j]))/Sn;
			t4 += 4.0*((Ch1R[j]-T1R[i])*(Ch1R[j]-T1R[i])+(Ch1I[j]-T1I[i])*(Ch1I[j]-T1I[i])
				   +(Ch2R[j]-T2R[i])*(Ch2R[j]-T2R[i])+(Ch2I[j]-T2I[i])*(Ch2I[j]-T2I[i]))/Sn;
                      }

		  }


	*SNR = t1/sqrt(t2);
        *dlogL = (t3-t4)/2.0;

	/* fclose(signal);
	   fclose(template); */

		free_dvector(T1R,0,Nfc-1);
		free_dvector(T2R,0,Nfc-1);
                free_dvector(T1I,0,Nfc-1);
                free_dvector(T2I,0,Nfc-1);

	return;
}


/* ************************************************************************************ */
/* ********************************* Fourier Routines ******************************* */
/* ************************************************************************************ */


void dfour1(double data[], unsigned long nn, int isign)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi, swap;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP2(data[j],data[i]);
			SWAP2(data[j+1],data[i+1]);
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



#undef SWAP2
#undef GET_PSUM
#undef NMAX
#undef NRANSI

