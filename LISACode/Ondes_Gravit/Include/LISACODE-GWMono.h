// $Id: LISACODE-GWMono.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWMono.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) donne les parametres de l'onde gravitationnelle a chaque instant.
 * Typiquement h_plus et h_croix
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
 *
 */

#ifndef __GWMONO_H
#define __GWMONO_H
 

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
/*! \brief Gravitational Waves instantaneous parameters h_plus and h_cross are described in this class.
 */
class GWMono : public GW
{
 protected:
/*! \brief Frequency */
    double Freq; // Frequency
/*! \brief h_plus polarisation component amplitude  */
    double Amplhp; // Amplitude of component h+  
/*! \brief h_cross polarisation component amplitude */
    double Amplhc; // Amplitude of component hx
/*! \brief Polarisation angle (rad)
 *
 * Angle between the projection of x (vernal point direction)
 * in the wave frame and the polarisation vector
 */
	double AnglPol; // Angle between the projection of x (direction of vernal point) in the wave frame and the polarisation vector in radians

/*! \brief Initial phase of h_plus polarisation component.  */
	double Phi0hp; // Initial phase for h+ component
/*! \brief Initial phase of h_cross polarisation component.  */
	double Phi0hc; // Initial phase for hx component	
  
 public:
  /* Constructor */
  GWMono();
	GWMono(	double Beta_n,
			double Lambda_n,
			double AnglPol_n,
			double Freq_n,
			double Amplhp_n,
			double Amplhc_n);
	GWMono(	double Beta_n, 
			double Lambda_n, 
			double AnglPol_n,
			double Freq_n,
			double Amplhp_n,
			double Amplhc_n,
			double Phi0hp_n,
			double Phi0hc_n);
  
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
/*! \brief Returns #AnglPol attribute */
  double getAnglPol() const {return(AnglPol);};
  void setAnglPol(double AnglPol_n);
/*! \brief Returns #Phi0hp attribute */
  double getPhi0hp() const {return(Phi0hp);};
/*! \brief Sets #Phi0hp attribute */
  void setPhi0hp(double Phi0hp_n) { Phi0hp = Phi0hp_n;};
/*! \brief Returns #Phi0hc attribute */
  double getPhi0hc() const {return(Phi0hc);};
/*! \brief Sets #Phi0hc attribute */
  void setPhi0hc(double Phi0hc_n) { Phi0hc = Phi0hc_n;};
  
  /*  Others methods */
  double hp(double t); //Return h_plus  
  double hc(double t); //Return h_croix

};

#endif // __GWMONO_H

// end of LISACODE-GWMono.h

