// $Id: LISACODE-MemoryReadDisk.h,v 1.6 2007/04/05 09:32:34 ruiloba Exp $
/*
 *  MemoryReadDisk.h
 *  LISACode
 *
 *
 * Description : DERIV CLASS from Memory
 * ------------
 * This class (module) reads data in file and stores them.
 *
 *
 *  Created on 01/11/05 by Antoine PETITEAU (APC)
 *  Last modification on 04/07/08 by Antoine PETITEAU (AEI)
 *
 */

#ifndef __MEMORYREADDISK_H
#define __MEMORYREADDISK_H

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
/*!\brief Class to manage disk reading.
 * This class (module) reads data in file and stores them.
 *
 */
class MemoryReadDisk : public Memory
{
 protected:
/*!\brief File name on which data are read*/
    char * NomFichMem; // Name of recording file
/*!\brief File encoding on which data are recorded (0: ASCII, 1: BINARY) */
    int FEncoding; // Encoding file type (0: ASCII, 1: BINARY)
/*!\brief File object managing reading file #NomFichMem */
	ifstream FichMem; // File where datas are stored
/*!\brief Data read from file. */
	vector< vector<double> > ReadData; // Data readed in file
/*!\brief Titles of read series.  */
	vector<string> TitlesReadData; // List of title of readed series
/*!\brief Index in read data. */
	vector<int> IndexInReadData; // 
	  
 public:
  /* Constructor */
  MemoryReadDisk();
  MemoryReadDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n); 
  MemoryReadDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n, int Encoding_n, int NbColumns, double tTot); 
  ~MemoryReadDisk();

  /* Access methods */
  double gettMax();
  
  /*  Others methods */
  void ReadASCIIFile(); // Read data in ASCII file
  void ReadBinaryFile(int NbColumns, double tTot); // Read data in binary file
  void AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName); // Add serie.
  void MakeTitles(char *  FileNameHead = " "); // 
  void ReceiveData(int SerieNumber, double Data); // Receive data
  void RecordAccData(double tStep, double t); // Record received datas (make it between each step of time)
};

#endif // __MEMORYREADDISK_H   

// end of LISACODE-MemoryReadDisk.h
