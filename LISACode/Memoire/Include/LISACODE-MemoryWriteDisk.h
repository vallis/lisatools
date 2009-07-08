// $Id: LISACODE-MemoryWriteDisk.h,v 1.5 2007/04/05 09:32:34 ruiloba Exp $
/*
 *  MemoryWriteDisk.h
 *  LISACode
 *
 *
 * Description : DERIVS CLASS from Memory
 * ----------
 * This class (module) manages and stores data supplied to its in a temporary memory
 * and in a file.
 * Index of first series is 0.
 *
 *
 *  Created on 19/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 04/07/08 by Antoine PETITEAU (AEI)
 *
 */

#ifndef __MEMORYWRITEDISK_H
#define __MEMORYWRITEDISK_H

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Serie.h"
#include "LISACODE-Memory.h"
using namespace std;

/*!\ingroup mem*/
/*!\brief Class to manage disk writting.
 * This class (module) manages and stores data supplied to its in a temporary memory
 * and in a file.
 * Index of first series is 0.
 *
 */
class MemoryWriteDisk : public Memory
	{
	protected:
		/*!\brief File name on which data are recorded */
		char * NomFichMem; // Name of recording file
		/*!\brief File encoding on which data are recorded (0: ASCII, 1: BINARY) */
		int FEncoding; // Encoding file type (0: ASCII, 1: BINARY)
		/*!\brief File object managing recording file #NomFichMem */
		ofstream FichMem; // File where datas are stored
		/*!\brief Spacecraft index for data series */
		vector<int> SCSerie; // List of spacecraft's index for datas' series
		/*!\brief Vector of series titles */
		vector<string> TitleSerie; // List of series' title
		/*!\brief True if header must be written in binary file */
		bool BinHeader; 
		/*!\brief For the header */
		double TimeOffset, TimeEnd ; 
		
	public:
		/* Constructor */
		MemoryWriteDisk();
		MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n);
		MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n, int Encoding_n);
		MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n, int Encoding_n, bool BinHeader_n, double TimeOffset_n, double TimeEnd_n);
		~MemoryWriteDisk();
		
		/* Access methods */
		double gettMax();
		
		/*  Others methods */
		void AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName); // Add serie.
		void MakeTitles(char * FileNameHead = " "); // Make titles which are displayed or recorded .
		void RecordAccData(double tStep, double t); // Record received datas (make it between each step of time)
		void CloseFile();
	};

#endif // __MEMORYWRITEDISK_H   

// end of LISACODE-MemoryWriteDisk.h

