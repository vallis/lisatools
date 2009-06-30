// $Id: LISACODE-TDITools.cpp,v 1.2 2007/04/05 09:33:25 ruiloba Exp $
/*
 *  TDITools.cpp
 *  V 1.4
 *  LISACode
 *
 *  Created on 30/06/05 by Antoine PETITEAU (APC)
 *  Last modification on 30/06/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-TDITools.h"

/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * Attributes are :
 * \arg #TDelay = empty
 * \arg #DelayMem = (0,0,0)
 * \arg #RapidOption = FALSE
 */
TDITools::TDITools()
{
	TDelay = new Memory();
	DelayMem[0] = 0.0;
	DelayMem[1] = 0.0;
	DelayMem[2] = 0.0;
	RapidOption = false;
	DelayInterpType = TRU;
	DelayInterpVal = 0;
}

/*! \brief Constructs an instance and initializes it with inputs default values.
 *
 * Attributes are :
 * \arg #TDelay = TDelay_n input
 * \arg #DelayMem = (0,0,0)
 * \arg #RapidOption = RapidOption_n input
 */
TDITools::TDITools(Memory * TDelay_n, bool RapidOption_n, INTERP DelayInterpType_n, double DelayInterpVal_n)
{
	TDelay = TDelay_n;
	DelayMem[0] = 0.0;
	DelayMem[1] = 0.0;
	DelayMem[2] = 0.0;
	RapidOption = RapidOption_n;
	DelayInterpType = DelayInterpType_n;
	DelayInterpVal = DelayInterpVal_n;
}

/*! \brief Destructor. */
TDITools::~TDITools()
{
	
}


/* Access methods */
/*! \brief Returns #DelayMem[mod(IndexDelay-1,3)] attribute. */
double TDITools::getDelay(int IndexDelay)
{
	int iD(IndexDelay-1);
	if(iD>=3)
		iD -= 3;
	//cout << "  --> D" << IndexDelay << " = Delay[" << iD << "] = " << DelayMem[iD] << endl;
	return(DelayMem[iD]);
}


/*  Others methods */
/*! \brief Updates #DelayMem attribute. 
* 
* For ech delay (0,1,2), Memory::gdata method is used with tComputeDelay input delay, truncated interpolation type, and zero InterpUtilValue. 
* 
*/
void  TDITools::RefreshDelay(double tComputeDelay)
{
	DelayMem[0] = TDelay->gData(0, tComputeDelay, TRU, 0);
	DelayMem[1] = TDelay->gData(1, tComputeDelay, TRU, 0);
	DelayMem[2] = TDelay->gData(2, tComputeDelay, TRU, 0);
	//cout << "  Refresh  delays at t = " << tComputeDelay << " : " << DelayMem[0] << " " << DelayMem[1] << " " << DelayMem[2] << endl;
}


// end of TDITools.cpp
