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
#define CVSHEADER "$Header: /afs/aeiw/cvsroot/waves/people/stas/LISAWP/waveforms/include/BaseIntegr.hh,v 1.1 2007/09/18 21:56:09 stba Exp $" 
*/

#ifndef BASEINTEGRHH
#define BASEINTEGRHH

#include <iostream>
#include "Matrix.hh"

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
 /** Integration routine, this code uses Euler simple advance 
         method to advance the solution over dt.
  	@param ystart initial values of functions
	@param dydx derivatives of functions
	@param yend final value of functions advances by dt
	@param tstart initial coordinate
	@param dt step
	@param n number of ODE (1st order)
  */


  void SimplestIntegrator(Matrix<double> ystart, Matrix<double> dydx,
                Matrix<double>& yend, double tstart, double dt, int n);

  
  };


}
#endif

