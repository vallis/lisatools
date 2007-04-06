/*
Copyright (C) 2006  S. Babak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/******************  CVS info ************************ 
*/
#define SVNIDAKWAVEFORMCC "$Id$"

#include "AKWaveform.hh"

namespace LISAWP{

  
AKWaveform::AKWaveform(double spin, double mu, double MBHmass, double tfin, double timestep){

     aa = spin;
     m = mu;
     M = MBHmass;	
		
     // transform masses in seconds
     m *= LISAWP_MTSUN_SI;
     M *= LISAWP_MTSUN_SI;
		
     dt = timestep;
     tEnd = tfin;

 /*    
     

   */  
     sourceSet = false;
     runDone = false;
	
}

void AKWaveform::SetSourceLocation(double thS, double phS, double thK, double phK, double D){

     thetaS = thS;
     phiS = phS;
     thetaK = thK;
     phiK = phK;
     dist = D;

     // translate distance in sec
     
     dist *= (LISAWP_PC_SI/LISAWP_C_SI);
	
     sourceSet = true;
	
}


void AKWaveform::EstimateInitialParams(double Tin, double eccen, double gamma0, \
		    double Phi0, double al0, double lam){
     
   double t;
   double advstep, cumulstep, newstep;
 
  // Stas: 12.01.07 we do exact backward integration
   double nu0 = pow( (1.0-eccen*eccen)/(6.0+2.0*eccen), 1.5 )/(LISAWP_TWOPI * M);
   
   Matrix<double> coord0(1,5);
   Matrix<double> coordn(1,5);
   Matrix<double> rhs(1,5);
   rhs = 0.0;
   coord0(0) = Phi0;
   coord0(1) = nu0;
   coord0(2) = gamma0;
   coord0(3) = eccen;
   coord0(4) = al0;
   lambba = lam; 

   int counter = 1;
   newstep = dt;
   cumulstep = dt;

   phi = coord0(0);
   nu = coord0(1);
   gamma = coord0(2);
   e = coord0(3);
   alpha = coord0(4);
   
   t = Tin;
   back = true;
   // First step: we integrate to the nearest integer of dt:

   double step0 = fmod(t, dt);
   if(step0 != 0.0){
      Derivs(t, coord0, rhs, 5);
      advstep =  IntegratorCKRK(coord0, rhs, coordn, t, step0, 5);
      phi = coordn(0);
      nu = coordn(1);
      gamma = coordn(2);
      e = coordn(3);
      alpha = coordn(4);
      coord0 = coordn;
      t = t - step0;
   }
  
  
   while (t >= 0.0){ 
       Derivs(t, coord0, rhs, 5);
       advstep =  IntegratorCKRK(coord0, rhs, coordn, t, dt, 5);
       phi = coordn(0);
       nu = coordn(1);
       gamma = coordn(2);
       e = coordn(3);
       alpha = coordn(4);
       coord0 = coordn;
       t = t - dt;

       if (t == 0.0){
      /*      std::cout << " time  = " << t << "\n" \
		   << " phi = " << phi << "\n" \
		   << " nu = " << nu << "\n" \
		   << " e = " << e << "\n" \
		   << " gamma = "<< gamma << "\n" \
		   << " alpha = " << alpha << std::endl; */
	    phi_at0 = phi;
            ecc_at0 = e;
            nu_at0 = nu;
            gamma_at0 = gamma;
            alpha_at0 = alpha;
       }
   }
 
}

/**** Stas 12.01.07 We use again initial conditions at t=0 and doing integreation
 * back to prebuffer seconda and forward to end time
 */

void AKWaveform::EvolveOrbit(double t0, double nu0, double eccen, double gamma0, \
		    double Phi0, double al0, double lam){

  int stab;
  double t;
  double advstep, cumulstep, newstep;
  stab = CheckPlunge(nu0, eccen);
  LISAWPAssert(stab, "Initial parameters correpsond to plunging orbit");
   
  Matrix<double> coord0(1,5);
  Matrix<double> coordn(1,5);
  Matrix<double> rhs(1,5);
  rhs = 0.0;
  coord0(0) = Phi0;
  coord0(1) = nu0;
  coord0(2) = gamma0;
  coord0(3) = eccen;
  coord0(4) = al0;
  lambba = lam;
   
  int counter = 1;
  newstep = dt;
  cumulstep = dt;
   
  phi = coord0(0);
  nu = coord0(1);
  gamma = coord0(2);
  e = coord0(3);
  alpha = coord0(4);
   
   
  std::vector<double> t_n;
  std::vector<double> phi_n;
  std::vector<double> nu_n;
  std::vector<double> gamma_n;
  std::vector<double> e_n;
  std::vector<double> alpha_n;
  t = 0.0;
  /*** NOTE: t0>0 is not implemented. Not needed so far. */
  if (t0 < 0.0){               // backwards integration
      back = true;
      while (t >= t0){
            Derivs(t, coord0, rhs, 5);
            advstep =  IntegratorCKRK(coord0, rhs, coordn, t, dt, 5);
            phi = coordn(0);
            nu = coordn(1);
            gamma = coordn(2);
            e = coordn(3);
            alpha = coordn(4);
            coord0 = coordn;
            t = t - dt;
            if (t >= t0){
              t_n.push_back(t);
              phi_n.push_back(phi);
              nu_n.push_back(nu);
              gamma_n.push_back(gamma);
              e_n.push_back(e);
              alpha_n.push_back(alpha);
	    }
      }
      for(int i = 0; i < (int)t_n.size(); i++){\
         int k = t_n.size() - i -1;
         tt.push_back(t_n[k]);
         phi_t.push_back(phi_n[k]);
         nu_t.push_back(nu_n[k]);
         gamma_t.push_back(gamma_n[k]);
         e_t.push_back(e_n[k]);
         al_t.push_back(alpha_n[k]);
      }
      t_n.resize(0);
      phi_n.resize(0);
      nu_n.resize(0);
      gamma_n.resize(0);
      e_n.resize(0);
      alpha_n.resize(0);
   
      t = 0.0;
      phi = Phi0;
      nu = nu0;
      gamma = gamma0;
      e = eccen;
      alpha = al0;
      lambba =  lam;
      coord0(0) = Phi0;
      coord0(1) = nu0;
      coord0(2) = gamma0;
      coord0(3) = eccen;
      coord0(4) = al0;
   
  }
  back = false;
  while(t <= tEnd){  // forward integration
      tt.push_back(t);
      phi_t.push_back(phi);
      nu_t.push_back(nu);
      gamma_t.push_back(gamma);
      e_t.push_back(e);
      al_t.push_back(alpha);
     /*  if( fabs(nu - 1.e-3) <= err ){
   	      err = fabs(nu - 1.e-3);
   	      nu0True = nu;
   	      e0True = e;
   	      t0 = t;
         }*/
      Derivs(t, coord0, rhs, 5);
       /*  std::cout << "*****************************" << std::endl;
         std::cout << rhs;
         std::cout << "*****************************" << std::endl;*/
      advstep =  IntegratorCKRK(coord0, rhs, coordn, t, dt, 5);
      counter ++;
      cumulstep += advstep;
      if (advstep < newstep)
         newstep = advstep;
      t += dt;
      phi = coordn(0);
      nu = coordn(1);
      gamma = coordn(2);
      e = coordn(3);
      alpha = coordn(4);
      coord0 = coordn;
   
      stab = CheckPlunge(nu, e);
      if (stab < 0){
   	   std::cout << "*** we reached plunge at t = " << t << std::endl;
   	   break;
      }
  }//end of while loop
/*  std::cout << "final azimuthal frequency = " << nu << std::endl;
  std::cout << "plunge frequency for e_final = " << e << " n_pl =  " << \
                    pow( (1.0-e*e)/(6.0+2.0*e), 1.5 )/(LISAWP_TWOPI * M) << std::endl;
  std::cout << "final phase = " << phi << std::endl;
  std::cout << "final alpha = " << alpha << std::endl;
  std::cout << "final gamma = " << gamma << std::endl;
  std::cout << "final time = " << t << std::endl; */
   
  cumulstep = cumulstep/(double)counter;
  runDone = true;
   
  
}

void AKWaveform::GetOrbitalParams(double t, double& nut, double& et, double& gt, double& pht, double& alt){
	LISAWPAssert(runDone, "Need to compute orbit first");
	int k = -1;
	for (int i=0; i<(int)tt.size()-1; i++){
              if (tt[i] == t){
		  std::cout << "recroding inst. orbit at t= " << tt[i] << std::endl;
		  nut = nu_t[i];
		  et = e_t[i];
		  gt = gamma_t[i];
		  pht = phi_t[i];
		  alt = al_t[i];
		  k = i;
	      }
	}
	LISAWPAssert(k != -1, "wrong instance of time");
}

void AKWaveform::GetOrbitalParamsAt0(double& nut, double& et, double& gt, double& pht, double& alt){

   double tmp;
   nut = nu_at0;
   if (ecc_at0 >=0.6){
      std::cout << "WARNING!!!! Initial eccentricity exceeds 0.6!" << std::endl;
   }
   
   et = ecc_at0;
   tmp = fmod(gamma_at0, LISAWP_TWOPI);
   gt = tmp;
   if (tmp < 0.0)
      gt = tmp + LISAWP_TWOPI;
   tmp = fmod(phi_at0, LISAWP_TWOPI);
   pht = tmp;
   if (tmp < 0.0)
      pht = tmp + LISAWP_TWOPI;
   tmp = fmod(alpha_at0, LISAWP_TWOPI);
   alt = tmp;
   if (tmp < 0.0)
      alt = tmp + LISAWP_TWOPI;
  // gt = gamma_at0;
  // pht = phi_at0;
  // alt = alpha_at0;

}


int AKWaveform::CheckPlunge(double fr, double ec){
	
    double rhs = pow( (1.0-ec*ec)/(6.0+2.0*ec), 1.5 )/(LISAWP_TWOPI * M);
    if(rhs - fr <= 0.0){
	    std::cout << "freq at plunge = " <<  rhs << std::endl;
	    return(-1);
    }else{
	    return(1);
    }

}



void AKWaveform::Derivs(double x, Matrix<double> y, Matrix<double>& dydx, int n){

    nu = y(1);
    e = y(3);

    double e2  = e*e;
    double e4 =  e2*e2;
    double fr = LISAWP_TWOPI * M * nu;
    double ome = 1.0 - e2;
    double iome = 1.0/ome;      // 1/(1-e^2)
    double iome2 = iome*iome;   // 1/(1-e^2)^2
    double siome = sqrt(iome);  // 1/sqrt(1-e^2)

    // dPhi/dt
    dydx(0) = LISAWP_TWOPI*nu;

    // d nu/dt
    dydx(1) = (9.6/LISAWP_PI)*(m/(M*M*M))*pow(fr, 11./3.)*iome2*iome2*siome* \
	      (  (1.0 + 73.0/24.0*e2 + 37.0/96.0*e4 )*ome + pow(fr, 2./3.)*( 1273.0/336.0 - \
		2561.0/224.0*e2 - 3885./128.0*e4 - 13147.0/5376.0*e4*e2) - fr*aa*cos(lambba)*siome*\
	       (73./12. + 1211./24.*e2 + 3143./96.*e4 + 65./64*e4*e2)  );
    //d gamma/dt
    dydx(2) = 6.*LISAWP_PI*nu *pow(fr, 2./3.) * iome*( 1. + 0.25*pow(fr,2./3.)*iome*(26. - 15.*e2) ) - \
	      12.*LISAWP_PI*nu*cos(lambba)*aa*fr*iome*siome;
    //de/dt
    dydx(3) = -(e/15.)*(m/(M*M))*siome*iome*iome2*pow(fr,8./3.)*( (304. + 121.*e2)*ome*\
		    (1. + 12.*pow(fr, 2./3.)) - (1./56.)*pow(fr,2./3.)*(133640. + 108984.*e2 -\
		25211.*e4) ) +  e*(m/(M*M))*aa*cos(lambba)*pow(fr, 11./3.)*iome2*iome2*( 1364./5. +\
	       5032./15.*e2 + 26.3*e4 );
    // d alpha/dt
    dydx(4) = 4.*LISAWP_PI*nu*aa*fr*siome*iome;
    
    // in case of backward integration
    //if (x < 0.0){
    if (back) 
      dydx *= (-1.);
    
    //}
	   
}


void AKWaveform::GetOrbitalEvolution(std::vector<float>& time, \
		std::vector<float>& Phit, std::vector<float>& nut, \
		std::vector<float>& gammat, std::vector<float>& et, \
		std::vector<float>& alt){

 
      int theSize = tt.size();
      
      LISAWPAssert(runDone, "you must compute orbit first");
      
      time.resize(theSize);
      Phit.resize(theSize);
      nut.resize(theSize);
      gammat.resize(theSize);
      et.resize(theSize);
      alt.resize(theSize);

      for (int i=0; i< theSize; i++){
         time[i] = (float)tt[i];
         Phit[i] = (float)phi_t[i];
         nut[i] = (float)nu_t[i];
         gammat[i] = (float)gamma_t[i];
         et[i] = (float)e_t[i];
         alt[i] = (float)al_t[i];
      }
}

void AKWaveform::GetFinalOrbitalParams(float& t, float& e_end, float& nu_end){

     LISAWPAssert(runDone, "you must compute orbit  first");

     int theSize = tt.size();
     t = tt[theSize-1];
     e_end = (float)e_t[theSize-1];
     nu_end = (float)nu_t[theSize-1];

}

int AKWaveform::GetWaveform(double ps0, double* hPlus, long hPlusLength, double* hCross, long hCrossLength){

    LISAWPAssert(runDone, "you must Run first");
    LISAWPAssert(sourceSet, "you must set source location");
   


    int theSize = tt.size();

    LISAWPAssert(theSize <= hPlusLength && theSize <= hCrossLength, "Buffer insufficient!");
       
    Sn = cos(thetaS)*cos(thetaK) + sin(thetaS)*sin(thetaK)*cos(phiS - phiK);
    
   // loop over orbital evolution
   double phiD = 0.0;
   //double OmOrb = LISAWP_TWOPI/LISAWP_YRSID_SI;
   double t;
   // double psiSL; MV 070330
   double psi;
   double cs;

   for (int i=0; i<theSize; i++){

	t = tt[i];
        e = e_t[i];
	nu = nu_t[i];
      //phiD = LISAWP_TWOPI*nu*(LISAWP_AU_SI/LISAWP_C_SI)*sin(thetaS)*cos(OmOrb*t - phiS);  
	phi = phi_t[i] + phiD;
        alpha = al_t[i];
	gamma = gamma_t[i];
    
    thetaL = acos(cos(thetaK)*cos(lambba) + sin(thetaK)*sin(lambba)*cos(alpha));
	LISAWPAssert(sin(thetaL) != 0.0, "sin(thetaL) = 0, cannot determine phiL");

	cs = ( sin(thetaK)*cos(phiK)*cos(lambba) - \
	     cos(phiK)*cos(thetaK)*sin(lambba)*cos(alpha) + sin(phiK)*sin(lambba)*sin(alpha));
	if (cs != 0){
	    phiL = ArcTan((sin(thetaK)*sin(phiK)*cos(lambba) - \
	      sin(phiK)*cos(thetaK)*sin(lambba)*cos(alpha) -cos(phiK)*sin(lambba)*sin(alpha)), cs);
	}else{
	    phiL = LISAWP_PI/2.0;
	}
    
        int N = (int)(30*e);
        if(e <0.136) N=4;	
	Waveform(N);

    // MV 070330: using standard polarization angle

    double up = (cos(thetaS)*sin(thetaL)*cos(phiS-phiL) - cos(thetaL)*sin(thetaS));
    double dw = (sin(thetaL)*sin(phiS-phiL));

    if (dw != 0.0) {
        psi = ArcTan(up, dw); // previously it was psiSL = ArcTan(dw,up);
    } else {
        psi = LISAWP_PI / 2.0;
    }
    
    // previously it was 2.0*ps0 - 2.0*psiSL
	*hPlus  =  Aplus*cos(2.0*ps0 + 2.0*psi) + Across*sin(2.0*ps0 + 2.0*psi);
	*hCross = -Aplus*sin(2.0*ps0 + 2.0*psi) + Across*cos(2.0*ps0 + 2.0*psi);

    // end MV 070330

	hPlus++;
	hCross++;
   } // end of for loop
   
   return(theSize);

}


void AKWaveform::Waveform(int N){

   double a, b, c;
   double Ln, nSL;
   double beta, gma;
   double ne, Ampl;
   Aplus = 0.0;
   Across = 0.0;
   

   Ln = cos(thetaS)*cos(thetaL) + sin(thetaS)*sin(thetaL)*cos(phiS - phiL);
       
   /// !!! the next assert must be removed and replaced by proper solution !!!
   LISAWPAssert(sin(thetaK) != 0.0, "thetaK = pi*n/2, resolve devision on sin (next line)");
   nSL = sin(thetaS)*sin(phiK - phiS)*sin(lambba)*cos(alpha) + \
	     (Sn*cos(thetaK) - cos(thetaS))*sin(lambba)*sin(alpha)/sin(thetaK);

   if (nSL != 0.0){
       beta = ArcTan( (cos(lambba)*Ln - Sn),(nSL) );
   }else{
       beta = LISAWP_PI/2.0;
   }
   gma = gamma + beta;

   Ampl = pow(LISAWP_TWOPI*nu*M, 2./3.)*m/dist;
   
   for (int n=1; n<=N; n++){
       ne = (double)n*e;
       a = - n*( gsl_sf_bessel_Jn(n-2, ne) - 2.*e*gsl_sf_bessel_Jn(n-1, ne) +\
		 (2.0/(double)n)*gsl_sf_bessel_Jn(n, ne) + \
		 2.0*e*gsl_sf_bessel_Jn(n+1, ne) - gsl_sf_bessel_Jn(n+2, ne) )* \
	        cos(phi*(double)n);
       b =  - n*sqrt(1.-e*e)*( gsl_sf_bessel_Jn(n-2, ne) - 2.0*gsl_sf_bessel_Jn(n, ne) +\
			       gsl_sf_bessel_Jn(n+2, ne) )*sin(phi*(double)n);
       c = 2.0*gsl_sf_bessel_Jn(n, ne)* cos(phi*(double)n);
		   
       a *= Ampl;
       b *= Ampl;
       c *= Ampl;
  /*     std::cout  << "Ampl = " << Ampl << std::endl;
       std::cout << "n = " << n <<"  an = " << a << "  bn = " << b << "  cn = " << c << std::endl;
       std::cout << "J(n-2) = " << gsl_sf_bessel_Jn(n-2, ne) << std::endl;
       std::cout << "J(n-1) = " << gsl_sf_bessel_Jn(n-1, ne) << std::endl;
       std::cout << "J(n) = " << gsl_sf_bessel_Jn(n, ne) << std::endl;
       std::cout << "J(n+1) = " << gsl_sf_bessel_Jn(n+1, ne) << std::endl;
       std::cout << "J(n+2) = " << gsl_sf_bessel_Jn(n+2, ne) << std::endl;
       std::cout << " phase =  " << (double)n * phi << std::endl;
*/

       
       Aplus += -(1.0 + Ln*Ln)*(a*cos(2.*gma) - b*sin(2.*gma)) + (1.0 - Ln*Ln)*c;
       Across += 2.0*Ln*( b*cos(2.*gma) + a*sin(2.*gma) );
	       
   }// end of for loop

}


double AKWaveform::ArcTan(double up, double down){

     double arct;
     arct = atan(up/down);
     if (down < 0.0) arct = arct + LISAWP_PI;

     return arct;
}

}// end of namespace
