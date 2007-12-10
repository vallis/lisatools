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

/******************  SVN info ************************ 
*/
#define SVNIDAKWAVEFORMHH "$Id$"

#ifndef AKWAVEFORMHH
#define AKWAVEFORMHH

#include <iostream>
//#include <fstream>
#include <math.h>
#include "BaseIntegr.hh"
#include "Macros.hh"
#include "Constants.hh"
#include <vector>
#include <gsl/gsl_sf_bessel.h>


/*** Class AKWaveform
 *
 * Generator of analytic kludge waveforms described in 
 * Barack & Cutler gr-qc/0310125
 *
 * @author Stas Babak, April 2006
 */

namespace LISAWP{
  
class AKWaveform:public BaseIntegr{

	public:	
	
    /*** Constructor
     * @param spin magnitude of (specific) spin of MBH
     * @param tfin integration time in sec, (t0 assumed to be zero)
     * @param mu mass of the compact object (solar mass)
     * @param MBHmass mass of the MBH (solar mass)
     * @param timestep sampling rate: dt (in sec)
     */
    AKWaveform(double spin, double mu, double MBHmass, double tfin, double timestep);

    /** Set the parameters defining source position wrt SSB
     * @param thS the source direction's polar angle (rad)
     * @param phS the azimuthal direction to the source (rad)
     * @param thK polar angle of MBH's spin
     * @param phK azimuthal direction of S
     * @param D distance to the source (in pc)
     */

    void SetSourceLocation(double thS, double phS, double thK, double phK, double D);

    /** Estimates orbital params at t=0 for eccentricity given at plunge
     * @param Tin duration of the backward integration
     * @param eccen eccentricity at lso or at time Tin (input)
     * @param gamma0 gamma angle at plunge
     * @param Phi0 azimuthal orbital phase at plunge
     * @param al0 alpha angle at plunge
     * @param lam lambda angle (const)
     */
    void EstimateInitialParams(double Tin, double eccen, double gamma0, \
		    double Phi0, double al0, double lam); 
    
    /** Computes orbital evolution
     * @param eccen value of eccentricity at plunge
     * @param gamma0 position of pericenter, as angle between LxS and pericenter
     *               at plunge
     * @param Phi0 azimuthal orbital phase at plunge
     * @param al0 alpha angle at plunge 
     * @param lam angle between L and S
     */

    void EvolveOrbit(double t0, double nu0, double eccen, double gamma0, \
		    double Phi0, double al0, double lam);


    /** Returns orbital evolution */
    
    void GetOrbitalEvolution(std::vector<float>& time, std::vector<float>& Phit,\
		    std::vector<float>& nut, std::vector<float>& gammat, \
		    std::vector<float>& et, std::vector<float>& alt);
      
    /** Fills up parameters at the end of evolution 
     * @param t to be filled up with the end time
     * @param e_end to be filled up with the final eccentricity
     * @param nu_end to be filled up witht the final orbital frequency 
     */
    
    void GetFinalOrbitalParams(float& t, float& e_end, float& nu_end);


    /** returns orbital elements at time t */
    void GetOrbitalParams(double t, double& nut, double& et, double& gt, double& pht, double& alt);
    
    /** returns orbital elements at time t=0 */
    void GetOrbitalParamsAt0(double& nut, double& et, double& gt, double& pht, double& alt);
    

    /** Returns size of waveform and fills up hplus, hcross
     *@param ps0 initial polarization angle
    */
     int GetWaveform(double ps0, double* hPlus, long hPlusLength, double* hCross, long hCrossLength, int debug, int Nharm);

	private:

     std::vector<double> tt;
     std::vector<double> phi_t;
     std::vector<double> nu_t;
     std::vector<double> gamma_t;
     std::vector<double> e_t;
     std::vector<double> al_t;


      /// general  parameters
     
     double aa; // spin
     double m;  // small mass
     double M;  // SMBH mass
     double dt; // time step
     double tEnd; // observation time (sec)
     
     /// inital orbital parameters
     double phi_0;
     double ecc0;
     double nu_0;
     double gamma_0;
     double alpha0;

     double phi_at0;
     double ecc_at0;
     double nu_at0;
     double gamma_at0;
     double alpha_at0;
    

   ///  location
     double thetaS;
     double phiS;
     double lambba;
     double thetaK;
     double phiK;
     double dist;
     
     bool sourceSet;
     bool runDone;
     bool back;
    
     /// current coordinates
     double phi;
     double nu;
     double gamma;
     double e;
     double alpha;

     double phiL, thetaL;
     double Sn;

     double Aplus;
     double Across;
    
     double t0; // feducial time where nu(t0) = 1mHz
     double e0True; // e0 at t0
     double nu0True; // could be slightly different from 1mHz
     
     /** Checks for plunge (nu_{max})
      * @param fr azimuthal freq.
      * @param ec eccentricity
      */
     int CheckPlunge(double fr, double ec);
    
     virtual void Derivs(double x, Matrix<double> y, Matrix<double>& dydx, int n);

     void Waveform(int N);     

     double ArcTan(double up, double down);

};
}//end of namespace

#endif
