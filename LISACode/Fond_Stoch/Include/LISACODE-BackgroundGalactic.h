// $Id: LISACODE-BackgroundGalactic.h,v 1.6 2007/03/29 15:16:17 lalanne Exp $
/*
 *  LISACODE-BackgroundGalactic.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * 
 *
 *
 *  Created on 30/05/06 by Antoine PETITEAU (APC)
 *  Last modification on 30/05/06 by Antoine PETITEAU (APC)
 *
 */

#ifndef __BACKGROUNDGALACTIC_H
#define __BACKGROUNDGALACTIC_H


#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Background.h"

using namespace std;
/*!\ingroup bg */
/*! \brief Background Galactic signal received by phasemeters is described in this class.
*/
class BackgroundGalactic : public Background
{
protected:
  /*! \brief Vector of time values associated to signal samples.*/
	vector<double> TimeList;
  /*! \brief Set of signals received by each photometer.
   *
   * \param SignalList[i][j] is the j-th sample of signal received by the i-th
   * photometer.
   */
	vector< vector<double> > SignalList;
  /*! \var NbData
    \brief Number of data or samples */
  /*! \var iRead
    \brief Last bin or index read */
	int NbData, iRead;
  /*! \var tmp_t
    \brief Last time value read */
  /*! \var tmp_ci
    \brief Last bin time lower or equal to current time */
  /*! \var tmp_cip1
    \brief First bin time greater than current time */
	double tmp_t, tmp_ci, tmp_cip1;
  /*! \var tmp_Sig_i
    \brief Signal value corresponding to #tmp_ci */
  /*! \var tmp_Sig_ip1
    \brief Signal value corresponding to #tmp_cip1  */
	vector<double> tmp_Sig_i, tmp_Sig_ip1;
	
public:
	/* Constructor */
	BackgroundGalactic();
	BackgroundGalactic(char * FileName, double Factor);
	BackgroundGalactic(Geometry * LISAGeo_n, char * FileName, double Factor);
	~BackgroundGalactic();
	
	/* Access methods */

	
	/*  Others methods */
	void ReadFile(char * FileName, double Factor_n); // Load file specified in argument
	double deltanu(int iSC, int IndirSens, double t);
};

#endif // __BACKGROUNDGALACTIC_H

// end of LISACODE-BackgroundGalactic.h
