// $Id: $
/*
 *  LISACODE-GWFile2.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#include "LISACODE-GWFile2.h"

// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * Inherited attributes from GW are set to default values. 
 * \arg	#LastUsedBin = 0
 */
GWFile2::GWFile2() : 
GW()
{
	hps = NULL;
	hcs = NULL;
	N = 0;
	t0 = 0.0;
	dt = -1.0;
	FileEncoding = 0;
	NRec = 0;
	InterpType = LAG;
	InterpVal = 4;
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs as arguments
 * \arg	#LastUsedBin = 0
 * #TimeList and #hList attributes are filled using #ReadASCIIFile method with \a FileName input as argument
 */
GWFile2::GWFile2(double Beta_n, double Lambda_n, double AnglPol_n, char * FileName_n) : 
GW(Beta_n, Lambda_n, AnglPol_n)
{
	hps = NULL;
	hcs = NULL;
	N = 0;
	t0 = 0.0;
	dt = -1.0;
	strcpy(FileName,FileName_n);
	FileEncoding = 0;
	NRec = 0;
	InterpType = LAG;
	InterpVal = 4;
	
	init();
}


/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs as arguments
 * \arg	#LastUsedBin = 0
 * #TimeList and #hList attributes are filled using #ReadASCIIFile or #ReadBinaryFile method with \a FileName input as argument
 * \a FileEncoding get type of file (0: ASCII , 1: Binary)
 */
GWFile2::GWFile2(double Beta_n, 
				 double Lambda_n, 
				 double AnglPol_n, 
				 char * FileName_n, 
				 int FileEncoding_n, 
				 double TimeOffset_n, 
				 double TimeStep_n, 
				 int Length_n, 
				 int Record_n) : 
GW(Beta_n, Lambda_n, AnglPol_n)
{
	hps = NULL;
	hcs = NULL;
	strcpy(FileName,FileName_n);
	FileEncoding = FileEncoding_n;
	t0 = TimeOffset_n;
	dt = TimeStep_n;
	N = Length_n;
	NRec = Record_n;
	InterpType = LAG;
	InterpVal = 4;
	
	init();
}


/*! \brief Destructor
 */
GWFile2::~GWFile2()
{
	if(hps)
		delete hps;
	if(hcs)
		delete hcs;
}

// **********************
// **  Access methods  **
// **********************

void GWFile2::setParam(int iP, double Param_n){
	switch (iP) {
		case 0:
			Beta = Param_n;
			break;
		case 1:
			Lambda = Param_n;
			break;
		case 2:
			AnglPol = Param_n;
			break;
		default:
			break;
	}
}

double GWFile2::getParam(int iP){
	switch (iP) {
		case 0:
			return (Beta);
			break;
		case 1:
			return (Lambda);
			break;
		case 2:
			return (AnglPol);
			break;
		default:
			return (0.0);
			break;
	}
}


void GWFile2::setFileName(char * FileName_n)
{
	strcpy(FileName,FileName_n);
}


// **********************
// **  Others methods  **
// **********************

void GWFile2::init()
{
	if(FileEncoding){
		// Read binary file
		ReadBinaryFile();
	}else{
		// Read ASCII
		ReadASCIIFile();
	}	
}


/*! The number of columns of the file must be 2 (h+,hx) or 3 (t,h+,hx). If it is 2, time informations 
 * are specifie in arguments \a TimeOffset and \a TimeStep.
 * If \a TimeStep is -1.0, it is means that the time informations are not specifed.
 * Header (lines begining with "#" charected) are ignored.
 * For each line, time and 2 components are read and pushed back into #TimeList and #hList attributes.
 */
void GWFile2::ReadASCIIFile()
{
	ifstream ReadFile;
	string junk;
	double t1, hptmp, hctmp;
	
	/****  Read the number of columns ****/
	ReadFile.open(FileName);
	if (!ReadFile)
		throw invalid_argument("GWFile2::ReadASCIIFile : The gravitational file can't be open !");
	// ** Read header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
		ReadFile.ignore(1024,'\n'); 
	};
	// ** Read the first line and count the number of NRec
	NRec = 0;
	while((ReadFile.peek() != '\n')&&(!ReadFile.eof())){
		ReadFile >> junk;
		NRec++;
	}
	ReadFile.close();
	ReadFile.clear();
	// ** Control on the number of columns
	cout << " (" << NRec; fflush(stdout);
	if((NRec<2)||(NRec>3))
		throw invalid_argument("GWFile2::ReadASCIIFile : The number of columns are bad (only 2 or 3) !");
	if((NRec==2)&&(dt<=0.0))
		throw invalid_argument("GWFile2::ReadASCIIFile : The time informations are not specified !");
	
	/****  Read the number of lines ****/
	ReadFile.open(FileName);
	if (!ReadFile)
		throw invalid_argument("GWFile2::ReadASCIIFile : The gravitational file can't be open !");
	// Read header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
		ReadFile.ignore(1024,'\n'); 
	};
	// Count the number of lines
	N = 0;
	while(!ReadFile.eof()){
		if((NRec==3)&&(N=0))
			ReadFile >> t0;
		if((NRec==3)&&(N=1))
			ReadFile >> t1;
		ReadFile.ignore(1024,'\n');
		if(!ReadFile.eof()){
			N++;
		}
	}
	ReadFile.close();
	ReadFile.clear();
	dt = t1 - t0;
	tend = t0 + N*dt;
	cout << "x" << N << ") (dt=" << dt << "s)..."; fflush(stdout);	      
	
	/****  Allocate memory ****/
	hps = new Serie2(t0-dt*InterpVal, dt, N+2*InterpVal);
	hcs = new Serie2(t0-dt*InterpVal, dt, N+2*InterpVal);
	hps->allocAll();
	hcs->allocAll();
	
	/****  Read data ****/
	ReadFile.open(FileName);
	if (!ReadFile)
		throw invalid_argument("GWFile2::ReadASCIIFile : The gravitational file can't be open !");
	// Read header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
		ReadFile.ignore(1024,'\n'); 
	};
	for(int iL=0; iL<N; iL++){
		if(NRec == 3)
			ReadFile >> t1;
		ReadFile >> hptmp >> hctmp;
		hps->setBinValue(iL+InterpVal, hptmp);
		hcs->setBinValue(iL+InterpVal, hctmp);
	}	
	ReadFile.close();
}


/*! \brief Reads binary file specified in argument
 *
 * The number of columns of the file must be 2 (h+,hx) or 3 (t,h+,hx). If it is 2, time informations 
 * are specifie in arguments \a TimeOffset and \a TimeStep.
 * If \a TimeStep is -1.0, it is means that the time informations are not specifed.
 * Header (lines begining with "#" charected) are ignored.
 * For each line, time and 2 components are read and pushed back into #TimeList and #hList attributes.
 */
void GWFile2::ReadBinaryFile()
{
	FILE * ReadBinFile;
	double * VecData;
	
	tend = t0 + N*dt;
	cout << " (" << NRec << "x" << N << ")"; fflush(stdout);
	
	/****  Allocate memory ****/
	hps = new Serie2(t0-dt*InterpVal, dt, N+2*InterpVal);
	hcs = new Serie2(t0-dt*InterpVal, dt, N+2*InterpVal);
	hps->allocAll();
	hcs->allocAll();
	
	/**** Read Data ****/
	ReadBinFile = fopen(FileName,"rb");
	if(ReadBinFile == NULL){
		throw invalid_argument("GWFile2::ReadBinaryFile : Problem in opening the file ");
	}
	
	// Allocate memory
	VecData = (double*)malloc(N*NRec*sizeof(double));
	
	// Read binary data
	fread(VecData,sizeof(double),N*NRec,ReadBinFile);
	
	// ** Sort data
	if(NRec == 2){
		// No time data
		for(int iL=0; iL < N; iL++){
			hps->setBinValue(iL+InterpVal, VecData[iL*2]);
			hcs->setBinValue(iL+InterpVal,VecData[iL*2+1]);
			//cout << TimeList[iL] << " " <<  hList[iL].x << " " <<  hList[iL].y <<  endl;
		}
	}else{
		// Time data included
		for(int iL=0; iL < N; iL++){
			hps->setBinValue(iL+InterpVal, VecData[iL*NRec+1]);
			hcs->setBinValue(iL+InterpVal, VecData[iL*NRec+2]);
		}
	}
	free(VecData);
}

 

/*! \brief Gives h_plus polarisation component depending on \a t input time.
 *
 * returned value = result of #Interpol method with t input time and 1 as arguments
 */
double GWFile2::hp(double t)
{	
	if((t>=t0)&&(t<tend))
		return(hps->gData(t, InterpType, InterpVal));
	else
		return(0.0);
}


/*! \brief Gives h_cross polarisation component depending on \a t input time.
 *
 * \return Result of #Interpol method with \a t input time and 2 as arguments.
 */
double GWFile2::hc(double t)
{
	if((t>=t0)&&(t<tend))
		return(hcs->gData(t, InterpType, InterpVal));
	else
		return(0.0);
}



void GWFile2::DispTempVal(double t, ostream * OutDisp)
{
	
}


// end of LISACODE-GWFile2.cpp
