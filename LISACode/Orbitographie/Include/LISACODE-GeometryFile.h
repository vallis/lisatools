// $Id: $
/*
 *  LISACODE-GeometryFile.h
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


#ifndef __GEOMETRYFILE_H
#define __GEOMETRYFILE_H


#include <stdexcept>
#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
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

class GeometryFile : public Geometry
{
protected:
	/*!\var XYZ
		\brief Data of spacecraft's positions : [spacecrat(0->1,1->2,2->3)][component(0->x,1->y,2->z)][itime]*/	
	double ***XYZ;
	
	/*!\var Time_sec
		\brief Time data. */
	double *Time_sec;
	
	/*!\var NbData
		\brief Number of data. */
	int NbData;
	
	/*!\var InputFileName
		\brief Name of file which contains spacecraft's positions. */	
	char * InputFileName;
	
public:
		// Constructor
		GeometryFile();
	GeometryFile(double t0_n, double rot0_n);
	GeometryFile(double t0_n, double rot0_n, double L0m_n);
	GeometryFile(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep);
	GeometryFile(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep, char * InputFileName_n);
	
	/* Destructor */
	~GeometryFile();
	
	/* Access methods */
	void init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep, char * InputFileName_n);
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
#endif // __GEOMETRYFILE_H

// end of GeometryFile.h
