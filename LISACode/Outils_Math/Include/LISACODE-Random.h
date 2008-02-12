// $Id: LISACODE-Random.h,v 1.4 2007/03/27 13:34:02 lalanne Exp $
/*
 *  LISACODE-Random.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  Wrapper for Mersenne twister random generator
 *  2001 Agner Fog. GNU General Public License www.gnu.org/copyleft/gpl.html 
 *  http://www.aner.org/random
 *
 *
 *  Created on 05/12/05 by  T.Regimbau
 *  Last modification on 05/12/05 by T.Regimbau 
 *
 */
/*!\ingroup mathTools
  *\defgroup random RandomMT
  * \{
*/
#ifndef __LISACODERANDOM_H
#define __LISACODERANDOM_H

#include <stdexcept>
#include <iostream>
#include <math.h>
#include <time.h>
#include "randomc.h"
//#include "random/userintf.cpp"
//#include "random/mersenne.cpp"
/*!\var RANDOM_GENERATOR
  \brief Mersenne twister random generator class.
 */
#define RANDOM_GENERATOR TRandomMersenne
#include "stocc.h"
//#include "random/stoc1.cpp"
#include "LISACODE-Serie.h"
using namespace std;



/*!\brief
 * Mersenne twister random generator class.
 * 
 */
class RandomMT
{
 protected:
  /*!\brief
   *  seed
   */
       int32 seedMT;	
 public:	
	
       //Constructors by default {0}
        RandomMT();
	RandomMT(int32); 
        //Destructor
	~RandomMT();

        /*  Others methods */
        double UniformMT(double,double);
        Serie UniformMTSerie(double,double,int,double,double);
        SerieC UniformMTSerieC(double,double,int,double,double);

        double NormalMT(double,double);
        Serie NormalMTSerie(double,double,int,double,double);
        SerieC NormalMTSerieC(double,double,int,double,double);
};


/*!\} */
#endif //__RANDOM_H

// end of LISACODE-Random.h
