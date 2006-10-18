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
#include "BBHChallenge1.hh"

using namespace LISAWP;

int main(){

float m1 = 2.0e6;               // mass1 (solar Mass)
float m2 = 1.0e6;               // mass2 (solar Mass)
float thS =  LISAWP_PI/5.;      // inclination angle (rad)
float Phi0 = LISAWP_PI/12.;     // initial orbital phase
float tc = 180.*24.*3600.;      // duration: 180 days
float psi = 0.0;                // polarization
float maxDuration = 31457280.0; // max. Duration = ~1year (sec)
float dt = 15.0;		// timestep
double Dist = 1.e10; 		// lum. distance (pc)
float taper = 8.0;		// applying window after 8
float t0 = 0.0;                 // initial time

BBHChallenge1 bbh(m1, m2);

//double omega0 = bbh.EstimateFreq0(tc);   // Initial orbital angular frequency

bbh.SetInitialOrbit(tc, Phi0);

bbh.ComputeInspiral(t0, dt, maxDuration);

std::cout << "Orbit was computed" << std::endl;

bbh.SetObserver(thS, Dist);

std::vector<double> hPlus;
std::vector<double> hCross;

bbh.ComputeWaveform(0.0, taper, hPlus, hCross);
//bbh.ComputeWaveformSSB(0.0, taper, psi, hPlus, hCross); // compute h+ hx in SSB

std::cout << "Waveform was computed" << std::endl;

std::cout << "Recording...." << std::endl;

std::vector<double> freq;
std::vector<double> phase;

bbh.GetOrbit(freq, phase);

std::ofstream fout("BBH.dat");

// Recording file:
fout << "# time/year		freq		phase		h+		hx" << std::endl;
std::string spr = "       ";

for (int i=1; i<hPlus.size(); i++){

   fout << ((double)i*dt)/maxDuration << spr << freq[i] << spr << phase[i] <<
	   spr << hPlus[i] << spr << hCross[i] << std::endl;

}
fout.close();


return(0);
	
}

