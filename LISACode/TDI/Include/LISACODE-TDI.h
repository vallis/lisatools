// $Id: LISACODE-TDI.h,v 1.6 2007/04/05 09:32:44 ruiloba Exp $
/*
 *  TDI.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) applique les combinaisons TDI sur les donnees stockes (par Memory)
 * des photodiodes-phasemeter. En sortie, elle donne la valeur des generateurs.
 * Les combinaisons de chaque groupe de generateurs sont "contenus" dans les classes derivees
 * de la classe TDI. 
 *
 *  Created on 30/06/05 by Antoine PETITEAU (APC)
 *  Last modification on 23/07/08 by Antoine PETITEAU (APC)
 *
 */

/*!\defgroup tdiHand TDI handling (directory TDI)
 */

/*!\ingroup tdiHand 
 *\defgroup tdi TDI
 *\{
 */
#ifndef __TDI_H
#define __TDI_H


#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-Tools.h"
#include "LISACODE-Memory.h"
#include "LISACODE-TDI_InterData.h"
#include "LISACODE-TDITools.h"
using namespace std;


/*!\brief Time Delay Interferometry combinaison class.
 */

class  TDI
	{
	protected:
		Tools * MT;
		/*!\brief Pointer to the list of delay's lengths.*/
		Memory * TDelay;
		/*!\brief Memory where the signals Eta are stored.*/
		TDI_InterData * Eta;
		/*!\brief Memory where the 6 signals of spacecraft are stored .*/
		Memory * SCSig;
		/*!\brief File where TDI result are recorded.*/
		ofstream * OutFile; 
		/*!\brief Encoding of file where TDI result are recorded.*/	
		int OutFileEncoding;
		/*!\brief Index of the serie in memory RecordMem.*/
		int iSerie;
		/*!\brief List of sign of the pack (1 for + and -1 for -).*/
		vector<int> Sign;
		/*!\brief List of index of signal Eta where the pack work (value=[1,6]).*/
		vector<int> IndexEta;
		/*!\brief List of list of delay's index for each pack (value=[1,6]).*/
		vector< vector<int> > IndexDelay;
		//vector< TDI_Unit * > LastUnit; // List of the last unit TDI for each pack.
		//INTERP InterpType; // Type of interpolation used to get data.
		//double InterpUtilValue; // Value used for interpolation.
		/*!\brief List of factor of the pack (double).*/
		vector<double> Fact;
		/*!\brief TDI access tools. */
		TDITools * TDIQuickMod;
		/*!\brief Temporary data (unused). */
		int tmpCountInterDelay;
		/*!\brief Temporary data (unused). */
		int tmpCountInterEta;
		
	public:
		
		/* Constructor */
		TDI();
		
		TDI(Tools * MT_n,
			Memory * TDelay_n,
			TDI_InterData * Eta_n,
			ofstream * OutFile_n,
			int OutFileEncoding_n,
			int iSerie_n);
		
		TDI(Tools * MT_n,
			Memory * TDelay_n,
			TDI_InterData * Eta_n,
			ofstream * OutFile_n,
			int OutFileEncoding_n,
			int iSerie_n,
			vector<int> Sign_n,
			vector<int> IndexEta_n,
			vector< vector <int> > IndexDelay_n,
			TDITools * TDIQuickMod_n//,
			//INTERP InterpType_n = LAG,
			//double InterpUtilValue_n = 6
			); //Constructeur avec implementation de packs decomposes
		
		TDI(Tools * MT_n,
			Memory * TDelay_n,
			TDI_InterData * Eta_n,
			ofstream * OutFile_n,
			int OutFileEncoding_n,
			int iSerie_n,
			vector<int> SignEtaDelays,
			TDITools * TDIQuickMod_n
			//,
			//INTERP InterpType_n = LAG,
			//double InterpUtilValue_n = 6
			);
		
		TDI(Tools * MT_n,
			Memory * TDelay_n,
			TDI_InterData * Eta_n,
			ofstream * OutFile_n,
			int OutFileEncoding_n,
			int iSerie_n,
			vector<int> SignEtaDelays,
			vector<double> Fact_n,
			TDITools * TDIQuickMod_n
			//,
			//INTERP InterpType_n = LAG,
			//double InterpUtilValue_n = 6
			);
		
		/*! \brief Constructor for directly use spacecraft signal (no use of Eta) and no records in out file.*/
		TDI(Tools * MT_n,
			Memory * TDelay_n,
			Memory * SCSig_n,
			vector<int> SignEtaDelays,
			vector<double> Fact_n,
			TDITools * TDIQuickMod_n
			);
		
		virtual ~TDI();
		
		
		// ** Access methods **
		/*! \brief Returns tmpCountInterDelay attribute */
		int getCountInterDelay(){return(tmpCountInterDelay);};
		/*! \brief Returns tmpCountInterEta attribute */
		int getCountInterEta(){return(tmpCountInterEta);};
		
		// **  Others methods **
		/*! \brief Read list of packs that are in the following form : -1231 for - D1 D2 D3 Eta1 */
		void ReadSignEtaDelays (vector<int> SignEtaDelays); 
		//void CreateAllPack(); // Create all packs
		/*! \brief Compute the result of generator in case where Eta is used. */
		double Compute(double tComputeDelay);
		/*! \brief Compute the result of generator in case where spacecraft signal are used. */
		double ComputeNoEta(double tComputeDelay, INTERP InterpType, int InterpUtilValue);
		//void RecordResult(double tComputeDelay); // Record the result of generator
		/*! \brief Record the result of generator and return the result */
		double RecordAndReturnResult(double tComputeDelay);
		/*! \brief Maximum number of delays */
		int NbDelayMax();
		
		
	};

/*!\}*/
#endif // __TDI_H

// end of TDI.h

