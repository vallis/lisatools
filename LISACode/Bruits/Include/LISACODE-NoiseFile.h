// $Id: LISACODE-NoiseFile.h,v 1.6 2007/04/05 09:31:44 ruiloba Exp $
/*
 *  LISACODE-NoiseFile.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  DERIV CLASS : Noise
 * ----------
 * Creation d'un bruit blanc de sigma donne. Ce sigma est obtenu a partir de la
 * Densite Spectrale de Puissance par Sigma = PSD / sqrt(tStep)
 *
 *
 *  Created on 22/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 22/10/05 by Antoine PETITEAU (APC)
 *
 */

#ifndef __NOISEFILE_H
#define __NOISEFILE_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include "randlib.h"
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Noise.h"
using namespace std;
/*!\ingroup noise*/
/*! \todo Correct class description in french*/
/*! 
* \brief Noise derived  class to treat files with noise data.
* 
*/
class NoiseFile : public Noise
{
protected:
    /*! \brief Name of the file where the noise is read.*/
    char * FileName; // Name of file where the noise is readed
    /*! \todo Are StoredData and NbDataStored necessary. Are they different to #NbData and #NoiseData?*/
    /*! \brief List of noise date read in the file.*/
    double * StoredData; // List of noise readed in file.   
    /*! \brief Number of data stored in the noise data list #StoredData.*/
    int NbDataStored;   // Number of noise data stored in StoredData.
    /*! \brief Multtplication factor on data.*/ 
    double FactMult; 
	/*! \brief Index of last bin read in #StoredData.*/ 
    int ReadBin; // Index of last bin readed in StoredData.
	
	
public:
		/* Constructor */
   		/*Doc in .cpp*/
		NoiseFile();
		NoiseFile(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, char * FileName_n);
		NoiseFile(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, char * FileName_n, double FactMult_n);
	
    /* Access methods */
   /*Doc in .cpp*/
    char * getFileName();
    void setFileName(char * FileName_n);
    int getNbDataStored();
	
	
    /*  Others methods */
   /*Doc in .cpp*/
    void loadNoise(); // Load noise for initialization
    void generNoise(int StartBin); // Generation of one noise for one step.
	
};

#endif // __NOISEFILE_H

// end of LISACODE-NoiseFile.h

