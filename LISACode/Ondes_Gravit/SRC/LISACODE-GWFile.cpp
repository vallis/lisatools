// $Id: LISACODE-GWFile.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWFile.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-GWFile.h"

/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * Inherited attributes from GW are set to default values. 
 * \arg	#TimeList = empty
 * \arg	#hList = empty
 * \arg	#LastUsedBin = 0
 */
GWFile::GWFile() : 
GW()
{
	TimeList.resize(0);
	hList.resize(0);
	LastUsedBin = 0;
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs
 * \arg	#TimeList = empty
 * \arg	#hList = empty
 * \arg	#LastUsedBin = 0
 */
GWFile::GWFile(double Beta_n, double Lambda_n, double AnglPol_n) : 
GW(Beta_n, Lambda_n, AnglPol_n)
{
	TimeList.resize(0);
	hList.resize(0);
	LastUsedBin = 0;
}


/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs as arguments
 * \arg	#TimeList = empty
 * \arg	#hList = empty
 * \arg	#LastUsedBin = 0
 * #TimeList and #hList attributes are filled using #ReadFile method with \a FileName input as argument
 */
GWFile::GWFile(double Beta_n, double Lambda_n, double AnglPol_n, char * FileName) : 
GW(Beta_n, Lambda_n, AnglPol_n)
{
	TimeList.resize(0);
	hList.resize(0);
	LastUsedBin = 0;
	ReadFile(FileName);
}

/*! \brief Destructor
 */
GWFile::~GWFile()
{
	TimeList.clear();
	hList.clear();
}



/*  Others methods */
/*! \brief Reads file specified in argument
 *
 * Header (lines begining with "#" charected) are ignored.
 * For each line, time and 2 components are read and pushed back into #TimeList and #hList attributes.
 */
void GWFile::ReadFile(char * FileName)
{
	ifstream ReadFile;
	string junk;
	double tmp_time;
	Couple tmp_h;
	
	ReadFile.open(FileName);
	if (!ReadFile){
		throw invalid_argument("GW: The gravitational file can't be open !");
	}
	// Lit le header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
		ReadFile.ignore(256,'\n'); 
	};
	// Read values
	while(!ReadFile.eof()){
		ReadFile >> tmp_time;
		ReadFile >> tmp_h.x;
		ReadFile >> tmp_h.y;
		
		if(ReadFile.eof())
			break;
		//cout << tmp_time << " " << tmp_h.x << " " << tmp_h.y << endl;
		TimeList.push_back(tmp_time);
		hList.push_back(tmp_h);
	};
	
	ReadFile.close();
}

/*! \brief Makes Lagrange order 3 interpolation at tinput time \a t, in list of #hList specified by \a type (1 for h_plus (hp) or 2 for h_cross (hc))
 *
 * \return \f[ returned value=\left\{ \begin{array}{ll}
 * \sum_{k=kmin}^{kmax} hList[k].x \cdot P_k & \textrm{if type=1} \\
 * \sum_{k=kmin}^{kmax} hList[k].y \cdot P_k & \textrm{if type=2} \\
 * \end{array} \right. \f]
 * \f[ \textrm{where } P_k=\prod_{j=kmin, j \ne k}^{kmax} \frac{t-TimeList[j]}{TimeList[k]-TimeList[j]} \f]
 * LastUsedBin is updated, so that \f[ TimeList[LastUsedBin] \le t \le TimeList[LastUsedBin+1] \f]
 *
 * \f[ ordermin=floor(\frac{order+1}{2}) \textrm{ where } order=3 \f]
 * \f[ kmin=min(0,LastUsedBin-ordermin+1) \f]
 * \f[ kmax=max(LastUsedBin+order+1-ordermin,sizeof(TimeList)-1) \f]
 */
double GWFile::Interpol(double t, int type)
{
	int order(3); // Order of lagrange interpolation
	double res(0.0);
	
	//cout << "t = " << t << "  ,  t0 = " << TimeList[0] << "  ,  tn = " << TimeList[TimeList.size()-2] << endl;
	
	// Find the good bin
	if((t<TimeList[0])||(t>TimeList[TimeList.size()-2])){
		// throw invalid_argument("GWFile::Interpol : The value for this time cannot be evaluated !");
	}else {
		while(t<TimeList[LastUsedBin]){
			LastUsedBin--;
		};
		while(t>=TimeList[LastUsedBin+1]){
			LastUsedBin++;
		};
	
		// Make Lagrange interpolation
		double Pk(0.0);
		int ordermin((int)(floor(double(order+1)/2.0)));
		int kmin(LastUsedBin-ordermin+1), kmax(LastUsedBin+(order+1-ordermin));
		if(kmin < 0)
			kmin = 0;
		if(kmax > int(TimeList.size())-1)
			kmax = int(TimeList.size())-1;
		for(int k=kmin; k<=kmax; k++){
			Pk = 1.0;
			for(int j=kmin; j<=kmax; j++){
				if(j!=k){
					//cout << " - TimeList[" << j << "] = " << TimeList[j] << "  TimeList[" << k << "] = " <<  TimeList[k];
					//cout << "   --> (TimeList[k]-TimeList[j]) = " << (TimeList[k]-TimeList[j]) << endl;
					Pk *= (t-TimeList[j])/(TimeList[k]-TimeList[j]);
				}
			}
			//cout << " Pk [" << k << "] = " << Pk;
			if(type==1){
				res += hList[k].x*Pk;
				//cout << "  - hpList[" << k << "] = " << hList[k].x << endl;
			}else{
				res += hList[k].y*Pk;
				//cout << "  - hxList[" << k << "] = " << hList[k].y << endl;
			}
			//cout << endl;
		}
	}
	
	//cout << " t = " << t << " Interp(" << type << ") = " << res << endl << endl; 
	
	return(res);
	
}


/*! \brief Gives h_plus polarisation component depending on \a t input time.
 *
 * returned value = result of #Interpol method with t input time and 1 as arguments
 */
double GWFile::hp(double t)
{	
	return(Interpol(t, 1));
}


/*! \brief Gives h_cross polarisation component depending on \a t input time.
 *
 * \return Result of #Interpol method with \a t input time and 2 as arguments.
 */
double GWFile::hc(double t)
{
	return(Interpol(t, 2));
}



// end of LISACODE-GWFile.cpp
