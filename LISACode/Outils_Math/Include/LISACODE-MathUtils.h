// $Id: LISACODE-MathUtils.h,v 1.5 2007/04/05 09:32:42 ruiloba Exp $
/*
 *  LISACODE-MathUtils.h
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 21/05/05 by Antoine PETITEAU (APC), Tania REGIMBAU (ARTEMIS)
 *  Last modification on 01/01/06 by Antoine PETITEAU (APC)
 *
 */

/*! \defgroup mathTools Mathematical Tools (directory Outils_Math)
*
*/
/*!\ingroup mathTools
 *\defgroup mathUtils Angles handling
 *\{
 */
#ifndef __MATHUTILS_H
#define __MATHUTILS_H

#include <math.h>
#include <vector>


#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


/*!\brief Angle conversion class.
 * 
 */
class MathUtils
{
public:
	
/*!\brief Angle conversion (from degrees to radians) */
	static double deg2rad( double angle_ )
{
		return(angle_*M_PI/180.);
}


/*!\brief Angle conversion (from radians to degrees) */
static double rad2deg( double angle_ )
{
	return(angle_*180/M_PI);
}
};
/*!\} */
#endif //__MATHUTILS_H

// end of LISACODE-MathUtils.h


