

/****************************  NEEDED LIBRARIES  ****************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include "Constants.h"
#include "Detector.h"
#include <stdio.h>
#include <stddef.h>
#include "arrays.h"


/*************  PROTOTYPE DECLARATIONS FOR EXTERNAL FUNCTIONS  **************/

void KILL(char*);
double AEnoise(double f);
double SNR_Check(double f, double fdot, double theta, double phi, double A, double iota, double psi, double phase);

int main(int argc,char **argv)
{

  long i, j, Nf;
  long rSeed;
  double fix, Mfac, Afac;
  double logf, f, Mc, l, b, DL, theta, phi, iota, psi, phase, A;
  double Porb, Porb_dot, m2_dot, fdot;
  double m1, m2, P, SNR, SNR1, SNR2;
  double x, y ,z, r;
  double x_ec, y_ec, z_ec;
  double fonfs, Sn, Acut, Sm;
  double fNy;
  unsigned long *indx;
  long cnt, cntb;
  long cnt2, cntb2;
  int vflag;
  char ErrorMessage[200];
  char Gfile[100];

  FILE* Infile;
  FILE* Output;
  FILE* Output2;

  if(argc !=2) KILL("Confusion_Maker3 rSeed\n");

  rSeed = atoi(argv[1]);

  const gsl_rng_type * Tor;
  gsl_rng * rnd;
  gsl_rng_env_setup();
  Tor = gsl_rng_default;
  rnd = gsl_rng_alloc(Tor);
  gsl_rng_set(rnd, rSeed);

  sprintf(Gfile, "Data/Galaxy_%s.dat", argv[1]);

  Output = fopen(Gfile,"w");

  sprintf(Gfile, "Data/Galaxy_Bright_%s.dat", argv[1]);

  Output2 = fopen(Gfile,"w");

  fix = pi/180.0;

  Mfac = Msun*G/(clight*clight*clight);

  Afac =  2.0*clight/kpc;

  fNy = 1.0/(2.0*dt);

  cnt = 0;
  cntb = 0;
  cnt2 = 0;
  cntb2 = 0;

      if ((Infile = fopen("Data/dwd_GWR_MLDC.dat","r")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the detached binaries file\n");
        KILL(ErrorMessage);
       }


    while ( !feof(Infile) )
     {
       fscanf(Infile, "%lf%lf%lf%lf%lf%lf%lf%lf\n", &m1, &m2, &Porb, &Porb_dot, &m2_dot, &l, &b, &DL);

       f = 2.0/Porb;

       f += 0.01*(1.0-2.0*gsl_rng_uniform(rnd))*f;       /* random +/- 1% tweak to the frequencies */

       fdot = -0.5*Porb_dot*f*f;

       Mc = pow((m1*m2),0.6)/pow((m1+m2), 0.2)*Mfac;

       /* Calculate the initial amplitude */
       A = Afac*pow(pi*f,(2.0/3.0))/DL * pow(Mc, 5.0/3.0);

       /* rotate from galactic to ecliptic coordinates */
       l *= fix;
       if(l < 0.0) l += 2.0*pi;
       b  = 90.0 - b;
       b *= fix;

       x = sin(b)*cos(l);

       y = sin(b)*sin(l);

       z = cos(b);

       /* rotate to ecliptic coordinates */
       x_ec = -0.05487556043*x + 0.4941094278*y - 0.8676661492*z;

       y_ec = -0.99382137890*x - 0.1109907351*y - 0.00035159077*z;

       z_ec = -0.09647662818*x + 0.8622858751*y + 0.4971471918*z;

       r = sqrt(x_ec*x_ec+y_ec*y_ec+z_ec*z_ec);

       theta = pi/2.0-acos(z_ec/r);

       phi = atan2(y_ec,x_ec);

       if(phi<0.0) phi += 2.0*pi;

       x = 1.0-2.0*gsl_rng_uniform(rnd);

       iota = acos(x);
  
       psi = pi*gsl_rng_uniform(rnd);

       phase = 2.0*pi*gsl_rng_uniform(rnd);

       Sn = AEnoise(f);

       /*  calculate michelson noise  */
       Sm = Sn/(4.0*sin(f/fstar)*sin(f/fstar));

       Acut = A*sqrt(T/Sm);

       SNR = 1.0;

       /* need to explicitly check SNRs of the brightest sources 
          Acut estimate of the SNR is only good to within a factor or 2 */
       if((Acut > 2.0) && (f < fNy)) {
	       SNR = SNR_Check(f, fdot, theta, phi, A, iota, psi, phase);
           // printf("%f %f %f\n", Acut, SNR, SNR/Acut);
       }

       if((f < fNy) && (SNR <= 5.0)) {
	       cnt++;
           fprintf(Output, "%.16f %.10e %f %f %e %f %f %f\n", f, fdot, theta, phi, A, iota, psi, phase);
       }

       if(SNR > 5.0) cntb++;

     }

    printf("%ld systems were identified as bright\n", cntb);
    printf("%ld systems contribute to the confusion noise\n", cnt);

   fclose(Infile);

   fclose(Output);
   fclose(Output2);

   sprintf(Gfile, "Data/count_%s.dat", argv[1]);
   Output = fopen(Gfile,"w");
   fprintf(Output,"%ld %ld\n", cnt, cntb);
   fclose(Output);


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


