// $Id: LISACODE-TDI_InterData.h,v 1.6 2007/04/05 09:32:44 ruiloba Exp $
/*
 *  TDI_InterData.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * 
 *
 *
 *  Created on 13/09/05 by Antoine PETITEAU (APC)
 *  Last modification on 13/09/05 by Antoine PETITEAU (APC)
 *
 */

/*!\ingroup tdiHand
 *\defgroup tdi_InterData TDI_InterData
 *\{
 */


#ifndef __TDI_INTERDATA_H
#define __TDI_INTERDATA_H
 

#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Serie.h"
#include "LISACODE-Memory.h"

using namespace std;

/*! \brief Time Delay Interferometry interpolated signal class.
 */

class TDI_InterData
{
 protected:
  /*! \brief Pointer to the list of delay's lengths. */
  Memory * TDelay;
  /*! \brief Memory where the signals of photodetector-phasemeter are stored. */
  vector<Memory *> * PDPMMem;
  /*! \brief Time during which the values Eta are stored. */
  double TimeStore;
  /*! \brief List of the memory where Eta's values are stored. */  
  vector<Serie> Eta;
  /*! \brief True if enough data are stored. */
  bool Usable;
  /*! \brief Type of interpolation used to obtain and to get data. */
  INTERP InterpType;
  /*! \brief Value used for interpolation. */
  double InterpUtilValue;
  /*! \brief Delay between the compute of Eta and the signals */
  double tShift;
  /*! \brief If true, there are no noise  */
  bool NoNoise;
 public:
  /* Constructor */
  TDI_InterData();
  TDI_InterData( Memory * TDelay_n,
				 vector<Memory *> * PDPMMem_n);
  TDI_InterData( Memory * TDelay_n,
				 vector<Memory *> * PDPMMem_n,
				 double TimeStore_n,
				 double tShift_n,
				 bool NoNoise,
				 INTERP InterpType_n = LAG,
				 double InterpUtilValue_n = 6);

  ~TDI_InterData();
  
  
  /* Access methods */
  /*! \brief Returns #TimeStore attribue . */
  double getTimeStore(){return(TimeStore);};
  void setTimeStore(double TimeStored_n);
  /*! \brief Returns first #Eta ref step attribue . */
  double gettStep(){return(Eta[0].getRefStep());};
  /*! \brief Returns #Usable attribute. */
  bool getUsable(){return(Usable);};
  /*! \brief Returns #tShift attribute. */
  double gettDelayCompute(){return(tShift);};
     
  /*  Others methods */
  void ComputeEta(); // Compute Eta
  double gData(int iSC, int IndirectDir, double Delay); // Return value interpolated for the delay (iSC=[1,3] and Indirect=[0,1]).
  double gData(int iSerie, double Delay); // Return value interpolated for the delay (iSerie=[1,6]).
//  int unusable(double tSinceFirstReception) const; // Return 0 if there are enough stored data to use them.

};
/*!\}*/
#endif // __TDI_INTERDATA_H

// end of TDI_InterData.h

