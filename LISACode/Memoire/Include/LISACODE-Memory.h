// $Id: LISACODE-Memory.h,v 1.6 2007/04/05 09:32:34 ruiloba Exp $
/*
 *  LISACODE-Memory.h
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) gere et stocke les donnees qui lui sont fournis dans une memoire temporaire
 * et/ou dans un fichier.
 * L'indice de la premiere serie est 0.
 *
 *
 *  Created on 19/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 27/10/05 by Antoine PETITEAU (APC)
 *
 */
/*!\defgroup mem Memory (directory Memoire)
 *\{
 */
#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <vector.h>
#include <fstream.h>
#include <string>
#include <sstream>
#include "LISACODE-MathUtils.h"
#include "LISACODE-Serie2.h"
#include "LISACODE-LISAConstants.h"

using namespace std;

/*!\brief Memory management class.	
 *
 */


class Memory
	{
	protected:
		/*!\brief List of stored data series (RAM) */
		vector<Serie2> ListTmpData; // List of stored datas' series (RAM)
		/*!\brief Vector elements are set to 1 if data are already received for the corresponding serie. */
		vector<bool> AlreadyRecDat;
		// List of boolean which is 1 if one data is already received for the corresponding serie 
		/*!\brief Memorization time (RAM) 
		 * Time during which the data are preserved.
		 */
		/*!\brief Time during which the data are preserves (RAM) */
		double tStoreData; 
		/*!\brief Time step for recording data */
		double tStepRecord;
		
	public:
		/* Constructor */
		Memory();
		Memory(double tStoreData_n, double tStepRecord_n);  
		virtual ~Memory();
		
		/* Access methods */
		/*!\brief Gets the number of the serie */
		int getNbSerie(){return(int(ListTmpData.size()));}; //Return the number of serie
		/*!\brief Gets #tStoreData attribute */
		double gettStoreData(){return(tStoreData);};
		void settStoreData(double tStoreData_n);
		/*!\brief  */
		/*!\brief Gets #tStepRecord attribute */
		double gettStepRecord(){return(tStepRecord);};
		void settStepRecord(double tStepRecord_n);
		virtual double gettMax();
		
		/*  Others methods */
		virtual void AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName); // Add serie.
		virtual void MakeTitles(char *  FileNameHead = " "); // Make titles which are displayed or recorded .
		void ReceiveData(int SerieNumber, double data); // Receive data
		virtual void RecordAccData(double tStep, double t); // Record received datas (make it between each step of time)
		double gData(int SerieNumber, double delay); // Return the data of specified serie delayed by specified delay.
		double gData(int SerieNumber, double delay, INTERP InterpolType, double InterpUtilValue);
		// Return the data of specified serie delayed by specified delay with the specified interpolation.
		int unusable(double tSinceFirstReception) const; // Return 0 if there are enough stored data to use them.
	};
/*!\}*/
#endif // __MEMORY_H   

// end of Memory.h

