// $Id: $
/*
 *  LISACODE-GeometryAnalytic.h
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


#ifndef __GEOMETRYANALYTIC_H
#define __GEOMETRYANALYTIC_H


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
 * Compute spacecraft position from analytical formulations of the article :
 * S. V. Dhurandhar, K. Rajesh Nayak, S. Koshti, and Jean-Yves Vinet. Fundamentals
 * of the lisa stable flight formation. gr-qc, 0410093, 2004. 
 *
 */

class GeometryAnalytic : public Geometry
	{
	protected:
		/*!\brief 0 for staticLISA, 1 for rigid orbits and 2 for eccentric orbits */	
		/*!\var alpha
		 \brief Orbital parameter. */	
		/*!\var nu
		 \brief Orbital angular constant. */	
		/*!\var tmu
		 \brief Orbital constant. */	
		/*!\var cmu
		 \brief Orbital constant. */	
		/*!\var smu
		 \brief Orbital constant. */	
		/*!\var e
		 \brief Eccentricity. */	
		/*!\var sqrtee
		 \brief Eccentricity derived constant. */	
		/*!\var arot
		 \brief Phase between satellites : constant,  \f$ arot=\frac{2 \cdot \pi}{3} \f$. */	
		double alpha, nu, tmu, cmu, smu, e, sqrtee, arot;
		/*!\var rot
		 \brief Satellites phase. */	
		/*!\var crot
		 \brief Satellites phase cosinus. */	
		/*!\var srot
		 \brief Satellites phase sinus. */	
		vector<double> rot, crot, srot;	
		
	public:
		// Constructor
		GeometryAnalytic();
		GeometryAnalytic(double t0_n, double rot0_n);
		GeometryAnalytic(double t0_n, double rot0_n, double L0m_n);
		GeometryAnalytic(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n);
		GeometryAnalytic(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep);
		
		/* Destructor */
		~GeometryAnalytic();
		
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
	};

/*!\} */ 
#endif // __GEOMETRYANALYTIC_H

// end of GeometryAnalytic.h
