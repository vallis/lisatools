/****************************************************************************/
/*                                                                          */
/* TITLE: Sample code for reading in LISA XML data sets                     */
/* AUTHOR: Neil J. Cornish                                                  */
/* DATE: June 22, 2006                                                      */
/* VERSION: 2.1.1                                                           */
/*                                                                          */
/* gcc -O2 -o DataImport DataImport.c ../io-C/lisaxml.c ../io-C/ezxml.c     */
/*  ../io-C/xmlbinary.c ../io-C/readxml.c -lm                               */
/*                                                                          */
/****************************************************************************/
  

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "readxml.h"
#include "xmlbinary.h"

#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

void dfour1(double data[], unsigned long nn, int isign);
void drealft(double data[], unsigned long n, int isign);
void KILL(char*);

int main(int argc,char **argv) {

  /* Error message character array */
   char ErrorMessage[100];
   double *ATDI, *ETDI;
   double *XTDI, *YTDI, *ZTDI;
   double fix, avg;
   double f, T, dt;
   double fmin, fmax;
   long NFFT;
  /* Filename character array */
   char Filename[50];
   FILE *infile;
   FILE *outfile;

   TimeSeries *timeseries;
    
   long i, j, jj, imin, imax, k;

  if (argc < 4) 
    KILL("Usage: ./DataImport file.xml fmin fmax\n");

    timeseries = getTDIdata(argv[1]);

    NFFT = timeseries->Length;

    dt = timeseries->Cadence;

    T = (double)(NFFT)*dt;

  /* convert Numerical Recipes FFT output into a strain spectral density */

  fix = sqrt(T)/((double)NFFT);

  fmin = atof(argv[2]);
  fmax = atof(argv[3]);

  if(fmin < 1.0e-6)
    {
      fmin = 1.0e-6;
      printf("fmin reset to 1.0e-6 Hz\n");
    }

  if(fmax > 1.0/(2.0*dt))
    {
      fmax = 1.0/(2.0*dt);
      printf("fmax reset to %e Hz\n",  1.0/(2.0*dt));
    }


  imin = (long)floor(fmin*T);
  imax = (long)floor(fmax*T);

  printf("%d %d\n", imin, imax);


 /* Dynamically allocate the X, Y and Z signal arrays */

  XTDI = (double*) malloc ((int)NFFT*sizeof(double));
  YTDI = (double*) malloc ((int)NFFT*sizeof(double));
  ZTDI = (double*) malloc ((int)NFFT*sizeof(double));

  if (!XTDI || !YTDI || !ZTDI) KILL("Out of memory.\n");


    printf("'%s': %d x %d doubles (every %g seconds)\n ",timeseries->Name,
                                                        timeseries->Records,
                                                        timeseries->Length,
                                                        timeseries->Cadence);

    printf("data in '%s'\n",timeseries->FileName);



    for(i=0; i<NFFT; i++)
      {

	XTDI[i] = timeseries->Data[1]->data[i];
	YTDI[i] = timeseries->Data[2]->data[i];
	ZTDI[i] = timeseries->Data[3]->data[i];

      }


    freeTimeSeries(timeseries);


 /* Dynamically allocate the A and E signal arrays */

  ATDI = (double*) malloc ((int)NFFT*sizeof(double));
  ETDI = (double*) malloc ((int)NFFT*sizeof(double));

  if (!ATDI || !ETDI) KILL("Out of memory.\n");

    for(i=0; i<NFFT; i++)
      {
	ATDI[i] = (2.0*XTDI[i] - YTDI[i] - ZTDI[i])/3.0;
        ETDI[i] = (ZTDI[i] - YTDI[i])/sqrt(3.0);
      }


  printf("Fourier Transforming the X, Y and Z channels\n");

  /* Calculate FFTs. */
   drealft(XTDI-1, NFFT, 1);
   drealft(YTDI-1, NFFT, 1);
   drealft(ZTDI-1, NFFT, 1);


  printf("Fourier Transforming the A and E channels\n");

  /* Calculate FFTs. */
   drealft(ATDI-1, NFFT, 1);
   drealft(ETDI-1, NFFT, 1);

 /* convert Numerical Recipes FFT output into a strain spectral density */

  for(i = 0; i < NFFT; i++)
    {
     XTDI[i] *= fix;
     YTDI[i] *= fix;
     ZTDI[i] *= fix;
     ATDI[i] *= fix;
     ETDI[i] *= fix;
    }

  printf("Saving the one-sided X strain spectal density to file\n");

  outfile = fopen("Data/X_FFT.dat", "w");

  for(i=imin; i < imax; i++)
   {
     j = 2*i;
    fprintf(outfile,"%.10e %e %e %e\n", log10(((double)i)/T), XTDI[j], XTDI[j+1], 0.5*log10(2.0*(XTDI[j]*XTDI[j]+XTDI[j+1]*XTDI[j+1])));
   }

  fclose(outfile);


  printf("Saving the one-sided Y strain spectal density to file\n");

  outfile = fopen("Data/Y_FFT.dat", "w");

  for(i=imin; i < imax; i++)
   {
     j = 2*i;
    fprintf(outfile,"%.10e %e %e %e\n", log10(((double)i)/T), YTDI[j], YTDI[j+1], 0.5*log10(2.0*(YTDI[j]*YTDI[j]+YTDI[j+1]*YTDI[j+1])));
   }

  fclose(outfile);

  printf("Saving the one-sided Z strain spectal density to file\n");

  outfile = fopen("Data/Z_FFT.dat", "w");

  for(i=imin; i < imax; i++)
   {
     j = 2*i;
    fprintf(outfile,"%.10e %e %e %e\n", log10(((double)i)/T), ZTDI[j], ZTDI[j+1], 0.5*log10(2.0*(ZTDI[j]*ZTDI[j]+ZTDI[j+1]*ZTDI[j+1])));
   }

  fclose(outfile);


  printf("Saving the one-sided A strain spectal density to file\n");

  outfile = fopen("Data/A_FFT.dat", "w");

  for(i=imin; i < imax; i++)
   {
     j = 2*i;
    fprintf(outfile,"%e %e\n", log10(((double)i)/T), 0.5*log10(2.0*(ATDI[j]*ATDI[j]+ATDI[j+1]*ATDI[j+1])));
   }

  fclose(outfile);


  printf("Saving the one-sided E strain spectal density to file\n");

  outfile = fopen("Data/E_FFT.dat", "w");

  for(i=imin; i < imax; i++)
   {
     j = 2*i;
    fprintf(outfile,"%e %e\n", log10(((double)i)/T), 0.5*log10(2.0*(ETDI[j]*ETDI[j]+ETDI[j+1]*ETDI[j+1])));
   }

  fclose(outfile);


  return 0;

}



void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n");
  exit(1);


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
