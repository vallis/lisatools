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
    StoredData.resize(0);
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
    StoredData.resize(0);
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
    // Read the header of file
    ifstream ReadFile;
    //Open file
    ReadFile.open(FileName);
	if (!ReadFile){
		throw invalid_argument("NoiseFile::loadNoise : Can not open the noise file ! ");
	}
    string junk;
    //Initialise values to read from the file
    double NewtStep(0.0), tMax(0.0);
    // Junk the five first words 
    ReadFile >> junk >> junk >> junk >> junk >> junk; // Read( # Time  : Step = )
    // Read new time step from the file 
    ReadFile >> NewtStep;
    //Verify if read step and step attribute of the class are the same and
    //show a message if they are different
    if(NewtStep-tStep > PRECISION)
        throw invalid_argument("NoiseFile::loadNoise : The time step in readed noise file is different than physical time step !");

    // Junk the three next words in the file
    ReadFile >> junk >> junk >> junk; // Read( , Duration = )
    // Read new time step from the file 
    ReadFile >> tMax;
    // Junk the next word in the file
    ReadFile >> junk; // Read( ; )
    // Junk the words in the file to the next ";"
    do{
        ReadFile >> junk;
    }while(junk != ";");
	
    // Read and store the noise which are in file
    // Read NbDataStored (attribute of the class) data
    // if the end of file is not reached before
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
    {
    NbDataStored = (int)(tMax/tStep+100*PRECISION);
    double Data_tmp(0.0);
    do{
        ReadFile >> Data_tmp;
        StoredData.push_back(Data_tmp);
    }while(((int)StoredData.size()<NbDataStored)&&(!ReadFile.eof()));
    }
    
    //Close file
    ReadFile.close();
    //Update varible NbDataStored if the number of data read is smaller than expected
    if((int)StoredData.size()<NbDataStored){
        cout << " WARNING : NoiseFile::loadNoise : There are not enough data in readed noise file !" << endl;
        NbDataStored = StoredData.size();
    }
	
    // Choice of the first bin which is readed
    ReadBin = 0;
    //ReadBin = (int)(((double)rand()*NbDataStored)/RAND_MAX);
    
    // Generation of all data noise
    //cout << "  - NbData       = " << NbData << endl;
    //cout << "  - NbDataStored = " << NbDataStored << endl;

    NoiseData.resize(NbData);
	/*for (int i=0; i<NbData; i++)
		cout << "StoredData[" << i << "] = " << StoredData[i] << endl; 
	*/
	for(int i=NbData-1; i<=0; i++){
        //cout << ReadBin << endl;
        ReadBin++;
        if(ReadBin>=NbDataStored){
            //cout << " Bin a 0" << endl;
            ReadBin = 0;
        }
        NoiseData[i] = StoredData[ReadBin];
    }
    //cout << "  - NbData = " << NbData << endl;
	//cout << endl << endl;
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
