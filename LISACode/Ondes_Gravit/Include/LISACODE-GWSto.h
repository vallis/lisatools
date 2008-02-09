/*
 *  LISACode-GWSto
 *  stochastique
 *
 *  Created by Auger Gerard on 22/10/07.
 *  Copyright 2007 APC. All rights reserved.
 *
/*
 *  LISACODE-GWsto.h
 *  V 1.3.1
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) donne les parametres d'une onde gravitationnelle a chaque instant.
 * Cette onde est sensé provenir de ?? avec h_plus en f^n et h_croix en f^n.
 * A chaque demande H+ et Hx sont fournient. 
 *
 *
 *
 *
 */

#ifndef __GWSto_H
#define __GWSto_H
 

#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-GW.h"
#include "LISACODE-Noise.h"
#include "LISACODE-NoiseWhite.h"
#include "LISACODE-NoiseFilter.h"
#include "LISACODE-NoiseFile.h"
#include "LISACODE-NoiseOof.h"
//#include "LISACODE-NoiseFilter_Sto.h"

using namespace std;
/*!\ingroup gw
*/
/*! \brief Gravitational Waves instantaneous parameters h_plus and h_cross are described in this class.*/


 
class GWSto : public GW
{
 protected:
 
 double tStep;
 double tDurAdd;
 double tFirst;
 double tLast;
 double Slope;
 double Tsample; 
 double Fknee; 
 double Fmin;
 int Nb_Ageing;
 double Fac_Hp;
 double Fac_Hc;
 double Fac_norm;
 
 double tder_hp,tder_hc; 
 
 Noise * NFhp;
 Noise * NFhc;

 int OrderLa ;
 //double tDelMax;

 public:
  /* Constructor */
	GWSto();
	
/*  Beta_n       : In Beta and Lambda of the source.
  Lambda_n		 :
  tStep_n        : In Sample
  Fknee_n        : In Minimum value where the shape = 1/f^3. 
  Fmin_n         : In Maximum value where the shape = 1/f^3.
  Fac_Hp_n       : In Multiplicatif parameter on the amplitude of Hp.
  Fac_Hc_n       : In Multiplicatif parameter on the amplitude of Hc.
*/	

	GWSto(	double Beta_n,
			double Lambda_n,
			double tStep_n,
			double Slope_n, 
			double Fknee_n , 
			double Fmin_n,
			double Fac_Hp_n,
			double Fac_Hc_n);


/* Cree une onde avec un bruit en f^3.
  Beta_n         : IN Beta and Lambda of the source.
  Lambda_n		 :
  tStep_n        : In Sample.
  tDurAdd_n      : In Number of events created with AddNoise.
  tFirst_n       : In Parameter for AddNoise.
  tLast_n        : In  Parameter for AddNoise.
  tder_n         : In  Parameter for AddNoise.
  Slope_n        : In Slope of f^Slope_n.
  Tsample_n      : In In general = tStep_n.
  Fknee_n        : In Minimum value where the shape = f^Slope.
  Fmin_n         : In Maximum value where the shape = f^Slope.
  Nb_Ageing_n    : Number of events for sabilization.
  Fac_Hp_n       : In Multiplicatif parameter on the amplitude of Hp.
  Fac_Hc_n       : In Multiplicatif parameter on the amplitude of Hc.
 */
			
	GWSto( 	double Beta_n,
			double Lambda_n,
			double tStep_n,
			double tDurAdd_n,
			double tFirst_n,
			double tLast_n,
			double tder_n,
			double Slope_n, 
			double Tsample_n, 
			double Fknee_n , 
			double Fmin_n,
			int Nb_Ageing_n,
			double Fac_Hp_n,
			double Fac_Hc_n);
  
  /* Access methods */
  /*  Others methods */
  double hp(double t); //Return h_plus  
  double hc(double t); //Return h_croix

};

#endif // __GWSto_H
// end of LISACODE-GWSto.h


