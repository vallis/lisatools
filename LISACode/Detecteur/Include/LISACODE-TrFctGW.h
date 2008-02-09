// $Id: LISACODE-TrFctGW.h,v 1.8 2007/03/29 15:16:14 lalanne Exp $
/*
 *  LISACODE-TrFctGW.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 21/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/09/05 by A.Petiteau, T.Regimbau 
 *
 */

#ifndef __TRFCTGW_H
#define __TRFCTGW_H

#include <stdexcept>
#include <iostream>
#include <string.h>
#include <math.h>
#include "LISACODE-MathUtils.h"
#include "LISACODE-Geometry.h"
#include "LISACODE-GW.h"

using namespace std;
/*!\ingroup detect*/
/*!\brief Gravitational Waves Transfer Function class
 */

class TrFctGW
{
protected :
	//double lambda, beta, psi;
  /*!\brief   Gravitational Waves sources */
    vector<GW  *> * GWSources;
  /*!\brief LISA's geometry.  */
    Geometry * LISAGeo;
    /*!\var u
     *  \brief Unit transverse vector. */
    /*!\var v
     *  \brief Unit transverse vector. */
    /*!\var k
     *  \brief (k,u,v) is direct trihedron. */
    vector<Vect> u,v,k;
	
	
	
public :
		/* Constructor */
		TrFctGW();
	//Takes as arguments:
	// the location of the TrFctGW in the barycentric system (angles theta, phi)
	// computes the unit vector w connecting the Sun to the TrFctGW and the unit transverse vectors u and v
	   TrFctGW(vector<GW *> * GWSources_n, Geometry * LISAGeo_n);
	   
	   /* Destructor */
	   ~TrFctGW (); 
	   
	   /* Access methods */
	   void init(vector<GW *> * GWSources_n, Geometry * LISAGeo_n);
	   
	   /* Others methods */
	   // Retuns the frequency fluctuation due to GW
	   // Takes as argument the link, the TrFctGW and the gw time series h+ and hx 
	   // See formula given by gr-qc/01112059 at the bottom of p13
	   double deltanu(int rec, int em, int order, double trec);
	   
};

#endif // __TRFCTGW_H

// end of LISACODE-TrFctGW.h

