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

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "Constants.hh"
#include "Macros.hh"
#include "AKWaveform.hh"

using namespace LISAWP;

int main(){


double D = 1.e9;     		// luminocity distance to the source
double dt = 15.0;    		// timestep 
double oneyear = pow(2., 21.)*15.0;
double Tmax = (double) oneyear;

double mu = 10.0;        	// mass of CO
double MBHmass = 1.0e6;  	// mass of MBH
double spin = 0.6;      
double nu0, nu_lso, e_in, nu_in; // to be determined             
double e_lso = 0.2;      	// estimated freq at plunge
double gamma0 = LISAWP_PI/7.0;
double Phi0 = 0.0;    		// initial orbital phase
double al0 = 1.5*LISAWP_PI;  	// alpha 
double lam = LISAWP_PI/1.5;      //lambda
double elat = LISAWP_PI/4.0;    	// thetaS 
double elon = LISAWP_PI + 0.1;   // phiS 
double thK = LISAWP_PI/3.1;      // thetaK
double phK = 1.3*LISAWP_PI;	// phiK
double pol = 0.0;                 // initial polarization angle
double Tend = 0.6*oneyear;       // estimated duration of the signal
float t_end, e_end, nu_end;
double t0 = 0.0;                 // initial time 

AKWaveform ak(spin, mu, MBHmass, Tmax, (double)dt);

ak.SetSourceLocation(elat, elon, thK, phK, D);

nu_lso =  pow((1.0-e_lso*e_lso)/(6.0+2.0*e_lso), 1.5)/(LISAWP_TWOPI*MBHmass*LISAWP_MTSUN_SI);

ak.EstimateInitialParams(Tend, e_lso, gamma0, Phi0, al0, lam);

ak.GetOrbitalParamsAt0(nu_in, e_in, gamma0, Phi0, al0);

ak.EvolveOrbit(t0, nu_in, e_in, gamma0, Phi0, al0, lam);

std::cout << "Orbit is computed" << std::endl;

ak.GetFinalOrbitalParams(t_end, e_end, nu_end);

 std::cout << "Estimated ecc at plunge = " << e_lso << std::endl;
 std::cout << "Actual   ecc   at plung = " << e_end << std::endl;
 std::cout << "Estiamted frequency at plunge = " << nu_lso << std::endl;
 std::cout << "Actual   frequency  at plunge = " << nu_end << std::endl;
 std::cout << "Estimated duration of the waveform = " << Tend/oneyear << "  years"
               << std::endl;
 std::cout << "Actual   duration  of the waveform = " << t_end/oneyear << "  years"
                << std::endl;

std::vector<double> time;
std::vector<double> hplus;
std::vector<double> hcross;
	
 
int sz = ak.GetWaveform(pol, hplus, hcross);

std::cout << " Waveform was computed " << std::endl;
std::cout << " Recording...." << std::endl;

double t = t0;
std::string spr = "         ";
std::ofstream fout("EMRI.dat");
fout << "#  time/year		hplus 		hcross" << std::endl;
for (int i=0; i<hplus.size(); i++){
      fout <<  t/oneyear  << spr << hplus[i] << spr << hcross[i] << std::endl;
      t += dt;
}
fout.close();


std::vector<float> t_o;
std::vector<float> phit;
std::vector<float> nut;
std::vector<float> gammat;
std::vector<float> et;
std::vector<float> alt;

ak.GetOrbitalEvolution(t_o, phit, nut, gammat, et, alt);

std::ofstream fout2("EMRIorbit.dat");
fout2 << " # time(sec)   phi, nu, gamma, e, alpha " << std::endl;

for (int i = 0; i < 1000; i++){

     fout2 << t_o[i] << spr << phit[i] << spr << nut[i] << spr << gammat[i] <<\
	     spr << et[i]  << spr << alt[i] << std::endl;
}

fout2.close();





}
