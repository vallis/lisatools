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
float Tmax = (float) oneyear;

float mu = 10.0;        	// mass of CO
float MBHmass = 1.0e6;  	// mass of MBH
float spin = 0.6;      
float nu0, nu_lso, e_in, nu_in; // to be determined             
float e_lso = 0.2;      	// estimated freq at plunge
float gamma0 = LISAWP_PI/7.0;
float Phi0 = 0.0;    		// initial orbital phase
float al0 = 1.5*LISAWP_PI;  	// alpha 
float lam = LISAWP_PI/1.5;      //lambda
float elat = LISAWP_PI/4.0;    	// thetaS 
float elon = LISAWP_PI + 0.1;   // phiS 
float thK = LISAWP_PI/3.1;      // thetaK
float phK = 1.3*LISAWP_PI;	// phiK
float pol = 0.0;                // initial polarization angle
float Tend = 0.6*oneyear;       // estimated duration of the signal
float t_end, e_end, nu_end;


AKWaveform ak(spin, mu, MBHmass, Tmax, (float)dt);

ak.SetSourceLocation(elat, elon, thK, phK, D);

nu_lso =  pow((1.0-e_lso*e_lso)/(6.0+2.0*e_lso), 1.5)/(LISAWP_TWOPI*MBHmass*LISAWP_MTSUN_SI);


ak.EstimateInitialParams((double)Tend, e_lso, nu_lso, &e_in, &nu_in);

ak.EvolveOrbit(nu_in, e_in, gamma0, Phi0, al0, lam);

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

std::vector<float> time;
std::vector<double> hplus;
std::vector<double> hcross;
	
 
ak.GetWaveform(pol, time, hplus, hcross);

std::cout << " Waveform was computed " << std::endl;
std::cout << " Recording...." << std::endl;

std::string spr = "         ";
std::ofstream fout("EMRI.dat");
fout << "#  time/year		hplus 		hcross" << std::endl;
for (int i=0; i<time.size(); i++){
      fout <<  time[i]/oneyear  << spr << hplus[i] << spr << hcross[i] << std::endl;
}
fout.close();


}
