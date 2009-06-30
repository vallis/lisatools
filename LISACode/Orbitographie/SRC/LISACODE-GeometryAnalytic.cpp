// $Id: $
/*
 *  LISACODE-GeometryAnalytic.cpp
 *  V 1.4.1
 *  LISACode
 *
 *  Created on 13/05/08 by A.Petiteau
 *  Last modification on 13/05/08 by A.Petiteau
 *
 */

#include "LISACODE-GeometryAnalytic.h"


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
GeometryAnalytic::GeometryAnalytic()
{
	init(0.0, 0.0, L0_m_default, -1, 2, 10.0);
	
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
GeometryAnalytic::GeometryAnalytic(double t0_n, double rot0_n)
{
	init(t0_n, rot0_n, L0_m_default, -1, 2, 10.0);
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
GeometryAnalytic::GeometryAnalytic(double t0_n, double rot0_n, double L0m_n)
{
	init(t0_n, rot0_n, L0m_n, -1, 2, 10.0);
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
GeometryAnalytic::GeometryAnalytic(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n)
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
GeometryAnalytic::GeometryAnalytic(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	init(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);
}



/* Destructor */
/*! \brief Destructor */
GeometryAnalytic::~GeometryAnalytic()
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
* \arg	#t0 = t0_n input (expected to be positive or null)
* \arg	#rot0 = rot0_n input
* \arg	#L0m = L0m_n input (expected to be positive or null)
* \arg	#order_default = order_default_n input
* \arg	#move = move_n input (expected to be 0 or 1)
* \arg	#alpha =\f$ \frac {L0m}{2 \cdot Rgc} \f$
* \arg	#nu =\f$ \frac{\pi}{3} + \frac{5 \cdot \alpha}{8} \f$
* \arg	#tmu =\f$ \frac{\alpha \cdot sin(\nu)}{sin(\frac{\pi}{3})}+\alpha \cdot cos(\nu)  \f$
* \arg	#cmu = \f$\frac{1}{\sqrt{1+t_\mu ^2}} \f$
* \arg	#smu =\f$  t_\mu \cdot c_\mu \f$
* \arg	#e =\f$ \sqrt{1+\frac{4}{\sqrt{3}} \cdot cos(\nu) \cdot \alpha +\frac{4}{3} \alpha^2}-1 \f$
* \arg	#sqrtee =\f$ \sqrt{1-e^2} \f$
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
void GeometryAnalytic::init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	cout << "Create orbits : Analytic ..." << endl;
	
	initGlobal(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);

	alpha=L0m/(2*Rgc);
	nu=(M_PI/3.)+(5./8.)*alpha;
	tmu=alpha*sin(nu)/(sin(M_PI/3.)+alpha*cos(nu));
	cmu=1./sqrt(1.+tmu*tmu);
	smu=tmu*cmu;
	e=sqrt(1.+(4./sqrt(3.)*cos(nu)*alpha)+(4./3.*alpha*alpha))-1.;
	sqrtee=sqrt(1.-e*e);
	arot=2./3.*M_PI;

	rot.resize(3);
	crot.resize(3);
	srot.resize(3);
	for(int nb=1; nb<=3; nb++){
		rot[nb-1]=(nb-1)*arot+rot0;
		crot[nb-1]=cos(rot[nb-1]);
		srot[nb-1]=sin(rot[nb-1]);
	}
	
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


void GeometryAnalytic::DispInfo()
{
	DispGeneralInfo();
	cout << "    - Type             = LISACode analytic orbit" << endl;
	cout << "--------------------" << endl;
}


/* Other methods */
/*! \brief Returns the cosinus and sinus of the eccentric anomaly, depending on time ts (s) and spacecraft number nb=[1,3].
* 
* \return
* \f$ zpsi = (cos(ex_2),sin(ex_2)) \f$, \n
* where :\n
* \f$ t = t0 + move \cdot ts \f$,  \n
* \f$ \psi = \omega \cdot t - rot_{nb-1} \f$, \n
* \f$ ex_{0} = \psi + e \cdot sin(\psi) \cdot cos(\frac{1-3 \cdot {cos(\psi)}^2}{2}) \f$,\n
* and
* \f$ ex_{k=1,2,3} = ex_{k-1} - \frac{ex_{k-1} - e \cdot sin(ex_{k-1}) - \psi}{1 - e \cdot cos(ex_{k-1})} \f$ .
*/
Couple GeometryAnalytic::exanom(int nb, double ts)
{
	double t(t0+move*ts); 
	int k;
	double psi, cpsi, spsi;
	double exold,exnew,dex;
	
	psi = omega*t -rot[nb-1];
	cpsi=cos(psi);
	spsi=sin(psi);
	exold=psi+e*spsi*(1.+e*(cpsi-e*(0.5-1.5*cpsi*cpsi)));
	
	for (k=0; k<3; k++)
	{
		cpsi=cos(exold);
		spsi=sin(exold);
		dex=(exold-e*spsi-psi)/(1.-e*cpsi);
		exnew=exold-dex;
		exold=exnew;
	}
	
	Couple zpsi(cpsi,spsi);
	return zpsi;
}




/*! \brief Returns the position of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
* 
* #crot, #srot and #sqrtee attributes are used, as #Rgc constant. \n
* Eccentric anomaly zpsi=(cpsi,spsi) is computed using #exanom method.\n
* \return  
* \f$ \overrightarrow{r}= \left(  \begin{array}{l}
								  Rgc \cdot (cpsi-e) \cdot cmu \cdot crot_{nb-1} - sqrtee  \cdot spsi \cdot srot_{nb-1} \\
								  Rgc \cdot (cpsi-e) \cdot cmu \cdot srot_{nb-1} - sqrtee  \cdot spsi \cdot crot_{nb-1} \\ 
								  - Rgc \cdot smu \cdot (cpsi-e) 
								  \end{array} \right) \f$
*/

Vect GeometryAnalytic::position(int nb, double t)
{
	Vect r;
	double cpsi,spsi;
	Couple zpsi;
	
	zpsi = exanom(nb,t);
	
	cpsi=zpsi.x;
	spsi=zpsi.y;
	
	r.p[0] = Rgc*((cpsi - e)*cmu*crot[nb-1] - sqrtee*spsi*srot[nb-1]);
	r.p[1] = Rgc*((cpsi - e)*cmu*srot[nb-1] + sqrtee*spsi*crot[nb-1]);
	r.p[2] = -Rgc*smu*(cpsi - e);
	
	//printf("Pos : t,nb,x,y,z = %10f %d %20.16e %20.16e %20.16e\n", t, nb, r.p[0], r.p[1], r.p[2]);
	//if (t > 4.){
	//	throw invalid_argument("GeometryAnalytic::position : On arrete !");
	//}	
	
	return r;
}



/*! \brief Returns the velocity of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
* 
* #crot, #srot and #sqrtee attributes are used, as #Rgc constant.\n
* Eccentric anomaly zpsi=(cpsi,spsi) is computed using #exanom method.
* \return  
* \f$ \overrightarrow{v}= \left(  \begin{array}{l}
								  \dot{\psi} \cdot (-spsi \cdot cmu \cdot crot_{nb-1} - sqrtee  \cdot cpsi \cdot srot_{nb-1}) \\
								  \dot{\psi} \cdot (-spsi \cdot cmu \cdot srot_{nb-1} + sqrtee  \cdot cpsi \cdot crot_{nb-1}) \\ 
								  \dot{\psi} \cdot smu \cdot spsi 
								  \end{array} \right) \f$ , \n
* where  \f$ \dot{\psi}= \frac {\omega \cdot Rgc}{1 -e \cdot cpsi} \f$
*/
Vect GeometryAnalytic::velocity(int nb, double t)
{
	
	double cpsi,spsi;
	Couple zpsi;
	Vect v;
	
	
	zpsi = exanom(nb,t);
	cpsi=zpsi.x;
	spsi=zpsi.y;
	
	double psidot = omega*Rgc/(1.-e*cpsi);
	
	v.p[0] = psidot*(-spsi*cmu*crot[nb-1] - sqrtee*cpsi*srot[nb-1]);
	v.p[1] = psidot*(-spsi*cmu*srot[nb-1] + sqrtee*cpsi*crot[nb-1]);
	v.p[2] = psidot*smu*spsi;
	
	/*	cout << "Vel : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<v.p[0]<<"  "<<v.p[1]<<"  "<<v.p[2]<< endl;
	if (t > 4.){
		throw invalid_argument("GeometryAnalytic::velocity : On arrete !");
	}*/	
	
	return v;
}


double GeometryAnalytic::tdelaySpecific(int em, int rec, int order, double trec)
{
	return (L0m/c_SI);
}

//end of LISACODE-GeometryAnalytic.cpp


