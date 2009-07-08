// $Id: LISACODE-LISA.h,v 1.8 2007/04/05 09:32:21 ruiloba Exp $
/*
 *  LISACODE-LISA.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) regroupe tous les elements contenus dans les satellites de LISA et gere le
 * deroulement de la simulation.
 * Elle contient tous les bruits (classe NoiseLaser, NoiseOptBench, NoiseInertialMass), les retards 
 * (classe Delay), les positions des satellites (classe Geometry), les ensembles 
 * photodiodes-phasemeter (classe PhoDetPhaMet) et la fonction de transfert des ondes gravitationnelles
 * (classe TrFctGW).
 * Elle est relie aux memoires vers lesquelles les donnees sont envoyees.
 *
 *
 *  Created on 02/05/05 by Antoine PETITEAU (APC)
 *  Last modification on 06/06/06 by Antoine PETITEAU (APC)
 *
 */
/*!\defgroup detect Detector (directory Dectecteur)
*\{
*/
#ifndef __LISA_H
#define __LISA_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-NoiseWhite.h"
#include "LISACODE-NoiseFilter.h"
#include "LISACODE-NoiseFile.h"
#include "LISACODE-NoiseFShape.h"
#include "LISACODE-NoiseOof.h"
#include "LISACODE-NoiseTwoFilter.h"
#include "LISACODE-TrFctGW.h"
#include "LISACODE-Geometry.h"
#include "LISACODE-GeometryAnalytic.h"
#include "LISACODE-Background.h"
#include "LISACODE-USOClock.h"
#include "LISACODE-PhoDetPhaMet.h"
#include "LISACODE-Memory.h"
#include "LISACODE-ConfigSim.h"
using namespace std;

/*! \brief This class contains and manages all the elements necessary to LISA satellites simulation. 
*
* It contains the set of noises (laser noise, optical bench noise and inertial mass noise, 
* see #NoisePointers), the delays, the satelites position (#SCPos), the set of 
* photodiodes-phasemeters (#PhotoDetects) and the gravitational waves
* transfert fonstion (#sGW). It is related to memory object to which data are sent (#RecordPDPM).
*
*/
class LISA
{
protected:
    /*! \brief #Geometry pointer : spacecraft orbitography structure. */
    Geometry * SCPos;
    /*! \brief Vector of noise pointers.
     *
     * It contains the addresses of all LISA related noises in next order:
     * noises of lasers, noises of optical bench, noises of inertial mass and others noises.
     */
    vector<Noise *> NoisePointers;// This vector of noise's pointers contains the addresses of all the noises classified in the following order :
	// Noises of Laser, Noises of Optical Bench, Noises of Inertial Mass, Others Noises
    /*! \brief Vector of photodetector-phasemeters. */
    vector<PhoDetPhaMet> PhotoDetects; // Vector of photodetector-phasemeters 
    /*! \brief Vector of memory objects where the photodetector-phasemeters signals are stored.*/
    vector<Memory *> * RecordPDPM; // Memory where the photodetector-phasemeters's signals are stored   
    /*! \brief Transfer fonction to compute the LISA answer to gravitationnals waves.*/
    TrFctGW sGW; // Transfer fonction which compute the LISA's answer to gravitationnals waves
    /*! \brief #Background pointer : gravitational background noise (small mass binaries signal received by phasemeters). */
    Background * GWB; // Confusion whites dwarfs background
    /*! \brief Time step to simulate continuous physical process.*/	
    double tStepPhy;    // Step in time for simulate physics process (continuous) 
    /*! \brief Duration (time) of photodetector-phasemeters signals stored.*/
    double tMemRAM;// Time during which the photodetector-phasemeters's signals are store
    /*! \brief Time step for phasemeters measurement.*/ 
    double tStepMes; // Step in time for measurement the phasemeters (Numeric)
    /*! \brief Vector of LISA USO clocks.
     *
     * There is one USO clock by spacecraft.
     */ 
    vector<USOClock> USOs; // One USO clock for each spacecraft 
	
public:
		// Constructeur
                /*Doc in .cpp*/
		LISA();
        	LISA(	double tStepPhy_n,
			double tStepMes_n,
			double tMemNoiseFirst_n,
			double tMemNoiseLast_n,
			double tMemRAM_n,
			double PSDLaser,
			double PSDOptBench,
			double PSDInertialMass,
			vector<Memory *> * RecordPDPM_n,
			vector<GW *> * GW_n,
			Background * GWB_n
			);
	LISA(ConfigSim * ConfigLISA, vector<Memory *> * RecordPDPM_n);
	/*! \brief Destructor */
	~LISA();
	
	/* Access methods */
	/*Doc in .cpp*/
	double gDelayT(int i, int IndirectDir, double t); // Renvoie la valeur du retard i en secondes
	double gArmLength(int i, int IndirectDir, double t); // Renvoie la longueur d'un bras
	Vect gPosSC(int i, double t); // Renvoie la valeur de la position du satellite
	
	/*  Others methods */
	/*Doc in .cpp*/
	//int ChargeNoises(double t); //Load the noises for de first step in time
	void Stabilization(); // Run for make stabilization of phasemeters' filter without change LISA 
	void MakeOneStepOfTime(double t); //Make one step in time after loading of noises
	vector<double> PresentMeanNoise(double t); //Return the list of noises's value 
	
};
/*!\}*/
#endif // __LISA_H

// end of LISA.h
