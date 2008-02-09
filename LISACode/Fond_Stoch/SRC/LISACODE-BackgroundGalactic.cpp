// $Id: LISACODE-BackgroundGalactic.cpp,v 1.8 2007/03/29 15:16:59 lalanne Exp $
/*
 *  LISACODE-BackgroundGalactic.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *
 *  Created on 30/05/06 by Antoine PETITEAU (APC)
 *  Last modification on 30/05/06 by Antoine PETITEAU (APC)
 *
 */


#include "LISACODE-BackgroundGalactic.h"

/* Constructor */

/*! \brief Constructs an instance and initializes its attributes with default values.
 *
 * Default values are:
 * \arg	#LISAGeo = Geometry instance with default attributes.
 * \arg	#TimeList = NULL
 * \arg	#SignalList = NULL
 * \arg	#NbData = 0
 * \arg	#iRead = -1
 * \arg #tmp_t = -1.0
 */
BackgroundGalactic::BackgroundGalactic() : 
Background()
{
	TimeList.resize(0);
	SignalList.resize(0);
	NbData = 0;
	iRead = -1;
	tmp_t = -1.0;
}

/*! \brief Constructs an instance and initializes its attributes with default values and inputs.
 *
 * It calls #ReadFile.
 * \param FileName: name of file containing signal values and associated times 
 * \param Factor: scale factor applied to read signal values
 * 
 * Attributes are set as follows
 * \arg	#LISAGeo = Geometry instance with default attributes.
 * \arg	#TimeList is filled by #ReadFile method.
 * \arg	#SignalList is filled by #ReadFile  method.
 * \arg	#NbData is filled by #ReadFile method.
 * \arg	#iRead = -1
 * \arg	#tmp_t = -1.0
 */
BackgroundGalactic::BackgroundGalactic(char * FileName, double Factor) : 
Background()
{
	TimeList.resize(0);
	SignalList.resize(0);
	NbData = 0;
	iRead = -1;
	tmp_t = -1.0;
	ReadFile(FileName, Factor);
}


/*! \brief Constructs an instance and initializes its attributes with default values and inputs.
 *
 * It calls #ReadFile.
 * \param LISAGeo_n: LISA orbit.
 * \param FileName: name of file containing signal values and associated times 
 * \param Factor: scale factor applied to read signal values
 *
 * Attributes will be set to next values:
 * \arg	#LISAGeo = LISAGeo_n input
 * \arg	#TimeList is filled by #ReadFile method.
 * \arg	#SignalList is filled by #ReadFile method.
 * \arg	#NbData is filled by #ReadFile method.
 * \arg	#iRead = -1
 * \arg	#tmp_t = -1.0
 */
BackgroundGalactic::BackgroundGalactic(Geometry * LISAGeo_n, char * FileName, double Factor) :
Background(LISAGeo_n)
{
	TimeList.resize(0);
	SignalList.resize(0);
	NbData = 0;
	iRead = -1;
	tmp_t = -1.0;
	ReadFile(FileName, Factor);
}


/*! \brief Destructor
 */
BackgroundGalactic::~BackgroundGalactic()
{

}



/*  Others methods */
/*! \brief Reads signal samples and associated times from file specified in argument.
 * 
 * #NbData is set to number of signal samples read from file.\n
 * It runs as follows:
 * File's lines beginning with "#" character are ignored.
 * While end of file is not reached :
 * \arg time is read, then pushed back into #TimeList attribute
 * \arg	6 signals are read, then multiplied by \a Factor input and pushed back into #SignalList attribute
 *
 * 
 */
void BackgroundGalactic::ReadFile(char * FileName, double Factor)
{
	ifstream ReadFile;
	string junk;
	double tmp_time;
	vector<double> tmp_signal(6);
	
	ReadFile.open(FileName);
	
	if (!ReadFile){
		cerr << "BackgroundGalactic::ReadFile : Error to open the file  " << FileName << endl;
		exit(1);
	}
	
	// Lit le header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
		ReadFile.ignore(256,'\n'); 
	};
	// Read values
	NbData = 0;
	while(!ReadFile.eof()){
		ReadFile >> tmp_time;
		//cout << tmp_time << endl;
		for(int i=0; i<6; i++){
			ReadFile >> tmp_signal[i];
			tmp_signal[i] *= Factor;
			//cout << " " << tmp_signal[i];
		}
		//cout << endl;
		TimeList.push_back(tmp_time);
		SignalList.push_back(tmp_signal);
		NbData++;
	};
	
	ReadFile.close();
}


/*! \brief Gives frequency fluctuation.
 *
 * \param iSC: spacecraft index (1,2 or 3 for each LISA satellite)
 * \param IndirSens: direction flag (0 if the optical bench is in the direct direction, else 1)
 * \param t: time 
 *
 * It computes \a tGeo to obtain deltanu:\n
 * \f$ tGeo=mod(t+t0_{LISAGeo},31557600) \f$ (annual periodicity)\n
 * If \f$ tGeo \notin [TimeList[0],TimeList[size(TimeList)-2])\f$, returned value =0.\n 
 * Else:\n
 * iRead index is  found, such as \f$ TimeList[iRead] \le tGeo < TimeList[iRead+1] \f$
 * \f[ \textrm{returned value}= \frac{TimeList[iRead+1]-tGeo}{TimeList[iRead+1]-TimeList[iRead]} \cdot tmp\_Sig\_i[iSC+3 \cdot IndirSens-1] \f]
 * \f[ +\frac{tGeo-TimeList[iRead]}{TimeList[iRead+1]-TimeList[iRead]} \cdot tmp\_Sig\_ip1[iSC+3 \cdot IndirSens-1] \f]
*/
double BackgroundGalactic::deltanu(int iSC, int IndirSens, double t)
{
	double tGeo(t+LISAGeo->gett0());
	double result(0.0);
	int iPha(iSC+3*IndirSens-1);
	
	while(tGeo>31557600){ // Periodicite sur l'annee
		tGeo-=31557600;
	};
	cout.precision(15);
	//cout << "tGeo = " << tGeo << endl;
	// Find the good bin
	if((tGeo<TimeList[0])||(tGeo>TimeList[TimeList.size()-2])){
		result = 0.0;
		// throw invalid_argument("GWFile::Interpol : The value for this time cannot be evaluated !");
	}else {
		if(fabs(tGeo-tmp_t)>PRECISION){
			double invdt;
			int iReadNew(iRead);
			tmp_t = tGeo;
			while(tGeo<TimeList[iReadNew]){
				iReadNew--;
			};
			while(tGeo>=TimeList[iReadNew+1]){
				iReadNew++;
			};
			if(iReadNew!=iRead){
				iRead = iReadNew;
				tmp_Sig_i = SignalList[iRead];
				tmp_Sig_ip1 = SignalList[iRead+1];
			}
			invdt = 1.0/(TimeList[iRead+1]-TimeList[iRead]);
			tmp_ci = (TimeList[iRead+1]-tGeo)*invdt;
			tmp_cip1 = (tGeo-TimeList[iRead])*invdt;
		}
		result = tmp_ci*tmp_Sig_i[iPha] + tmp_cip1*tmp_Sig_ip1[iPha];
	}
	return(result);
}

// end of LISACODE-GWFile.cpp
