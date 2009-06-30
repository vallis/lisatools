// $Id:  $
/*
 *  LISACODE-Tools.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 28/05/09 by  Antoine Petiteau
 *  Last modification on 28/05/09 by Antoine Petiteau 
 *
 */

#include "LISACODE-Tools.h"

/*! \brief Constructs an instance and initializes it with default values. */
Tools::Tools()
{
	_MemAllocSize_;
	DispScr = true;
}

Tools::Tools(bool DispScr_n)
{
	_MemAllocSize_;
	DispScr = DispScr_n;
}


/*! \brief Destructor */
Tools::~Tools ()
{
	
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// **********
// * Memory *
// **********

void* Tools::AllocMemory(size_t size) {
	void *ptr;
	_MemAllocSize_ += size;
	//cout << "Alloc " << ptr << " for " << size << "o (Total = " << _MemAllocSize_/(1024.*1024.) << " Mo)";fflush(stdout);
	ptr = malloc(size);
	//cout << " --> OK" << endl;fflush(stdout);
	
	if ( ptr == NULL ) {
		fprintf(stderr,"\n\n **** Memory allocation failed.  Unable to allocate %lf Mo.  Already allocated memory : %lf Mo ***\n\n",
				size/(1024.*1024.), _MemAllocSize_/(1024.*1024.));
		exit(1);
	}
	
	return ptr;
}


void  Tools::Free(void *ptr, size_t size)
{
	//cout << "Free " << ptr << " for " << size << "o (Total = " << _MemAllocSize_/(1024.*1024.) << " Mo)";fflush(stdout);
	_MemAllocSize_ -= size;
	free(ptr);
	//cout << " --> OK" << endl;fflush(stdout);
}


void Tools::MemDisplay()
{
	if(DispScreen()){
		cout << " * Allocated memory  = " << _MemAllocSize_ / (1024.*1024.*sizeof(char)) << " Mo" << endl;
		fflush(stdout);
	}
}




// end of LISACODE-Tools.cpp
