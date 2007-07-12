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
#define CVSHEADER "$Header: /afs/.aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/waveforms/src/BaseIntegr.cc,v 1.4 2006/05/17 11:03:03 stba Exp $" 
*/

#include "BaseIntegr.hh"

/** Base Integrator class, to be inherited by classes required integration
@author Stas Babak, September 2005.
*/

namespace LISAWP{


//void BaseIntegr::Derivs(double x, Matrix<double> y, Matrix<double>& dydx, int n){
   
   // This is an example which will be overloaded

  // dydx(0) = 1.;
  // dydx(1) = x*y(1);

//}

double BaseIntegr::IntegratorCKRK(Matrix<double> ystart, Matrix<double> dydx,
		Matrix<double>& yend, double tstart, double dt, int n){


  int i;
  double newstep;
  Matrix<double> a(1,7);
  Matrix<double> bb(7,6);
  Matrix<double> cc(1,7);
  Matrix<double> dc(1,7);
  a = 0.0;
  bb = 0.0;
  cc = 0.0;
  dc = 0.0;
  a(2)=0.2; a(3)=0.3; a(4)=0.6; a(5)=1.0; a(6)=0.875;
  bb(2,1)=0.2;
  bb(3,1)= 3.0/40.0; bb(3,2)= 9.0/40.0;
  bb(4,1)=0.3; bb(4,2) = -0.9; bb(4,3) = 1.2;
  bb(5,1)=-11.0/54.0; bb(5,2)=2.5; bb(5,3)=-70.0/27.0; bb(5,4)=35.0/27.0;
  bb(6,1)=1631.0/55296.0; bb(6,2)=175.0/512.0; bb(6,3)=575.0/13824.0;
  bb(6,4)=44275.0/110592.0; bb(6,5)=253.0/4096.0;
  cc(1)=37.0/378.0; cc(3)=250.0/621.0; cc(4)=125.0/594.0; cc(6)=512.0/1771.0;
  dc(5)=-277.0/14336.0; dc(1)=cc(1)-2825.0/27648.0; dc(3)=cc(3)-18575.0/48384.0;
  dc(4)=cc(4)-13525.0/55296.0; dc(6)=cc(6)-0.25;

  
  Matrix<double> ak2(1,n);
  Matrix<double> ak3(1,n);
  Matrix<double> ak4(1,n);
  Matrix<double> ak5(1,n);
  Matrix<double> ak6(1,n);
  Matrix<double> ytemp(1,n);
  Matrix<double> yerr(1,n);

  for(i=0; i<n; i++){                                     // first step
     ytemp(i) = ystart(i) + bb(2,1)*dt*dydx(i);
  }
  Derivs(tstart+a(2)*dt, ytemp, ak2, n);
  for(i=0; i<n; i++){                                   // second step
     ytemp(i) = ystart(i) + dt*( bb(3,1)*dydx(i) + bb(3,2)*ak2(i) );
  }
  Derivs(tstart+a(3)*dt, ytemp, ak3, n);
  for(i=0; i<n; i++){                                   // third step
     ytemp(i) = ystart(i) + dt*( bb(4,1)*dydx(i) + bb(4,2)*ak2(i) + bb(4,3)*ak3(i) );
  }
  Derivs(tstart+a(4)*dt, ytemp, ak4, n);
  for(i=0; i<n; i++){                                   // fourth step
     ytemp(i) = ystart(i) + dt*( bb(5,1)*dydx(i) + bb(5,2)*ak2(i) + bb(5,3)*ak3(i) +
     bb(5,4)*ak4(i) );
  }
  Derivs(tstart+a(5)*dt, ytemp, ak5, n);
  for(i=0; i<n; i++){                                   // fifth step
     ytemp(i) = ystart(i) + dt*( bb(6,1)*dydx(i) + bb(6,2)*ak2(i) + bb(6,3)*ak3(i) +
     bb(6,4)*ak4(i) + bb(6,5)*ak5(i) );
  }
  Derivs(tstart+a(6)*dt, ytemp, ak6, n);
  for(i=0; i<n; i++){                                   // sixth step
     yend(i) = ystart(i) + dt*( cc(1)*dydx(i) + cc(3)*ak3(i) + cc(4)*ak4(i) + cc(6)*ak6(i) );
  }
  for(i=0; i<n; i++)                                    // estimate error
     yerr(i) = dt*( dc(1)*dydx(i) + dc(3)*ak3(i) + dc(4)*ak4(i) + dc(5)*ak5(i) + dc(6)*ak6(i) );

  ak2.MakeEmpty();
  ak3.MakeEmpty();
  ak4.MakeEmpty();
  ak5.MakeEmpty();
  ak6.MakeEmpty();
  ytemp.MakeEmpty();

// compute improved step size as a suggestion.

  Matrix<double> yscal(1,n);
  for(i=0; i<n; i++){
    yscal(i) = fabs(ystart(i)) + fabs(dt*dydx(i)) + 1.0e-30;
    yerr(i) = yerr(i)/yscal(i);
  }
  double tollerance = 1.0e-6;
  double maxerr = fabs(yerr(0));

  for(i=1; i<n; i++){
    if(fabs(yerr(i))>maxerr)
        maxerr = fabs(yerr(i));
  }
  newstep = dt;
  double ratio = tollerance/maxerr;
  if (ratio < 1.0)
     newstep = 0.9*dt*pow(ratio,0.25);
  else
     newstep = 0.9*dt*pow(ratio,0.2);

  return(newstep);

}



void BaseIntegr::IntegratorRKF45(Matrix<double> ystart, Matrix<double> dydx_in, Matrix<double>& yend, \
		  double tstart, double step, Matrix<double>& dydx_out, Matrix<double>& yerr, int n){

     
//     const gsl_odeiv_step_type *T = gsl_odeiv_step_rkf45;
//     gsl_odeiv_step *s = gsl_odeiv_step_alloc(T, n);

     int N = n;
     
    /* int (*fnc)(double, const double*, double*, void*) = NULL;
     fnc = &func;
     gsl_odeiv_system sys= {func, jac, N, &N};
   */	
}



int BaseIntegr::func(double t, const double y[], double f[], void *params){
  	
	int  N = *(int *)params;
	double x = t;
	Matrix<double> y0(N);
	Matrix<double> dydx(N);
	for (int i=0; i<N; i++){
		y0(i) = y[i];
	}
	Derivs(x,y0, dydx, N);
	for (int i=0; i<N; i++){
		f[i] = dydx(i);
	}

//	return GSL_SUCCESS;	
        return(1);
}

int BaseIntegr::jac(double t, const double y[], double * dfdy, double dfdt[], void *params){

     dfdy = NULL;
}


} // end of the namespace

