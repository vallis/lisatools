// $Id: $
/*
 *  LISACODE-Geometry.cpp
 *  V 1.4.1
 *  LISACode
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/12/07 by E.Plagnol
 *
 */

#include "LISACODE-Geometry.h"


// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * #init is called with the following arguments :
 * \arg	t0_n = 0.0
 * \arg	rot0_n = 0.0
 * \arg	L0m_n = #L0_m_default
 * \arg	order_default_n = -10
 * \arg	move_n = 1
 * \arg	tStep = 10.0
 */
Geometry::Geometry()
{
	initGlobal(0.0, 0.0, L0_m_default, -10, 1, 10.0);
	
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
	initGlobal(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);
}



/* Destructor */
/*! \brief Destructor */
Geometry::~Geometry()
{
	
}


// **********************
// **  Access methods  **
// **********************

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
void Geometry::initGlobal(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	if (L0m_n<0.0)
		throw invalid_argument("Geometry::init: The arm length must be positive !");
	L0m = L0m_n;
	if (t0_n<0.0)
		throw invalid_argument("Geometry::init: The initial time must be positive !");
	t0 = t0_n;
	rot0 = rot0_n;
	if ((move_n < 0))
		throw invalid_argument("Geometry::init: The move parameter must be between 0 and 1 !");
	if(move_n >0)
		move = 1;
	else
		move = 0;
	
	// Fix order for time delay
	order_default = order_default_n;
	
	// L0m = L0_m_default;
	
	tRangeStorePos = tRangeStorePos_default;
	tRangeStoreDelay = tRangeStoreDelay_default;
	if(tStep<tRangeStoreDelay_default){
		tRangeStoreDelay = tStep;
	}
	tStorePos = -1.0e30;
	tStoreDelay = -1.0e30;
}

void Geometry::DispGeneralInfo()
{
	cout << "--------------------" << endl;
	cout << " Orbits : " << endl;
	cout.precision(13);
	cout << "    - Initial time     = " << t0 << " s" << endl;
	cout << "    - Initial rotation = " << rot0 << " s" << endl;
	cout << "    - Armlength        = " << L0m << " s" << endl;
	cout << "    - Motion           = " << move << endl;
	cout << "    - Delay order      = " << order_default << endl;
	cout << "    - tRangeStorePos   = " << tRangeStorePos << " s" << endl;
	cout << "    - tRangeStoreDelay = " << tRangeStoreDelay << " s" << endl;
}

void Geometry::DispInfo()
{
	DispGeneralInfo();
}

void Geometry::sett0(double t0_n)
{
	t0 = t0_n;
}


void Geometry::settRangeStorePos(double tRangeStorePos_n)
{
	tRangeStorePos = tRangeStorePos_n;
}

void Geometry::settRangeStoreDelay(double tRangeStoreDelay_n)
{
	tRangeStoreDelay = tRangeStoreDelay_n;
}


// **********************
// **  Others methods  **
// **********************

/*! \brief Returns the position of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
 * 
 * Default function : in correct using this function is not use
 * Returns the started positions
 */


Vect Geometry::position(int nb, double t)
{
	Vect r;	
	switch(nb) {
		case 1 :
			r.p[0] = 148139203884.545 ;
			r.p[1] = -120289.815755034 ;
			r.p[2] = -2467045747.34137 ;
			break;
		case 2 :
			r.p[0] = 150301279343.337 ;
			r.p[1] = -2478707492.80212 ;
			r.p[2] = 1287274939.89722 ;
			break;
		case 3 :
			r.p[0] = 150301281317.278 ;
			r.p[1] = 2478470406.4486 ;
			r.p[2] = 1287271536.97728 ;
			break;
		default:
			r.p[0] = 0.0 ;
			r.p[1] = 0.0 ;
			r.p[2] = 0.0 ;
	}
	return r;
}



/*! \brief Returns the velocity of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
 * 
 * Default function : in correct using this function is not use
 * Returns 0 for all the components.
 */
Vect Geometry::velocity(int nb, double t)
{
	Vect v;	
	
	v.p[0] = 0.0 ;
	v.p[1] = 0.0 ;
	v.p[2] = 0.0 ;
	
	return v;
}



double Geometry::tdelaySpecific(int em, int rec, int order, double trec)
{
	return (L0m/c_SI);
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
	
	//cout << em << " - " << rec << endl; 
	
	if (((em!=1)&&(em!=2)&&(em!=3))||((rec!=1)&&(rec!=2)&&(rec!=3)))
		throw invalid_argument ("error in Geometry::tdelay: Spacecraft number must be 1, 2 or 3 ! ");
	
	if(order != -10)
		order = order_default;
	if ((order<-2)||(order>2))
		throw invalid_argument ("error in Geometry::tdelay: Order must be 0 (for 0), 1 (for 1/2), 2 (for 1) or -1 (for analytical MLDC eccentric formulation)");
	
	if(order >= 0){
		Vect ri, rj, vi, vj, rij, nij, n;
		
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
		
		if (order==0){
			return c0;
		}else{
			// order 1/2
			c1=tij*(n*vj);
			if (order==1){
				return(c0+c1);
			}else{
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
	}else{
		return( tdelaySpecific(em, rec, order, trec) );
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
	if(fabs(t*move-tStorePos)>tRangeStorePos){
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
	if(fabs(trec*move-tStoreDelay)>tRangeStoreDelay){
		tStoreDelay = trec;
		//cout << "In gtdelay : " << endl;
		for(int i=1; i<4; i++){
			DelayStore[i-1] = tdelay(i,(i+1)%3+1,order,trec);
			//cout << " L" << i << (i+1)%3+1<< endl;
			DelayStore[i+2] = tdelay(i,i%3+1,order,trec);
			//cout << " L" << i << i%3+1 << endl;
		}
	}
	
	//printf("In gtdelay : %d -> %d : order = %d at %10f : %20.16e\n", em, rec, order, trec, DelayStore[bs+em-1]);
	
	return(DelayStore[bs+em-1]);
}



//end of LISACODE-Geometry.cpp


