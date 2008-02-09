// $Id: LISACODE-MemoryWriteDisk.h,v 1.5 2007/04/05 09:32:34 ruiloba Exp $
/*
 *  MemoryWriteDisk.h
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

#ifndef __MEMORYWRITEDISK_H
#define __MEMORYWRITEDISK_H

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <vector.h>
#include <fstream.h>
#include <string>
#include <sstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Serie.h"
#include "LISACODE-Memory.h"
using namespace std;

/*!\ingroup mem*/
/*!\brief Class to manage disk writting.
 *
 */
class MemoryWriteDisk : public Memory
{
 protected:
/*!\brief File name on which data are recorded */
    char * NomFichMem; // Name of recording file
/*!\brief File encoding on which data are recorded */
    int FEncoding; // Encoding file type
/*!\brief File object managing recording file #NomFichMem */
	ofstream FichMem; // File where datas are stored
/*!\brief Spacecraft index for data series */
	vector<int> SCSerie; // List of spacecraft's index for datas' series
/*!\brief Vector of series titles */
	vector<string> TitleSerie; // List of series' title
  
 public:
  /* Constructor */
  MemoryWriteDisk();
  MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n);
  MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n, int Encoding_n);
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

