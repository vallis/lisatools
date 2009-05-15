// $Id: LISACODE-GWFile.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWFile.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#include "LISACODE-GWFile.h"

// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * Inherited attributes from GW are set to default values. 
 * \arg	#LastUsedBin = 0
 */
GWFile::GWFile() : 
GW()
{
  LastUsedBin = 0;
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs as arguments
 * \arg	#LastUsedBin = 0
 * #TimeList and #hList attributes are filled using #ReadASCIIFile method with \a FileName input as argument
 */
GWFile::GWFile(double Beta_n, double Lambda_n, double AnglPol_n, char * FileName_n) : 
GW(Beta_n, Lambda_n, AnglPol_n)
{
	strcpy(FileName,FileName_n);
	FileEncoding = 0;
	TimeOffset = -1.0;
	TimeStep = -1.0;
	Length = 0;
	Records = 0;
	init();
}


/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs as arguments
 * \arg	#LastUsedBin = 0
 * #TimeList and #hList attributes are filled using #ReadASCIIFile or #ReadBinaryFile method with \a FileName input as argument
 * \a FileEncoding get type of file (0: ASCII , 1: Binary)
 */
GWFile::GWFile(double Beta_n, 
	       double Lambda_n, 
	       double AnglPol_n, 
	       char * FileName_n, 
	       int FileEncoding_n, 
	       double TimeOffset_n, 
	       double TimeStep_n, 
	       int Length_n, 
	       int Records_n) : 
GW(Beta_n, Lambda_n, AnglPol_n)
{
	strcpy(FileName,FileName_n);
	FileEncoding = FileEncoding_n;
	TimeOffset = TimeOffset_n;
	TimeStep = TimeStep_n;
	Length = Length_n;
	Records = Records_n;
	init();
}


/*! \brief Destructor
 */
GWFile::~GWFile()
{
  free(TimeList);
  free(hList);
}

// **********************
// **  Access methods  **
// **********************

void GWFile::setParam(int iP, double Param_n){
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

double GWFile::getParam(int iP){
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


void GWFile::setFileName(char * FileName_n)
{
	strcpy(FileName,FileName_n);
}


// **********************
// **  Others methods  **
// **********************

void GWFile::init()
{
	LastUsedBin = 0;	
	if(FileEncoding){
		// Read binary file
		ReadBinaryFile(FileName, TimeOffset, TimeStep, Length, Records);
	}else{
		// Read ASCII
		ReadASCIIFile(FileName, TimeOffset, TimeStep);
	}	
}


/*! The number of columns of the file must be 2 (h+,hx) or 3 (t,h+,hx). If it is 2, time informations 
 * are specifie in arguments \a TimeOffset and \a TimeStep.
 * If \a TimeStep is -1.0, it is means that the time informations are not specifed.
 * Header (lines begining with "#" charected) are ignored.
 * For each line, time and 2 components are read and pushed back into #TimeList and #hList attributes.
 */
void GWFile::ReadASCIIFile(char * FileName, double TimeOffset, double TimeStep)
{
	ifstream ReadFile;
	string junk;
	
	/****  Read the number of columns ****/
	ReadFile.open(FileName);
	if (!ReadFile)
	  throw invalid_argument("GWFile::ReadASCIIFile : The gravitational file can't be open !");
	// Read header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
	  ReadFile.ignore(1024,'\n'); 
	};
	// Read the first line and count the number of records
	Records = 0;
	while((ReadFile.peek() != '\n')&&(!ReadFile.eof())){
	  ReadFile >> junk;
	  Records++;
	}
	ReadFile.close();
	ReadFile.clear();
	// Control on the number of columns
	cout << " (" << Records; fflush(stdout);
	if((Records<2)||(Records>3))
	  throw invalid_argument("GWFile::ReadASCIIFile : The number of columns are bad (only 2 or 3) !");
	if((Records==2)&&(TimeStep<=0.0))
	  throw invalid_argument("GWFile::ReadASCIIFile : The time informations are not specified !");

	/****  Read the number of lines ****/
	ReadFile.open(FileName);
	if (!ReadFile)
	  throw invalid_argument("GWFile::ReadASCIIFile : The gravitational file can't be open !");
	// Read header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
	  ReadFile.ignore(1024,'\n'); 
	};
	// Count the number of lines
	NbDat = 0;
	while(!ReadFile.eof()){
	  ReadFile.ignore(1024,'\n');
	  if(!ReadFile.eof()){
	    NbDat++;
	  }
	}
	ReadFile.close();
	ReadFile.clear();
	cout << "x" << NbDat << ") ..."; fflush(stdout);	      
	
	/****  Allocate memory ****/
	TimeList = (double*)malloc(NbDat*sizeof(double));
	hList = (Couple*)malloc(NbDat*sizeof(Couple));

	/****  Read data ****/
	ReadFile.open(FileName);
	if (!ReadFile)
	  throw invalid_argument("GWFile::ReadASCIIFile : The gravitational file can't be open !");
	// Read header
	while((ReadFile.peek() == '#')&&(!ReadFile.eof())){
	  ReadFile.ignore(1024,'\n'); 
	};
	if(Records == 2){
	  for(int iL=0; iL<NbDat; iL++){
	    TimeList[iL] = TimeOffset + iL*TimeStep;
	    ReadFile >> hList[iL].x;
	    ReadFile >> hList[iL].y;
	  }
	}else{
	  for(int iL=0; iL<NbDat; iL++){
	    ReadFile >> TimeList[iL];
	    ReadFile >> hList[iL].x;
	    ReadFile >> hList[iL].y;
	  }
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
void GWFile::ReadBinaryFile(char * FileName, double TimeOffset, double TimeStep, int Length, int Records)
{
  FILE * ReadBinFile;
  double * VecData;

  NbDat = Length;

	cout << " (" << Records << "x" << NbDat << ")"; fflush(stdout);
  
  /****  Allocate memory ****/
  TimeList = (double*)malloc(NbDat*sizeof(double));
  hList = (Couple*)malloc(NbDat*sizeof(Couple));
  
  /**** Read Data ****/
  ReadBinFile = fopen(FileName,"rb");
  if(ReadBinFile == NULL){
    throw invalid_argument("GWFile::ReadBinaryFile : Problem in opening the file ");
  }
  
  // Allocate memory
  VecData = (double*)malloc(NbDat*Records*sizeof(double));
  
  // Read binary data
  fread(VecData,sizeof(double),NbDat*Records,ReadBinFile);
  
  // Sort data
  if(Records == 2){
    // No time data
    for(int iL=0; iL < NbDat; iL++){
      TimeList[iL] = TimeOffset + iL*TimeStep;
      hList[iL].x =  VecData[iL*2];
      hList[iL].y =  VecData[iL*2+1];
      //cout << TimeList[iL] << " " <<  hList[iL].x << " " <<  hList[iL].y <<  endl;
    }
  }else{
    // Time data included
    for(int iL=0; iL < NbDat; iL++){
      TimeList[iL] = VecData[iL*Records];
      hList[iL].x =  VecData[iL*Records+1];
      hList[iL].y =  VecData[iL*Records+2];
    }
  }
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
	if((t<TimeList[0])||(t>TimeList[NbDat-2])){
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
		if(kmax > NbDat-1)
			kmax = NbDat-1;
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



void GWFile::DispTempVal(double t, ostream * OutDisp)
{
	
}


// end of LISACODE-GWFile.cpp
