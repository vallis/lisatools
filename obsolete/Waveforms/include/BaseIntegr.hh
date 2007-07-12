/*
Copyright (C) 2005  S. Babak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/******************  CVS info ************************ 
#define CVSTAG "$Name:  $"
#define CVSHEADER "$Header: /afs/.aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/waveforms/include/BaseIntegr.hh,v 1.4 2006/05/17 11:03:03 stba Exp $" 
*/

#ifndef BASEINTEGRHH
#define BASEINTEGRHH

#include <iostream>
#include "Matrix.hh"
//#include <gsl/gsl_errno.h>
//#include <gsl/gsl_odeiv.h>

namespace LISAWP{

/// This is the virtual base class which will be used by majority 
/// of codes which need to use integration of ODE.

  class BaseIntegr{
 public:
  /** Default constructor */
  BaseIntegr(){}
  /** virtual destructro */
  virtual ~BaseIntegr(){}
 
 protected:
 
  /** Computes Derivatives  
  	@param x initial coordinate
	@param y function at x
	@param dydx derivative of function at x
	@param n number of ODE (1st order)
  */
  

  virtual void Derivs(double x, Matrix<double> y, Matrix<double>& dydx, int n) = 0;

  /** Integration routine, this code uses Cash-Karp Runge-Kutta 
  method to advance the solution over dt.
  	@param ystart initial values of functions
	@param dydx derivatives of functions
	@param yend final value of functions advances by dt
	@param tstart initial coordinate
	@param dt step
	@param n number of ODE (1st order)
  */

  double IntegratorCKRK(Matrix<double> ystart, Matrix<double> dydx,
                Matrix<double>& yend, double tstart, double dt, int n);


  /** Integration routine based on Runge-Kutta-Fehlberg(4,5) method. Uses gsl function
   * @param ystart starting vaues of functions
   * @param dydx_in input derivatives (at tstart)
   * @param yend final values of functions at tstart+step
   * @param tstart starting point
   * @param step advance step
   * @param dydx_out derivatives at the final point
   * @param yerr errors of estimation y at final point
   * @param n number of first order ODE
   */
  
  void IntegratorRKF45(Matrix<double> ystart, Matrix<double> dydx_in, Matrix<double>& yend, \
		  double tstart, double step, Matrix<double>& dydx_out, Matrix<double>& yerr,\
		  int n);

 /** Function required for gsl integrator */
  int func(double t, const double y[], double f[], void *params);

  int jac(double t, const double y[], double * dfdy, double dfdt[], void *params);
  
  };


}
#endif

