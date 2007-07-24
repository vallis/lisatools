

/****************************  NEEDED LIBRARIES  ****************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_rng.h>

#include "LISAconstants.h"
#include <stdio.h>
#include <stddef.h>
#include "nrutil2.h"
#include "NoiseParameters.h"
#include "extras.h"



/*************  PROTOTYPE DECLARATIONS FOR EXTERNAL FUNCTIONS  **************/

void KILL(char*);
void indexx(unsigned long n, double *arr, unsigned long *indx);

int main(int argc,char **argv)
{

  long i, j, Nf;
  long rSeed;
  double fix, Mfac, Afac;
  double logf, f, Mc, l, b, DL, theta, phi, iota, psi, phase, A;
  double m1, m2, P, SNR1, SNR2;
  double x, y ,z, r;
  double x_ec, y_ec, z_ec;
  double fonfs, Sn, conf, BS;
  double fNy;
  double *ff, *AA, *th, *ph, *inc, *iph, *ps;
  unsigned long *indx;
  long cnt, cntb;
  int vflag;
  char ErrorMessage[200];
  char Gfile[100];
  FILE* Infile;
  FILE* Output;
  FILE* Output2;

  if(argc !=3) KILL("Galaxy_Maker rSeed vflag\n");

  rSeed = atoi(argv[1]);

/*  if(rSeed < 0) rSeed *= -1; looks like it works with both positive
     and negative seeds */

  const gsl_rng_type * Tor;
  gsl_rng * rnd;
  gsl_rng_env_setup();
  Tor = gsl_rng_default;
  rnd = gsl_rng_alloc(Tor);
  gsl_rng_set(rnd, rSeed);

/*  double u = gsl_rng_uniform (rnd); generating random uniform number 0-1 
  printf ("Stas %.5f\n", gsl_rng_uniform (rnd)); */
  

  sprintf(Gfile, "Data/Galaxy_%s.dat", argv[1]);

  Output = fopen(Gfile,"w");

  sprintf(Gfile, "Data/Galaxy_Bright_%s.dat", argv[1]);

  Output2 = fopen(Gfile,"w");

  vflag = atoi(argv[2]);

  fix = pi/180.0;

  Mfac = Msun*G/(c*c*c);

  Afac =  2.0*c/kpc;

  fNy = 1.0/(2.0*dt);

  cnt = 0;
  cntb = 0;

  /* if vflag = 1 we include the verification binaries */

  if(vflag == 1)
    {
      printf("Verification Binaries are Included\n");
      if ((Infile = fopen("Data/Verification.dat","r")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the verification file\n");
        KILL(ErrorMessage);
       }

    while ( !feof(Infile) )
     {
       fscanf(Infile,"%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf\n", &f, &P, &DL, &m1, &m2, &iota, &psi, &phase, &theta, &phi);
       cntb++;
       cnt++;
       DL /= 1000.0;
       theta *= fix;
       phi *= fix;
       Mc = pow((m1*m2),3.0/5.0)/pow((m1+m2),1.0/5.0);

       /* Calculate the instrinsic amplitude */
       A = Afac*pow(pi*f,(2.0/3.0))/DL * pow(Mc*Mfac, 5.0/3.0);
       x = 1.0-2.0*gsl_rng_uniform(rnd);
       iota = acos(x);
       psi = pi*gsl_rng_uniform(rnd);
       phase = 2.0*pi*gsl_rng_uniform(rnd);
       fprintf(Output, "%.16f %f %f %e %f %f %f\n", f, theta, phi, A, iota, psi, phase);
       fprintf(Output2, "%.16f %f %f %e %f %f %f\n", f, theta, phi, A, iota, psi, phase);
     }

      fclose(Infile);
    }

      if ((Infile = fopen("Data/dwd_GWR_all_pars.dat","r")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the galaxy file\n");
        KILL(ErrorMessage);
       }


    while ( !feof(Infile) )
     {
       fscanf(Infile, "%lf%lf%lf%lf%lf\n", &logf, &Mc, &l, &b, &DL);

       /* random +/- 100/year tweak to the frequencies */

       f = pow(10.0,logf)+100.0*(1.0-2.0*gsl_rng_uniform(rnd))/year;

      conf = 0.0;

      if(f <= pow(10.0,-3.0)) conf = pow(10.0,-44.62)*pow(f,-2.3);
      if((f > pow(10.0,-3.0)) && (f < pow(10.0,-2.7))) conf = pow(10.0,-50.92)*pow(f,-4.4);
      if((f > pow(10.0,-2.7)) && (f < pow(10.0,-2.4))) conf = pow(10.0,-62.8)*pow(f,-8.8);
      if((f > pow(10.0,-2.4)) && (f < pow(10.0,-2.0))) conf = pow(10.0,-89.68)*pow(f,-20.0);
      /* Nelemans, Yungelson, Zwart confusion noise estimate */

       fonfs = f/fstar;

       Sn = conf+(4.0*Ss21+2.0*Ss21*fonfs*fonfs+8.0*Sa21*(1.0+pow((1.0e-4/f),2.0) )*pow(2.0*pi*f,-4.0))/(4.0 * L*L);


       /* Calculate the instrinsic amplitude */
       A = Afac*pow(pi*f,(2.0/3.0))/DL * pow(Mc*Mfac, 5.0/3.0);

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

       BS = A*sqrt(T)/(sqrt(Sn));

       if(f < fNy)
        {
	 cnt++;
         fprintf(Output, "%.16f %f %f %e %f %f %f\n", f, theta, phi, A, iota, psi, phase);
        }

       if((BS > 2.0) && (f < fNy))
	 {
	   cntb++;
           fprintf(Output2, "%.16f %f %f %e %f %f %f\n", f, theta, phi, A, iota, psi, phase);
         }

     }

    printf("%ld sources were identified as bright\n", cntb);
    printf("the galaxy contains %ld sources\n", cnt);

   fclose(Infile);
   fclose(Output);
   fclose(Output2);

   sprintf(Gfile, "Data/count_%s.dat", argv[1]);
   Output = fopen(Gfile,"w");
   fprintf(Output,"%ld %ld\n", cnt, cntb);
   fclose(Output);


   /* next we sort the bright file by frequency */

   sprintf(Gfile, "Data/Galaxy_Bright_%s.dat", argv[1]);

   Infile = fopen(Gfile,"r");

   ff = dvector(1,cntb);
   AA = dvector(1,cntb);
   th = dvector(1,cntb);
   ph = dvector(1,cntb);
   inc = dvector(1,cntb);
   iph = dvector(1,cntb);
   ps = dvector(1,cntb);

   for(i=1; i<=cntb; i++)
    {
     fscanf(Infile, "%lg%lg%lg%lg%lg%lg%lg\n", &ff[i], &th[i], &ph[i], &AA[i], &inc[i], &ps[i], &iph[i]);
    }

   fclose(Infile);

   indx = lvector(1,cntb);

   indexx(cntb, ff, indx);

   Output = fopen(Gfile,"w");

   for(i=1; i<=cntb; i++)
    {
     fprintf(Output, "%.16f %f %f %e %f %f %f\n", ff[indx[i]], th[indx[i]], ph[indx[i]], AA[indx[i]],
             inc[indx[i]], ps[indx[i]], iph[indx[i]]);
    }

   fclose(Output);
   
   gsl_rng_free(rnd);

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
