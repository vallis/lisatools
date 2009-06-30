// $Id: LISACODE-LISAConstants.h,v 1.5 2007/04/20 12:21:53 ruiloba Exp $
/*
 *  LISACODE-LISAConstants.h
 *  V 1.4
 *  LISACode
 *
 * Description :
 * ----------
 * Caracteristic constant of LISA. 
 *
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 19/09/05 by A.Petiteau, T.Regimbau 
 *
 */

/*! \file LISACODE-LISAConstants.h
 * \brief Physical constants of LISA instrument.
 *
 */

#ifndef __LISACONSTANTS_H
#define __LISACONSTANTS_H

#include <math.h>

#include "LISACODE-PhysicConstants.h"

//Version
/*!\brief Simulator Version */
const char LCVersion[] = "1.4.5"; // Version du simulateur
const char DateOfLastUpdate[] = "16/06/2009"; // Version du simulateur

//Geometrie
/*!\brief Arms length (distance between every pair of satellites) in meters.*/
const double L0_m_default = 5.0e9; // Longueur des bras en m
/*!\brief Distance between the LISA barycenter and the Sun.*/
const double Rgc = au_m; // Distance du barycentre de LISA au Soleil 
/*!\brief Angular velocity. */
const double omega = 2.*M_PI/Yr_SI; // angular velocity
/*!\brief Default time step for LISA geometry positions computation.
 */
const double tRangeStorePos_default = 10.0; // Maximum difference between time and store time for position storage
/*!\brief Default time step for LISA delays computation.
 */
const double tRangeStoreDelay_default = 10.0; // Maximum difference between time and store time for delay storage

//Optique
/*!\brief Nominal lasers wave length  in meters.*/
const double la0Laser_m = 1.064e-6; // Longueur d'onde nominale des lasers en m
/*!\brief Nominal lasers frequency in Hz.*/
const double nu0Laser_Hz = c_SI/la0Laser_m; // Frequence nominale des lasers en Hz
/*!\brief Lasers power in Watts.*/
const double LaserPower_W_default = 1 ; // Puissance du laser en Watt

#endif // __LISACONSTANTS_H

//end of LISACODE-LISAConstants.h
