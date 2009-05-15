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
#include <vector.h>
#include <time.h>
#include <iostream>

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
		
		/*!\brief Return time in convention ISO-8601 */
		static char * TimeISO8601()
		{
			time_t rawtime;
			static char strTime[22];
			
			time ( &rawtime );
			
			strcpy(strTime,ctime(&rawtime));
			
			// * Year
			strTime[0] = ctime(&rawtime)[20];
			strTime[1] = ctime(&rawtime)[21];
			strTime[2] = ctime(&rawtime)[22];
			strTime[3] = ctime(&rawtime)[23];
			strTime[4] = '-';
			
			// * Month
			strTime[5] = '0';
			if(strncmp(ctime(&rawtime)+4,"Jan",3) == 0)
				strTime[6] = '1';
			if(strncmp(ctime(&rawtime)+4,"Feb",3) == 0)
				strTime[6] = '2';
			if(strncmp(ctime(&rawtime)+4,"Mar",3) == 0)
				strTime[6] = '3';
			if(strncmp(ctime(&rawtime)+4,"Apr",3) == 0)
				strTime[6] = '4';
			if(strncmp(ctime(&rawtime)+4,"May",3) == 0)
				strTime[6] = '5';
			if(strncmp(ctime(&rawtime)+4,"Jun",3) == 0)
				strTime[6] = '6';
			if(strncmp(ctime(&rawtime)+4,"Jul",3) == 0)
				strTime[6] = '7';
			if(strncmp(ctime(&rawtime)+4,"Aug",3) == 0)
				strTime[6] = '8';
			if(strncmp(ctime(&rawtime)+4,"Sep",3) == 0)
				strTime[6] = '9';
			if(strncmp(ctime(&rawtime)+4,"Oct",3) == 0){
				strTime[5] = '1';
				strTime[6] = '0';
			}
			if(strncmp(ctime(&rawtime)+4,"Nov",3) == 0){
				strTime[5] = '0';
				strTime[6] = '1';
			}
			if(strncmp(ctime(&rawtime)+4,"Dec",3) == 0){
				strTime[5] = '0';
				strTime[6] = '2';
			}
			
			// * Day
			strTime[7] = '-';
			if(ctime(&rawtime)[8] == ' ')
				strTime[8] = '0';
			
			strTime[10] = 'T';
			
			strTime[19] = '\0'; 
			
			//cout << ctime(&rawtime) << endl;
			
			return(strTime);
			
		}
	};
/*!\} */
#endif //__MATHUTILS_H

// end of LISACODE-MathUtils.h


