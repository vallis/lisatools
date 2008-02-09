// $Id: LISACODE-Background.cpp,v 1.6 2007/03/29 15:16:59 lalanne Exp $
/*
 *  LISACODE-Background.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *
 *  Created on 30/05/06 by Antoine PETITEAU (APC)
 *  Last modification on 30/05/06 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-Background.h"

/* Constructeur */

/*! \brief Constructs an instance and initializes its attribute (#LISAGeo) with default values.
 *
 * \arg	LISAGeo = Geometry instance with default attributes.
 */
Background::Background()
{
	LISAGeo = new(Geometry);
	
}


/*! \brief Constructs an instance and initializes its attribute with input \a LISAGeo_n. 
 *
 * \arg	#LISAGeo = LISAGeo_n input
 */
Background::Background(Geometry * LISAGeo_n)
{
	LISAGeo = LISAGeo_n;
}


/*! \brief Destructor
 */
Background::~Background()
{
	
}


/* Methodes d'acces */ 
/*! \brief Sets #LISAGeo attribute with input.
 *
 * \arg	#LISAGeo =  \a LISAGeo_n input
 */
void Background::setGeometry(Geometry * LISAGeo_n)
{
	LISAGeo = LISAGeo_n;
}


/* Others methods */
/*! \brief Gives deltanu.
 *
 * Virtual unused method. \n
 * \return 0.0
 */
double Background::deltanu(int iSC, int IndirSens, double t)
{
	return(0.0);
}

// end of LISACODE-GW.cpp
