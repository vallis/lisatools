// $Id: LISACODE-Serie2.cpp,v 1.5 2007/04/05 09:33:22 ruiloba Exp $
/*
 *  LISACODE-Serie2.cpp
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * 
 *
 *
 *  Created on 27/05/09 by Antoine Petiteau (AEI)
 *  Last modification on 27/05/09 by Antoine Petiteau (AEI)
 *
 */

#include "LISACODE-Serie2.h"


// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg  x0 = 0
 * \arg  dx =1
 */
Serie2::Serie2()
{
	ys = NULL;
	N = 0;
	x0 = 0.0;
	dx = 1.0;
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
}

/*! \brief Constructs an instance and initializes it with inputs and default values.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 */
Serie2::Serie2(double start, double delta)
{
	ys = NULL;
	N = 0;
	x0 = start;
	dx = delta;
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
	xrbinE = false;
}


/*! \brief Constructs an instance and initializes it with inputs and default values.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 * \arg  ys = length elements, with 0 values
 */
Serie2::Serie2(double start, double delta, int length)
{
	ys = NULL;
	Nmax = length;
	N = 0;
	x0 = start;
	dx = delta;
	ys = (double**)malloc(Nmax*sizeof(double*));
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
	xrbinE = false;
}


/*! \brief Constructs an instance and initializes it with inputs.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 * \arg  ys = ys_n input
 */
Serie2::Serie2(double start, double delta, double ** ys_n, int N_n, int Nmax_n)
{
 	x0 = start;
	dx = delta;
	Nmax = Nmax_n;
	N = N_n;
	ys = (double**)malloc(Nmax*sizeof(double*));
	for(int i=0; i<N; i++)
		ys[i] = new double((*ys_n[i]));
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
	xrbinE = false;
}


/*! \brief Destructor. */
Serie2::~Serie2()
{
	//for(int i=0; i<Nmax; i++)
	//	delete ys[i];
	//free(ys);
}


// **********************
// **  Access methods  **
// **********************

/*! \brief Sets #ys attribute size to lenght input. */
void Serie2::setNmax(int Nmax_n)
{
	if(Nmax == 0){
		Nmax = Nmax_n;
		ys = (double**)malloc(Nmax*sizeof(double*));
	}else{
		double ** tmpys(ys);
		ys = NULL;
		ys = (double**)malloc(Nmax_n*sizeof(double*));
		for(int i=0; i<Nmax_n; i++)
			ys[i] = tmpys[i];
		for(int i=Nmax_n; i<N; i++)
			delete tmpys[i];
		free(tmpys);
		Nmax = Nmax_n;
	}
}


/*! \brief Gets reference value corresponding to bin input.
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size. \n
 * \return
 * \f$ x0 + dx \cdot bin \f$
 */
double Serie2::getRef(int bin) const
{
	//if((bin<0)||(bin > N-1))
	//	throw invalid_argument("Serie2: The required bin does not exist !");  
	return(x0 + dx*(bin));
}

/*! \brief Gets reference y value corresponding to bin input.
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size. \n
 * \return
 * ys[bin]
 */
double Serie2::getBinValue(int bin) const
{
	//if((bin<0)||(bin > N-1))
	//	throw invalid_argument("Serie2: The required bin does not exist !");  
	return((*ys[bin]));
}

/*! \brief Sets reference y value corresponding to bin and x inputs.
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size. \n
 * Then ys is filled :
 * ys[bin]=x .
 */
void Serie2::setBinValue(int bin, double x)
{
	if((bin<0)||(bin > N-1))
		throw invalid_argument("Serie2: The required bin does not exist !");  
	(*ys[bin]) = x;
}



/* Other methods */
//Add data at the begining of the serie
/*! \brief Adds data at the begining of the serie. */
void Serie2::addData(double y)
{
	double * ysswap;
	
	if(N < Nmax){
		N++;
		for(int k=N-1; k>0; k--)
			ys[k] = ys[k-1];
		ys[0] = new double;
	}else{
		ysswap = ys[N-1];
		for(int k=N-1; k>0; k--)
			ys[k] = ys[k-1];
		ys[0] = ysswap;
	}
	(*ys[0]) = y; 
} 


void Serie2::allocAll()
{
	for(int i=N; i<Nmax; i++)
		ys[i] = new double(0.0);
	N = Nmax;
}


/*! \brief Returns the exact value if x is a multiple of dx, else interpolated value.
 *
 * Interpolation method depends on InterpolType input. \n
 * It is checked. Its expected values are TRU, LIN, CUB and LAG.
 *
 * \return
 * \f[ \left\{ \begin{array}{ll} 
 TruncVal(x) & \textrm{if } modulo(\frac{x-x_0}{dx},1) \approx 0 \\
 TruncVal(x) & \textrm{if } InterpolType = TRU \\
 InterLinear(x) & \textrm{if } InterpolType = LIN \\
 InterCubic(x) & \textrm{if } InterpolType = CUB \\
 InterLagrange(x, int(InterpUtilValue)) & \textrm{if } InterpolType =  LAG
  \end{array} \right.\f]
 */
double Serie2::gData(double x, INTERP InterpolType, double InterpUtilValue)
{
	//cout << "Serie2::gData :" << endl;
	//cout << "  fmod(xr,1.0) = " << fmod(xr,1.0) << endl;
	
	xr = (x-x0)/dx ;
	bin = MathUtils::ifloor(xr);
	xrbinE = true;
	//if(fmod((x-x0)/dx,1.0) < PRECISION){
		//cout << "  --> Tronque" << endl;
	if((xr-bin) < PRECISION){
		return(TruncVal(x));
	}else{
		switch(InterpolType){
			case TRU :
				return(TruncVal(x));
				break;
			case LIN :
				return(InterLinear(x));
				break;
			case CUB :
				return(InterCubic(x));
				break;
			case LAG :
				return(InterLagrange(x, int(InterpUtilValue)));
				break;
				//case SIN :
				//	return(InterSincTrunc(x, int(InterpUtilValue)));
				//	break;	
			default :
				throw invalid_argument("Serie2: No interpolation method !");
		}
	}
}


// Return truncated value
/*! \brief Returns truncated value.
 *
 * First, bin index is computed :
 * \f[ bin= floor(\frac{x-x_0}{dx}) \f]
 * Then bin is checked ; it must be positive or null, and lower than #ys attribute size. \n
 *
 * \return
 * \f[ ys[bin] \f]
 */
inline double Serie2::TruncVal(double x)
{
	if(!xrbinE)
		bin = MathUtils::ifloor((x-x0)/dx);
	if((bin<0)||(bin > N-1)){
		cerr << "Error: bin = " << bin << " (x = " << x << ") is not included in [0," << N-1 << "]." << endl;  
		throw invalid_argument("Serie2: TruncVal(): The required bin does not exist !");
	}
	//cout << "  TruncVal : x0 = " << x0 << " , x = " << x << " , bin = " << bin;
	//cout << "  , res = " << ys[bin] << endl;
	xrbinE = false;
	return((*ys[bin]));
}


// Return value obtained by linear interpolation
/*! \brief Returns linear interpolation result.
 *
 * First, bin index is computed :
 * \f[ bin= floor(\frac{x-x_0}{dx}) \f]
 * Then bin is checked ; it must be positive or null, and lower than #ys attribute size. \n
 *
 * \return
 * \f[ (bin+1-\frac{x-x_0}{dx}) \cdot ys[bin] + \frac{x-x_0}{dx-bin} \cdot ys[bin+1] \f]
 */
inline double Serie2::InterLinear(double x)
{
	if(!xrbinE){
		xr = (x-x0)/dx ;
		bin = MathUtils::ifloor(xr);
	}
	if((bin<0)||(bin+1 > N-1)){
		cerr << "Error: bin = " << bin << " (x = " << x << ") is not included in [0," << N-1 << "]." << endl;  
		throw invalid_argument("Serie2: InterLinear(): The required bin does not exist !");
	}
	xrbinE = false;
	return( (bin + 1.0 - xr) * (*ys[bin]) + (xr-bin) * (*ys[bin+1]) ); //Linear interpolation	
}


// Return value obtained by cubic interpolation
/*! \brief Returns cubic interpolation result.
 *
 * Indices are computed : \n
 * \f$ bin_1= floor(\frac{x-x_0}{dx}) \f$ \n
 * \f$ bin_2= bin_1+1 \f$ \n
 * \f$ bin_0= bin_1-1 \f$ \n
 * \f$ bin_3= bin_2+1 \f$ \n
 * Indices are checked ; \f$bin_0 \f$ must be positive or null, and \f$bin_3 \f$ must be lower than #ys attribute size.\n
 *
 * \return
 * \f$ {\mu}^3 \cdot (ys[bin_3]-ys[bin_2]-ys[bin_0]+ys[bin_1]) \f$ \n
 * \f$ + {\mu}^2 \cdot (2 \cdot ys[bin_0]-2 \cdot ys[bin_1]+ys[bin_2]-ys[bin_3]) \f$ \n
 * \f$ + {\mu} \cdot (ys[bin_2]-ys[bin_0]) \f$ \n
 * \f$ + ys[bin_3] \f$, \n
 * where :\n
 * \f$ \mu= \frac{x-x_0}{dx}-floor(\frac{x-x_0}{dx}) \f$
 */
double Serie2::InterCubic(double x)
{
	if(!xrbinE){
		xr = (x-x0)/dx ;
		bin = MathUtils::ifloor(xr);
	}
	int bin0, bin1, bin2, bin3;
	double y0,y1,y2,y3,a0,a1,a2,a3;
	double mu;
	
	bin1=bin;
	bin2=bin1+1;  
	bin0=bin1-1;
	bin3=bin2+1;
	if((bin0<0)||(bin3 > N-1))
		throw invalid_argument("Serie2: InterCubic(): The required bin does not exist !");  
	
	y0=(*ys[bin0]);
	y1=(*ys[bin1]);
	y2=(*ys[bin2]);
	y3=(*ys[bin3]);
	
	a0=y3-y2-y0+y1;
	a1=y0-y1-a0;
	a2=y2-y0;
	a3=y1;
	
	mu=(xr)-floor(xr);
	
	xrbinE = false;
	return ((mu*mu*mu)*a0+(mu*mu)*a1+mu*a2+a3);
}

//Return value obtained by hermite interpoilation
/*! \brief Returns hermite interpolation result, depending on x, tension and bias inputs.
 *
 * Indices are computed : \n
 * \f$ bin_1= floor(\frac{x-x_0}{dx}) \f$ \n
 * \f$ bin_2= bin_1+1 \f$ \n
 * \f$ bin_0= bin_1-1 \f$ \n
 * \f$ bin_3= bin_2+1 \f$ \n
 * Indices are checked ; \f$bin_0 \f$ must be positive or null, and \f$bin_3 \f$ must be lower than #ys attribute size.\n
 *
 * \return
 * \f$ (\mu^3 - 2 \cdot \mu^2 + \mu) \cdot ys[bin_1] \f$
 * \f$	+( \mu^3 - 2 \cdot \mu^2 + \mu)) \cdot  \f$
 * \f$((ys[bin_1]-ys[bin_0]) \cdot (1+bias) \cdot \f$ \n
 * \f$ \frac{1-tension}{2}+(ys[bin_2]-ys[bin_1]) \cdot (1-bias) \cdot \frac{1-tension}{2}) \f$, \n
 * where :\n
 * \f$  \mu= \frac{x-x_0}{dx}-floor(\frac{x-x_0}{dx}) \f$
 */
double Serie2::InterHermite(double x, double tension, double bias)
{
	if(!xrbinE){
		xr = (x-x0)/dx ;
		bin = MathUtils::ifloor(xr);
	}
	int bin0, bin1, bin2, bin3;
	double y0,y1,y2,y3,a0,a1,a2,a3;
	double mu, mu2, mu3;
	double m0, m1;
	
	bin1=bin+1;
	bin2=bin1+1;
	bin0=bin1-1;
	bin3=bin2+1;
	if((bin0<0)||(bin3 > N-1))
		throw invalid_argument("Serie2: InterHermite(): The required bin does not exist !");  
	
	y0=(*ys[bin0]);
	y1=(*ys[bin1]);
	y2=(*ys[bin2]);
	y3=(*ys[bin3]);
	
	
	mu=(xr)-floor(xr);
	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0  = (y1-y0)*(1+bias)*(1-tension)/2;
	m0 += (y2-y1)*(1-bias)*(1-tension)/2;
	m1  = (y2-y1)*(1+bias)*(1-tension)/2;
	m1 += (y3-y2)*(1-bias)*(1-tension)/2;
	a0 = 2*mu3 - 3*mu2 + 1;
	a1 = mu3 - 2*mu2 + mu;
	a2 =  mu3 - mu2;
	a3 = -2*mu3 + 3*mu2;
	
	xrbinE = false;
	return(a0*y1+a1*m0+a2*m1+a3*y2); 
}


void Serie2::InitLagPolDen(int order)
{
	LagPolDen[order] = (double*) malloc((order+1)*sizeof(double));
	for(int k=0; k<=order; k++){
		LagPolDen[order][k] = 1.0;
		for(int j=0; j<=order; j++){
			if(j!=k)
				LagPolDen[order][k] *= 1.0/(k-j);
		}
	}
}


/*! \brief Returns Lagrange interpolation result.
 *
 * Indices are computed : \n
 * \f$ bin= floor(\frac{x-x_0}{dx}) \f$ \n
 * \f$ kmin= bin-ordermin+1  \textrm{ , where } ordermin=floor(\frac{order+1}{2}) \f$ \n
 * \f$ kmax= bin+order+1-ordermin \f$ \n
 * Indices are checked : 
 * \arg bin must be positive or null, and lower than (#ys attribute size -1)
 * \arg kmin must be positive or null\n
 * \arg kmax must be and lower than (#ys attribute size -1)
 *
 * \return
 * \f$ \sum_{k=kmin}^{kmax} ys[k] \cdot P_k
 \textrm{ , where }
 P_k=\prod_{j=kmin, j \ne k}^{kmax} \frac{x-x_0-j \cdot dx}{(k-j) \cdot dx} \f$
 */
inline double Serie2::InterLagrange(double x, int order)
{
	//cout << "////\\\\Serie2::InterLagrange : ordre = " << order << "  et x = " << x << endl;
	if(!xrbinE){
		xr = (x-x0)/dx ;
		bin = MathUtils::ifloor(xr);
	}
	double res(0.0), Pk(0.0);
	int ordermin(MathUtils::ifloor(double(order+1)/2.0));
	//cout << int(ys.size())-1 << endl;
	if((bin<0)||(bin+1 > N-1)){
		cerr << "Error: bin = " << bin << " (x = " << x << ") is not included in [0," << N-1 << "]." << endl;  
		throw invalid_argument("Serie2::InterLagrange : The required bin does not exist !");  
	}
	int kmin(bin-ordermin+1), kmax(bin+(order+1-ordermin));
	if(kmin < 0){
		cerr << "Error: For bin = " << bin << " (x = " << x << "), kmin = bin-ordermin+1 < 0 " << endl;
		throw invalid_argument("Serie2::InterLagrange : The required bin does not exist !");
	}
	if(kmax > N-1){
		cerr << "Error: For bin = " << bin << " (x = " << x << "), kmax = order+1-ordermin > " << N-1 << endl;
		throw invalid_argument("Serie2::InterLagrange : The required bin does not exist !");
	}
	//cout << "  InterLagrange : x0 = " << x0 << " , x = " << x;
	//cout << " , bin = " << bin << " , kmin = " << kmin << " , kmax = " << kmax;
	
	/*
	// Old Lagrange
	for(int k=kmin; k<=kmax; k++){
		Pk = 1.0;
		for(int j=kmin; j<=kmax; j++){
			if(j!=k)
				Pk *= (x-x0-j*dx)/((k-j)*dx);
		}
		res += (*ys[k])*Pk;
	}
	*/
	
	// ** New Lagrange with tabulation of polynome denominator
	int RealOrder (kmax - kmin);
	if(LagPolDen[RealOrder] == NULL)
		InitLagPolDen(RealOrder);
	
	for(int k=kmin; k<=kmax; k++){
		Pk = LagPolDen[RealOrder][k-kmin];
		for(int j=kmin; j<=kmax; j++){
			if(j!=k)
				Pk *= xr-j ;
		}
		res += (*ys[k])*Pk;
	}
	
	
	//cout << "  , res = " << res << endl;
	xrbinE = false;
	return(res);
}


/*double Serie2::InterSincTrunc(double x, int Nlenght) const
{
	//cout << "////\\\\Serie2::InterLagrange : ordre = " << order << "  et x = " << x << endl;
	double res(0.0);
	int Nhalf((int)(floor((double)(Nlenght-1)/2.0)));
	int bin((int)(round(xr)));
	double D(fmod(xr,1.0));
	if(D>=0.5)
		D -= 1.0;
	//cout << "Serie2::InterSincTrunc : N = " << Nlenght << endl;
	//cout << "  x   = " << x << endl;
	//cout << "  D   = " << D << endl;
	if(bin-Nhalf < 0)
		throw invalid_argument("Serie2::InterSincTrunc : The required bin does not exist !");
	if(bin+Nhalf > int(ys.size())-1)
		throw invalid_argument("Serie2::InterSincTrunc : The required bin does not exist !");
	//cout << "  InterLagrange : x0 = " << x0 << " , x = " << x;
	//cout << " , bin = " << bin << " , kmin = " << kmin << " , kmax = " << kmax;
	
	for(int k=-1*Nhalf; k<=Nhalf; k++){
		res += ys[bin+k]*sin(D-k)/(D-k);
	}
	//cout << "res = " << res << endl;
	//cout << endl;
	return(res);
}
*/


//end of Serie2.cpp
