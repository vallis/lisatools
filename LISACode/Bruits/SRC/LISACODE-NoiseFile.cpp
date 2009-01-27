// $Id: LISACODE-NoiseFile.cpp,v 1.5 2007/03/29 15:16:50 lalanne Exp $
/*
 *  LISACODE-NoiseFile.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 22/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 22/10/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-NoiseFile.h"

/* Constructor */
/*! \brief Base constructor.
 *
 * It sets default values for class attributes and reads noise data from the file.
 * ?Update sentence in relation to loadNoise understanding : Data read are stored 
 * in #StoredData and write in Noise::NoiseData.
 * \arg tStep = 0.01
 * \arg tDurAdd = 1.0
 * \arg tFirst = 5.0
 * \arg tLast = -20.0
 * \arg NoiseType = File 
 * \arg FileName = DefaultNoise
*/
NoiseFile::NoiseFile()
{
    tStep = 0.01;
    tDurAdd = 1.0;
    tFirst = 5.0;
    tLast = -20.0;
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    FileName = "DefaultNoise";
    /* Set size of vector to store read data to 0. 
       It deletes all eventual elements*/
    loadNoise();
	strcpy(NoiseType,"File");
}
/* Constructor */
/*! \brief Constructor.
 *
 * It sets values for class attributes and reads et stores data such it is done by 
 * the default constructor (NoiseFile#NoiseFile()).\n
 *  #NoiseType attribute is set to \p File.  
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 * \param FileName_n Value of #FileName. 
 */
NoiseFile::NoiseFile(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, char * FileName_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    FileName = FileName_n;
	FactMult = 1.0;
    loadNoise();
    //cout << "  - NoiseData size = " << NoiseData.size() << endl;
    //for(int i=0; i<10; i++)
    //    cout << "    NoiseData[i] = " << NoiseData[i] << endl;
	strcpy(NoiseType,"File");
}

NoiseFile::NoiseFile(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, char * FileName_n, double FactMult_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    FileName = FileName_n;
	FactMult = FactMult_n;
    loadNoise();
    //cout << "  - NoiseData size = " << NoiseData.size() << endl;
    //for(int i=0; i<10; i++)
    //    cout << "    NoiseData[i] = " << NoiseData[i] << endl;
	strcpy(NoiseType,"File");
}

/* Access methods */
/*! \brief It returns the name of the file where the noise is read, #FileName.
*/ 
char * NoiseFile::getFileName()
{
    return(FileName);
}
/*! \brief It sets the name of the file where the noise is read, #FileName.
*/
void NoiseFile::setFileName(char * FileName_n)
{
    FileName = FileName_n;
}
/*! \brief It returns #NbDataStored, the number of data stored in the noise data list #StoredData.
*/ 
int NoiseFile::getNbDataStored()
{
    return(NbDataStored);
}


/* Others methods */
/*! \brief Initializes instance using data read from #FileName file.
 *
 * Noise computation time step is checked : it must be equal to physical time step. \n
 * Noises read from #FileName file are stored.
 * Read data size is checked : it must be equal to #NbDataStored. \n
 */
void NoiseFile::loadNoise()
{
    ifstream ReadFile;
	char junk[256];
	double t0(0.0), t1(0.0), tMax(0.0);
	double ** ReadData;
	int NbColumns, NbReadData;
	int ReadCol(2); // Index of read column
    
	cout << "Read noise in file " << FileName << " ... " << endl; fflush(stdout);
	
	// **** Count the number of columns
    ReadFile.open(FileName);
	if (!ReadFile){
		throw invalid_argument("NoiseFile::loadNoise : Can not open the noise file ! ");
	}
	while(ReadFile.peek() == '#'){
		ReadFile.ignore(16384,'\n');
	};
	NbColumns = 0;
	while((ReadFile.peek() != '\n')&&(!ReadFile.eof())){
		ReadFile >> junk;
		NbColumns++;
	}
	ReadFile.close();
	ReadFile.clear();
	
	// **** Count the number of rows
	ReadFile.open(FileName);
	while(ReadFile.peek() == '#'){
		ReadFile.ignore(16384,'\n');
	};
	NbReadData = 0;
	while(!ReadFile.eof()){
		//Reading time
		switch (NbReadData){
			case 0 :
				ReadFile >> t0;
				break;
			case 1 :
				ReadFile >> t1;
				break;
			default :
				ReadFile >> tMax;
		}
		// Reading end of row
		ReadFile.ignore(16384,'\n');
		if(!ReadFile.eof()){	
			NbReadData++;
		}
	};
	ReadFile.close();
	ReadFile.clear();
	
	cout << "   --> Number of readed data = " << NbReadData << endl;
	
	ReadData = (double**)malloc(2*sizeof(double*));
	ReadData[0] = (double*)malloc(NbReadData*sizeof(double)); // Alloc for time
	ReadData[1] = (double*)malloc(NbReadData*sizeof(double)); // Alloc for noise data 
	
	// **** Read and store data
	ReadFile.open(FileName);
	if (!ReadFile){
		throw invalid_argument("NoiseFile::loadNoise : Can not open the noise file ! ");
	}
	while(ReadFile.peek() == '#'){
		ReadFile.ignore(16384,'\n');
	};
	for(int nl=0;nl<NbReadData;nl++){
		for(int col=1; col<=NbColumns; col++){
			if(col ==  1){   
				ReadFile >> ReadData[0][nl]; // Read time
			}else{
				if(col == ReadCol ){  
					ReadFile >> ReadData[1][nl]; // Read noise data
				}else{
					ReadFile >> junk;
				}
			}
		}
	}
	ReadFile.close();
	ReadFile.clear();
	
	// **** Start time to 0
	for(int nl=0;nl<NbReadData;nl++){
		ReadData[0][nl] -= t0;
		//cout << ReadData[0][nl] << " " << ReadData[1][nl] << endl;
	}
	tMax -= t0;
	
	// **** Resample read data in noise data
	int order(7);
	int LastUsedBin(0);
	double tcur(0.0);
	
	NbDataStored = (int)(tMax/tStep+100*PRECISION);
	StoredData = (double*)malloc(NbDataStored*sizeof(double));
	
	cout << "   --> Number of stored data = " << NbDataStored << endl;
	
	
	for(int id=0; id < NbDataStored; id++){
		tcur = id*tStep;
		StoredData[id] = 0.0;
		//cout << id << " = " << tcur; 
		// Find the good bin
		if((tcur<ReadData[0][0])||(tcur>ReadData[0][NbReadData-1])){
			 throw invalid_argument("NoiseFile::loadNoise : The value for this time cannot be evaluated !");
		}else {
			while(tcur<ReadData[0][LastUsedBin]){
				LastUsedBin--;
			};
			while(tcur>=ReadData[0][LastUsedBin+1]){
				LastUsedBin++;
			};
			
			// Make Lagrange interpolation
			double Pk(0.0);
			int ordermin((int)(floor(double(order+1)/2.0)));
			int kmin(LastUsedBin-ordermin+1), kmax(LastUsedBin+(order+1-ordermin));
			if(kmin < 0)
				kmin = 0;
			if(kmax > NbReadData-1)
				kmax = NbReadData-1;
			//cout << "min=" << kmin << " max=" << kmax << endl;
			for(int k=kmin; k<=kmax; k++){
				Pk = 1.0;
				for(int j=kmin; j<=kmax; j++){
					if(j!=k){
						//cout << " - ReadData[0][" << j << "] = " << ReadData[0][j] << "  ReadData[0][" << k << "] = " <<  ReadData[0][k];
						//cout << "   --> (ReadData[0][k]-ReadData[0][j]) = " << (ReadData[0][k]-ReadData[0][j]) << endl;
						Pk *= (tcur-ReadData[0][j])/(ReadData[0][k]-ReadData[0][j]);
					}
				}
				StoredData[id] += ReadData[1][k]*Pk;
				//cout << k << ":" << StoredData[id] << " , ";
			}	
			//cout << endl;
		}
	}
	
	ofstream ControlFile("NoiseControl.txt");
	ControlFile.precision(15);
	for(int id=0; id < NbDataStored; id++){
		StoredData[id] *= FactMult; 
		ControlFile << id*tStep << " " << StoredData[id] << endl;
	}
	ControlFile.close();
	
    // **** Choice of the first bin which is readed
    ReadBin = 0;
    ReadBin = (int)(((double)genunf(0.0, 1.0) * NbDataStored));
	cout << "   --> Start bin : " << ReadBin; fflush(stdout);
    
    // Generation of all data noise
    //cout << "  - NbData       = " << NbData << endl;
    //cout << "  - NbDataStored = " << NbDataStored << endl;

    NoiseData.resize(NbData);
	//for (int i=0; i<NbDataStored; i++)
	//	cout << "StoredData[" << i << "] = " << StoredData[i] << endl; 
	
	for(int i=NbData-1; i>=0; i--){
        ReadBin++;
        if(ReadBin>=NbDataStored){
            //cout << " Bin a 0" << endl;
            ReadBin = 0;
        }
        NoiseData[i] = StoredData[ReadBin];
		//cout << " + + NoiseData["  << i << "] = " << NoiseData[i] << " StoredData[" << ReadBin << "] = " << StoredData[ReadBin] << endl;
    }
	
	//for (int i=0; i<NbData; i++)
	//	cout << "NoiseData[" << i << "] = " << NoiseData[i] << endl; 
	
   // cout << "  - NbData = " << NbData << endl;
	cout << " --> OK" << endl;
	 
}

/*! \brief Noise generation (for one measurement), using Startbin input as beginning index.
 * 
 * #NoiseData StartBin first data are set.\n
 * \f[  \textrm{ for i=0,\dots,Startbin } NoiseData[StartBin-i] = \left\{ \begin{array}{ll}
 * StoredData[i] & \textrm{ if } i \le NbDataStored \\
 * StoredData[0] & \textrm{ else } 
 * \end{array} \right. \f]
 */
void NoiseFile::generNoise(int StartBin)
{
	for(int i=StartBin; i>=0; i--){
		//cout << "ReadBin = " << ReadBin;
		ReadBin++;
		if(ReadBin>=NbDataStored){
			//cout << " Bin a 0" << endl;
			ReadBin = 0;
		}
		NoiseData[i] = StoredData[ReadBin];
		//cout << "  NoiseData[" << i << "] = " << NoiseData[i] << endl;
	}
}


// end of LISACODE-NoiseFile.cpp
