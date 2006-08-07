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

#ifndef SPINBBHWAVEFORMHH
#define SPINBBHWAVEFORMHH

#include <iostream>
#include <fstream>
#include <math.h>
#include "Constants.hh"
#include "Macros.hh"
#include <vector>

/** Class BBHChallenge1
 *
 * Here we compute the waveform for two non-spinning
 * BHs. The amplitude is restricted to the leading order only
 * and the orbital phase is computed up to 2PN order.
 * The closest waveform would be "TaylorT3" in classification
 * introduced in PRD 63 044023 (see also erratum).
 *
 * @author Stas Babak, 2006
 */

namespace LISAWP{

class BBHChallenge1{

  public:
	
     /*** Constructor
      * @param mass1 mass of the 1st companion in solar masses
      * @param mass2 mass of the 2nd companion in solar masses
      */
	  
      BBHChallenge1(float mass1, float mass2);
      
     /*** initial orbital arameters
      * @param coalTime time of coalescence
      * @param phi0 initial orbital phase
      */
      
      void SetInitialOrbit(float coalTime, float phi0);

      /** Estimates (returns) coalescence time (in sec) 
       * @param omega0 initial orbital angular frequency (in Hz)
       */
      
      double EstimateTc(float omega0);


      /** Estimates (returns) initial orbital frequency (in Hz)
       * @param tc time of coalescence (in sec)
       */
      
      double EstimateFreq0(float tc);

      /**  Computes inspiralling motion 
       * @param timeStep sampling step (in sec).
       * @maxDuration maximum duaration (in sec).
       * */
      
      void ComputeInspiral(float t0, float timeStep, float maxDuration);

     /** define direction to the detector/baricenter 
      * @param thetaD theta direction to the BSS/detector in rad.
      * @param D distance to the observer (in pc)
      */
    
      void SetObserver(float thetaD, double D);

      /** Computes and returns two polarizations in the source frame
       * @param truncateTime waveform truncation time as time left to
       *  t_MECO or t_LSO (in sec)
       *  @param taper if non-zero aplies window (tanh) function starting from 
       *  r = taper(M), for instance tape=7, means window the waveform starting at 7M. 
       *  @param hPlus to be filled up with h+ in rad. frame
       *  @param hCross to be filled up with hx in rad. frame
       */

       void ComputeWaveform(float truncateTime, float taper,  std::vector<double>& hPlus,\
                             std::vector<double>& hCross);

       /** Computes and returns two polarizations in the SSB frame (polarization axis 
	* are defiend in the "Convension" document and in the documentation to "LISA
	* Simulator").
	* @param truncateTime waveform truncation time as time left to
        *  t_MECO or t_LSO (in sec)
        *  @param taper if non-zero aplies window (tanh) function starting from 
        *  r = taper(M), for instance tape=7, means window the waveform starting at 7M. 
	*  @param psi polarization angle
        *  @param hPlus to be filled up with h+ in rad. frame
        *  @param hCross to be filled up with hx in rad. frame
        */
       void ComputeWaveformSSB(float truncateTime, float taper, float psi, \
		       std::vector<double>& hPlus, std::vector<double>& hCross);

      /**  Returns to the user  orbital evolution 
       * @param frequency vector with omega_orb(t)
       * @param phase vector with phase_orb(t)
       * */

       void GetOrbit(std::vector<double>& frequency, std::vector<double>& phase);

  private:

       double m1;
       double m2;
       double mu;
       double M;
       double eta;
       double theta, dist;
       double tc, omega0;
       std::vector<double> time;
       std::vector<double> Phase;
       std::vector<double> freq;

       bool stab;
       bool runDone;
       bool observerSet;
       bool orbitSet;
       double om;
       double Phi;
       double dt;
       double om_prev;
       double omegaLSO;
       double omegaMECO;
 
      /** Computes PPN orbital frequency (no spin, analyt.)
       * @param tau tau time ~ (tc - t)
       */
      double PPNfreq(double tau);
      /** Computes PPN orbital phase (no spin, analyt.)
       * @param om instanteneous orbital frequecy
       */
       double PPNphase(double omeg);

       /** Checks termination point of the orbital evolution
	* returns true if we still can evolve 
	* @param freq orbital angular frequency
	*/ 
       bool BBHChallenge1::CheckStab(double freq);

};

}// end of namespace


#endif
