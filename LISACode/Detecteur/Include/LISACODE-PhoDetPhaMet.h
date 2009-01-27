// $Id: LISACODE-PhoDetPhaMet.h,v 1.7 2007/04/05 09:32:21 ruiloba Exp $
/*
 *  LISACODE-PhoDetPhaMet.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description : BASIC CLASS
 * ----------
 * Cette classe (module) simule le role d'une photodiode associee au phasemetre. 
 * Elle recoit les bruits tous exprimes en terme d'une difference de phase et les combine (sommation
 * ou soustration avec un retard ou non) en y integrant egalement la difference de phase du a l'onde
 * gravitationnelle. Les bruits arrivent echantillonnes selon un pas de temps physique. Le phasemetre
 * ressort une information qui correspond a la difference de phase entre les deux faisceaux arrivants
 * sur la photodiode. Cette information qui est la mesure est echantillonnee selon le pas de temps de
 * la mesure. Cet ensemble dispose d'un filtre passe-bas qui elimine les frequences superieures a la
 * moitie de la frequence de mesure. Le pas de temps du simulateur etant le pas de mesure, a chaque
 * demande de reception du signal le phasemetre stocke une mesure en memoire. 
 *
 *
 *  Created on 18/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 06/06/06 by Antoine PETITEAU (APC)
 *
 */


#ifndef __PHODETPHAMET_H
#define __PHODETPHAMET_H


#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-Geometry.h"
#include "LISACODE-Background.h"
#include "LISACODE-Noise.h"
#include "LISACODE-Filter.h"
#include "LISACODE-USOClock.h"
#include "LISACODE-TrFctGW.h"
#include "LISACODE-Memory.h"
using namespace std;
/*!\ingroup detect */
/*\brief Noise type.*/

enum NOISEORIG{ LA,	 /*!< Laser Noise*/ 
				OB,  /*!< Optical Bench Noise*/
				IM,  /*!< Inertial Mass Noise*/
				OP /*!< Optical Path Noise = Shot Noise + Others Optical Path Noises*/
};
/*!\ingroup detect */
/*!\brief Photodetector-phasemeter interferences type.*/
enum PDPMINTERF{ S, /*!< interferences between external and internal beams*/  			TAU /*!< interferences between the two internal beams*/
}; // Photodetector-phasemeters which make interferences between the two internal beam




/*!\ingroup detect */
/*!\brief Phasemeter photodiode class.
*
* All noises are phase differences. \n
* Noises are combined (addition, substraction, delay). \n
* Gravitational Wave contribution is added. \n
* Noises sampling is equal to physical time step. \n
* Phasemeter output is phase difference between 2 beams received by photodiode;
* its sampling is equal to measurement time step. \n
* Low pass filter keeps only frequencies lower than half measurement frequency. \n
* Simulator time step is equal to measurement step,
* so that 1 phasemeter signal data is stored in memory when 1 signal data is received.
*
*/

class PhoDetPhaMet 
{
protected:
  /*!\brief Type of interferences made by the photodetector-phasemeter */
	PDPMINTERF InterfType; // 
  /*!\brief  Direction flag :  0 if the optical bench is in the direct direction, else 1 */
	int IndirectDir; // Value is 0 if the optical bench is in the direct direction, 1 if not
  /*!\brief  Spacecraft index corresponding to the photodetector-phasemeter */
	int iSC; // Index of spacecraft where is the photodetector-phasemeter
  /*!\brief  Pointer to LISA geometry */
 	Geometry * SCPos; // Pointer to LISA's geometry
  /*!\brief #Noise pointers vector. */
	vector<Noise *> * NPs; // Pointer to vector of noise's pointers
  /*!\brief Pointers to spacecrafts USO clocks. */
	USOClock * USO; // Pointer to USO of the spacecraft
  /*!\brief Pointer to the storage memory of the photodetector-phasemeters signal. */
	Memory * RecordData; // Memory where the photodetector-phasemeters's signals are stored
  /*!\brief Pointer to the transfer function. */
	TrFctGW * sGW; // Pointer to the transfer function
  /*!\brief #Background pointer : confusion whites dwarfs background. */
	Background * GWB; // Pointer to the confusion whites dwarfs background
  /*!\brief Physical simulation time step. */
	double tStepPhy; // Time step for physical simulation
  /*!\brief Measurement time step */
    double tStepMes; // Time step of measurement
  /*!\brief Physical data vector.*/
    vector<double> InterfPhyData; // List of physics datas 
  /*!\brief Filtered physical data vector. */
	vector<double> FilterPhyData; // List of filtered physics datas
  /*!\brief Number of data stored (in InterfPhyData and FilterPhyData) */
	int NbDataStored; // Number of data stored (in InterfPhyData and FilterPhyData)
  /*!\brief Number of data added (in InterfPhyData and FilterPhyData) for each measurement */
	int NbDataAdd; // Number of data added (in InterfPhyData and FilterPhyData) for each mesurement
  /*!\brief #Filter pointer to a low pass filter. */
	Filter *PBFilter; // Low_pass filter
  /*!\brief Filter flag : If true, the filter is applied */
	bool FilterON; // If true, the filter is apply
  /*!\brief Filter parameters  : 
   * attenuation [dB], 
   * oscillations in bandwidth [dB], 
   * low transition frequency divided by measurement frequency, 
   * high transition frequency divided by measurement frequency. */
	vector<double> FilterParam; // Parameters of filter : attenuation [dB], oscillations in bandwidth [dB], low transition frequency in factor of frequency of measurment ,  high transition frequency in factor of frequency of measurment
  /*!\brief Noise flag : if true, there is no noise */
	bool NoNoise; // If true, there are no noise
	
	
public:
		/* Constructor */
		PhoDetPhaMet();
	PhoDetPhaMet( PDPMINTERF InterfType_n,
				  int IndirectDir_n,
				  int iSC_n,
				  Geometry * SCPos_n,
				  vector<Noise *> * NPs_n,
				  USOClock * USO_n,
				  Memory * RecordData_n,
				  double tStepPhy_n,
				  double tStepMes_n);
	PhoDetPhaMet(	PDPMINTERF InterfType_n,
					int IndirectDir_n,
					int iSC_n,
					Geometry * SCPos_n,
					vector<Noise *> * NPs_n,
					USOClock * USO_n,
					Memory * RecordData_n,
					TrFctGW * sGW_n,
					Background * GWB_n,
					double tStepPhy_n,
					double tStepMes_n);
	PhoDetPhaMet(	PDPMINTERF InterfType_n,
					int IndirectDir_n,
					int iSC_n,
					Geometry * SCPos_n,
					vector<Noise *> * NPs_n,
					USOClock * USO_n,
					Memory * RecordData_n,
					TrFctGW * sGW_n,
					Background * GWB_n,
					double tStepPhy_n,
					double tStepMes_n,
					bool FilterON_n,
					vector<double> FilterParam_n);
	~PhoDetPhaMet();
	
	
	/* Access methods */
	void init(	PDPMINTERF InterfType_n,
                int IndirectDir_n,
				int iSC_n,
				Geometry * SCPos_n,
				vector<Noise *> * NPs_n,
				USOClock * USO_n,
				Memory * RecordData_n,
                TrFctGW * sGW_n,
				Background * GWB_n,
                double tStepPhy_n,
                double tStepMes_n,
				bool FilterON_n,
				vector<double> FilterParam_n); //Initialisation
  /*!\brief Returns #IndirectDir attribute  */
	double getIndirectDir() const {return(IndirectDir);};
  /*!\brief Returns #iSC attribute */
	double getiSC() const {return(iSC);};
	void DisplayStoredData();
	double gettStab();
	
	/*  Others methods */
	double gN(NOISEORIG OrigN, int iSC, int IndirectDir, double tDelay);
	// Return value of specified noise delayed of "delay" (iSC={1,2,3}, IndirectDir={0,1})
	double gGWB(int iSC, int IndirectDir, double t);
	// Return value of Gravitationnal Wave Background (iSC={1,2,3}, IndirectDir={0,1})
	void ReceiveSignal(double t); // Compute the received signal on photodetector-phasemeters
	void IntegrateSignal(double t); // Store the result in memory: one measurement
	bool getNoNoise(); // Return true if there are no noises
	
};

#endif // __PHODETPHAMET_H

// end of PhoDetPhaMet.h

