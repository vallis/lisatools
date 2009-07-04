// $Id: LISACODE-Serie.h,v 1.6 2007/04/05 09:32:42 ruiloba Exp $
/*
 *  LISACode-Serie2.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * 
 *
 *
 *  Created on 01/06/05 by Tania REGIMBAU  (ARTEMIS)
 *  Last modification on 18/05/09 by Antoine PETITEAU
 *
 */
/*!\ingroup mathTools
 *\defgroup serie2 Serie2
 * \{
 */
#ifndef __SERIE2_H
#define __SERIE2_H


#include <stdexcept>
#include <fstream.h>
#include <math.h>
#include <complex>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"

using namespace std;

/*!\brief Interpolation type*/

/*!\brief Serie2 interpolation class.
 */
class Serie2 
	{
	protected:
		/*!\brief Reference serie data. */
		double ** ys;
		/*!\brief Number of data. */
		int N;
		/*!\brief Maximal number of data. */
		int Nmax;
		/*!\brief Reference serie start value. */
		double x0;
		/*!\brief Reference serie step. */
		double dx;
		
		/*!\brief Table containing Used. */
		double * LagPolDen[ORDERMAXLAG];
		
		bool xrbinE;
		double xr;
		int bin;
		
	public:
		// ** Constructor **
		Serie2();
		Serie2(double start, double delta);
		Serie2(double start, double delta, int length);
		Serie2(double start, double delta, double ** ys_n, int N_n, int Nmax_n);
		
		// **  Destructor **
		~Serie2();
		
		// ** Access methods **
		/*!\brief Returns number of values (size of #ys attribute). */
		int getNbVal() const {return(N); };
		double getRefStep() const {return(dx);};
		void setRefStart(double start) {x0 = start;};
		void setRefStep(double delta) {dx = delta;};
		
		void setNmax(int Nmax_n);
		
		double getRef(int bin) const;
		double getBinValue(int bin) const;
		void setBinValue(int bin, double x);
		
		// **  Others methods **
		/*! \brief Add data at the begining and remove the last one if the maximal number of data is reached */ 
		void addData(double y);
		/*! \brief Allocation of all the memory with default value 0 for new data */ 
		void allocAll();
		/*! \brief Return the data for the reference x with the specified interpolation method. 
		 * InterpUtilValue is a value which can be necessary for make the interpolation 
		 * (For example for lagrange interpolation, InterpUtilValue is the order of the interpolation).
		 */
		double gData(double x, INTERP InterpolType, double InterpUtilValue);
		inline double TruncVal(double x); // Return truncated value
		inline double InterLinear(double x); // Return value obtained by linear interpolation 
		double InterCubic(double x); // Return value obtained by cubic interpolation
		double InterHermite(double x, double tension, double bias); // Return value obtained by hermite interpolation
		void InitLagPolDen(int order);
		inline double InterLagrange(double x, int order); // Return value obtained by lagrange interpolation
		//double InterSincTrunc(double x, int Nlenght) const; // Return value obtained by lagrange interpolation
	};

/*!\}*/
#endif // __SERIE2_H

// end of Serie2.h
