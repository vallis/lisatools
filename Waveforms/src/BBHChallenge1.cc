/*
Copyright (C) 2005  S. Babak

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
#define CVSHEADER "$Header: Exp $" 
*/

#include "BBHChallenge1.hh"

namespace LISAWP{

BBHChallenge1::BBHChallenge1(float mass1, float mass2){

   m1 = (double) mass1;
   m2 = (double) mass2;
   M = m1 + m2;
   eta = m1*m2/(M*M);
   m1 *= LISAWP_MTSUN_SI; // mass in seconds
   m2 *= LISAWP_MTSUN_SI; 
   M *= LISAWP_MTSUN_SI;
   mu = m1*m2/M;
  
   orbitSet = false;
   observerSet = false;
   runDone = false;
   stab = false;

   omegaLSO = 1./(pow(6, 1.5)*M);

   omegaMECO = pow(-(54. + 6.*eta - 6.*sqrt(1539. - 1008.*eta + 19.*eta*eta))/\
		   (81. - 57.*eta + eta*eta),1.5)/(27.*M);
}
  

void BBHChallenge1::SetInitialOrbit(float coalTime, float phi0){

   Phi = (double) phi0;
   tc = coalTime;
   //omega0 = EstimateFreq0(0.2*eta/M*tc);
   omega0 = EstimateFreq0(tc);

   orbitSet = true;
}

double BBHChallenge1::EstimateTc(float omega0){

   double x, a, b;
   double fr =  pow(M*omega0, -8./3.);
   double nu = pow(M*omega0, 2./3.);
   double nuf = M*omega0;
   double y;

    // Analytic expression for tau_c, used as initial guess:
   x = (1./256.)*fr*( 1. +  (743./252. +11./3.*eta)*nu - 32./5.*LISAWP_PI*nuf +
		 (3058673./508032. + 5429./504.*eta + 617./72.*eta*eta)*nu*nu  );

   // range for root searching
   a = 0.85*x; b = 1.15*x;

   double ya = omega0 - PPNfreq(a);
   if (fabs(ya) <= 1.e-10) return(a*5.*M/eta);
   double yb = omega0 - PPNfreq(b);
   if (fabs(yb) <= 1.e-10) return(b*5.*M/eta);

  // std::cout << "Stas: ya = " << ya << "  yb = " << yb << std::endl;
   LISAWPAssert(ya<0. && yb>0., "Function doesn't change sign.");

   // Find the root (tau_c) of omega_0 = omega(tau_c) 
   
   int count = 1;
   int maxIter = 40;  // maximal number of iterations
    for (int i = 0; i< maxIter; i++){   // bisection method
	    x = 0.5*(a+b);
	    y = (omega0 - PPNfreq(x));
	  //  std::cout << "test: " << count << "   x = " << x*5.*M/eta << "  y(x) = " << y <<  std::endl;
	    y*(omega0-PPNfreq(a)) >0 ? a = x : b = x;
	    if (fabs((a-b)/a) <= 1.e-6){ 
    //               std::cout << "test2: inital freq= " << omega0 << " error  " 
    //			   << (PPNfreq(a) - omega0)/omega0 << std::endl;
		   return(a*5.*M/eta);
		   break;
	    } 
	    count ++;
    }
    if (count >= maxIter){
	    std::cerr << "Maximum iterations " << count << " was reached" << std::endl;
	    std::cerr << "Value of function = " << y << "  (a-b) = " << fabs(a-b) << std::endl;
	    exit(1);
    }

}


 double BBHChallenge1::EstimateFreq0(float tc){

  double tauc = 0.2*eta*tc/M; 
  double t38 = pow(tauc, -3./8.);
  double t14 = pow(tauc, -0.25);
  double rhs = 0.125*t38*(  1. + (11./32.*eta + 743./2688.)*t14 - 0.3*LISAWP_PI*t38 + \
                 (1855099./14450688. + 56975./258048.*eta + 371./2048.*eta*eta)*t14*t14  );
  double f0 = rhs/M;

  return(f0);
  
 }

 void BBHChallenge1::ComputeInspiral(float t0, float timeStep, float maxDuration){
 
   LISAWPAssert(orbitSet, "You need to first set the initial orbit!");
   double tau, nu;

   dt = timeStep;
   
   tau = 0.2*eta/M*(tc-t0);
   om = PPNfreq(tau);
   om_prev = om;
   stab = CheckStab(om);
   if(!stab){
       std::cerr << "MECO/LSO reached with initial conditions!" << std::endl;
       exit(1);   
   }
   
   double ph_res = PPNphase(omega0);
   double Phi_diff = Phi - ph_res;  // need to bring initial phase to Phi0

//   double om0 = om;
//   tau = 0.2*eta/M*tc;
//   nu = PPNfreq(tau);
//   double om_Diff = om0 - nu;    // need to bring initial freq. to omega0

   time.resize(0);
   Phase.resize(0);
   freq.resize(0);
	   
   double t = t0;
   while(stab){
     
      LISAWPAssert(t<tc, "Attempt to go beyond coalescence time!");
      tau = 0.2*eta/M*(tc - t);
      om = PPNfreq(tau);
      
      Phi = PPNphase(om) + Phi_diff;
      stab = CheckStab(om);
      om_prev = om;
      time.push_back(t);
      Phase.push_back(Phi);
      freq.push_back(om);
      t = t + dt;   // advance time
      if(t >(double)maxDuration){
	      break;
      }
   }  // end of while loop
   runDone = true;
   std::cout << "final time =  " <<  t/31457280.0 << "  years" << std::endl; 

 }

 void BBHChallenge1::SetObserver(float thetaD, double D){

    theta = (double)thetaD;
    dist = D*LISAWP_PC_SI/LISAWP_C_SI; //distance in seconds

    observerSet = true;
 }


 void BBHChallenge1::ComputeWaveform(float truncateTime,  float taper,  \
		 std::vector<double>& hPlus, std::vector<double>& hCross){

    LISAWPAssert(observerSet, "You must specify direction to observer!");
    LISAWPAssert(runDone, "You must compute inspiraling trajectory first!");

    double cs = cos(theta);
    double Mom23;
    double hp, hc;

    int size = time.size();
    LISAWPAssert((int)Phase.size() == size, "Sizes do not match!");

    int subtractN = (int) floor(truncateTime/dt);
    LISAWPAssert(subtractN < size, "Truncation time is larger than the waveform duration!");
    size  = size - subtractN;

    hCross.resize(0);
    hPlus.resize(0);
    
    double Ampl = (M*eta/dist);
    double wk = 1.0;
    double xmax;
    if (taper != 0.0)
	 xmax = 1./taper;
    double Ataper = 150;   // this parameter regulates steepnes of the taper
    
    //std::ofstream fout32("BBHtest.dat");   
   for(int k=0; k<size; k++){
       Mom23 = pow(M*freq[k], 2./3.);
       if (taper != 0.0) 
           wk = 0.5*( 1.0 - tanh(Ataper*(Mom23-xmax)) );
       hp = Ampl * Mom23 * 2.0*(1. + cs*cs)*cos(2.0*Phase[k]);
       hc = Ampl * Mom23 *4.0*cs*sin(2.*Phase[k]); 
     //  fout32 << k << "      "  << time[k] <<  "       " << 1./Mom23 << "      " << 
//	       hPlus(k)   << "       " << wk*hPlus(k) << std::endl;
       hPlus.push_back(hp*wk);
       hCross.push_back(hc*wk);
    }
  // fout32.close();
 }

 

void BBHChallenge1::ComputeWaveformSSB(float truncateTime, float taper, float psi, \
		       std::vector<double>& hPlus, std::vector<double>& hCross){

    LISAWPAssert(observerSet, "You must specify direction to observer!");
    LISAWPAssert(runDone, "You must compute inspiraling trajectory first!");

    double cs = cos(theta);
    double Mom23;
    double hp, hc;

    int size = time.size();
    LISAWPAssert((int)Phase.size() == size, "Sizes do not match!");

    int subtractN = (int) floor(truncateTime/dt);
    LISAWPAssert(subtractN < size, "Truncation time is larger than the waveform duration!");
    size  = size - subtractN;

    hCross.resize(0);
    hPlus.resize(0);
    
    double Ampl = (M*eta/dist);
    double wk = 1.0;
    double xmax;
    if (taper != 0.0)
	 xmax = 1./taper;
    double Ataper = 150;   // this parameter regulates steepnes of the taper

   //std::ofstream fout32("BBHtest.dat");   
   double cpsi = cos(2.*psi);
   double spsi = sin(2.*psi);
   for(int k=0; k<size; k++){
       Mom23 = pow(M*freq[k], 2./3.);
       if (taper != 0.0) 
           wk = 0.5*( 1.0 - tanh(Ataper*(Mom23-xmax)) );
       hp = wk*Ampl * Mom23 * 2.0*(1. + cs*cs)*cos(2.0*Phase[k]);
       hc = wk*Ampl * Mom23 *4.0*cs*sin(2.*Phase[k]); 
     //  fout32 << k << "      "  << time[k] <<  "       " << 1./Mom23 << "      " << 
//	       hPlus(k)   << "       " << wk*hPlus(k) << std::endl;
       hPlus.push_back(hp*cpsi + hc*spsi);
       hCross.push_back(hc*cpsi - hp*spsi);
    }
  // fout32.close();

}




 void BBHChallenge1::GetOrbit(std::vector<double>& frequency, std::vector<double>& phase){

   int size = time.size();
   frequency.resize(size);
   phase.resize(size);
   
   frequency = freq;
   phase = Phase;

 }

double BBHChallenge1::PPNfreq(double tau){

   double rhs;
   double t38 = pow(tau, -3./8.);
   double t14 = pow(tau, -0.25);

   rhs = 0.125*t38*(  1. + (11./32.*eta + 743./2688.)*t14 - 0.3*LISAWP_PI*t38 + \
		 (1855099./14450688. + 56975./258048.*eta + 371./2048.*eta*eta)*t14*t14  );
   return (rhs/M);

}

double BBHChallenge1::PPNphase(double omeg){

   double v = pow(M*omeg, 2./3.);
   double fr = M*omeg;
   double p;

   p = -1./(32.*eta*v*fr) * (1. + (3715./1008. + 55./12.*eta)*v - 10.*LISAWP_PI*fr  +\
		    (15293365./1016064. + 27145./1008.*eta + 3085./144.*eta*eta)*v*v);
   return(p);

}


bool BBHChallenge1::CheckStab(double freq){

  bool valid = true;
  if( freq >= omegaMECO){
	 std::cout << "Instability condition is met at MECO: orb. ang. freq. =  " << om << std::endl;
	 valid = false;
//	 return(false);
  }
  if( freq >= omegaLSO ){
	 std::cout << "Instability condition is met at LSO: orb. ang. freq. = " << om << std::endl;
	 valid = false;
//	 return(false);
  }
  if( om < om_prev){
	 std::cout << "Instability condition is met at domega/dt = 0: orb. ang. freq. = " << om << std::endl;
	 valid = false;
//	 return(false);
  }
  return(valid); 
}



}// end of namespace

