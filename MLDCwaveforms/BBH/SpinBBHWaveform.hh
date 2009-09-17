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
#define CVSHEADER "$Header: /afs/aeiw/cvsroot/waves/people/stas/LISAWP/waveforms/include/SpinBBHWaveform.hh,v 1.1 2007/09/18 21:56:11 stba Exp $" 
*/

#ifndef SPINBBHWAVEFORMHH
#define SPINBBHWAVEFORMHH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <vector>
#include "BaseIntegr.hh"
#include "Constants.hh"
#include "Macros.hh"
#include "Matrix.hh"

/**  Class SpinBBHWaveform
 *
 * Here I compute the waveform for two spinning bodies in 
 * circular orbit. This waveform is based on Kidder and BCV2
 * work/convencions. The phase is computed up to 3.5PN including
 * the spin-spin term and amplitude is computed up to 1PN.
 * For details see corresponding pdf document
 *
 * @author Stas Babak, 2006
 */

namespace LISAWP{

 class SpinBBHWaveform:public BaseIntegr{

 public:

   /** Constructor, defines inital conditions  
    * @param mass1 mass of the 1st companion in solar masses
    * @param mass2 mass of the 2nd companion in solar masses
    * @param chi1 amplitude of the first spin
    * @param chi2 amplitude of the second spin
    * @param timeStep timestep of integration (sec)
    */

   SpinBBHWaveform(double mass1, double mass2, double chi1, double chi2, double timeStep);


   /** computes inspiralling trajectory 
    * @param t0 starting point of the waveform (usually -900)
    * @param omega0 initial orbital frequency
    * @param phi0 initial orbital phase
    * @param iota0 initail theta of \f[ \bf{\hat{L_{N}}} \f] in rad, 
    * iota=0 equivalent to non-spinning case 
    * @param alpha0 initial phi of \f[ \bf{\hat{L_{N}}} \f] in rad   
    * @param S1z z-component of \f[ \bf{\hat{S_1}} \f]
    * @param phiS1 azimuthal angle of the first spin in rad
    * @param S2z z-component of \f[ \bf{\hat{S_2}} \f]
    * @param phiS2 azimuthal angle of the second spin in rad
    * @paaram maxDuration maximum duration from t=0 (needed if Tc outside observation time)
    * @param TaperQFactor Q of the half-Hannig taper
    */
    
   void ComputeInspiral(double t0, double tc, double phiC, double iota0, double alpha0, \
                     double thetaS10, double phiS10, double thetaS20, double phiS20, double maxDuration,\
                     double TaperQFactor);

   /** define direction to the detector/baricenter 
    * @param theta_s colatitude position of the source
    * @param phi_s longitude position of the source
    * @param D distance to the observer (in pc)
    */
   void SetObserver(double theta_s, double phi_s, double D);

   /** computes and returns plus polarization 
    * @param order PPN order of amplitude: 0 only leading order, 1 only 0.5PN, 2 only 
    *  1.0PN, default computes all of them.
    *  @param taper separaton at which taper kicks in
    *  @param smooth applies smoothning window at t=0 and t_MECO/LSO
    *  @param hPlus to be filled up with h+ in rad. frame
    *  @param hCross to be filled up with hx in rad. frame
    */
   
   int ComputeWaveform(int order, double taper, double* hPlus, long hPlusLength,\
                    double* hCross, long hCrossLength);
   
   private:

   double m1;
   double m2;
   double mu;
   double dm;
   double m1M2, m2M2;
   double S1x, S1y, S1z;
   double S2x, S2y, S2z;
   double Lnx, Lny, Lnz;
   double LS1, LS2, S1S2;
   double iota, alpha;
   double theta, dist;
   double beta, sigma;
   double thetaJ, phiJ;
   double thetaS, phiS;
   double ctS, stS; 
   std::vector<double> time;
   std::vector<double> Phase;
   std::vector<double> freq;
   std::vector<double> io;
   std::vector<double> al;
   std::vector<double> s1x;
   std::vector<double> s1y;
   std::vector<double> s1z;
   std::vector<double> s2x;
   std::vector<double> s2y;
   std::vector<double> s2z;
   double x1, x2;
   double eta, M;
   double maxDur, tIni;
   bool spinSet, orbitSet, observerSet;
   bool nonspin, runDone;
   bool back;
   bool MECO;
   bool LSO;
   double om_lso;
   bool nospin_route;
   double om_prev;
   double dt, tend;
   double om; 
   double Phi;
   double dEdomega;
   double Tc, PhiOrbC;
   double psi;
   double taperQ;

   //double t;

  /** checks MECO condition */

   bool CheckMECO();
   
   /** computes derivatives */
   
   virtual void Derivs(double x, Matrix<double> y, Matrix<double> &dydx, int n);

   /** Computes PPN orbital frequency (no spin, analyt.)
    * @param t time at which freq will be computed
    * @param tc time of coalescence
    */
   double ComputeOrbFreq(double t, double tc);
   /** Computes PPN orbital phase (no spin, analyt.)
    * @param om instanteneous orbital frequecy
    * @param phi_C orbital(!) phase at coalescence
    */
   double ComputeOrbPhase(double om, double phi_C);
   
   /** Computes orbital phase using time domain expression */ 

   double ComputeOrbPhase(double t, double Tc, double phi_C);
   
   /** Computes the taper, currently we use half-Hanning window */
   
   double halfhann(double t, double t0, double t1);

 };



} // end of namespace


#endif

