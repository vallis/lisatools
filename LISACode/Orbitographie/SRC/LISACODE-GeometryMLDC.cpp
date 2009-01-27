// $Id: $
/*
 *  LISACODE-GeometryMLDC.cpp
 *  V 1.4.1
 *  LISACode
 *
 *  Created on 21/12/07 by E.Plagnol
 *  Last modification on 13/05/08 by A.Petiteau
 *
 */

#include "LISACODE-GeometryMLDC.h"


/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
*
* #init is called with the following arguments :
* \arg	t0_n = 0.0
* \arg	rot0_n = 0.0
* \arg	L0m_n = #L0_m_default
* \arg	order_default_n = -1
* \arg	move_n = 1
* \arg	tStep = 10.0
*/
GeometryMLDC::GeometryMLDC()
{
	init(0.0, 0.0, L0_m_default, -1, 1, 10.0);
	
}

/*! \brief Constructs an instance and initializes it with default values and t0_n and rot0_n inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = #L0_m_default
* \arg	order_default_n = -1
* \arg	move_n = 1
* \arg	tStep = 10.0
*/
GeometryMLDC::GeometryMLDC(double t0_n, double rot0_n)
{
	init(t0_n, rot0_n, L0_m_default, -1, 1, 10.0);
}


/*! \brief Constructs an instance and initializes it with default values and t0_n, rot0_n and L0m_n inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = L0m_n input
* \arg	order_default_n = -1
* \arg	move_n = 1
* \arg	tStep = 10.0
*/
GeometryMLDC::GeometryMLDC(double t0_n, double rot0_n, double L0m_n)
{
	init(t0_n, rot0_n, L0m_n, -1, 1, 10.0);
}


/*! \brief Constructs an instance and initializes it with default values and t0_n, rot0_n, L0m_n,  order_default_n and move_n inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = L0m_n input
* \arg	order_default_n = order_default_n input
* \arg	move_n = move_n input
* \arg	tStep = 10.0
*/
GeometryMLDC::GeometryMLDC(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n)
{
	init(t0_n, rot0_n, L0m_n, order_default_n, move_n, 10.0);
}

/*! \brief Constructs an instance and initializes it with t0_n, rot0_n, L0m_n,  order_default_n, move_n and tStep inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = L0m_n input
* \arg	order_default_n = order_default_n input
* \arg	move_n = move_n input
* \arg	tStep = tStep input
*/
GeometryMLDC::GeometryMLDC(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	init(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);
}



/* Destructor */
/*! \brief Destructor */
GeometryMLDC::~GeometryMLDC()
{
	
}


/* Access methods */
/*! \brief Initializes attrubutes using t0_n, rot0_n, L0m_n,  order_default_n, move_n and tStep inputs.
*
* Inputs are checked.
*
* #tdelay method is used to set #DelayStore attribute
*
* #position method is used to set #SCposStore attribute
*
* Attributes are set :

*
* \arg	#arot =\f$ \frac{2 \cdot \pi}{3} \f$
* \arg	#rot :\f$ rot[i] = i \cdot arot \cdot rot0  \textrm{ for i=1,2,3 } \f$
* \arg	#crot : \f$ crot[i] = cos(rot_i)  \textrm{ for i=1,2,3 } \f$
* \arg	#srot : \f$ srot[i=0,1,2] = sin(rot_i) \textrm{ for i=1,2,3 } \f$
* \arg	#SCposStore :\f$ SCposStore[i] = position(i,0) \textrm{ for i=1,2,3,4 }  \f$
* \arg	#DelayStore :
* \f$  \textrm{ for i=1,2,3,4 }  
* \left\{  \begin{array}{l} 
	* DelayStore[i-1] = tdelay(i,i,mod((i+1),3)+1,2,0) \\ 
	* DelayStore[i+2] = tdelay(i,i,mod(i,3)+1,2,0)
	* \end{array} \right. \f$
	* \arg	#tRangeStorePos = #tRangeStorePos_default
	* \arg	#tRangeStoreDelay = \f$ min(tStep,tRangeStoreDelay\_default) \f$
	*/
void GeometryMLDC::init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	//cout << "Create orbits : MLDC ..." << endl;
	
	initGlobal(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);
	Approx = move_n;
			
	arot=2./3.*M_PI;
	rot.resize(3);
	crot.resize(3);
	srot.resize(3);
	for(int nb=1; nb<=3; nb++){
		rot[nb-1]=(nb-1)*arot+rot0;
		crot[nb-1]=cos(rot[nb-1]);
		srot[nb-1]=sin(rot[nb-1]);
	}
	
	sqrt_3 = sqrt(3.);
	e_mldc = 0.00964838;
	
	for(int i=1; i<4; i++){
		SCposStore[i-1] = position(i,0.0);
	}
	
	for(int i=1; i<4; i++){
		DelayStore[i-1] = tdelay(i,(i+1)%3+1,2,0.0);
		//cout << " L" << i << (i+1)%3+1<< endl;
		DelayStore[i+2] = tdelay(i,i%3+1,2,0.0);
		//cout << " L" << i << i%3+1 << endl;
	}
	
	cout << " --> OK" << endl;
}


void GeometryMLDC::DispInfo()
{
	DispGeneralInfo();
	cout << "    - Approx           = " << Approx<< endl;
	cout << "    - Type             = MLDC" << endl;
	cout << "--------------------" << endl;
}


/* Other methods */

/*! \brief Returns the position of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
* 
*
*/

Vect GeometryMLDC::position(int nb, double t)
{
	Vect r;
	double beta;
	double alpha;
	double c_alpha,s_alpha,c_beta,s_beta;
	double sq_e_mldc;
	
	alpha=omega*(t0+move*t);
	c_alpha=cos(alpha); 
	s_alpha=sin(alpha);
	beta=rot[nb-1];
	c_beta=crot[nb-1] ;
	s_beta=srot[nb-1];
	sq_e_mldc=pow(e_mldc,2);
	
	if(Approx<=1){
		//cout << "1st order in excentricity" << endl;
		// ** 1st order in excentricity
		
		r.p[0] = au_m*(c_alpha+e_mldc*(s_alpha*c_alpha*s_beta-(1+s_alpha*s_alpha)*c_beta));
		r.p[1] = au_m*(s_alpha+e_mldc*(s_alpha*c_alpha*c_beta-(1+c_alpha*c_alpha)*s_beta));
		r.p[2] = -au_m*e_mldc*sqrt_3*cos(alpha-beta);
		
	}else{
		// ** 2nd order in excentricity **
		//cout << "2nd order in excentricity " << endl;		
		
		/*
		 r.p[0] =   0.5 * au_m * e_mldc * ( cos(2.0*alpha-beta) - 3.0*cos(beta) )
		 + 0.125 * au_m * sq_e_mldc * ( 3.0*cos(3.0*alpha-2.0*beta) - 5.0*( 2.0*cos(alpha)+cos(alpha-2.0*beta) ) )
		 + au_m * cos(alpha);
		 
		 r.p[1] =   0.5 * au_m * e_mldc * ( sin(2.0*alpha-beta) - 3.0*sin(beta) )
		 + 0.125 * au_m * sq_e_mldc * ( 3.0*sin(3.0*alpha-2.0*beta) - 5.0*( 2.0*sin(alpha)-sin(alpha-2.0*beta) ) )
		 + au_m * sin(alpha);
		 
		 r.p[2] = - sqrt_3 * au_m * e_mldc * cos(alpha-beta) 
		 + sqrt_3 * au_m * sq_e_mldc * ( cos(alpha-beta)*cos(alpha-beta) + 2.0*sin(alpha-beta)*sin(alpha-beta) );
         */
		
		r.p[0] =   0.5 * au_m * e_mldc * ( cos(2.0*alpha-beta) - 3.0*c_beta )
		+ 0.125 * au_m * sq_e_mldc * ( 3.0*cos(3.0*alpha-2.0*beta) - 5.0*( 2.0*c_alpha+cos(alpha-2.0*beta) ) )
		+ au_m * c_alpha;
		
		r.p[1] =   0.5 * au_m * e_mldc * ( sin(2.0*alpha-beta) - 3.0*s_beta)
		+ 0.125 * au_m * sq_e_mldc * ( 3.0*sin(3.0*alpha-2.0*beta) - 5.0*( 2.0*s_alpha-sin(alpha-2.0*beta) ) )
		+ au_m * s_alpha;
		
		r.p[2] = - sqrt_3 * au_m * e_mldc * cos(alpha-beta) 
		+ sqrt_3 * au_m * sq_e_mldc * ( cos(alpha-beta)*cos(alpha-beta) + 2.0*sin(alpha-beta)*sin(alpha-beta) );
	}

	/*		cout << "Pos : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<r.p[0]<<"  "<<r.p[1]<<"  "<<r.p[2]<< "  "<<  endl;
	if (t > 4.){
		throw invalid_argument("GeometryAnalytic::position : On arrete !");
	}*/	
	
	return r;
}



/*! \brief Returns the velocity of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
*
*
*/

Vect GeometryMLDC::velocity(int nb, double t)
{
	Vect v;
	double beta;
	double alpha;
	double c_alpha,s_alpha,c_beta,s_beta,d_alpha;
	
	alpha=omega*(t0+move*t);
	d_alpha=omega;
	c_alpha=cos(alpha);  
	s_alpha=sin(alpha);
	beta=rot[nb-1];  
	c_beta=crot[nb-1] ; 
	s_beta=srot[nb-1];
	
	if(Approx<=1){
		//cout << "1st" << endl;
		// ** 1st order in excentricity
	    v.p[0] = au_m*(-s_alpha+e_mldc*((c_alpha*c_alpha-s_alpha*s_alpha)*s_beta-2*s_alpha*c_alpha*c_beta))*d_alpha;
	    v.p[1] = au_m*( c_alpha+e_mldc*((c_alpha*c_alpha-s_alpha*s_alpha)*c_beta+2*c_alpha*s_alpha*s_beta))*d_alpha;
	    v.p[2] = au_m*e_mldc*sqrt_3*sin(alpha-beta)*d_alpha;
	    //cout << "Vel old : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<v.p[0]<<"  "<<v.p[1]<<"  "<<v.p[2]<< endl;
	    //vx_old=v.p[0] ; vy_old=v.p[1] ; vz_old=v.p[2] ;
	}else{
		// ** 2nd order in excentricity **
		//cout << "2nd" << endl;
		v.p[0] = 0.5*au_m*e_mldc*(-2*sin(2.0*alpha-beta))
	    +0.125*au_m*pow(e_mldc,2)*(-9.0*sin(3.0*alpha-2.0*beta)-5.0*(-2.0*sin(alpha)-sin(alpha-2.0*beta)))
	    -au_m*sin(alpha);
		v.p[0] = v.p[0]*d_alpha ;
		
		v.p[1] = 0.5*au_m*e_mldc*(2*cos(2.0*alpha-beta))
	    +0.125*au_m*pow(e_mldc,2)*(9.0*cos(3.0*alpha-2.0*beta)-5.0*(2.0*cos(alpha)-cos(alpha-2.0*beta)))
	    +au_m*cos(alpha);
		v.p[1] = v.p[1]*d_alpha ;
		
		v.p[2] = +sqrt_3*au_m*e_mldc*sin(alpha-beta)
	    +sqrt_3*au_m*pow(e_mldc,2)*(-2*cos(alpha-beta)*sin(alpha-beta)+4.0*sin(alpha-beta)*cos(alpha-beta));
		v.p[2] = v.p[2]*d_alpha ;
	}
	
	/*	cout << "Vel : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<v.p[0]<<"  "<<v.p[1]<<"  "<<v.p[2]<< endl;
	if (t > 1000.){
		throw invalid_argument("GeometryAnalytic::velocity : On arrete !");
	}*/
		
	return v;
}



//end of LISACODE-GeometryMLDC.cpp


