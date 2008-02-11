// $Id: $
/*
 *  LISACODE-Geometry.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/12/07 by E.Plagnol
 *
 */
/*!\defgroup geometry Geometry (directory Orbitographie)
 * \{
*/

#ifndef __GEOMETRY_H
#define __GEOMETRY_H


#include <stdexcept>
#include <iostream.h>
#include <math.h>
#include <vector.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Couple.h"
#include "LISACODE-Vect.h"
using namespace std;


/*!\brief Orbit geometry class.	
 *
 */

class Geometry
{
protected:
	/*!\brief Nominal arm length in meters.
	*
	* Nominal distance between two satellites.
	*
	*/	
	double L0m;
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
	/*!\var t0
		\brief Initial time (seconds)*/	
	/*!\var rot0
		\brief Initial rotation (radians)*/	
	double t0, rot0; // Initial time (seconds) and initial rotation (radians)
	/*!\var rot
		\brief Satellites phase. */	
	/*!\var crot
		\brief Satellites phase cosinus. */	
	/*!\var srot
		\brief Satellites phase sinus. */	
	vector<double> rot, crot, srot;
	/*!\brief 0 for LISA fixed, 1 for LISA moved (default)*/	
	int move; // 0 for LISA fixed, 1 for LISA moved (default)
	/*!\brief 1 for Vinet orbits, 2 for MLDC orbits (this set by move_n) (e.p.)*/	
	int orbit_type; 
	/*!\brief excentricity for MLDC orbits  (e.p.)*/
	double e_mldc;
	/*!\brief sqrt(3) for MLDC orbits (e.p.)*/
	double sqrt_3;	
	/*!\brief if -1 read the specified order else by-pass the specified order*/	
	int order_default; // if -1 read the specified order else by-pass the specified order
	/*!\brief Stored positions*/	
	Vect SCposStore[3]; // Stored positions
	/*!\brief last position computation time. */	
	double tStorePos; // Time at storage
	/*!\brief last position storage time. */	
	double tRangeStorePos;
	/*!\brief Stored delays for each one of six arms.
		*
		* Each arm corresponds to a pair of emitter (em) and receiver (rec):
		\arg arm 2: (em,rec)=(1,3)
		\arg arm 3: (em,rec)=(2,1)
		\arg arm 1: (em,rec)=(3,2)
		\arg arm 6: (em,rec)=(1,2)
		\arg arm 5: (em,rec)=(2,3)
		\arg arm 4: (em,rec)=(3,1)
		*/
	double DelayStore[6]; // Stored delays arm(2,3,1,6,4,5) or em->rec(13,21,32,12,23,31)
	/*!\brief Last delay computation time. */	
	double tStoreDelay; // Time at storage
	/*!\brief Last delay storage time. */	
	double tRangeStoreDelay;
	
public:
		// Constructor
		Geometry();
	Geometry(double t0_n, double rot0_n);
	Geometry(double t0_n, double rot0_n, double L0m_n);
	Geometry(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n);
	Geometry(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep);
	
	/* Destructor */
	~Geometry();
	
	/* Access methods */
	void init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep);
	/*!\brief Returns #L0m attribute*/	
	double getL0(){return(L0m);};
	/*!\brief Returns #t0 attribute*/	
	double gett0(){return(t0);};
	
	/* Other methods */	
	// Takes the time t (s) as argument and spacecraft number (1, 2 or 3)
	// Returns the cosinus and sinus of the eccentric anomaly 
	Couple exanom(int nb, double ts);
	// Takes the time t (s) as argument and spacecraft number (1, 2 or 3)
	// Returns the position of the spacecraft in the barycentric frame 
	Vect position(int nb, double t);
	Vect velocity(int nb, double t);
	// Get delay for specified order (1/2 -> order=1 and 1 -> order=2)
	double tdelay(int em, int rec, int order, double trec);
	// Get contribution of specified order (1/2 -> order=1 and 1 -> order=2) in delay
	double tdelayOrderContribution(int em, int rec, int order, double trec);
	// Get spacecrafts relative velocity along an arm
	double ArmVelocity(int em, int rec, double trec);
	Vect VectNormal(double t);
	// Give direct value for position
	Vect gposition(int nb, double t);
	double gtdelay(int em, int rec, int order, double trec);
};

/*!\} */ 
#endif // __GEOMETRY_H

// end of Geometry.h