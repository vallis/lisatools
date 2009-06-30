// $Id: $
/*
 *  LISACODE-GeometryMLDC.h
 *  V 1.4.1
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 13/05/08 by A.Petiteau
 *  Last modification on 13/05/08 by A.Petiteau
 *
 */


#ifndef __GEOMETRYMLDC_H
#define __GEOMETRYMLDC_H


#include <stdexcept>
#include <iostream.h>
#include <math.h>
#include <vector.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Couple.h"
#include "LISACODE-Vect.h"
#include "LISACODE-Geometry.h"

using namespace std;


/*!\ingroup geometry*/
/** 
* Compute spacecraft position from :
*
* This are the orbits used for MLDC
*
*/

class GeometryMLDC : public Geometry
{
protected:
	double Approx;
	/*!\var rot
	\brief Satellites phase. */	
	/*!\var crot
	\brief Satellites phase cosinus. */	
	/*!\var srot
	\brief Satellites phase sinus. */	
	vector<double> rot, crot, srot;	
	/*!\brief excentricity for MLDC orbits */
	double e_mldc, L0s;
	/*!\brief Constants for MLDC orbits */
	double sqrt_3, i32, r1532, pi3o2, pi2o3, pi4o3;
	
public:
		// Constructor
		GeometryMLDC();
	GeometryMLDC(double t0_n, double rot0_n);
	GeometryMLDC(double t0_n, double rot0_n, double L0m_n);
	GeometryMLDC(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n);
	GeometryMLDC(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep);
	
	/* Destructor */
	~GeometryMLDC();
	
	/* Access methods */
	void init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep);
	/*!\brief Display informations */	
	void DispInfo();
	
	/* Other methods */	
	// Returns the cosinus and sinus of the eccentric anomaly for the time t (s) as argument and spacecraft number (1, 2 or 3)
	Couple exanom(int nb, double ts);
	// Returns the position of the spacecraft in the barycentric frame for the time t (s) as argument and spacecraft number (1, 2 or 3)
	Vect position(int nb, double t);
	// Returns the velocity of the spacecraft in the barycentric frame for the time t (s) as argument and spacecraft number (1, 2 or 3)
	Vect velocity(int nb, double t);
	/*! \brief  Returns delay compute specific formulation */
	double tdelaySpecific(int em, int rec, int order, double trec);
};

/*!\} */ 
#endif // __GEOMETRYMLDC_H

// end of GeometryMLDC.h
