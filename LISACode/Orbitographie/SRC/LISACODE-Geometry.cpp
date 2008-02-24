// $Id: $
/*
*  LISACODE-Geometry.cpp
 *  V 1.4
 *  LISACode
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/12/07 by E.Plagnol
 *
 */

#include "LISACODE-Geometry.h"


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
Geometry::Geometry()
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
Geometry::Geometry(double t0_n, double rot0_n)
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
Geometry::Geometry(double t0_n, double rot0_n, double L0m_n)
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
Geometry::Geometry(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n)
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
Geometry::Geometry(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	init(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);
}



/* Destructor */
/*! \brief Destructor */
Geometry::~Geometry()
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
void Geometry::init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	if (L0m_n<0.0)
		throw invalid_argument("Geometry::init: The arm length must be positive !");
	L0m = L0m_n;
	if (t0_n<0.0)
		throw invalid_argument("Geometry::init: The initial time must be positive !");
	t0 = t0_n;
	rot0 = rot0_n;
	if ((move_n < 0)||(move_n > 3))
		throw invalid_argument("Geometry::init: The move parameter must be between 0 and 3 !");
	if (move_n <= 2){
		move = move_n;
		orbit_type=1;
	}else{
		move = move_n-2;
		orbit_type=2;
	}
	
	// Fix order for time delay
	order_default = order_default_n;
	
	// For MLDC orbits
	// mettre ici une option pour lisa fixe : move=0 + option 
	cout << "E.P. : orbit_type,move =" << orbit_type<< " "<<move << endl ;
	sqrt_3 = sqrt(3.);
	e_mldc = 0.00964838;
	
	// For standard orbit (ARTEMIS orbit)
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
	
	tRangeStorePos = tRangeStorePos_default;
	tRangeStoreDelay = tRangeStoreDelay_default;
	if(tStep<tRangeStoreDelay_default){
		tRangeStoreDelay = tStep;
	}
	cout << "Geometry : tRangeStorePos   = " << tRangeStorePos << " s" << endl;
	cout << "Geometry : tRangeStoreDelay = " << tRangeStoreDelay << " s" << endl;

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
Couple Geometry::exanom(int nb, double ts)
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

Vect Geometry::position(int nb, double t)
{
	double cpsi,spsi;
	Vect r;
	double beta;
	double alpha;
	double c_alpha,s_alpha,c_beta,s_beta;
	Couple zpsi;
	
	if (orbit_type == 1){
		zpsi = exanom(nb,t);
		
		cpsi=zpsi.x;
		spsi=zpsi.y;
		
		r.p[0] = Rgc*((cpsi - e)*cmu*crot[nb-1] - sqrtee*spsi*srot[nb-1]);
		r.p[1] = Rgc*((cpsi - e)*cmu*srot[nb-1] + sqrtee*spsi*crot[nb-1]);
		r.p[2] = -Rgc*smu*(cpsi - e);
	}else{
		alpha=omega*t;
		c_alpha=cos(alpha); s_alpha=sin(alpha);
		beta=rot[nb-1];  c_beta=crot[nb-1] ; s_beta=srot[nb-1];
		// 1st order in excentricity 
		/*
		r.p[0] = au_m*(c_alpha+e_mldc*(s_alpha*c_alpha*s_beta-(1+s_alpha*s_alpha)*c_beta));
		r.p[1] = au_m*(s_alpha+e_mldc*(s_alpha*c_alpha*c_beta-(1+c_alpha*c_alpha)*s_beta));
		r.p[2] = -au_m*e_mldc*sqrt_3*cos(alpha-beta);
		*/
		//cout << "Pos OLD : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<r.p[0]<<"  "<<r.p[1]<<"  "<<r.p[2]<< "  "<<  endl;
		//2nd order in excentricity   (copied form Synth Lisa)
		r.p[0] = 0.5*au_m*e_mldc*(cos(2.0*alpha-beta)-3.0*cos(beta))
		   +0.125*au_m*pow(e_mldc,2)*(3.0*cos(3.0*alpha-2.0*beta)-5.0*(2.0*cos(alpha)+cos(alpha-2.0*beta)))
                   +au_m*cos(alpha);
		r.p[1] =   0.5*au_m*e_mldc*(sin(2.0*alpha-beta)-3.0*sin(beta))
		   +0.125*au_m*pow(e_mldc,2)*(3.0*sin(3.0*alpha-2.0*beta)-5.0*(2.0*sin(alpha)-sin(alpha-2.0*beta)))
		   +au_m*sin(alpha);
           
		r.p[2] = -sqrt_3*au_m*e_mldc*cos(alpha-beta)
		   +sqrt_3*au_m*pow(e_mldc,2)*(cos(alpha-beta)*cos(alpha-beta)+2.0*sin(alpha-beta)*sin(alpha-beta));                                            
		//cout << "Pos NEW : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<r.p[0]<<"  "<<r.p[1]<<"  "<<r.p[2]<< "  "<<  endl;

	}
	//cout << "Pos : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<r.p[0]<<"  "<<r.p[1]<<"  "<<r.p[2]<< "  "<<  endl;
	/*
	if (t > 4.){
		throw invalid_argument("Position :: On arrete !");
	}
	*/
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
Vect Geometry::velocity(int nb, double t)
{
	
	double cpsi,spsi;
	Vect v;
	double beta;
	double alpha;
	double c_alpha,s_alpha,c_beta,s_beta,d_alpha;
	
	Couple zpsi;
	
	if (orbit_type == 1){
		zpsi = exanom(nb,t);
		cpsi=zpsi.x;
		spsi=zpsi.y;
		
		double psidot = omega*Rgc/(1.-e*cpsi);
		
		v.p[0] = psidot*(-spsi*cmu*crot[nb-1] - sqrtee*cpsi*srot[nb-1]);
		v.p[1] = psidot*(-spsi*cmu*srot[nb-1] + sqrtee*cpsi*crot[nb-1]);
		v.p[2] = psidot*smu*spsi;
	}else{
		alpha=omega*t;
		d_alpha=omega;
		c_alpha=cos(alpha);  s_alpha=sin(alpha);
		beta=rot[nb-1];  c_beta=crot[nb-1] ; s_beta=srot[nb-1];
		v.p[0] = au_m*(-s_alpha+e_mldc*((c_alpha*c_alpha-s_alpha*s_alpha)*s_beta-2*s_alpha*c_alpha*c_beta))*d_alpha;
		v.p[1] = au_m*( c_alpha+e_mldc*((c_alpha*c_alpha-s_alpha*s_alpha)*c_beta+2*c_alpha*s_alpha*s_beta))*d_alpha;
		v.p[2] = au_m*e_mldc*sqrt_3*sin(alpha-beta)*d_alpha;
	}
	/*	cout << "Vel : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<v.p[0]<<"  "<<v.p[1]<<"  "<<v.p[2]<< endl;
	if (t > 4.){
		throw invalid_argument("Velocity :: On arrete !");
	  }*/
	
	return v;
}



/*! \brief Gets delay for specified order, depending on emitter em, receiver rec, reception time trec inputs.
 *
 * Inputs are checked:
 * \arg em and rec expected values are 1,2 and 3
 * \arg order expected values are 0, 1 (for 1/2) and 2 (for 1)
 *
 * Computation:\n
  * \return
 * \f$ delay = \sum_{i=0}^{order} c_i \f$, \n
 * where : \n
 * ri is computed using #position method with rec and trec inputs, \n
 * vi is computed using #velocity method with rec and trec inputs, then divided by #c_SI, \n
 * rj is computed using #position method with em and trec inputs, \n
 * vj is computed using #velocity method with em and trec inputs, then divided by #c_SI, \n
 * \f$ \overrightarrow{r_{ij}} = \overrightarrow{r_j}-\overrightarrow{r_i} \f$, \n
 * \f$ c_0 = \frac{\big\| \overrightarrow{r_{ij}} \big\|}{C} \f$, \n
 * \f$ \overrightarrow{n} = -\frac{\overrightarrow{r_{ij}}}{\big\| \overrightarrow{r_{ij}}} \f$, \n
 * \f$ c_1 = t_{ij} \cdot \overrightarrow{n} \cdot \overrightarrow{v_j} \f$, \n
 * \f$ c_2 = c_{21}+c_{22}+c_{23} \f$, \n
 * \f$ c_{21} = \frac{t_{ij}}{2} \cdot ( {\overrightarrow{v_j}}^2 + (\overrightarrow{n} \cdot \overrightarrow{v_j})^2 ) \f$, \n
 * \f$ c_{22} = -\frac{{t_{ij}}^2}{2} \cdot C \cdot RSchw  \f$, \n
 * \f$ c_{23} = -\frac{RSchw}{C} \cdot (1+\gamma_u) \cdot log \big(\frac{\sqrt{(C \cdot t_{ij} \cdot \overrightarrow{n} \cdot  \overrightarrow{r_{ij}})^2+{r_i}^2+C \cdot t_{ij} + \overrightarrow{n} \cdot  \overrightarrow{r_i}}}{\overrightarrow{n} \cdot  \overrightarrow{r_i} + \big\| \overrightarrow{r_i} \big\|}\big)  \f$. \n
 * #c_SI and #gamma_u constants are used.\n
 */
double Geometry::tdelay(int em, int rec, int order, double trec)
{
	double tij, c0, c1, c2, c21, c22, c23, num, denom;
	Vect ri, rj, vi, vj, rij, nij, n;
	
	//cout << em << " - " << rec << endl; 
	
	if (((em!=1)&&(em!=2)&&(em!=3))||((rec!=1)&&(rec!=2)&&(rec!=3)))
		throw invalid_argument ("error in Geometry::tdelay: Spacecraft number must be 1, 2 or 3 ! ");
	
	if(order_default != -1)
		order = order_default;
	if ((order!=0)&&(order!=1)&&(order!=2))
		throw invalid_argument ("error in Geometry::tdelay: Order must be 0, 1(for 1/2) or 2 (for 1)");
	
	
	ri = position(rec, trec);
	vi = velocity(rec, trec);
	vi= (1./c_SI)*vi;
	
	rj = position(em, trec);
	vj = velocity(em, trec);
	vj = (1./c_SI)*vj;
	
	rij = rj-ri;
	nij = rij.unit();
	n=(-1.)*nij;
	
	tij = rij.norme()/c_SI;
	tij=-tij;  
	c0= tij;
	
	if (order==0)
	{return c0;}
	else
	{
		// order 1/2
		c1=tij*(n*vj);
		if (order==1)
		{return(c0+c1);}
		else
		{
			// order 1
			c21 = 0.5*tij*((vj*vj)+pow((n*vj),2.));
			c22 = -0.5*tij*tij*c_SI*RSchw*(n*rj)/pow((rj*rj),1.5);
			num=sqrt(pow(c_SI*tij+(n*ri),2.)+(ri*ri)-pow((ri*n),2.))+c_SI*tij+(n*ri);
			denom=(n*ri)+sqrt((ri*ri));
			c23 = (RSchw/c_SI)*(1.+gamma_u)*log(num/denom);
			c2=c21+c22+c23;
			return (c0+c1+c2);
		}
	} 
}

/*! \brief Gets contribution of specified order, depending on emitter em, receiver rec, reception time trec inputs.
 *
 * Inputs are checked:
 * \arg em and rec expected values are 1,2 and 3
 * \arg order expected values are 1 (for 1/2) and 2 (for 1)
 *
 * \return
 * \f$ \left\{ \begin{array}{ll} c_1 & \textrm{if $order=1$} \\ c_2 & \textrm{if $order=2$}\\ 0 & \textrm{else} \end{array} \right. \f$
 *
 * where :\n
 * ri is computed using #position method with rec and trec inputs, \n
 * vi is computed using #velocity method with rec and trec inputs, then divided by #c_SI, \n
 * rj is computed using #position method with em and trec inputs, \n
 * vj is computed using #velocity method with em and trec inputs, then divided by #c_SI, \n
 * \f$ \overrightarrow{r_{ij}} = \overrightarrow{r_j}-\overrightarrow{r_i} \f$, \n
 * \f$ \overrightarrow{n} = -\frac{\overrightarrow{r_{ij}}}{\big\| \overrightarrow{r_{ij}}\big\|} \f$, \n
 * \f$ c_1 = t_{ij} \cdot \overrightarrow{n} \cdot \overrightarrow{v_j} \f$, \n
 * \f$ c_2 = c_{21}+c_{22}+c_{23} \f$, \n
 * \f$ c_{21} = \frac{t_{ij}}{2} \cdot ( {\overrightarrow{v_j}}^2 + (\overrightarrow{n} \cdot \overrightarrow{v_j})^2 ) \f$, \n
 * \f$ c_{22} = -\frac{{t_{ij}}^2}{2} \cdot C \cdot RSchw  \f$, \n
 * \f$ c_{23} = -\frac{RSchw}{C} \cdot (1+\gamma_u) \cdot log \big(\frac{\sqrt{(C \cdot t_{ij} \cdot \overrightarrow{n} \cdot  \overrightarrow{r_{ij}})^2+{r_i}^2+C \cdot t_{ij} + \overrightarrow{n} \cdot  \overrightarrow{r_i}}}{\overrightarrow{n} \cdot  \overrightarrow{r_i} + \big\| \overrightarrow{r_i} \big\|}\big)  \f$. \n
 * #c_SI and #gamma_u constants are used
*/
double Geometry::tdelayOrderContribution(int em, int rec, int order, double trec)
{
	double tij, c21, c22, c23, num, denom;
	Vect ri, rj, vi, vj, rij, nij, n;
	
	if ((em!=1)&&(em!=2)&&(em!=3)&&(rec!=1)&&(rec!=2)&&(rec!=3))
		throw invalid_argument ("error in Geometry::tdelay: Spacecraft number must be 1, 2 or 3 ! ");

	if ((order!=1)&&(order!=2))
		throw invalid_argument ("error in Geometry::tdelayOrderContribution: order must be 1(for 1/2) or 2 (for 1)");

	
	ri = position(rec, trec);
	vi = velocity(rec, trec);
	vi= (1./c_SI)*vi;
	
	rj = position(em, trec);
	vj = velocity(em, trec);
	vj = (1./c_SI)*vj;
	
	rij = rj-ri;
	nij = rij.unit();
	n=(-1.)*nij;
	
	tij = rij.norme()/c_SI;
	tij = -tij;
	
	// order 1/2
	if (order==1){
		return(tij*(n*vj));
	}
	// order 1	
	if (order==2){
		c21 = 0.5*tij*((vj*vj)+pow((n*vj),2.));
		c22 = -0.5*tij*tij*c_SI*RSchw*(n*rj)/pow((rj*rj),1.5);
		num=sqrt(pow(c_SI*tij+(n*ri),2.)+(ri*ri)-pow((ri*n),2.))+c_SI*tij+(n*ri);
		denom=(n*ri)+sqrt((ri*ri));
		c23 = (RSchw/c_SI)*(1.+gamma_u)*log(num/denom);
		return (c21+c22+c23);
	}
	return(0.0);
}

/*! \brief Gets spacecrafts relative velocity along an arm
 *
 * \param em emitter
 * \param rec receiver
 * \param trec reception time
 *
 * Inputs are checked: em and rec expected values are 1,2 and 3.
 *
 * \return
 * \f$  (\overrightarrow{v_j} - \overrightarrow{v_i}) \cdot \overrightarrow{n} \f$, \n
 * where :\n
 * ri is computed using #position method with rec and trec inputs, \n
 * vi is computed using #velocity method with rec and trec inputs, then divided by #c_SI, \n
 * rj is computed using #position method with em and trec inputs, \n
 * vj is computed using #velocity method with em and trec inputs, then divided by #c_SI, \n
 * \f$ \overrightarrow{r_{ij}} = \overrightarrow{r_j}-\overrightarrow{r_i} \f$ and
 * \f$ \overrightarrow{n} = -\frac{\overrightarrow{r_{ij}}}{\big\| \overrightarrow{r_{ij}}\big\|} \f$ .\n
 * #c_SI and #gamma_u constatnts are used
 */
double Geometry::ArmVelocity(int em, int rec, double trec)
{
	Vect ri, rj, vi, vj, rij, nij, n;
	
	if ((em!=1)&&(em!=2)&&(em!=3)&&(rec!=1)&&(rec!=2)&&(rec!=3))
		throw invalid_argument ("error in Geometry::tdelay: Spacecraft number must be 1, 2 or 3 ! ");
	
	
	ri = position(rec, trec);
	vi = velocity(rec, trec);
	
	rj = position(em, trec);
	vj = velocity(em, trec);
	
	rij = rj-ri;
	nij = rij.unit();
	n=(-1.)*nij;
	
	return(vj*n-vi*n);	
}


/*! \brief Gets unitary vector normal to plane defined by 3 satellites, depending on t time input.
 *
 * #position method is used to set #SCposStore attribute
 *
 * \return
 * \f$\frac{(\overrightarrow{position(2,t)}-\overrightarrow{position(3,t)}) \bigwedge (\overrightarrow{position(3,t)}-\overrightarrow{position(1,t)})}{\big\| (\overrightarrow{position(2,t)}-\overrightarrow{position(3,t)}) \bigwedge (\overrightarrow{position(3,t)}-\overrightarrow{position(1,t)}) \big\|} \f$
*/
Vect Geometry::VectNormal(double t)
{
	//double cpsi,spsi;
	Vect n, n1, n2, r;
	
	// Spacecraft 1
	n1 = position(2,t)-position(3,t);
	n2 = position(3,t)-position(1,t);

	r.p[0] = n1.p[1]*n2.p[2]-n1.p[2]*n2.p[1];
	r.p[1] = n1.p[2]*n2.p[0]-n1.p[0]*n2.p[2];
	r.p[2] = n1.p[0]*n2.p[1]-n1.p[1]*n2.p[0];
	
	
	return (r.unit());
}


/*! \brief Gives direct value for position, depending on spacecraft number nb and time t inputs.
 *
 * #position method is used to set #SCposStore attribute.\n
 * \f$ \textrm{if $abs(t-tStorePos)>tRangeStorePos$, } \f$
 * \f[ tStorePos = t \textrm{ and } SCposStore[i-1] = position(i,t) \textrm{ for i=1,2,3,4} \f]
 * \return
 * \f$ SCposStore[nb-1]  \f$
 */
Vect Geometry::gposition(int nb, double t)
{
	if(fabs(t-tStorePos)>tRangeStorePos){
		tStorePos = t;
		for(int i=1; i<4; i++){
			SCposStore[i-1] = position(i,t);
		}
	}
	return(SCposStore[nb-1]);
}


/*! \brief Gives delay, depending on transmitter em, receiver rec, order and trec time inputs.
 *
 * #tdelay method is used to set #DelayStore attribute
 *
 * \f$ bs =  \left\{  \begin{array}{ll} 3 &  \textrm{if (mod(em,3)+1=rec)} \\ 0 & \textrm{else} \end{array} \right.\f$ \n
 * If (abs(trec-tStoreDelay)>tRangeStorePos)\n
 * \f[  \left\{  \begin{array}{ll}  StoreDelay = trec & \\
 * \textrm{ for $i=1,2,3,4$}  & 
 * \left\{  \begin{array}{l} DelayStore[i-1] = tdelay(i,i,mod((i+1),3)+1,2,0) \\ DelayStore[i+2] = tdelay(i,i,mod(i,3)+1,2,0) \end{array} \right.
 * \end{array} \right. \f]
 * \return
 * \f$ DelayStore[bs+em-1] \f$
 */
double Geometry::gtdelay(int em, int rec, int order, double trec)
{
	int bs((em%3+1 == rec)?3:0);
	if(fabs(trec-tStoreDelay)>tRangeStoreDelay){
		tStoreDelay = trec;
		//cout << "In gtdelay : " << endl;
		for(int i=1; i<4; i++){
			DelayStore[i-1] = tdelay(i,(i+1)%3+1,order,trec);
			//cout << " L" << i << (i+1)%3+1<< endl;
			DelayStore[i+2] = tdelay(i,i%3+1,order,trec);
			//cout << " L" << i << i%3+1 << endl;
		}
	}
	
	return(DelayStore[bs+em-1]);
}



//end of LISACODE-Geometry.cpp


