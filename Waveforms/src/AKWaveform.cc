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
#define CVSTAG "$Name:  $"
#define CVSHEADER "$Header: /afs/.aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/waveforms/src/AKWaveform.cc,v 1.3 2006/05/22 14:31:32 stba Exp $" 
*/

#include "AKWaveform.hh"

namespace LISAWP{

  
AKWaveform::AKWaveform(float spin, float mu, float MBHmass, float tfin, float timestep){

     aa = (double)spin;
     m = (double)mu;
     M = (double)MBHmass;	
		
     // transform masses in seconds
     m *= LISAWP_MTSUN_SI;
     M *= LISAWP_MTSUN_SI;
		
     dt = (double)timestep;
     tEnd = (double)tfin;

 /*    
     

   */  
     sourceSet = false;
     runDone = false;
	
}

void AKWaveform::SetSourceLocation(float thS, float phS,  float thK, float phK, float D){

     thetaS = (double) thS;
     phiS = (double) phS;
     thetaK = (double) thK;
     phiK = (double) phK;
     dist = (double) D;

     // translate distance in sec
     
     dist *= (LISAWP_PC_SI/LISAWP_C_SI);
	
     sourceSet = true;
	
}


void AKWaveform::EstimateInitialParams(double Tin, float e_lso, float nu_lso, float *e_in, float *nu_in){
     
    double tstep = 100.0;
    double Tobs = Tin;
    double eb, nub;
    double dedt, dnudt;
    double e2;
    double e4;
    double fr;
    double iome;      // 1/(1-e^2)
    double iome25;   // 1/(1-e^2)^2.5

    eb = e_lso;
    nub = nu_lso;
  //  nub = pow( (1.0-e_lso*e_lso)/(6.0+2.0*e_lso), 1.5 )/(LISAWP_TWOPI * M);
   // *nu_lso = nub;
  //  std::cout << "lso freq = " << nub << std::endl; 
	    
    while(Tobs >= 0.0){
        e2  = eb*eb;
        e4 =  e2*e2;
        fr = LISAWP_TWOPI * M * nub;
	iome = 1.0/(1. - eb*eb);
	iome25 = pow(iome, 2.5);
        dnudt = (9.6/LISAWP_PI)*(m/(M*M*M))*pow(fr, 11./3.)* \
	      (  (1.0 + 73.0/24.0*e2 + 37.0/96.0*e4 )*iome*iome25 + \
		pow(fr, 2./3.)*( 1273.0/336.0 - 2561.0/224.0*e2 - \
			3885./128.0*e4 - 13147.0/5376.0*e4*e2)*iome*iome*iome25);
        dedt = -(eb/15.)*(m/(M*M))*iome25*pow(fr,8./3.)*(304. + 121.*e2);
	
	Tobs = Tobs - tstep;
	eb = eb - dedt*tstep;
	nub = nub - dnudt*tstep;

    }
    *e_in = eb;
    *nu_in = nub;

   // std::cout << "in. freq = " << nub << std::endl; 
    
	
}

void AKWaveform::EvolveOrbit(float nu0, float eccen, float gamma0, \
		    float Phi0, float al0, float lam){

   int stab;	
   double t;
   double advstep, cumulstep, newstep;
   stab = CheckPlunge(nu0, eccen);
   LISAWPAssert(stab, "Initial parameters correpsond to plunging orbit"); 

   Matrix<double> coord0(1,5);
   Matrix<double> coordn(1,5);
   Matrix<double> rhs(1,5);
   rhs = 0.0;
   coord0(0) = (double)Phi0;
   coord0(1) = (double)nu0;
   coord0(2) = (double)gamma0;
   coord0(3) = (double)eccen;
   coord0(4) = (double)al0;
   lambda = (double) lam; 

   int counter = 1;
   newstep = dt;
   cumulstep = dt;

   phi = coord0(0);
   nu = coord0(1);
   gamma = coord0(2);
   e = coord0(3);
   alpha = coord0(4);
   t= 0.0;
   
   double err = fabs(nu - 1.e-3);
   while(t <= tEnd){
      tt.push_back(t);
      phi_t.push_back(phi);
      nu_t.push_back(nu);
      gamma_t.push_back(gamma);
      e_t.push_back(e);
      al_t.push_back(alpha);
      if( fabs(nu - 1.e-3) <= err ){
	      err = fabs(nu - 1.e-3);
	      nu0True = nu;
	      e0True = e;
	      t0 = t;
      }

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
   
   cumulstep = cumulstep/(double)counter;
   runDone = true;

}

double AKWaveform::GetTruet0(){
	LISAWPAssert(runDone, "Need to compute orbit first");
	return(t0);
}
double AKWaveform::GetTruee0(){
	LISAWPAssert(runDone, "Need to compute orbit first");
	return(e0True);
}
double AKWaveform::GetTruenu0(){
	LISAWPAssert(runDone, "Need to compute orbit first");
	return(nu0True);
}

int AKWaveform::CheckPlunge(double fr, double ec){
	
    double rhs = pow( (1.0-ec*ec)/(6.0+2.0*ec), 1.5 )/(LISAWP_TWOPI * M);
    if(rhs - fr <= 1.0e-5){
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
		2561.0/224.0*e2 - 3885./128.0*e4 - 13147.0/5376.0*e4*e2) - fr*aa*cos(lambda)*siome*\
	       (73./12. + 1211./24.*e2 + 3143./96.*e4 + 65./64*e4*e2)  );
    //d gamma/dt
    dydx(2) = 6.*LISAWP_PI*nu *pow(fr, 2./3.) * iome*( 1. + 0.25*pow(fr,2./3.)*iome*(26. - 15.*e2) ) - \
	      12.*LISAWP_PI*nu*cos(lambda)*aa*fr*iome*siome;
    //de/dt
    dydx(3) = -(e/15.)*(m/(M*M))*siome*iome*iome2*pow(fr,8./3.)*( (304. + 121.*e2)*ome*\
		    (1. + 12.*pow(fr, 2./3.)) - (1./56.)*pow(fr,2./3.)*(133640. + 108984.*e2 -\
		25211.*e4) ) +  e*(m/(M*M))*aa*cos(lambda)*pow(fr, 11./3.)*iome2*iome2*( 1364./5. +\
	       5032./15.*e2 + 26.3*e4 );
    // d alpha/dt
    dydx(4) = 4.*LISAWP_PI*nu*aa*fr*siome*iome;
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


void AKWaveform::GetWaveform(float ps0, std::vector<float>& time, \
		std::vector<double>& hplus, std::vector<double>& hcross){

    LISAWPAssert(runDone, "you must Run first");
    LISAWPAssert(sourceSet, "you must set source location");

    int theSize = tt.size();
    time.resize(theSize);
    hplus.resize(theSize);
    hcross.resize(theSize);
       
    Sn = cos(thetaS)*cos(thetaK) + sin(thetaS)*sin(thetaK)*cos(phiS - phiK);
    
   // loop over orbital evolution
   double phiD = 0.0;
   //double OmOrb = LISAWP_TWOPI/LISAWP_YRSID_SI;
   double t;
   double psiSL;
   double cs;
   for (int i=0; i<theSize; i++){

	t = tt[i];
        e = e_t[i];
	nu = nu_t[i];
      //phiD = LISAWP_TWOPI*nu*(LISAWP_AU_SI/LISAWP_C_SI)*sin(thetaS)*cos(OmOrb*t - phiS);  
	phi = phi_t[i] + phiD;
        alpha = al_t[i];
	gamma = gamma_t[i];
    
        thetaL = acos(cos(thetaK)*cos(lambda) + sin(thetaK)*sin(lambda)*cos(alpha));
	LISAWPAssert(sin(thetaL) != 0.0, "sin(thetaL) = 0, cannot determine phiL");

	cs = ( sin(thetaK)*cos(phiK)*cos(lambda) - \
	     cos(phiK)*cos(thetaK)*sin(lambda)*cos(alpha) + sin(phiK)*sin(lambda)*sin(alpha));
	if (cs != 0){
	    phiL = ArcTan((sin(thetaK)*sin(phiK)*cos(lambda) - \
	      sin(phiK)*cos(thetaK)*sin(lambda)*cos(alpha) -cos(phiK)*sin(lambda)*sin(alpha)), cs);
	}else{
	    phiL = LISAWP_PI/2.0;
	}
    
        int N = (int)(30*e);
        if(e <0.136) N=4;	
	Waveform(N);

	if (sin(phiS-phiL) != 0.0){
  	   psiSL = -ArcTan( (cos(thetaS)*sin(thetaL)*cos(phiS-phiL) - cos(thetaL)*sin(thetaS)),\
		    (sin(thetaL)*sin(phiS-phiL)) );
	}else{
           psiSL = LISAWP_PI/2.0;
	}
	
        time[i] = t;
	hplus[i] = Aplus*cos(2.*ps0 - 2.*psiSL) + Across*sin(2*ps0 - 2*psiSL);
	hcross[i] = Aplus*cos(2.*ps0 - 2.*psiSL) - Across*sin(2*ps0 - 2*psiSL);

   }// end of for loop

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
   nSL = sin(thetaS)*sin(phiK - phiS)*sin(lambda)*cos(alpha) + \
	     (Sn*cos(thetaK) - cos(thetaS))*sin(lambda)*sin(alpha)/sin(thetaK);

   if (nSL != 0.0){
       beta = ArcTan( (cos(lambda)*Ln - Sn),(nSL) );
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
