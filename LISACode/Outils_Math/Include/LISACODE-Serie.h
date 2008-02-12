// $Id: LISACODE-Serie.h,v 1.6 2007/04/05 09:32:42 ruiloba Exp $
/*
 *  LISACode-Serie.h
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
 *  Last modification on 15/12/05 by Tania REGIMBAU
 *
 */
/*!\ingroup mathTools
 *\defgroup serie Serie
 * \{
*/
#ifndef __SERIE_H
#define __SERIE_H


#include <stdexcept>
#include <fstream>
#include <iostream>
#include <math.h>
#include <complex>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"

using namespace std;

/*!\brief Interpolation type*/

enum INTERP{TRU, /*!<Truncated interpolation*/
	LIN,  /*!<Linear interpolation*/
	CUB,  /*!<Cubic interpolation*/
	LAG,  /*!<Lagrange interpolation*/
	SIN /*!<Truncated sinc interpolation*/
}; 
/*!\brief Serie interpolation class.
 */
class Serie 
{
protected:
/*!\brief Reference serie data. */
    vector<double> ys; //
/*!\brief Reference serie start value. */
	double x0; //Start value of reference serie
/*!\brief Reference serie step. */
	double dx; //Step of reference serie
	
public:
		/* Constructor */
		Serie();
	Serie(double start, double delta);
	Serie(double start, double delta, int length);
	Serie(double start, double delta, vector<double> ys_n);
	Serie(char * fname);
	
	/*  Destructor */
	~Serie();
	
	/* Access methods */
/*!\brief Returns number of values (size of #ys attribute). */
	int getNbVal() const {return(int(ys.size())); };
	void setNbVal(int lenght);
	void setRefStart(double start);
/*!\brief Returns #dx attribute. */
	double getRefStep() const {return(dx);};
	void setRefStep(double delta);
	double getRef(int bin) const;
	double getBinValue(int bin) const;
	void setBinValue(int bin, double x);
	
	
	/*  Others methods */
	void addData(double y); //Add data at the begining of the serie
	void delLastData(); // Delete the last data of the serie
	void delLastData(double xMax); // Delete the last data of the serie which the reference is than xMax
	void wfile(char* name); // Write the serie to a file as two columns X, Y
	void rfile(char* name); // Read the serie to a file as two columns X, Y
	double gData(double x, INTERP InterpolType, double InterpUtilValue) const;
	// Return the data for the reference x with the specified interpolation method. 
	// InterpUtilValue is a value which can be necessary for make the interpolation 
	// (For example for lagrange interpolation, InterpUtilValue is the order of the interpolation).  
	double TruncVal(double x) const; // Return truncated value
	double InterLinear(double x) const; // Return value obtained by linear interpolation 
	double InterCubic(double x) const; // Return value obtained by cubic interpolation
	double InterHermite(double x, double tension, double bias) const; // Return value obtained by hermite interpolation
	double InterLagrange(double x, int order) const; // Return value obtained by lagrange interpolation
	//double InterSincTrunc(double x, int Nlenght) const; // Return value obtained by lagrange interpolation
};


/*!\brief complex serie interpolation class.
 */
class SerieC 
{
protected:
/*!\brief Reference serie data. */
    vector<complex<double> > ys; //
/*!\brief Reference serie start value. */
	double x0; //Start value of reference serie
/*!\brief Reference serie step. */
	double dx; //Step of reference serie
	
public:
		/* Constructor */
		SerieC();
	SerieC(double start, double delta);
	SerieC(double start, double delta, int length);
	SerieC(double start, double delta, vector<complex<double> > ys_n);
	SerieC(char * fname);
	
	/*  Destructor */
	~SerieC();
	
	/* Access methods */
/*!\brief Returns number of values (size of #ys attribute) */
	int getNbValC() const {return(int(ys.size())); };
	void setNbValC(int lenght);
	void setRefStartC(double start);
/*!\brief Returns #dx attribute */
	double getRefStepC() const {return(dx);};
	void setRefStepC(double delta);
	double getRefC(int bin) const;
	complex<double> getBinValueC(int bin) const;
	void setBinValueC(int bin, complex<double> x);
	
	
	/*  Others methods */
	void addDataC(complex<double> y); //Add data at the begining of the serie
	void delLastDataC(); // Delete the last data of the serie
	void delLastDataC(double xMax); // Delete the last data of the serie which the reference is than xMax
	void wfileC(char* name); // Write the serie to a file as two columns X, Y
	void rfileC(char* name); // Read the serie to a file as two columns X, Y
	
};

/*!\}*/
#endif // __SERIE_H

// end of Serie.h



