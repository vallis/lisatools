// $Id: LISACODE-Serie.cpp,v 1.5 2007/04/05 09:33:22 ruiloba Exp $
/*
 *  LISACODE-Serie.cpp
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
 *  Last modification on 27/05/09 by Antoine Petiteau (AEI)
 *
 */

#include "LISACODE-Serie.h"


/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg  x0 = 0
 * \arg  dx =1
 */
Serie::Serie()
{
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
Serie::Serie(double start, double delta)
{
	x0 = start;
	dx = delta;
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
}


/*! \brief Constructs an instance and initializes it with inputs and default values.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 * \arg  ys = length elements, with 0 values
 */
Serie::Serie(double start, double delta, int length)
{
	x0 = start;
	dx = delta;
	ys.resize(length);
	for(int i=0; i<length; i++)
		ys[i] = 0.0;
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
}


/*! \brief Constructs an instance and initializes it with inputs.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 * \arg  ys = ys_n input
 */
Serie::Serie(double start, double delta, vector<double> ys_n)
{
 	x0 = start;
	dx = delta;
	ys = ys_n;
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
}


/*! \brief Constructs an instance and initializes it with data read in fname input file.
 *
 * Input file must have at least 2 elements.\n
 * Attributes are set :
 * \arg  x0 = first read element
 * \arg  dx = difference betwwen second and first read elements
 * \arg  ys = read elements
 */
Serie::Serie(char * fname)
{
	double x,y;
	ifstream file;
	file.open(fname,ios::in);
	
	if(file.eof())
		throw invalid_argument("Serie: Not enough of data !"); 
	file >> x0 >> y;
	ys.push_back(y);
	if(file.eof())
		throw invalid_argument("Serie: Not enough of data !");
	file >> x >> y;
	ys.push_back(y);
	
	dx = x-x0;
	
	while(!file.eof()) {
		file >> x >> y;
		ys.push_back(y);
	}
	
	file.close();
	
	for(int i=0; i<ORDERMAXLAG; i++)
		LagPolDen[i] = NULL;
}


/* Destructor */
/*! \brief Destructor. */
Serie::~Serie()
{
	while(ys.size()>0)
		ys.pop_back();
}


/* Access methods */
/*! \brief Sets #ys attribute size to lenght input. */
void Serie::setNbVal(int lenght)
{
	ys.resize(lenght);
}

/*! \brief Sets #x0 attribute size to start input. */
void Serie::setRefStart(double start)
{
	x0 = start;
}

/*! \brief Sets #dx attribute size to delta input. */
void Serie::setRefStep(double delta)
{
	dx = delta;
	
}

/*! \brief Gets reference value corresponding to bin input.
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size. \n
 * \return
 * \f$ x0 + dx \cdot bin \f$
 */
double Serie::getRef(int bin) const
{
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: The required bin does not exist !");  
	return(x0 + dx*(bin));
}

/*! \brief Gets reference y value corresponding to bin input.
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size. \n
 * \return
 * ys[bin]
 */
double Serie::getBinValue(int bin) const
{
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: The required bin does not exist !");  
	return(ys[bin]);
}

/*! \brief Sets reference y value corresponding to bin and x inputs.
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size. \n
 * Then ys is filled :
 * ys[bin]=x .
 */
void Serie::setBinValue(int bin, double x)
{
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: The required bin does not exist !");  
	ys[bin] = x;
}



/* Other methods */
//Add data at the begining of the serie
/*! \brief Adds data at the begining of the serie. */
void Serie::addData(double y)
{
	ys.insert(ys.begin(),y);
} 

// Delete the last data of the serie
/*! \brief Deletes the last data of the serie. */
void Serie::delLastData()
{
	ys.pop_back();
}

// Delete the last data of the serie
/*! \brief Deletes the last elements of the serie #ys, between index associated to \a xMax and the end of the serie.
*/
void Serie::delLastData(double xMax)
{
	int NbBinMax((int)(xMax/dx+0.5));
	while(NbBinMax<((int)ys.size())){
		ys.pop_back();
	}
}


// Write the serie to a file as two columns X, Y
/*! \brief Writes the serie as two columns (X,Y) into fname input file. */
void Serie::wfile(char* fname)
{
	int k;
	ofstream file;
	double x;
	file.open(fname,ios::out | ios::trunc);
	
	for(k=0; k< int(ys.size()); k++){
		x=x0+k*dx;
		file << x << " " << ys[k];
		if(k != int(ys.size())-1) file << endl;
	}
	file.close();
}

// Read the serie to a file as two columns X, Y
/*! \brief Reads the serie as two columns (X,Y) from fname input file. */
void Serie::rfile(char* fname)
{
	double x,y;
	ifstream file;
	file.open(fname,ios::in);
	
	for(int k=0; k< int(ys.size()); k++) {
		if(file.eof())
			throw invalid_argument("Serie: Not enough of data !"); 
		file >> x >> y;
		ys[k] = y;
	}
	
	file.close();
	
}


// Return the exact value if x is a multiple of dx, else return interpolate value
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
double Serie::gData(double x, INTERP InterpolType, double InterpUtilValue)
{
	//cout << "Serie::gData :" << endl;
	//cout << "  fmod((x-x0)/dx,1.0) = " << fmod((x-x0)/dx,1.0) << endl;
	if(fmod((x-x0)/dx,1.0) < PRECISION){
		//cout << "  --> Tronque" << endl;
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
				throw invalid_argument("Serie: No interpolation method !");
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
double Serie::TruncVal(double x) const
{
	int bin((int)floor((x-x0)/dx));
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: TruncVal(): The required bin does not exist !");
	//cout << "  TruncVal : x0 = " << x0 << " , x = " << x << " , bin = " << bin;
	//cout << "  , res = " << ys[bin] << endl;
	return(ys[bin]);
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
double Serie::InterLinear(double x) const
{
	int bin((int)floor((x-x0)/dx));
	if((bin<0)||(bin+1 > int(ys.size())-1))
		throw invalid_argument("Serie: InterLinear(): The required bin does not exist !"); 
	cout.precision(20);
	return((bin + 1 - (x-x0)/dx)*ys[bin] + ((x-x0)/dx-bin)*ys[bin+1]); //Linear interpolation	
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
double Serie::InterCubic(double x) const
{
	int bin0, bin1, bin2, bin3;
	double y0,y1,y2,y3,a0,a1,a2,a3;
	double mu;
	
	bin1=(int)floor((x-x0)/dx);
	bin2=bin1+1;  
	bin0=bin1-1;
	bin3=bin2+1;
	if((bin0<0)||(bin3 > int(ys.size())-1))
		throw invalid_argument("Serie: InterCubic(): The required bin does not exist !");  
	
	y0=ys[bin0];
	y1=ys[bin1];
	y2=ys[bin2];
	y3=ys[bin3];
	
	a0=y3-y2-y0+y1;
	a1=y0-y1-a0;
	a2=y2-y0;
	a3=y1;
	
	mu=((x-x0)/dx)-floor((x-x0)/dx);
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
double Serie::InterHermite(double x, double tension, double bias) const
{
	int bin0, bin1, bin2, bin3;
	double y0,y1,y2,y3,a0,a1,a2,a3;
	double mu, mu2, mu3;
	double m0, m1;
	
	bin1=(int)floor((x-x0)/dx)+1;
	bin2=bin1+1;
	bin0=bin1-1;
	bin3=bin2+1;
	if((bin0<0)||(bin3 > int(ys.size())-1))
		throw invalid_argument("Serie: InterHermite(): The required bin does not exist !");  
	
	y0=ys[bin0];
	y1=ys[bin1];
	y2=ys[bin2];
	y3=ys[bin3];
	
	
	mu=((x-x0)/dx)-floor((x-x0)/dx);
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
	
	return(a0*y1+a1*m0+a2*m1+a3*y2); 
}


void Serie::InitLagPolDen(int order)
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
double Serie::InterLagrange(double x, int order)
{
	double xr((x-x0)/dx);
	//cout << "////\\\\Serie::InterLagrange : ordre = " << order << "  et x = " << x << endl;
	double res(0.0), Pk(0.0);
	int ordermin((int)(floor(double(order+1)/2.0)));
	int bin((int)(floor((x-x0)/dx)));
	//cout << int(ys.size())-1 << endl;
	if((bin<0)||(bin+1 > int(ys.size())-1))
		throw invalid_argument("Serie::InterLagrange : The required bin does not exist !");   
	int kmin(bin-ordermin+1), kmax(bin+(order+1-ordermin));
	if(kmin < 0)
		throw invalid_argument("Serie::InterLagrange : The required bin does not exist !");
	if(kmax > int(ys.size())-1)
		throw invalid_argument("Serie::InterLagrange : The required bin does not exist !");
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
		res += ys[k]*Pk;
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
		res += ys[k]*Pk;
	}
	
	//cout << "Serie::InterLagrange : res = " << res << endl;
	
	return(res);
}


/*double Serie::InterSincTrunc(double x, int Nlenght) const
{
	//cout << "////\\\\Serie::InterLagrange : ordre = " << order << "  et x = " << x << endl;
	double res(0.0);
	int Nhalf((int)(floor((double)(Nlenght-1)/2.0)));
	int bin((int)(round((x-x0)/dx)));
	double D(fmod((x-x0)/dx,1.0));
	if(D>=0.5)
		D -= 1.0;
	//cout << "Serie::InterSincTrunc : N = " << Nlenght << endl;
	//cout << "  x   = " << x << endl;
	//cout << "  D   = " << D << endl;
	if(bin-Nhalf < 0)
		throw invalid_argument("Serie::InterSincTrunc : The required bin does not exist !");
	if(bin+Nhalf > int(ys.size())-1)
		throw invalid_argument("Serie::InterSincTrunc : The required bin does not exist !");
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




/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg  x0 = 0
 * \arg  dx =1
 */
SerieC::SerieC()
{
	x0 = 0.0;
	dx = 1.0;
}

/*! \brief Constructs an instance and initializes it with inputs and default values.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 */
SerieC::SerieC(double start, double delta)
{
	x0 = start;
	dx = delta;
}


/*! \brief Constructs an instance and initializes it with inputs and default values.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 * \arg  ys = lenght elements, with 0 values
 */
SerieC::SerieC(double start, double delta, int lenght)
{
	x0 = start;
	dx = delta;
	ys.resize(lenght);
	for(int i=0; i<lenght; i++)
	{
		ys[i]= 0.0;
	}
}


/*! \brief Constructs an instance and initializes it with inputs.
 *
 * \arg  x0 = start
 * \arg  dx = delta
 * \arg  ys = ys_n input
 */
SerieC::SerieC(double start, double delta, vector<complex<double> > ys_n)
{
 	x0 = start;
	dx = delta;
	ys = ys_n;
}


/*! \brief Constructs an instance and initializes it with data read in fname input file.
 *
 * Input file must have at least 2 elements.\n
 * Attributes are set :
 * \arg  x0 = first read element
 * \arg  dx = difference betwwen second and first read elements
 * \arg  ys = read elements
 */
SerieC::SerieC(char * fname)
{
	double x,yre,yim;
	ifstream file;
	complex<double> y, i(0,1);
	file.open(fname,ios::in);
	
	if(file.eof())
		throw invalid_argument("Serie: Not enough of data !"); 
	file >> x0 >> yre >> yim;
	y= yre + i*yim;
	ys.push_back(y);
	if(file.eof())
		throw invalid_argument("Serie: Not enough of data !");
	file >> x >> yre >>yim;
	y= yre + i*yim;
	ys.push_back(y);
	
	dx = x-x0;
	
	while(!file.eof()) {
		file >> x >> yre >> yim;
		y= yre + i*yim;
		ys.push_back(y);
	}
	
	file.close();
}


/* Destructor */
/*! \brief Destructor */
SerieC::~SerieC()
{
	while(ys.size()>0)
		ys.pop_back();
}


/* Access methods */
/*! \brief Sets #ys attribute size to lenght input */
void SerieC::setNbValC(int lenght)
{
	ys.resize(lenght);
}

/*! \brief Sets #x0 attribute size to start input */
void SerieC::setRefStartC(double start)
{
	x0 = start;
}

/*! \brief Sets #dx attribute size to delta input */
void SerieC::setRefStepC(double delta)
{
	dx = delta;
	
}

/*! \brief Gets reference value corresponding to bin input
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size.\n
 * \return
 * \f$ x0 + dx \cdot bin \f$
 */
double SerieC::getRefC(int bin) const
{
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: The required bin does not exist !");  
	return(x0 + dx*(bin));
}

/*! \brief Gets reference y value corresponding to bin input
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size.\n
 * \return
 *  ys[bin]
 */
complex<double> SerieC::getBinValueC(int bin) const
{
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: The required bin does not exist !");  
	return(ys[bin]);
}

/*! \brief Sets reference y value corresponding to bin and x inputs
 *
 * Input is checked ; it must be positive or null, and lower than #ys attribute size.\n
 * \return
 *  ys[bin]=x
 */
void SerieC::setBinValueC(int bin, complex<double> x)
{
	if((bin<0)||(bin > int(ys.size())-1))
		throw invalid_argument("Serie: The required bin does not exist !");  
	ys[bin] = x;
}



/* Other methods */
//Add data at the begining of the serie
/*! \brief Adds data at the begining of the serie. */
void SerieC::addDataC(complex<double> y)
{
	ys.insert(ys.begin(),y);
} 

// Delete the last data of the serie
/*! \brief Deletes the last data of the serie. */
void SerieC::delLastDataC()
{
	ys.pop_back();
}

// Delete the last data of the serie
/*! \brief Deletes the last data of the serie, while x reference is greater than xMax input. */
void SerieC::delLastDataC(double xMax)
{
	int NbBinMax((int)(xMax/dx));
	while(NbBinMax<((int)ys.size())){
		ys.pop_back();
	}
}


// Write the serie to a file as two columns X, Y
/*! \brief Writes the serie as 3 columns (X,Y.real,Y.imag) into fname input file. */
void SerieC::wfileC(char* fname)
{
	int k;
	ofstream file;
	double x;
	file.open(fname,ios::out | ios::trunc);
	
	for(k=0; k< int(ys.size()); k++){
		x=x0+k*dx;
		file << x << " " << real(ys[k]) << " " << imag(ys[k]) ;
		if(k != int(ys.size())-1) file << endl;
	}
	file.close();
}

// Read the serie to a file as two columns X, Y
/*! \brief Writes the serie as 3 columns (X,Y.real,Y.imag) from fname input file. */
void SerieC::rfileC(char* fname)
{
	double x,yre,yim;
	complex<double> i(0,1);
	ifstream file;
	file.open(fname,ios::in);
	
	for(int k=0; k< int(ys.size()); k++) {
		if(file.eof())
			throw invalid_argument("Serie: Not enough of data !"); 
		file >> x >> yre >> yim;
		ys[k] = yre +i*yim;
	}
	
	file.close();
	
}



//end of Serie.cpp
