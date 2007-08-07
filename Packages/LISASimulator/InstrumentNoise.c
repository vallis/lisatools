/****************************************************************************/
/*                                                                          */
/* TITLE: LISA's Instrument Noise                                           */
/* AUTHORS: Louis J. Rubbo, Neil J. Cornish, and Olivier Poujade            */
/* DATE: June 23, 2006                                                      */
/* VERSION: 2.1.1                                                           */
/*                                                                          */
/*                                                                          */
/* ABSTRACT: This program produces a series of data streams each of which   */
/* contains a realization of the instrument noise from a particular         */
/* detector.                                                                */
/*                                                                          */
/*                                                                          */
/* COMPILE: gcc -o InstrumentNoise InstrumentNoise.c ran2.c gasdev.c -lm    */
/* NOTE: current version uses GSL random number generation; see Compile     */
/****************************************************************************/


/*************************  REQUIRED HEADER FILES  **************************/

#include <stdio.h>
#include <math.h>
#include "LISAconstants.h"
#include "NoiseParameters.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

/*************  PROTOTYPE DECLARATIONS FOR EXTRENAL FUNCTIONS  **************/

/* double ran2(long*);
   double gasdev(long*); */



/*************  PROTOTYPE DECLARATIONS FOR INTERNAL FUNCTIONS  **************/

void ShotNoise(double*, long, gsl_rng * rnd);
void AccNoise(double*, long, gsl_rng * rnd);
void AccStream(double*, double, long, gsl_rng * rnd);
void FileWrite(double*, char*);
void HighPassRecFilter(double*, long);
void KILL(char*);

#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

void dfour1(double data[], unsigned long nn, int isign);
void drealft(double data[], unsigned long n, int isign);



/* ============================  MAIN PROGRAM  ============================ */

int main(int argc,char **argv)
{

          /* -------------  DECLARATION OF VARIABLES  ------------- */

 /* Number of data poins in the observation run with extra padding */
  long Ntot;

 /* Noise array */
  double* n_ij;

 /* Random number seed */
  long rSeed;

 /* Loop index */
  long a;

 /* rSeed recorded for file naming */
  FILE* filename;

          /* -------------------  CALCULATIONS  ------------------- */

  printf("\n\n");
  printf("  ______________________________________________________  \n");
  printf(" |                                                      | \n");
  printf(" |                LISA's Instrument Noise               | \n");
  printf(" |______________________________________________________| \n\n");


 /* Set the seed value for the random number generator (note that the seed 
    needs to be a negative integer number) */

  if(argc !=2) KILL("InstrumentNoise rSeed\n");

  rSeed = atoi(argv[1]);

  if(rSeed > 0) rSeed *= -1;

  const gsl_rng_type * Tor;
  gsl_rng * rnd;
  gsl_rng_env_setup();
  Tor = gsl_rng_default;
  rnd = gsl_rng_alloc(Tor);
  gsl_rng_set(rnd, rSeed);

/* a normal random number with mean zero and stdev 1 is obtained with
   gsl_ran_gaussian(rnd,1.0) */

  filename = fopen("Data/seed.txt","w");

  fprintf(filename,"%d\n",rSeed);

  fclose(filename);


 /* Calculate the number of data points needed */

  Ntot = (int)NFFT + 2*(int)ceil(Tpad/dt);


 /* Dynamically allocate memory for the noise array */

  n_ij = (double*) malloc (Ntot*sizeof(double));

  if (!n_ij) KILL("Out of memory!\n");


 /* Shot noise */

  printf("Calculating the shot noise on ...\n");

  ShotNoise(n_ij, Ntot, rnd);


 /* Acceleration Noise */

  printf("\nCalculating the acceleration noise on ...\n");

  AccNoise(n_ij, Ntot, rnd);


 /* Free the allocated memory */

  free(n_ij);

  printf("\n----------------------------------------------------------\n");
  printf("The program is now complete.\n\n\n");

  gsl_rng_free(rnd);

  return 0;
}

/* ======================================================================== */





/***************************  INTERNAL FUNCTIONS  ***************************/

void ShotNoise(double* n_ij, long Ntot, gsl_rng * rnd)
{

          /* -------------  DECLARATION OF VARIABLES  ------------- */

 /* Loop index */
  long a;

 /* Filename character array */
  char* Filename;

  /* padded noise array */
  double *SF;

  /* scaling factor */
  double fac;

  /* padded file length */
  long NP;

  /* low f cut-off */
  long amin;

  NP = (long)pow(2.0,floor(log((double)Ntot)/log(2.0))+1.0);

  SF = (double*) malloc (NP*sizeof(double));

  /* kill everything below 1e-6 */
  amin = (long)((1.0e-5*dt)*(double)(2*NP));


          /* -------------------  CALCULATIONS  ------------------- */

 /* Spacecraft #1 */

  printf("   spacecraft 1\n");

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Ss21)/dt;
  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;
  for (a = amin ; a < NP-1 ; a++) SF[a] = fac*gsl_ran_gaussian(rnd,1.0);
  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];
  Filename = "Binary/ShotNoise21";
  FileWrite(n_ij, Filename);

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Ss31)/dt;
  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;
  for (a = amin ; a < NP-1 ; a++) SF[a] = fac*gsl_ran_gaussian(rnd,1.0);
  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];
  Filename = "Binary/ShotNoise31";
  FileWrite(n_ij, Filename);


 /* Spacecraft #2 */

  printf("   spacecraft 2\n");

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Ss12)/dt;
  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;
  for (a = amin ; a < NP-1 ; a++) SF[a] = fac*gsl_ran_gaussian(rnd,1.0);
  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];
  Filename = "Binary/ShotNoise12";
  FileWrite(n_ij, Filename);

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Ss32)/dt;
  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;
  for (a = amin ; a < NP-1 ; a++) SF[a] = fac*gsl_ran_gaussian(rnd,1.0);
  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];
  Filename = "Binary/ShotNoise32";
  FileWrite(n_ij, Filename);


 /* Spacecraft #3 */

  printf("   spacecraft 3\n");

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Ss13)/dt;
  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;
  for (a = amin ; a < NP-1 ; a++) SF[a] = fac*gsl_ran_gaussian(rnd,1.0);
  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];
  Filename = "Binary/ShotNoise13";
  FileWrite(n_ij, Filename);

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Ss23)/dt;
  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;
  for (a = amin ; a < NP-1 ; a++) SF[a] = fac*gsl_ran_gaussian(rnd,1.0);
  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];
  Filename = "Binary/ShotNoise23";
  FileWrite(n_ij, Filename);

  free(SF);


  return;
}





void AccNoise(double* n_ij, long Ntot, gsl_rng * rnd)
{

          /* -------------  DECLARATION OF VARIABLES  ------------- */
          
 /* Filename character array */
  char* Filename;

          /* -------------------  CALCULATIONS  ------------------- */

 /* Spacecraft #1 */

  printf("   spacecraft 1\n");

  AccStream(n_ij, Sa21, Ntot, rnd);
  Filename = "Binary/AccNoise21";
  FileWrite(n_ij, Filename);

  AccStream(n_ij, Sa31, Ntot, rnd);
  Filename = "Binary/AccNoise31";
  FileWrite(n_ij, Filename);


 /* Spacecraft #2 */

  printf("   spacecraft 2\n");

  AccStream(n_ij, Sa12, Ntot, rnd);
  Filename = "Binary/AccNoise12";
  FileWrite(n_ij, Filename);

  AccStream(n_ij, Sa32, Ntot, rnd);
  Filename = "Binary/AccNoise32";
  FileWrite(n_ij, Filename);


 /* Spacecraft #3 */

  printf("   spacecraft 3\n");

  AccStream(n_ij, Sa13, Ntot, rnd);
  Filename = "Binary/AccNoise13";
  FileWrite(n_ij, Filename);

  AccStream(n_ij, Sa23, Ntot, rnd);
  Filename = "Binary/AccNoise23";
  FileWrite(n_ij, Filename);


  return;
}





void AccStream(double* n_ij, double Sn, long Ntot, gsl_rng * rnd)
{

          /* -------------  DECLARATION OF VARIABLES  ------------- */

 /* Loop index */
  long a;

  /* relative scaling */
  double scale;

  /* padded noise array */
  double *SF;

  double f;

  /* scaling factor */
  double fac;

  /* padded file length */
  long NP;

  /* low f cut-off */
  long amin;

  NP = (long)pow(2.0,floor(log((double)Ntot)/log(2.0))+1.0);

  SF = (double*)malloc(NP*sizeof(double));

  /* kill everything below 1e-5 */
  amin = (long)((1.0e-5*dt)*(double)(2*NP));

          /* -------------------  CALCULATIONS  ------------------- */

  fac = (1.0/(double)(NP))*sqrt(2.0*T*Sn)/dt;

  for (a = 0 ; a < amin ; a++) SF[a] = 0.0;

  for (a = amin ; a < NP-1 ; a++)
   {
     f = (double)(a)/(dt*(double)(2*NP));
     SF[a] = fac*pow(2.0*pi*f,-2.0)*gsl_ran_gaussian(rnd,1.0)+2.0*pi*1.0e-4*fac*pow(2.0*pi*f,-3.0)*gsl_ran_gaussian(rnd,1.0);
   }

  SF[NP-1] = 0.0;
  drealft(SF-1, NP, -1);
  for (a = 0 ; a < Ntot ; a++) n_ij[a] = SF[a];

  free(SF);

  return;
}


void FileWrite(double* n_ij, char* intFilename)
{

          /* -------------  DECLARATION OF VARIABLES  ------------- */

 /* Number of data points in a section with padding */
  long N;

 /* Initial bin number within the n_ij array to start for each section */
  long Nint;

 /* Loop indexes */
  long a, b;

 /* Filename character array */
  char Filename[50];

 /* Error message character array */
  char ErrorMessage[100];

 /* File pointer */
  FILE* NoiseOut;


          /* -------------------  CALCULATIONS  ------------------- */

 /* Calculate the section width */

  N = (int)ceil(NFFT/Ndiv) + 2*(int)ceil(Tpad/dt);


 /* Save each section to an individual file */

  for(a = 1 ; a <= Ndiv ; a++)
  {
    sprintf(Filename, "%s_%i.dat", intFilename, a);

    if ((NoiseOut = fopen(Filename, "wb")) == NULL)
    {
      sprintf(ErrorMessage, "Cannot open the file %s.\n", Filename);
      KILL(ErrorMessage);
    }

    Nint = (a-1)*(int)ceil(NFFT/Ndiv);

    for (b = Nint ; b < Nint+N ; b++) 
      fwrite(&n_ij[b], sizeof(double), 1, NoiseOut);

    fclose(NoiseOut);
  }


  return;
}





void HighPassRecFilter(double* n_ij, long Ntot)
{

          /* -------------  DECLARATION OF VARIABLES  ------------- */

 /* Low frequency cutoff */
  double f_low;

 /* Temporary summation variable */
  double sum;

 /* Filtered signal */
  double* Sig;

 /* Bin number that conatins the period of the low frequency cutoff */
  long m;

 /* Half value of m */
  long mt;

 /* Loop indexes */
  long a, b;


          /* -------------------  CALCULATIONS  ------------------- */

 /* Set the value of the low frequency cutoff (Hertz) */

  f_low = 1.0e-6;


 /* Dynamically allocate the memory for the filtered signal array */

  Sig = (double*) malloc (Ntot*sizeof(double));


 /* Calculate bin numbers associated with the cutoff period */

  m = (int)(1.0/(dt*f_low));

  mt = m/2;

   sum = n_ij[0];
   Sig[0] = sum;

  for (a = 1 ; a <= mt ; a++)
  {

    for (b = 2*a-1 ; b < 2*a+1 ; b++) sum += n_ij[b];

    Sig[a] = sum/((double)(2*a+1));

  }

  for (a = mt+1 ; a < Ntot-mt ; a++)
  {
    sum += n_ij[a+mt] - n_ij[a-mt-1];

    Sig[a] = sum/((double)(2*mt+1));
  }

  
   sum = n_ij[Ntot-1];

   Sig[Ntot-1] = sum;

  for (a = Ntot-2 ; a >= Ntot-mt ; a--)
  {

    for (b = 2*(Ntot-1-a)-1 ; b < 2*(Ntot-1-a)+1 ; b++) sum += n_ij[Ntot-1-b];

    Sig[a] = sum/((double)(2*(Ntot-1-a)+1));

  }


 /* Apply the high pass filter to the time data series */

  for (a = 0 ; a < Ntot ; a++) n_ij[a] -= Sig[a];


 /* Free the allocated memory */

  free(Sig);


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



void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n");
  exit(1);

 
  return;
}
