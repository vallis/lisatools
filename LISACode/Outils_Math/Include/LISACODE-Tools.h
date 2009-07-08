// $Id:  $
/*
 *  LISACODE-Tools.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  Class of tools 
 *
 *
 *  Created on 28/05/09 by  Antoine Petiteau
 *  Last modification on 28/05/09 by Antoine Petiteau 
 *
 */
/*!\ingroup mathTools
 *\defgroup tools Tools
 *\{
 */
#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdexcept>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;


/*! \brief Couple management class.	
 */
class Tools
{
private:
	/*! \brief  */
	size_t _MemAllocSize_;
	
	/*! \brief True if screen displays */
	bool DispScr;

public:

	Tools();
	Tools(bool DispScr_n);
	
	~Tools(); 
	
	//*** Memory ***//
	void* AllocMemory(size_t size);
	void Free(void *ptr, size_t size);
	void MemDisplay();
	
	//*** Display ***//
	bool DispScreen() {return(DispScr);};
	void setDispScreen() {DispScr=true;};
	void unsetDispScreen() {DispScr=false;};
	
};

/*! \}*/
#endif //__TOOLS_H

// end of LISACODE-Tools.h
