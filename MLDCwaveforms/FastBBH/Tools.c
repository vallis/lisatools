#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <fftw3.h>

#include "Declarations.h"
#include "Constants.h"
#include "numrec.h"

void SBH_Barycenter(SBH_structure SBH, double *hp, double *hc)
{
  double m1, m2, Mtot, mu, Mchirp, DL, tc, eta, beta, sigma, chi1, chi2, dm; 
  double costheta, sintheta, costhetaL0, sinthetaL0, gamma0;
  double costhetaS10, sinthetaS10, costhetaS20, sinthetaS20;
  double phi, phiL0, phiS10, phiS20, phic, Phase, psi;
  double LdotS1, LdotS2, LdotN, S1dotS2, fold;
  double Phi0, Phi10, Phi15, Phi20;
  double Lmag, S1mag, S2mag;
  double Jmag, JdotL;
  double f, t, td, dt;
  double Amp, p15, p20, p150, p200;
  double fac, etain;
  double costhetaL, sinthetaL, phiL, thomas;
  double tcurrent, tfinal, hcurrent;
  double cphil, sphil;
  double N[3], LSvals[10], updatedvals[10], fourthordervals[10];
  double OLSvals[10], OOLSvals[10];
  double derivvals[10];
  double LcrossN[3];
  double JcrossL[3];
  double Jtot[3];
  double cPhi[7], sPhi[7];
  double maxerr, oldh;
  double A, B, C, D, deltat;
  double Ax, ci, si, ci2, si2, ci4, si4, ci6, OPhase;
  double x, xrt, taper, xold, xmax;
  double tmax, VAmp, r;
  double shp, shc, c2psi, s2psi;
  double En, Enold;
  double tend = 0.0,taper1,taper2; /* tend stores final time where hp and hc are non-zero...*/
  double taperQFactor; /* Added by Stas -> needed for the half-hann taper */
  int valid = 0;

  int i, j, n, idxm;
  int iend;
  int index, laststep;
  
  double tTaper;

  /* Vectors for storing all the data.  We don't know how many points we need, so pick an arbitrary length.*/

  double *timevec;
  double *mulvec;
  double *sphilvec;
  double *cphilvec;
  double *betavec;
  double *sigmavec;
  double *thomasvec;
  double *freak;
  double *Energy;

  /* For the spline:*/

  double *muly2;
  double *sphily2;
  double *cphily2;
  double *betay2;
  double *sigmay2;
  double *thomasy2;
  double *freak2;
  double *Energy2;

  double *tdvals;
  double *interpmul;
  double *interpphil;
  double *interpbeta;
  double *interpsigma;
  double *interpthomas;
  double stas1, stas2;

  double x2, x3, x4, x5;
  double T_endpad, dtx, bd, sd, tx, bx, sx, fx, tstop, fstop;
  double t1, t2, f1, f2;

  FILE *Out;

  dt = SBH.TimeSample;
  n = (int)((SBH.Tobs + 2.0*SBH.Tpad)/SBH.TimeSample);
  VAmp = 2.0*SBH.AmplPNorder;
  xmax = 1.0/SBH.Rmin;

  T_endpad = 1.0e5;  /* this is added to make sure we integrate far enough to find merger. ok for signals
                        with merger frequencies greater than 1e-5. The barycenter waveform is still only
                        produced out to SBH.Tobs+SBH.Tpad */

  tdvals = dvector(0, n-1);
  interpmul = dvector(0, n-1);
  interpphil = dvector(0, n-1);
  interpbeta = dvector(0, n-1);
  interpsigma = dvector(0, n-1);
  interpthomas = dvector(0, n-1);

  timevec = dvector(0,VECLENGTH);
  mulvec = dvector(0,VECLENGTH);
  sphilvec = dvector(0,VECLENGTH);
  cphilvec = dvector(0,VECLENGTH);
  betavec = dvector(0,VECLENGTH);
  sigmavec = dvector(0,VECLENGTH);
  thomasvec = dvector(0,VECLENGTH);
  freak = dvector(0,VECLENGTH);
  Energy = dvector(0,VECLENGTH);

  /* For the spline:*/

  muly2 = dvector(0,VECLENGTH);
  sphily2 = dvector(0,VECLENGTH);
  cphily2 = dvector(0,VECLENGTH);
  betay2 = dvector(0,VECLENGTH);
  sigmay2 = dvector(0,VECLENGTH);
  thomasy2 = dvector(0,VECLENGTH);
  freak2 = dvector(0,VECLENGTH);
  Energy2 = dvector(0,VECLENGTH);

  /* Double Kerr Parameters: m1/Msun, m2/Msun, tc/sec, DL/Gpc, chi1, chi2, cos(theta), cos(thetaL(0)),
   cos(thetaS1(0)), cos(thetaS2(0)), phi, phiL(0), phiS1(0), phiS2(0), phic.  
   These are arranged into those with fixed ranges, and those without. Note: I am using redshifted masses. */

  m1 = SBH.Mass1;
  m2 = SBH.Mass2;
  tc = SBH.CoalescenceTime;
  DL = SBH.Distance/1.0e9;
  chi1 = SBH.Spin1;
  chi2 = SBH.Spin2;
  costheta = sin(SBH.EclipticLatitude);
  costhetaL0 = cos(SBH.InitialPolarAngleL);
  costhetaS10 = cos(SBH.PolarAngleOfSpin1);
  costhetaS20 = cos(SBH.PolarAngleOfSpin2);
  phi = SBH.EclipticLongitude;
  phiL0 = SBH.InitialAzimuthalAngleL;
  phiS10 = SBH.AzimuthalAngleOfSpin1;
  phiS20 = SBH.AzimuthalAngleOfSpin2;
  phic = SBH.PhaseAtCoalescence;
  taperQFactor = SBH.TaperQFactor;
  
 /* printf("Stas: The taper freq factor = %f\t%f\n", taperFreqFactor, SBH.TaperFreqFactor);*/
 
  /* printf("m1 = %.12e   m2 = %.12e    tc = %.12e   DL =  %.12e   x1 = %.12e    x2 = %.12e \n",	\
      m1, m2, tc, DL, chi1, chi2);
  printf("theta = %.12e   thetaL = %.12e  thS1 = %.12e  thS2  = %.12e \n", \
      PI - SBH.EclipticLatitude, SBH.InitialPolarAngleL, SBH.PolarAngleOfSpin1, SBH.PolarAngleOfSpin2);
  printf("phi = %.12e   phiL = %.12e  phiS1 = %.12e   phiS2 = %.12e  phic = %.12e \n", 
  phi, phiL0, phiS10, phiS20, phic); */

  /* Useful functions of these parameters:*/

  Mtot = m1 + m2;
  dm = (m1-m2)/Mtot;
  mu = m1*m2/Mtot;
  eta = mu/Mtot;
  etain = 1.0/eta;
  Mchirp = pow(mu, 0.6)*pow(Mtot, 0.4);

  Amp = pow(Mchirp*TSUN,5./3.)/(DL*GPC/CLIGHT);
  
 
  fac = eta/(5.0*Mtot*TSUN);

  Phi0 = -2.*pow(fac,0.625);
  Phi10 = -pow(fac,0.375)*(3715./4032.+55./48.*eta);
  p15 = 0.375*pow(fac,0.25);
  p150 = p15*(4.*PI);
  p20 = -pow(fac,0.125);
  p200 = p20*(9275495./7225344.+284875./129024.*eta+1855./1024.*eta*eta);

  /* Sky position:*/
  
  sintheta = sqrt(1.0 - costheta*costheta);
  N[0] = sintheta*cos(phi);
  N[1] = sintheta*sin(phi);
  N[2] = costheta;
  
  /* LSvals: 0-2 are L, 3-5 S1, and 6-8 S2 (all unit vectors)*/

  sinthetaL0 = sqrt(1.0-(costhetaL0*costhetaL0));
  LSvals[0] = sinthetaL0*cos(phiL0);
  LSvals[1] = sinthetaL0*sin(phiL0);
  LSvals[2] = costhetaL0;
 
  sinthetaS10 =  sqrt(1.0-(costhetaS10*costhetaS10));
  LSvals[3] = sinthetaS10*cos(phiS10);
  LSvals[4] = sinthetaS10*sin(phiS10);
  LSvals[5] = costhetaS10;

  sinthetaS20 =  sqrt(1.0-(costhetaS20*costhetaS20));
  LSvals[6] = sinthetaS20*cos(phiS20);
  LSvals[7] = sinthetaS20*sin(phiS20);
  LSvals[8] = costhetaS20;


  LSvals[9] = 0.0;  /* used to hold Thomas phase*/

  LdotS1 = calcLdotS1(LSvals);
  LdotS2 = calcLdotS2(LSvals);
  S1dotS2 = calcSdotS(LSvals);

  /* Initial beta and sigma:*/

  beta = (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
      
  sigma = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    

  /* Set up range of integration.  The extra Tpad=900 s is more than enough for the light travel time across the LISA orbit.*/

  tcurrent = -SBH.Tpad/TSUN;
  tmax = SBH.Tobs + SBH.Tpad + T_endpad;  /* will decrease if xmax lies within integration range */
  if(tc > tmax) tmax = tc+T_endpad;
  tfinal = tmax/TSUN;

  f = Freq(0.0, Mtot, Mchirp, eta, beta, sigma, tc);

  /* Initial orbital radius*/
  r = pow(Mtot,1./3.)/pow(PI*f*TSUN,2./3.);

  x = pow(PI*Mtot*f*TSUN,2./3.);

  Lmag = mu*Mtot*TSUN*TSUN/sqrt(x);
  S1mag = chi1*m1*m1*TSUN*TSUN;
  S2mag = chi2*m2*m2*TSUN*TSUN;

  Jtot[0] = Lmag*LSvals[0]+S1mag*LSvals[3] + S2mag*LSvals[6];
  Jtot[1] = Lmag*LSvals[1]+S1mag*LSvals[4] + S2mag*LSvals[7];
  Jtot[2] = Lmag*LSvals[2]+S1mag*LSvals[5] + S2mag*LSvals[8];

  Jmag = sqrt(calcLdotN(Jtot, Jtot));

  for(i = 0; i < 3; i++) Jtot[i] /= Jmag;

  JdotL = calcLdotN(Jtot, LSvals);
  calcLcrossN(JcrossL, Jtot, LSvals);

  /* printf("L %e S1 %e S2 %e J %e\n", Lmag, S1mag, S2mag, Jmag); */

  calcderivvals(derivvals, LSvals, r, m1, m2, Mtot, mu, chi1, chi2);
  LdotN = calcLdotN(LSvals, N);
  calcLcrossN(LcrossN, LSvals, N);
  derivvals[9] = -2.*LdotN/(1.-LdotN*LdotN)*(LcrossN[0]*derivvals[0]+LcrossN[1]*derivvals[1]+LcrossN[2]*derivvals[2]);

  x = calcLdotN(JcrossL, LcrossN);
  
  stas1 = x;
  stas2 = calcLdotN(Jtot,LcrossN);
  gamma0 = -atan2(stas2, stas1);
  /*printf("Stas: gamma =  %f \n", gamma0);
  
  printf("Nom %e \n", x); 

  x /= sqrt((1.0-JdotL*JdotL)*(1.0-LdotN*LdotN));
  
  printf("sin(i) = %e,  sin(kappa) =  %e \n", sqrt(1.0-JdotL*JdotL), sqrt(1.0-LdotN*LdotN));

  gamma0 = acos(x); */
  /*gamma0 = 0.0;   Stas Don't forget to change it back!!!! */

/*-acos( (Lnz*Ldotn + cos(theta))/(sin(iota)*sqrt(1.-Ldotn*Ldotn)) );*/

  /* printf("Orbital phase rotation to match Kidder frame = %f\n", gamma0); */

  for(i = 0; i <= 9; i++) LSvals[i] -= SBH.Tpad*derivvals[i];  /* back up the initial values to time -Tpad */

  index = 0;  /* index for the various storage vectors */
  hcurrent = RK_H;
  laststep = ENDNO;

  LdotS1 = calcLdotS1(LSvals);
  LdotS2 = calcLdotS2(LSvals);
  S1dotS2 = calcSdotS(LSvals);

  /* these hold the variables one and two steps back */
  for (i = 0; i <= 9; i++) OOLSvals[i] = LSvals[i];
  for (i = 0; i <= 9; i++) OLSvals[i] = LSvals[i];

  /*  beta and sigma at time -Tpad */

  beta = (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);     
  sigma = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;   

  timevec[index] = tcurrent*TSUN;
  mulvec[index] = LSvals[2];
  sphilvec[index] = LSvals[1];
  cphilvec[index] = LSvals[0]; 
  betavec[index] = beta;
  sigmavec[index] = sigma;
  thomasvec[index] = LSvals[9]; 

   f = Freq(timevec[index], Mtot, Mchirp, eta, betavec[index], sigmavec[index], tc);
   freak[index] = f;
   x = pow(Mtot*TSUN*PI*f,1.0/3.0);
   x2 = x*x; x3 = x2*x; x4 = x3*x; x5 = x4*x;
   Energy[index] = -0.5*mu*TSUN*x2*(1.0 - x2*(9.0+eta)/12.0 
     +x3*(8.0/(3.0*Mtot*Mtot))*(LdotS1*chi1*m1*(m1+0.75*m2)+LdotS2*chi2*m2*(m2+0.75*m1))
     -x4*(81.0-57.0*eta+eta*eta)/24.0
     +x4/eta*(chi1*chi2*m1*m1*m2*m2)/(Mtot*Mtot*Mtot*Mtot)*(S1dotS2-3.0*LdotS1*LdotS2));
  
  valid = 1;

  /* Start the integration: */
  
  while(tcurrent < tfinal)
    {	
      
   if (valid != 0) {  /* Check to make sure we're chirping and PN valid */
	
	 if (tcurrent+hcurrent-tfinal > 0.)
	   {
	     hcurrent = tfinal-tcurrent;
	     laststep = ENDYES;
	   }
	
         do
	  {	    
	    rkckstep(updatedvals, fourthordervals, hcurrent, LSvals, tcurrent, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc);
	    maxerr = 0.0;
	    for(i = 0; i <= 9; i++)
	      {
		double errori = fabs((updatedvals[i]-fourthordervals[i])/RK_EPS);
		maxerr = (errori > maxerr) ? errori : maxerr;
	      }
	    oldh = hcurrent;
	    
	    if (maxerr > 1.) 
	      {
		hcurrent *= 0.9*pow(maxerr,-0.25);
		laststep = ENDNO;
	      }
	    else if (maxerr <= 0.0006)
	      hcurrent *= 4.;
	    else
	      hcurrent *= 0.9*pow(maxerr,-0.2);
	  }
	while(maxerr > 1.);
	
	/* these hold the variables one and two steps back */
	for (i = 0; i <= 9; i++) OOLSvals[i] = OLSvals[i];
	for (i = 0; i <= 9; i++) OLSvals[i] = LSvals[i];

	for (i = 0; i <= 9; i++) LSvals[i] = updatedvals[i];
	
	if (laststep == ENDYES)
	  tcurrent = tfinal;
	else
	  tcurrent += oldh;

        index++;

	timevec[index] = tcurrent*TSUN;
	
	LdotS1 = calcLdotS1(LSvals);
	LdotS2 = calcLdotS2(LSvals);
	S1dotS2 = calcSdotS(LSvals);
      
	mulvec[index] = LSvals[2];
        sphilvec[index] = LSvals[1];
        cphilvec[index] = LSvals[0]; 
      
	betavec[index] =  (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) 
         + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
	
	sigmavec[index] = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    
	
	thomasvec[index] = LSvals[9];

         f = Freq(timevec[index], Mtot, Mchirp, eta, betavec[index], sigmavec[index], tc);
         freak[index] = f;
         x = pow(Mtot*TSUN*PI*f,1.0/3.0);
         x2 = x*x; x3 = x2*x; x4 = x3*x; x5 = x4*x;
         Energy[index] = -0.5*mu*TSUN*x2*(1.0 - x2*(9.0+eta)/12.0 
           +x3*(8.0/(3.0*Mtot*Mtot))*(LdotS1*chi1*m1*(m1+0.75*m2)+LdotS2*chi2*m2*(m2+0.75*m1))
	   -x4*(81.0-57.0*eta+eta*eta)/24.0
           +x4/eta*(chi1*chi2*m1*m1*m2*m2)/(Mtot*Mtot*Mtot*Mtot)*(S1dotS2-3.0*LdotS1*LdotS2));

	 // printf("%d %.12e %.12e %.12e\n", index, timevec[index], freak[index], Energy[index]); 

       
	 /* Stopping condition used prior to Challenge 4 */
	 if (taperQFactor < 1.0e-6)
         {
          if ((freak[index] >= freak[index-1]) && (x2 < xmax))
	    {
             valid = 1;
            }
          else
            {
             valid = 0;    
	    }
	  }
         else  /* The Challenge 4 stopping condition */
	  {
          if ((freak[index] >= freak[index-1]) && (Energy[index] <= Energy[index-1]))
	    {
             valid = 1;
            }
          else
            {
             valid = 0;    
             iend = index - 2;
	    }
	  }

	xold = x2; /* When the integration is done, will contain largest x reached... */
       
        }
      else {

        tcurrent = tfinal;
   	tmax = timevec[index];

	/*	for(i = 1; i < 10; i++) 
	 {
	 timevec[index+i] = timevec[index+i-1]+(timevec[index-i+1]-timevec[index-i]);
	mulvec[index+i] = mulvec[index-i];
	sphilvec[index+i] = sphilvec[index-i];
	cphilvec[index+i] = cphilvec[index-i];
	betavec[index+i] = betavec[index-i];
	sigmavec[index+i] = sigmavec[index-i];
	thomasvec[index+i] = thomasvec[index-i];
	freak[index+i] = freak[index-i];
	Energy[index+i] = Energy[index-i];
        printf("%d %.12e %.12e %.12e\n", index, timevec[index+i], freak[index+i], Energy[index+i]); 
	 }

       */


      }
    }


  /* Here we catch the MECO location more precisely */
   if (taperQFactor > 1.0e-6)
      {
        index = iend;   /* go back two steps */
	tcurrent = timevec[index]/TSUN;
	for (i = 0; i <= 9; i++) LSvals[i] = OOLSvals[i];
        hcurrent /= 100.0;  /* advance forward more carefully */
        valid = 1;
 
       do
	 {
        rkckstep(updatedvals, fourthordervals, hcurrent, LSvals, tcurrent, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc);
	for (i = 0; i <= 9; i++) LSvals[i] = updatedvals[i];
	tcurrent += hcurrent;
        index++;
	timevec[index] = tcurrent*TSUN;
	LdotS1 = calcLdotS1(LSvals);
	LdotS2 = calcLdotS2(LSvals);
	S1dotS2 = calcSdotS(LSvals);
	mulvec[index] = LSvals[2];
        sphilvec[index] = LSvals[1];
        cphilvec[index] = LSvals[0]; 
	betavec[index] =  (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) 
         + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
	sigmavec[index] = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    
	thomasvec[index] = LSvals[9];

         f = Freq(timevec[index], Mtot, Mchirp, eta, betavec[index], sigmavec[index], tc);
         freak[index] = f;
         x = pow(Mtot*TSUN*PI*f,1.0/3.0);
         x2 = x*x; x3 = x2*x; x4 = x3*x; x5 = x4*x;
         Energy[index] = -0.5*mu*TSUN*x2*(1.0 - x2*(9.0+eta)/12.0 
           +x3*(8.0/(3.0*Mtot*Mtot))*(LdotS1*chi1*m1*(m1+0.75*m2)+LdotS2*chi2*m2*(m2+0.75*m1))
	   -x4*(81.0-57.0*eta+eta*eta)/24.0
           +x4/eta*(chi1*chi2*m1*m1*m2*m2)/(Mtot*Mtot*Mtot*Mtot)*(S1dotS2-3.0*LdotS1*LdotS2));

	 // printf("%d %.12e %.12e %.12e\n", index, timevec[index], freak[index], Energy[index]); 

        if ((freak[index] >= freak[index-1]) && (Energy[index] <= Energy[index-1]))
	    {
             valid = 1;
            }
          else
            {
             valid = 0;    
	    }

	xold = x2; /* When the integration is done, will contain largest x reached... */
	 } while(valid == 1);

   	tmax = timevec[index];
        tstop = timevec[index-1];
	fstop = freak[index-1];
      }
  
  if (index >= VECLENGTH) {
    fprintf(stderr, "VECLENGTH is too small--fix and recompile (%d,%d).\n",index,VECLENGTH);
    exit(1);
  }

  idxm = index;

  spline(timevec, mulvec, index, 1.e30, 1.e30, muly2);
  spline(timevec, sphilvec, index, 1.e30, 1.e30, sphily2);
  spline(timevec, cphilvec, index, 1.e30, 1.e30, cphily2);
  spline(timevec, betavec, index, 1.e30, 1.e30, betay2);
  spline(timevec, sigmavec, index, 1.e30, 1.e30, sigmay2); 
  spline(timevec, thomasvec, index, 1.e30, 1.e30, thomasy2);
  spline(timevec, freak, index, 1.e30, 1.e30, freak2); 
  spline(timevec, Energy, index, 1.e30, 1.e30, Energy2);



   /* 1022 1.939528324258e+07 1.263687099744e-03 -2.677703663596e-01
      1023 1.939533753970e+07 1.268360846640e-03 -2.681595405665e-01
      1024 1.939538422842e+07 1.264424782073e-03 -2.678388861247e-01 */

  
  /* End precession. */
  
  t = -SBH.Tpad;
  dt = (SBH.Tobs+2.0*SBH.Tpad)/(double)(n);
  
  index = 0;

  /* Now interpolate. */

  for(i = 0; i < n; i++)
    {   

      tdvals[i] = t;  
      
      if (tdvals[i] <= tmax) /* Make sure we have data for the point. */
	{
	  while(tdvals[i] > timevec[index+1])
	     index++;
	
	  deltat = timevec[index+1]-timevec[index];

	  A = (timevec[index+1]-tdvals[i])/deltat;
	  B = 1.0-A;
	  C = (pow(A,3.0)-A)*deltat*deltat/6.0;
	  D = (pow(B,3.0)-B)*deltat*deltat/6.0;
	  
	  interpmul[i] = A*mulvec[index] + B*mulvec[index+1] + C*muly2[index] + D*muly2[index+1];  

          cphil = A*cphilvec[index] + B*cphilvec[index+1] + C*cphily2[index] + D*cphily2[index+1]; 

          sphil = A*sphilvec[index] + B*sphilvec[index+1] + C*sphily2[index] + D*sphily2[index+1]; 
	
	  interpphil[i] = atan2(sphil,cphil);

	  interpbeta[i] = A*betavec[index] + B*betavec[index+1] + C*betay2[index] + D*betay2[index+1];
  
	  interpsigma[i] = A*sigmavec[index] + B*sigmavec[index+1] + C*sigmay2[index] + D*sigmay2[index+1];

	  interpthomas[i] = A*thomasvec[index] + B*thomasvec[index+1] + C*thomasy2[index] + D*thomasy2[index+1];
	 
	}
      else  /* no longer have data, freeze values at last valid point */
        {
	  interpmul[i] =   interpmul[i-1];
	  interpphil[i] = interpphil[i-1];
	  interpbeta[i] = interpbeta[i-1];
	  interpsigma[i] = interpsigma[i-1];
	  interpthomas[i] = interpthomas[i-1];
        }

      t += dt;


      /*  printf("%d %.14e %.14e %.14e %.14e %e %e %e %e %e\n", index, timevec[index], timevec[index+1], tdvals[i], t, interpmul[i], interpphil[i], interpbeta[i], interpsigma[i], interpthomas[i]); */
    }
  
  /* Now calculate hp and hc, starting with the final time and working backwards. 
   This makes calculating the taper slightly easier, since otherwise we would have 
   to find out what the final time (tend) would be prior to actually reaching it. */

  tend = tdvals[n-1];

  tTaper = taperQFactor/fstop;

/*  printf("Stas: check -> tTaper = %e \n ", tTaper);  
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      Stas:    NOTE
      The below has to be changed once the integration is extended to 10^5 seconds!!!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   


  if (tc >= tend){
     tTaper = SBH.Tpad;
  }*/
 /* if (tTaper > SBH.Tpad)
       tTaper = SBH.Tpad; */
 /* FILE *fp1;
  fp1 = fopen( "tmp/Taper.dat", "w" ); 
  FILE *fp2;
  fp2 = fopen( "tmp/FreqN.dat", "w" ); 
  printf("Stas: check  f_old = %e \n", fold);
  printf("Stas: check  tstop = %f, fstop = %e, tmax = %f \n", tstop, fstop, tmax);
  printf("Stas: check -> tend = %f,  fend = %e, tTaper = %e , tmax-tend =  %e \n ", tend, fstop, tTaper, tmax-tend);*/
  for(i = n-1; i >= 0; i--)
    { 
      
      td = tdvals[i];
      if (td <= tmax) /* Make sure we have data for the point. */
	{
	    
	  costhetaL = interpmul[i];
	  sinthetaL = sqrt(1.-costhetaL*costhetaL);
	  phiL = interpphil[i];
	  beta = interpbeta[i];
	  sigma = interpsigma[i];
	  thomas = -interpthomas[i];

	  f = Freq(td, Mtot, Mchirp, eta, beta, sigma, tc);
    /* fprintf(fp2, "%f\t %e\n", td, f);*/
	    
	    x = pow(PI*Mtot*f*TSUN,2./3.);

		/* Since we are working backwards in time, x (which scales with f) should
		// decrease with each succesive iteration of the loop (successive decreasing
		// values of i). If x increased from one iteration to the next, it would
		// imply that f had decreased with time (i.e. going from the current iteration
		// to the previous one), casting into doubt the validity of the Post-Newtonian
		// expansion.*/
		if (taperQFactor < 1.0e-6){
           if ((x < xold) && (x < xmax))
              valid = 1;
           else
              valid = 0;    
       }else{
           if (x < xold)
               valid = 1;
            else
               valid = 0;
 	    }
	   /* if((x < xmax) && (x < xold) ) */
   	 if(valid != 0 )
	      {
	    Phi15 = p150 - p15*beta;
	    Phi20 = p200 - p20*(15./32.*sigma);
	    
	    Phase = 2.0*phic + etain*(Phi0*pow(tc-td,0.625) + Phi10*pow(tc-td,0.375) + Phi15*pow(tc-td,0.25) + Phi20*pow(tc-td,0.125)); 
	    
	    LdotN = costhetaL*costheta + sinthetaL*sintheta*cos(phiL - phi);
	  
		/* 1/2 Hann window taper which is zero for t > tend and 1 for 
		// t < tend-3/fold (the width can be varied, but should be at least 2.0/fold):*/
		if (tTaper == 0.){
         taper = 0.5*(1.+tanh(SBH.TaperSteepness*(1./SBH.TaperApplied-x)));
		}else{
		   taper = halfhann(td,tstop,tstop-tTaper);
		  /* taper = halfhann(td,tmax,tmax-tTaper);*/
		}   
	/*	taper2 = 0.5*(1.+tanh(SBH.TaperSteepness*(1./SBH.TaperApplied-x))); */
						
	/*	taper = taper2*taper1; */
    /*  fprintf( fp1, "%e\t%e\n",  td,  taper); */

            Ax = 2.*Amp*pow(PI*f,2./3.)*taper;
         
            ci = LdotN;
            ci2 = ci*ci;
            ci4 = ci2*ci2;
            ci6 = ci2*ci4;
            si2 = 1.0-ci2;
            si4 = si2*si2;
            si = sqrt(si2);
            xrt = sqrt(x);
   
            OPhase = (Phase+thomas)/2.0 + gamma0;
            
            /*if(td == 0.0){
               printf("Stas: at t = %f, orb. phase = %f, thomas phase = %f, %f, %f, total phase = %f\n", td, 0.5*Phase, \
               0.5*thomas+gamma0, 0.5*thomas, gamma0, OPhase);
            }*/

            cPhi[1] = cos(OPhase);
            sPhi[1] = sin(OPhase);

           for(j = 2; j <= 6; j++)
            {
	      cPhi[j] = cPhi[j-1]*cPhi[1]-sPhi[j-1]*sPhi[1];
	      sPhi[j] = sPhi[j-1]*cPhi[1]+cPhi[j-1]*sPhi[1];
            }

	   /* dominant harmonic */
	    shp = Ax*(1.0+ci2)*cPhi[2];
	    shc = -2.0*Ax*ci*sPhi[2];
            

            if(VAmp > 0.0)
	      {
              /* 0.5 PN correction */
	      shp += xrt*Ax*si*dm/8.0*((5.0+ci2)*cPhi[1]-9.0*(1.0+ci2)*cPhi[3]);
	      shc += -xrt*Ax*3.0*si*ci*dm/4.0*(sPhi[1]-3.0*sPhi[3]); 
	      }

           if(VAmp > 1.0)
	      {
              /* 1 PN correction */
	      shp += -x*Ax*(cPhi[2]*((19.0+9.0*ci2-2.0*ci4)-eta*(19.0-11.0*ci2-6.0*ci4))/6.0
                             -4.0*cPhi[4]*si2*(1.0+ci2)*(1.0-3.0*eta)/3.0);
	      shc += x*Ax*(sPhi[2]*ci*((17.0-4.0*ci2)-eta*(13.0-12.0*ci2))/3.0
	                    -8.0*sPhi[4]*(1.0-3.0*eta)*ci*si2/3.0);
              }

           if(VAmp > 2.0)
	      {
              /* 1.5 PN correction */
		shp += -xrt*x*Ax*((dm*si/192.0)*(cPhi[1]*((57.0+60.0*ci2-ci4)-2.0*eta*(49.0-12.0*ci2-ci4))
						    -27.0*cPhi[3]*((73.0+40.0*ci2-9.0*ci4)-2.0*eta*(25.0-8.0*ci2-9.0*ci4))/2.0
                                                    +625.0*cPhi[5]*(1.0-2.0*eta)*si2*(1.0+ci2)/2.0)
                                     -TPI*(1.0+ci2)*cPhi[2]);
	        shc += xrt*x*Ax*((dm*si*ci/96.0)*(sPhi[1]*((63.0-5.0*ci2)-2.0*eta*(23.0-5.0*ci2))
                                                     -27.0*sPhi[3]*((67.0-15.0*ci2)-2.0*eta*(19.0-15.0*ci2))/2.0
                                                     +625.0*sPhi[5]*(1.0-2.0*eta)*si2/2.0)
				    -4.0*PI*ci*sPhi[2]);
              }

           if(VAmp > 3.0)
	      {
              /* 2 PN correction */
		shp += -x*x*Ax*(cPhi[2]*((22.0 + (396.0 * ci2) + (145.0 * ci4) - (5.0 * ci6))
                                            +5.0*eta*(706.0 - (216.0 * ci2) - (251.0 * ci4) + (15.0 * ci6))/3.0
                                            -5.0*eta*eta*(98.0 - (108.0 * ci2) + (7.0 * ci4) + (5.0 * ci6)))/120.0
                                  +cPhi[4]*2.0*si2*((59.0 + (35.0 * ci2) - (8.0 * ci4)) 
						    -5.0*eta*(131.0 + (59.0 * ci2) - (24.0 * ci4))/3.0
						    +5.0*eta*eta*(21.0 - (3.0 * ci2) - (8.0 * ci4)))/15.0
				  -cPhi[6]*81.0*si4*(1.0+ci2)*(1.0-5.0*eta+5.0*eta*eta)/40.0
				   +(dm*si/40.0)*(sPhi[1]*(11.0 + (7.0 * ci2) + (10.0 * (5.0+ci2)*ln2))
                                                 -cPhi[1]*5.0*PI*(5.0+ci2)
						 -sPhi[3]*27.0*(7.0-10.0*ln32)*(1.0+ci2)
						  +cPhi[3]*135.0*PI*(1.0+ci2)));
	        shc += x*x*Ax*(sPhi[2]*ci*(68.0 + (226.0 * ci2) - (15.0 * ci4)
					      +5.0*eta*(572.0 - (490.0 * ci2) + (45.0 * ci4))/3.0
                                              -5.0*eta*eta*(56.0 - (70.0 * ci2) + (15.0 * ci4)))/60.0
				  +sPhi[4]*4.0*ci*si2*((55.0-12.0*ci2)-5.0*eta*(119.0-36.0*ci2)/3.0
                                                       +5.0*eta*eta*(17.0-12.0*ci2))/15.0
                                  -sPhi[6]*81.0*ci*si4*(1.0-5.0*eta+5.0*eta*eta)/20.0
                                  -(dm*3.0*si*ci/20.0)*(cPhi[1]*(3.0+10.0*ln2)+sPhi[1]*5.0*PI
							-cPhi[3]*9.0*(7.0-10.0*ln32)-sPhi[3]*45.0*PI));        
              }


	    
	    psi = atan2(costheta*cos(phi-phiL)*sinthetaL-costhetaL*sintheta, sinthetaL*sin(phi-phiL));  
	    
	    c2psi = cos(2.*psi);
	    s2psi = sin(2.*psi);

            hp[i] = -shp*c2psi - shc*s2psi;
            hc[i] = shp*s2psi - shc*c2psi;

	      }
           else {  /* x > xmax or anti-chirping */

        	hp[i] = 0.;
	        hc[i] = 0.;
			tend = td; /* If we didn't evaluate hp and hc at this step, update tend */

              }

			xold = x; /* Done with this step - set xold to the current value */

	}

      else {  /* t > tmax */
	
	 hp[i] = 0.;
	 hc[i] = 0.;
	 tend = td; /* If we didn't evaluate hp and hc at this step, update tend */
      }

    }
  
 /*   for(i=90000; i<90010; i++){
        printf("%d    %.12e  %.12e \n", i, hp[i], hc[i]);
    }
*/
  free_dvector(tdvals, 0, n-1);
  free_dvector(interpmul, 0, n-1);
  free_dvector(interpphil, 0, n-1);
  free_dvector(interpbeta, 0, n-1);
  free_dvector(interpsigma, 0, n-1);
  free_dvector(interpthomas, 0, n-1);

  free_dvector(timevec, 0,VECLENGTH);
  free_dvector(mulvec, 0,VECLENGTH);
  free_dvector(sphilvec, 0,VECLENGTH);
  free_dvector(cphilvec, 0,VECLENGTH);
  free_dvector(betavec, 0,VECLENGTH);
  free_dvector(sigmavec, 0,VECLENGTH);
  free_dvector(thomasvec, 0,VECLENGTH);

  /* For the spline:*/

  free_dvector(muly2, 0,VECLENGTH);
  free_dvector(sphily2, 0,VECLENGTH);
  free_dvector(cphily2, 0,VECLENGTH);
  free_dvector(betay2, 0,VECLENGTH);
  free_dvector(sigmay2, 0,VECLENGTH);
  free_dvector(thomasy2, 0,VECLENGTH);

  
 /* fclose(fp1); 
  fclose(fp2);*/


}

double Freq(double t, double Mtot, double Mchirp, double eta, double beta, double sigma, double tc)  
{
  double fac, f10, f15, f20, f;

  fac = eta*(tc-t)/(5.*Mtot*TSUN);

  /* printf("%e %e %e %e %e\n", fac, eta, tc, t, Mtot*TSUN); */

  f10 = 743./2688.+11./32.*eta;
  f15 = -3.*(4.*PI-beta)/40.;
  f20 = 1855099./14450688.+56975./258048.*eta+371./2048.*eta*eta-3./64.*sigma;

  f = (pow(fac,-3./8.) + f10*pow(fac,-5./8.) + f15*pow(fac,-3./4.) + f20*pow(fac,-7./8.))/8./PI/Mtot/TSUN;

  return(f);
}


void rkckstep(double outputvals[], double fourthorderoutputvals[], double h, double currentvals[], double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc)
{
  int i;
  
  double **k;
  double *intvals;
 
  intvals = dvector(0,9);
  k = dmatrix(0,9,0,5);
  
  update(0, 0.0, h, currentvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, k);
  
  for(i = 0; i <= 9; i++) intvals[i] = currentvals[i] + B21*k[i][0];
  
  update(1, A2, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, k);
  
  for(i = 0; i <= 9; i++) intvals[i] = currentvals[i] + B31*k[i][0] + B32*k[i][1];
  
  update(2, A3, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, k);
  
  for(i = 0; i <= 9; i++) intvals[i] = currentvals[i] + B41*k[i][0] + B42*k[i][1] + B43*k[i][2];
  
  update(3, A4, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, k);
  
  for(i = 0; i <= 9; i++) intvals[i] = currentvals[i] + B51*k[i][0] + B52*k[i][1] + B53*k[i][2] + B54*k[i][3];

  update(4, A5, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, k);
  
  for(i = 0; i <= 9; i++) intvals[i] = currentvals[i] + B61*k[i][0] + B62*k[i][1] + B63*k[i][2] + B64*k[i][3] + B65*k[i][4];
  
  update(5, A6, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, k);
  
  for(i = 0; i <= 9; i++) 
    {
      outputvals[i] = currentvals[i] + C1*k[i][0] + C2*k[i][1] + C3*k[i][2] + C4*k[i][3] + C5*k[i][4] + C6*k[i][5];
      fourthorderoutputvals[i] = currentvals[i] + D1*k[i][0] + D2*k[i][1] + D3*k[i][2] + D4*k[i][3] + D5*k[i][4] + D6*k[i][5];
    }
  
  free_dmatrix(k,0,9,0,5);
  free_dvector(intvals,0,9);
}

void update(int j, double A, double h, double intvals[], double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc, double **k)
{
  
  double r;
  int i;
  double *derivvals;
  double LcrossN[3];
  double LdotS1;
  double LdotS2;
  double S1dotS2;
  double LdotN;
  double beta;
  double sigma;

  derivvals = dvector(0,9);
  LdotS1 = calcLdotS1(intvals);
  LdotS2 = calcLdotS2(intvals);
  S1dotS2 = calcSdotS(intvals);
  
  beta =  (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
  sigma = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    
  
  r = pow(Mtot,1./3.)/pow(PI*Freq((t+A*h)*TSUN, Mtot, Mchirp, eta, beta, sigma, tc)*TSUN,2./3.);
  
  calcderivvals(derivvals, intvals, r, m1, m2, Mtot, mu, chi1, chi2);
  
  LdotN = calcLdotN(intvals, N);
  
  calcLcrossN(LcrossN, intvals, N);
  
  derivvals[9] = -2.*LdotN/(1.-LdotN*LdotN)*(LcrossN[0]*derivvals[0]+LcrossN[1]*derivvals[1]+LcrossN[2]*derivvals[2]);
    
  for(i = 0; i <= 9; i++) k[i][j] = h*derivvals[i];
  
  free_dvector(derivvals,0,9);
 
}



void calcderivvals(double derivvals[], double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
  /* Calculates the L and S derivatives */
{
  derivvals[3] = S1xdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
  derivvals[4] = S1ydot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
  derivvals[5] = S1zdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
  derivvals[6] = S2xdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
  derivvals[7] = S2ydot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
  derivvals[8] = S2zdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
  derivvals[0] = (-chi1*pow(m1,2.)*derivvals[3]-chi2*pow(m2,2.)*derivvals[6])/(mu*sqrt(Mtot*r));
  derivvals[1] = (-chi1*pow(m1,2.)*derivvals[4]-chi2*pow(m2,2.)*derivvals[7])/(mu*sqrt(Mtot*r));
  derivvals[2] = (-chi1*pow(m1,2.)*derivvals[5]-chi2*pow(m2,2.)*derivvals[8])/(mu*sqrt(Mtot*r));
}

double S1xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
  return (((2.+1.5*m2/m1)*mu*sqrt(Mtot*r)-1.5*chi2*pow(m2,2.)*calcLdotS2(inputs))*(inputs[1]*inputs[5]-inputs[2]*inputs[4])+0.5*chi2*pow(m2,2.)*(inputs[7]*inputs[5]-inputs[8]*inputs[4]))/(pow(r,3.));
}

double S1ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
  return (((2.+1.5*m2/m1)*mu*sqrt(Mtot*r)-1.5*chi2*pow(m2,2.)*calcLdotS2(inputs))*(inputs[2]*inputs[3]-inputs[0]*inputs[5])+0.5*chi2*pow(m2,2.)*(inputs[8]*inputs[3]-inputs[6]*inputs[5]))/(pow(r,3.));
}

double S1zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
  return (((2.+1.5*m2/m1)*mu*sqrt(Mtot*r)-1.5*chi2*pow(m2,2.)*calcLdotS2(inputs))*(inputs[0]*inputs[4]-inputs[1]*inputs[3])+0.5*chi2*pow(m2,2.)*(inputs[6]*inputs[4]-inputs[7]*inputs[3]))/(pow(r,3.));
}

double S2xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
  return (((2.+1.5*m1/m2)*mu*sqrt(Mtot*r)-1.5*chi1*pow(m1,2.)*calcLdotS1(inputs))*(inputs[1]*inputs[8]-inputs[2]*inputs[7])+0.5*chi1*pow(m1,2.)*(inputs[4]*inputs[8]-inputs[5]*inputs[7]))/(pow(r,3.));
}

double S2ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
  return (((2.+1.5*m1/m2)*mu*sqrt(Mtot*r)-1.5*chi1*pow(m1,2.)*calcLdotS1(inputs))*(inputs[2]*inputs[6]-inputs[0]*inputs[8])+0.5*chi1*pow(m1,2.)*(inputs[5]*inputs[6]-inputs[3]*inputs[8]))/(pow(r,3.));
}

double S2zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
  return (((2.+1.5*m1/m2)*mu*sqrt(Mtot*r)-1.5*chi1*pow(m1,2.)*calcLdotS1(inputs))*(inputs[0]*inputs[7]-inputs[1]*inputs[6])+0.5*chi1*pow(m1,2.)*(inputs[3]*inputs[7]-inputs[4]*inputs[6]))/(pow(r,3.));
}

double calcLdotS1(double inputs[])
{
  return inputs[0]*inputs[3]+inputs[1]*inputs[4]+inputs[2]*inputs[5];
}

double calcLdotS2(double inputs[])
{
  return inputs[0]*inputs[6]+inputs[1]*inputs[7]+inputs[2]*inputs[8];
}

double calcSdotS(double inputs[])
{
  return inputs[3]*inputs[6]+inputs[4]*inputs[7]+inputs[5]*inputs[8];
}

double calcLdotN(double inputs[], double nvec[])
{
  return inputs[0]*nvec[0] + inputs[1]*nvec[1] + inputs[2]*nvec[2];
}

void calcLcrossN(double output[], double inputs[], double nvec[])
{
  output[0] = inputs[1]*nvec[2]-inputs[2]*nvec[1];
  output[1] = inputs[2]*nvec[0]-inputs[0]*nvec[2];
  output[2] = inputs[0]*nvec[1]-inputs[1]*nvec[0];
}


/* ********************************************************************************** */
/*                                                                                    */
/*                  Vector/Matrix memory (de)allocation routines                      */
/*                                                                                    */
/* ********************************************************************************** */


int *ivector(long nl, long nh)
/* allocate an int vector with subscript range v[nl..nh] */
{
    int *v;

    v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
    if (!v) fprintf(stderr,"allocation failure in ivector()");
    return v-nl+NR_END;
}

void free_ivector(int *v, long nl, long nh)
/* free an int vector allocated with ivector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}

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

unsigned long *lvector(long nl, long nh)
/* allocate an unsigned long vector with subscript range v[nl..nh] */
{
    unsigned long *v;

    v=(unsigned long *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(long)));
    if (!v) fprintf(stderr,"allocation failure in lvector()");
    return v-nl+NR_END;
}

void free_lvector(unsigned long *v, long nl, long nh)
/* free an unsigned long vector allocated with lvector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}

double **dmatrix(long nrl, long nrh, long ncl, long nch)
/* allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] */
{
    long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
    double **m;

    /* allocate pointers to rows */
    m=(double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
    if (!m) fprintf(stderr, "allocation failure 1 in matrix()");
    m += NR_END;
    m -= nrl;

    /* allocate rows and set pointers to them */
    m[nrl]=(double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
    if (!m[nrl]) fprintf(stderr,"allocation failure 2 in matrix()");
    m[nrl] += NR_END;
    m[nrl] -= ncl;

    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

    /* return pointer to array of pointers to rows */
    return m;
}

void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch)
/* free a double matrix allocated by dmatrix() */
{
    free((FREE_ARG) (m[nrl]+ncl-NR_END));
    free((FREE_ARG) (m+nrl-NR_END));
}


double ***d3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
/* allocate a double 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] */
{
    long i,j,nrow=nrh-nrl+1,ncol=nch-ncl+1,ndep=ndh-ndl+1;
    double ***t;

    /* allocate pointers to pointers to rows */
    t=(double ***) malloc((size_t)((nrow+NR_END)*sizeof(double**)));
    if (!t) fprintf(stderr,"allocation failure 1 in d3tensor()");
    t += NR_END;
    t -= nrl;

    /* allocate pointers to rows and set pointers to them */
    t[nrl]=(double **) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double*)));
    if (!t[nrl]) fprintf(stderr,"allocation failure 2 in d3tensor()");
    t[nrl] += NR_END;
    t[nrl] -= ncl;

    /* allocate rows and set pointers to them */
    t[nrl][ncl]=(double *) malloc((size_t)((nrow*ncol*ndep+NR_END)*sizeof(double)));
    if (!t[nrl][ncl]) fprintf(stderr,"allocation failure 3 in f3tensor()");
    t[nrl][ncl] += NR_END;
    t[nrl][ncl] -= ndl;

    for(j=ncl+1;j<=nch;j++) t[nrl][j]=t[nrl][j-1]+ndep;
    for(i=nrl+1;i<=nrh;i++) {
        t[i]=t[i-1]+ncol;
        t[i][ncl]=t[i-1][ncl]+ncol*ndep;
        for(j=ncl+1;j<=nch;j++) t[i][j]=t[i][j-1]+ndep;
    }

    /* return pointer to array of pointers to rows */
    return t;
}

void free_d3tensor(double ***t, long nrl, long nrh, long ncl, long nch,
    long ndl, long ndh)
/* free a float d3tensor allocated by d3tensor() */
{
    free((FREE_ARG) (t[nrl][ncl]+ndl-NR_END));
    free((FREE_ARG) (t[nrl]+ncl-NR_END));
    free((FREE_ARG) (t+nrl-NR_END));
}

double gasdev2(long *idum)
{
  double ran2(long *idum);
  static int iset=0;
  static double gset;
  double fac, rsq, v1, v2;

  if(*idum < 0) iset = 0;
  if(iset == 0){
    do{
      v1 = 2.0 * ran2(idum)-1.0;
      v2 = 2.0 * ran2(idum)-1.0;
      rsq = v1*v1+v2*v2;
    } while (rsq >= 1.0 || rsq == 0.0);
    fac=sqrt(-2.0*log(rsq)/rsq);
    gset = v1 * fac;
    iset = 1;
    return(v2*fac);
  } else {
    iset = 0;
    return (gset);
  }
}

double ran2(long *idum)
{
    int j;
    long k;
    static long idum2=123456789;
    static long iy=0;
    static long iv[NTAB];
    double temp;

    if (*idum <= 0) {
        if (-(*idum) < 1) *idum=1;
        else *idum = -(*idum);
        idum2=(*idum);
        for (j=NTAB+7;j>=0;j--) {
            k=(*idum)/IQ1;
            *idum=IA1*(*idum-k*IQ1)-k*IR1;
            if (*idum < 0) *idum += IM1;
            if (j < NTAB) iv[j] = *idum;
        }
        iy=iv[0];
    }
    k=(*idum)/IQ1;
    *idum=IA1*(*idum-k*IQ1)-k*IR1;
    if (*idum < 0) *idum += IM1;
    k=idum2/IQ2;
    idum2=IA2*(idum2-k*IQ2)-k*IR2;
    if (idum2 < 0) idum2 += IM2;
    j=iy/NDIV;
    iy=iv[j]-idum2;
    iv[j] = *idum;
    if (iy < 1) iy += IMM1;
    if ((temp=AM*iy) > RNMX) return RNMX;
    else return temp;
}

/* Evaluate the half of a Hann window which is zero at t0 and one at t1. If t falls
// outside of the range t0...t1, set the value of the window to either zero or one,
// depending on whether it's on the zero side of the window or the one side. */
double halfhann(double t, double t0, double t1){
	
	double w;
	
	if(t > t0 && t > t1){
		if(t0 > t1) return 0;
		else return 1;
	} else if(t < t0 && t < t1){
		if(t0 < t1) return 0;
		else return 1;
	} else {
		w = cos(PI*(t-t1)/(2.0*(t0-t1)));
		return w*w;
	}
	
}

void spline(double *x, double *y, int n, double yp1, double ypn, double *y2)
/* Unlike NR version, assumes zero-offset arrays.  CHECK THAT THIS IS CORRECT. */
{
  int i, k;
  double p, qn, sig, un, *u;

  u = dvector(0, n-2);
  
  /* Boundary conditions: Check which is best.  */
  if (yp1 > 0.99e30)
    y2[0] = u[0] = 0.0;
  else {
    y2[0] = -0.5;
    u[0] = (3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
  }

  for(i = 1; i < n-1; i++) {
    sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p = sig*y2[i-1] + 2.0;
    y2[i] = (sig-1.0)/p;
    u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }

  /* More boundary conditions. */
  if (ypn > 0.99e30)
    qn = un = 0.0;
  else {
    qn = 0.5;
    un = (3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
  }
  
  y2[n-1] = (un-qn*u[n-2])/(qn*y2[n-2]+1.0);
  
  for (k = n-2; k >= 0; k--)
    y2[k] = y2[k]*y2[k+1]+u[k];
  
  free_dvector(u, 0, n-2);
}
    
  
void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);
 
  return;
}
