// $Id: LISACODE-GWPeriGate.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWPeriGate.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) donne h_plus et h_croix sous la forme d'un signal porte periodique.
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
 *
 */

#ifndef __GWPERIGATE_H
#define __GWPERIGATE_H


#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
*/
/*! \brief Gravitational Waves periodic gate signal.
 *
 * h_plus and h_cross polarisation componenrs are modelised as periodic gate signals.
 */
class GWPeriGate : public GW
{
protected:
/*! \brief Frequency */
    double Freq; //Frequence de l'onde 
/*! \brief hp component amplitude  */
    double Amplhp; //Amplitude de la composante h+ de l'onde
/*! \brief hc component amplitude */
    double Amplhc; //Amplitude de la composante hx de l'onde
/*! \brief Polarisation angle (rad)
 *
 * Angle between the projection of x (vernal point direction)
 * in the wave frame and the polarisation vector
 */
	double AnglPol; //Angle entre la projection de x (direction du point vernal) dans le plan de l'onde et i (vecteur de polarisation de l'onde) en radians 
	
	
public:
	/* Constructor */
	GWPeriGate();
	GWPeriGate(double Beta_n, double Lambda_n, double AnglPol_n, double Freq_n, double Amplhp_n, double Amplhc_n);
	
	/* Access methods */
/*! \brief Returns #Freq attribute */
	double getFreq() const {return(Freq);};
	void setFreq(double Freq_n);
/*! \brief Returns #Amplhp attribute */
	double getAmplhp() const {return(Amplhp);};
	void setAmplhp(double Amplhp_n);
/*! \brief Returns #Amplhc attribute */
	double getAmplhc() const {return(Amplhc);};
	void setAmplhc(double Amplhc_n);
	
	/*  Others methods */
	double hp(double t); //Return h_plus  
	double hc(double t); //Return h_croix
	
};

#endif // __GWPERIGATE_H

// end of LISACODE-GWPeriGate.h

