// $Id: LISACODE-Noise.h,v 1.6 2007/03/29 15:16:05 lalanne Exp $
/*
 *  LISACODE-Noise.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  BASIC CLASS
 * ----------
 * Cette classe (module) stocke et renvoie avec un eventuel retard, un type de
 * donnees correspondant a un bruit. Le stockage s'effectue selon le pas de temps "physique"
 *
 *
 *  Created on 13/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 05/07/05 by Antoine PETITEAU (APC)
 *
 */

#ifndef __NOISE_H
#define __NOISE_H


#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cstring>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
using namespace std;
/*!\defgroup noise Noise (directory Bruits)
*\{
*/

/*! 
* \brief Noise base class.
*
* It modelises and stores in a memory object any noise (white noise, filtered noise, 
* noise read from a file, etc). Noise samples could be eventually delayed (tDurAdd).
* The storage is done using a given a physical time step (tStep).
*/


class Noise 
{
protected:
	/*! \brief Time step in seconds between two saved data. It is used to simulate the continuous signal.*/
	double tStep; // Step in time in seconds between two stored data (simulate the continuous signal).
	/*! \brief Noise computation time step (for each measurement).*/
	double tDurAdd; // Duration for an addition of noise.
	/*! \brief Time of the first data in data vector.*/
	double tFirst; // Time for the first data in data list.
	/*! \brief Time of the last data in data vector.*/
	double tLast; // Time for the last data in data list.
	/*! \brief Number of bins (for each measurement).*/
	int NbBinAdd; // Number of bin add for each addition of noise.
	/*! \brief Nominal number of data in the noise data vector #NoiseData.*/
	int NbData;// Nominal data's number in list of noise data
	/*! \brief Vector of noise data.*/
	vector<double> NoiseData; // List of noise data.
	/*! \brief String to describe the noise type.*/
	char NoiseType [30]; 	// String to describe the noise type
	
public:
	/* Constructor */
	/*Doc in .cpp*/

	Noise();    
	Noise(double tStep_n, double tDurAdd_n); 
        Noise(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n);
	
    virtual ~Noise();
	
    /* Access methods */
    /*Doc in .cpp*/
    bool TestType(char * SubmitType) ; 

    /*! \brief It returns the time step between two saved data, that is the value of #tStep attribute. */
    double gettStep() const {return(tStep);};
    void settStep(double tStep_n);

    /*! \brief It returns the duration for a noise addition, that is the value of #tDurAdd attribute. */
    double gettDurAdd() const {return(tDurAdd);};
    void settDurAdd(double tDurAdd_n);

    
    /*! \brief It returns the delay of the first data in data vector, that is the value of #tFirst attribute. */
    double gettFirst() const {return(tFirst);};
    void settFirst(double tFirst_n);
    
    /*! \brief It returns the delay of the last data in data vector, that is the value of #tLast attribute. */
    double gettLast() const {return(tLast);};
    void settLast(double tLast_n);
    /*! \brief It returns the number of bins added, that is the value of #NbBinAdd attribute. */
    int getNbBinAdd() const { return(NbBinAdd);};
	
	
    /*  Others methods */
    /*Doc in .cpp*/

        virtual void loadNoise();   // Load noise for initialization    
       virtual void generNoise(int StartBin);  // Generation of noise.  
      void addNoise();   // Add one noise to the noise's list for a one duration.  
     double getNoise(double tDelay) const; 
     double getNoise(double tDelay , int order ) const; // Return the noise's value for the specified delay (delay < 0 for the past).						  
  //int unusable(double t) const; // Return 0 if the stored data can be usued.  
	
};
/*!\}*/

#endif // __NOISE_H

// end of LISACODE-Noise.h

