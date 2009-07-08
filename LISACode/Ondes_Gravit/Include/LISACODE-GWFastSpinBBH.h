// $Id: LISACODE-GWBinary.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWFastSpinBBH.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  This class (module) returns h_plus and h_cross for a spinning black holes binary system.
 *
 *
 *  Created on 20/08/08 by  N. Cornish updated for LISACode by A.Petiteau (AEI)
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */

#ifndef __GWFastSpinBBH_H
#define __GWFastSpinBBH_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-GW.h"
using namespace std;

/*!\ingroup gw
 */
/*! \brief Gravitational Waves parameters for a spinning black holes binary system are defined in this class.
 */
	


class GWFastSpinBBH : public GW
	{
		protected :
		// *************** Parameters of system *************** 
		/*! \brief Mass of object 1 (in solar masses) */
		double m1;                
		/*! \brief Mass of object 2 (in solar masses) */
		double m2;
		/*! \brief Time of coalescence (in seconds) */
		double tc;
		/*! \brief Distance between source and detector  (in kiloparsec) */
		double DL;
		/*! \brief Spin of object 1  */
		double chi1;
		/*! \brief Spin of object 2 */
		double chi2;
		/*! \brief Polar angle of spin S1 */
		double PolarAngleOfSpin1;
		/*! \brief Polar angle of spin S2 */
		double PolarAngleOfSpin2; 
		/*! \brief Azimuthal angle of spin S1 */   
		double AzimuthalAngleOfSpin1;
		/*! \brief Azimuthal angle of spin S1 */
		double AzimuthalAngleOfSpin2;
		/*! \brief Phase at coalescence */
		double phic;
		/*! \brief Initial polar angle of orbital momentum \brief  */
		double InitialPolarAngleL;
		/*! \brief Initial azimuthal angle of orbital momentum \brief  */
		double InitialAzimuthalAngleL;
		/*! \brief  */
		double TaperApplied;
		/*! \brief High Post-Newtonian order in calculation (maximum 2 PN)*/
		double AmplPNorder;
		/*! \brief Time offset */
		double Toffset;
		/*!  \brief Duration of observation */
		double Tobs;
		/*!  \brief  */
		double Rmin;
		/*! \brief  */
		double TaperSteepness;
		
		
		// *************** Used variables ***************
		/*! \brief Total mass */
		double Mtot;
		/*! \brief Difference of mass */
		double dm;
		/*! \brief Reduced mass */
		double mu;
		/*! \brief Mass ratio and it's inverse */
		double eta, etain;
		/*! \brief Chirp mass */
		double Mchirp;
		
		double Amp;
		double sintheta, costheta;
		double gamma0; // Orbital phase rotation to match Kidder frame
		double tmax, xmax, xold;
		double phi;
		double Phi0, Phi10, p15, p150, p20, p200;
		double sf, sx;
		double ci;
		double psi;
		double Ax;
		double DeltaPhase2L;
		
		/*! \brief Last time of compute amplitude */
		double timeCur;
		
		/*! \brief Current index for the interpolation and its maximal value*/
		int idxcur, idxm;
		
		/*! \brief sin and cos of twice the polarisaion angle  for calculation of amplitude components */
		double c2psi, s2psi;
		
		/*! \brief h_+ and h_x in the source reference frame */
		double shp, shc;
		
		// Vector for storing data 
		double *timevec;	// time
		double *mulvec;		// L[2]
		double *sphilvec;	// L[1]
		double *cphilvec;	// L[0]
		double *betavec;	// beta
		double *sigmavec;	// sigma
		double *thomasvec;	// thomas
		
		double *muly2;
		double *sphily2;
		double *cphily2;
		double *betay2;
		double *sigmay2;
		double *thomasy2;
		
		double *uspline;
		
		/*! \brief Runge-Kutta parameters */
		double **krk;
		int RK_VECLENGTH, RK_VECLENGTH_Old;
		double A2, A3, A4, A5, A6;
		double B21, B31, B32, B41, B42, B43, B51, B52, B53, B54, B61, B62, B63, B64, B65;
		double C1, C2, C3, C4, C5, C6;
		double D1, D2, D3, D4, D5, D6;
		double RK_H, RK_EPS;
		
		int NAmpShared;
		
		double tLastCompute;
		
	public:
		// ** Constructor **
		/*! \brief Constructs an instance and initializes it with default values.*/
		GWFastSpinBBH();
		
		/*! \brief Constructs an instance and initializes it with values in arguments.*/
		GWFastSpinBBH (double Toffset_n, double Tobs_n);
		
		/*! \brief Constructs an instance and initializes it with values in arguments.*/
		GWFastSpinBBH (double Beta_n,
					   double Lambda_n, 
					   double Mass1_n,                
					   double Mass2_n,                 
					   double CoalescenceTime_n,
					   double Distance_n,
					   double Spin1_n,
					   double Spin2_n,
					   double PolarAngleOfSpin1_n,
					   double PolarAngleOfSpin2_n,    
					   double AzimuthalAngleOfSpin1_n,
					   double AzimuthalAngleOfSpin2_n,
					   double PhaseAtCoalescence_n,
					   double InitialPolarAngleL_n,
					   double InitialAzimuthalAngleL_n,
					   double AmplPNorder_n,
					   double Toffset_n,
					   double Tobs_n,
					   double TaperApplied_n,
					   double TaperSteepness_n,
					   double Rmin_n); 
		
		// ** Destructor **
		~GWFastSpinBBH (); 
		
		// ** Access methods **
		
		/*! \brief Sets parameters specified by \iP.
		 *  Value \iP corresponding to :
		 *	\arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians)
		 *	\arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians)
		 *	\arg 2 -> #m1 : \f$ m_1 \f$ : Mass of object 1 (in solar masses)
		 *	\arg 3 -> #m2 : \f$ m_2 \f$ : Mass of object 2 (in solar masses)
		 *  \arg 4 -> #tc : \f$ t_{coal}\f$ : (in seconds)
		 *  \arg 5 -> #DL : \f$ r \f$ : Distance (in kpc)
		 *  \arg 6 -> #chi1 : \f$ S_1 \f$ : Spin of object 1
		 *  \arg 7 -> #chi2 : \f$ S_2 \f$ : Spin of object 2
		 *  \arg 8 -> #PolarAngleOfSpin1  : \f$ \theta_{S_1} \f$ : Polar angle of spin S1 (in radians)
		 *  \arg 9 -> #PolarAngleOfSpin2  : \f$ \theta_{S_2}\f$ : Polar angle of spin S2 (in radians)
		 *  \arg 10 -> #AzimuthalAngleOfSpin1  : \f$ \varphi_{S_1} \f$ : Azimuthal angle of spin S1 (in radians)
		 *  \arg 11 -> #AzimuthalAngleOfSpin2  : \f$ \varphi_{S_2} \f$ : Azimuthal angle of spin S2 (in radians)
		 *  \arg 12 -> #phic : \f$ \phi_c \f$ : Phase at coalescence (in raidans)
		 *  \arg 12 -> #InitialPolarAngleL : \f$ \theta_{L, 0} \f$ : Initial polar angle of orbital momentum (in raidans)
		 *  \arg 14 -> #InitialPolarAngleL : \f$ \varphi_{L, 0} \f$ : Initial azimuthal angle of orbital momentum (in radians)
		 */
		void setParam(int iP, double Param_n);
		double getParam(int iP);
		
		double getMass1() {return(m1);};
		double getMass2() {return(m2);};
		double getDistance() {return(DL);};
		// ...
		void setMass1(double M1_n);
		void setMass2(double M2_n);
		void setDistance(double r_n);
		void setAmplPNorder(double AmplPNorder_n);
		// ...	
		
		
		// **  Others methods **
		/*!  \brief Initialization of all pointers at NULL */
		void initNULL();
		
		/*!  \brief Allocaion of memory */
		void AllocMemory();
		
		
		/*! \brief Set Runge-Kutta parameters */
		void initRK();
		
		/*! \brief Initialisation (integration of precession and spline) */
		void init(); 

		/*! \brief Compute shared data for calcultion of amplitude */
		void AmpShared(double t);
		
		/*! \brief Compute wave amplitude component hplus */
		double hp(double t);   
		/*! \brief Compute wave amplitude component hcross */
		double hc(double t); 
		
		/*! \brief Display temporary values */
		void DispTempVal(double t, ostream * OutDisp);
		
		/*! \brief Compute frequency */
		double Freq(double t, double Mtot, double Mchirp, double eta, double beta, double sigma, double tc);
		
		/*! \brief Compute one step in tegration using Runge Kutta method at order 6 */
		void rkckstep(double outputvals[], double fourthorderoutputvals[], double h, double currentvals[], double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc);
		
		/*! \brief Calculate derivatives for one step of the Runge-Kutta method */
		void update(int j, double A, double h, double intvals[], double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc, double **k);
		
		/*! \brief Calculate derivatives of S1, S2 and L */
		void calcderivvals(double derivvals[], double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate derivative of component x of S1 */
		double S1xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate derivative of component y of S1 */
		double S1ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate derivative of component z of S1 */
		double S1zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate derivative of component x of S2 */
		double S2xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate derivative of component y of S2 */
		double S2ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate derivative of component z of S2 */
		double S2zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);
		
		/*! \brief Calculate scalar product L.S1 */
		double calcLdotS1(double inputs[]);
		
		/*! \brief Calculate scalar product L.S1 */
		double calcLdotS2(double inputs[]);
		
		/*! \brief Calculate scalar product L.S2 */
		double calcSdotS(double inputs[]);
		
		/*! \brief Calculate scalar product L.N */
		double calcLdotN(double inputs[], double nvec[]);
		
		/*! \brief Calculate scalar product LxN */
		void calcLcrossN(double output[], double inputs[], double nvec[]);
		
		/*! \brief Spline data : compute y2 */
		void spline(double *x, double *y, int n, double yp1, double ypn, double *y2);
				
	};


#endif // __GWFastSpinBBH_H

// end of LISACODE-GWFastSpinBBH.h
