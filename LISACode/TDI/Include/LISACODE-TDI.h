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
 *  Last modification on 30/09/05 by Antoine PETITEAU (APC)
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
#include "LISACODE-Memory.h"
#include "LISACODE-TDI_InterData.h"
#include "LISACODE-TDITools.h"
using namespace std;


 /*!\brief Time Delay Interferometry combinaison class.
 */

class  TDI
{
 protected:
  /*!\brief Pointer to the list of delay's lengths.*/
 	Memory * TDelay;
 /*!\brief Memory where the signals Eta are stored.*/
	TDI_InterData * Eta;
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
 /*!\brief TDI access tools. */
	TDITools * TDIQuickMod;
 /*!\brief Temporary data (unused). */
	int tmpCountInterDelay;
 /*!\brief Temporary data (unused). */
	int tmpCountInterEta;
 
 public:
  /* Constructor */
  TDI();
  
  TDI( Memory * TDelay_n,
	   TDI_InterData * Eta_n,
	   ofstream * OutFile_n,
	   int OutFileEncoding_n,
	   int iSerie_n);

  TDI( Memory * TDelay_n,
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
  
  TDI( Memory * TDelay_n,
	   TDI_InterData * Eta_n,
	   ofstream * OutFile_n,
	   int OutFileEncoding_n,
	   int iSerie_n,
	   vector<int> SignEtaDelays,
	   TDITools * TDIQuickMod_n//,
	   //INTERP InterpType_n = LAG,
	   //double InterpUtilValue_n = 6
	   );
  
  virtual ~TDI();
  
  
  /* Access methods */
/*! \brief Returns tmpCountInterDelay attribute */
  int getCountInterDelay(){return(tmpCountInterDelay);};
/*! \brief Returns tmpCountInterEta attribute */
  int getCountInterEta(){return(tmpCountInterEta);};
  
  /*  Others methods */
  void ReadSignEtaDelays (vector<int> SignEtaDelays); // Read list of packs that are in the following form : -1231 for - D1 D2 D3 Eta1
  //void CreateAllPack(); // Create all packs
  double Compute(double tComputeDelay); // Compute the result of generator
  //void RecordResult(double tComputeDelay); // Record the result of generator
  double RecordAndReturnResult(double tComputeDelay); // Record the result of generator and return the result
  int NbDelayMax(); // Maximum number of delays
											

};

/*!\}*/
#endif // __TDI_H

// end of TDI.h

