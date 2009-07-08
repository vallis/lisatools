/*!\ingroup mathTools
 *\defgroup ellipFilter Elliptic Filter
 *\{
 */
#ifndef _ELLIPTICFILTER_H_
#define _ELLIPTICFILTER_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex>
using namespace std;

#define alog(A)   log(A)
#define alog10(A) log10(A)

/*!\brief Elliptic cell structure	
 */

// Structure holding a elliptic cell
// H(Z) = a0*(1+a1Z-1+Z-2)/(1+b1Z-1+b2Z-2)
/************************************/
typedef struct {
/*!\var a0
  \brief Scale factor
*/
/*!\var a1
  \brief First direct coefficient divided by scale factor
*/
  double a0, a1;
/*!\var b1
  \brief First recursive coefficient
*/
/*!\var b2
  \brief Second recursive coefficient
*/
  double b1, b2;
/*!\var zero
  \brief Complex value corresponding to zero (0)
*/
/*!\var pole
  \brief Complex value corresponding to pole
*/
  complex<double> zero, pole;
/*!\var u
  \brief Memory
*/
  double u[2];
} QuadCell;
/************************************/

/*!\brief Pure imaginary=(0,1)	
 */
/************************************/
static complex<double> Im(0,1);
/************************************/


// Prototypes
/**************************************************/
void elli(double eps, double A, double wr, double wc, double fe,
	  int NCellMax,
	  int *NCells,
	  complex<double> poles[], complex<double> zeros[],
	  double CoefA[], double CoefB[], double CoefC[], double CoefD[]);
double ak(double y);
double cak(double y);
double sn(double y, double A, double ak1, double ak3);
double FilterQuadCell(double xn, QuadCell *Cell);


double FilterQuadCellChain(double xn, int NCells, QuadCell Cell[]);

complex<double> TransfZQuadCell(complex<double> Z, QuadCell Cell);

complex<double> TransfZQuadCellChain(complex<double> Z, int NCells, QuadCell Cell[]);

double AbsRespFunctQuadCell(double f, QuadCell Cell);
  
double AbsRespFunctQuadCellChain(double f, int NCells, QuadCell Cell[]);

double HmQuadCell(QuadCell Cell);

double KmQuadCell(QuadCell Cell);

void PoleMatching(int NCells, QuadCell Cell[]);
void OrderCellMaxNorm(int NCells, QuadCell Cell[]);
double CalcScalingFact(int NCells, QuadCell Cell[]);
double CalcEllipticFilter(double fe,    // sampling frequency [Hz]
			  double at,    // attenuation [dB]
			  double bp,    // oscillations in bandwidth [dB]
			  double fb,    // low transition frequency [Hz]
			  double fa,    // high transition frequency [Hz]
			  int NCellMax, // maximum number of cells
			  QuadCell **FilterCellsOut,  // Output cells
			  int *NCellsOut   // output number of cells
			  );
void CalcEllipticFilter4LISACode(double fe,    // sampling frequency [Hz]
				 double at,    // attenuation [dB]
				 double bp,    // oscillations in bandwidth [dB]
				 double fb,    // low transition frequency [Hz]
				 double fa,    // high transition frequency [Hz]
				 int NCellMax, // maximum number of cells
				 double CellsCoef[][5], // Cells coefficients
				 int *NCellsOut   // output number of cells
				 );
/***************************************************/
/*! \} */
#endif
