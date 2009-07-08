/*
 *  LISACODE-LISACodeLightClass.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ---------- 
 * Class for computing quickly TDI signal corresponding only to GW sources 
 * (no noise, no phasemeter, ...)  
 *
 *  Created by Antoine Petiteau (AEI) on 09/09/08.
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */

#ifndef __LISACODELIGHTCLASS_H
#define __LISACODELIGHTCLASS_H

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fftw3.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-Tools.h"
#include "LISACODE-GW.h"
#include "LISACODE-GWMono.h"
#include "LISACODE-GWBinary.h"
#include "LISACODE-GWNewton2.h"
#include "LISACODE-GWFile.h"
#include "LISACODE-GWFile2.h"
#include "LISACODE-GWPeriGate.h"
#include "LISACODE-GWSto.h"
#include "LISACODE-GWFastSpinBBH.h"
#include "LISACODE-GWCusp.h"
#include "LISACODE-GWNew.h"
#include "LISACODE-GWBuffer.h"
#include "LISACODE-Geometry.h"
#include "LISACODE-GeometryAnalytic.h"
#include "LISACODE-GeometryMLDC.h"
#include "LISACODE-GeometryFile.h"
#include "LISACODE-TrFctGW.h"
#include "LISACODE-Memory.h"
#include "LISACODE-TDI_InterData.h"
#include "LISACODE-TDITools.h"
#include "LISACODE-TDI.h"
#include "LISACODE-MemoryWriteDisk.h"

class LISACodeLightClass
	{
	protected:
		Tools * MT;
		
		/*! \brief Time step */
		double dt;
		/*! \brief Inital time  */
		double t0;
		/*! \brief Time of observation */
		double Tobs; 
		
		bool TDIDelayApprox;
		
		double tStoreGeo;
		
		/*! \brief TDI interpolation : type */
		INTERP TDIInterpType;
		
		/*! \brief TDI interpolation : Number of value need */
		int TDIInterpVal;
		
		/*! \brief TDI interpolation : type */
		INTERP TDIDelayInterpType;
		
		/*! \brief TDI interpolation : Number of value need */
		int TDIDelayInterpVal;
		
		/*! \brief Time required for TDI interpolation */
		double tMemInterpTDI;
		
		int iTEndGW;
		
		/*! \brief Type of waveform interpolation in GWBuffer*/
		INTERP GWBufInterpType;
		
		/*! \brief Number of value need for making waveform interpolation in GWBuffer*/
		int GWBufInterpVal;
		
		/*! \brief Time required for GW interpolation */
		double tMemInterpGW;
		
		/*! \brief Table of GW sources */
		vector< GW * > * GWs;
		
		/*! \brief Total number of parameters */
		int NbParams;
		
		/*! \brief LISA orbits */
		Geometry * LISAGeo;
		
		/*! \brief Compute of phasemeter signal only due to GWs */
		TrFctGW * SigGWs;
		
		Memory * SCSig;
		
		Memory * DelayTDI;
		
		TDITools *TDIQuick;
		
		/*!\brief List of TDI generators. */
		TDI ** TDIGens;
		
		/*!\brief Vector of TDI data. */
		vector< vector<int> > TDIsPacks;
		
		/*!\brief Vector of TDI data. */
		vector< vector<double> > TDIsPacksFact;
		
		/*!\brief Maximum Delays Number. */
		int NbMaxDelays;
		
		bool NoUsed;
		
		int NbtDat, NbfDat, NbTDI;
		
		double **tDat;
		
		double *tmptDat;
		
		fftw_complex *tmpfDat;
		
		fftw_plan FwdPlan;
		
		bool TFUsed;
		
		double FreqMin, FreqMax;
		
		/*! \brief Minimal limit for delay  */
		double tMinDelay;
		
		/*! \brief Maximal limit for delay */
		double tMaxDelay;
		
		/*! \brief Shift in time need for TDI interpolationat current time */
		double tTDIShift;
		
		/*! \brief Duration need for TDI computation */
		double tMemTDI; 
		
		bool TDIAE;
		
		/*! \brief Time offset in GW computation */
		double tGWExtra;
		
		
		public :
		// ** Constructor **
		
		LISACodeLightClass();
		
		LISACodeLightClass(Tools * MT_n, double t0_n, double dt_n, double Tobs_n);
		
		LISACodeLightClass(Tools * MT_n, double t0_n, double dt_n, double Tobs_n, vector <GW *> * GWs_n);
		
		/*! \brief Create an instance of LISACodeLigth with \NbGWSrc GWs which sources are descirbed 
		 * by \GWSrcTypes using the following convention : 
		 * \arg 1 -> Monochromatic source.
		 * \arg 2 -> Binary with fixed frequency.
		 * \arg	3 -> Binary described by Post-Newtonian model.
		 * \arg	4 -> Spinning Black Holes binary
		 * \arg	5 -> Cosmic String Cusp
		 */
		LISACodeLightClass(Tools * MT_n, double t0_n, double dt_n, double Tobs_n, int * GWSrcTypes, int NbGWSrc);
		
		~LISACodeLightClass();
		
		// ** Access methods **
		double getNbTDI(){ return(NbTDI);};
		double getNbtDat(){ return(NbtDat);};
		double getNbfDat(){ return(NbfDat);};
		int getNbParams(){ return(NbParams);};
		double gettExtra(){ return(tGWExtra);};
		void setTDIInterpType(INTERP TDIInterpType_n) {TDIInterpType = TDIInterpType_n;};
		void setTDIInterpVal(int TDIInterpVal_n) {TDIInterpVal = TDIInterpVal_n;};
		void setTDIDelayInterpType(INTERP TDIDelayInterpType_n) {TDIDelayInterpType = TDIDelayInterpType_n;};
		void setTDIDelayInterpVal(int TDIDelayInterpVal_n) {TDIDelayInterpVal = TDIDelayInterpVal_n;};
		void setTDIDelayApprox() {TDIDelayApprox = true;};
		void unsetTDIDelayApprox() {TDIDelayApprox = false;};
		void settStoreGeo(double tStoreGeo_n) {tStoreGeo = tStoreGeo_n;};
		void settEndGW(double TEndGW);
		void settGWExtra(double tGWExtra_n) {tGWExtra = tGWExtra_n;};
		void setGWBufInterpType(INTERP GWBufInterpType_n) {GWBufInterpType = GWBufInterpType_n;};
		void setGWBufInterpVal(int GWBufInterpVal_n) {GWBufInterpVal = GWBufInterpVal_n;};
		void setTF() {TFUsed = true;};
		
		double getFreqMin() {return(FreqMin);};
		double getFreqMax() {return(FreqMax);};
		
		void ChangeParamsGWs(double * NewParams);
		
		void AddGW(GW * GW_n);
		void AddGWMono();
		void AddGWBinaryFixFreq();
		void AddGWBinaryInspiralPN();
		void AddGWSpinBBH();
		/*! \brief Add GW source : Spinning Black Holes Binary */
		void AddGWSpinBBH (double Beta_n,
						   double Lambda_n, 
						   double Mass1_n,                
						   double Mass2_n,                 
						   double CoalescenceTime_n,
						   double Distance_n,
						   double Spin1_n,
						   double Spin2_n,
						   double PolarAngleOfSpin1_n,
						   double PolarAngleOfSpin2_n,    
						   double AzimuthalAngleOfSpin1_n,
						   double AzimuthalAngleOfSpin2_n,
						   double PhaseAtCoalescence_n,
						   double InitialPolarAngleL_n,
						   double InitialAzimuthalAngleL_n,
						   double Tobs);
		void AddGWCusp();
		void BufferGW(int iGW);
		
		void DispConfig();
		
		// ** Others methods **
		
		/*! \brief Initialization of time data : To do one time after setting and before GW including */
		void initTimeBase();
		/*! \brief Initialization of memories and LISA structure (spacecraft and TDI)  : To do one time after setting and after GW including */
		void initFirst();
		/*! \brief Initialization of GW and LISA (running for compute data need by TDI) */
		void init();
		
		/*! \brief Compute signal and delay of LISA */
		void RunLISA(double tTDI);
		
		/*! \brief Compute TDI */
		void ComputeTDI(double ** &tDatExt);
		
		/*! \brief Compute TDI Fourier Transform */
		void ComputeFTTDI(fftw_complex ** &fDatExt);
		
		/*! \brief Compute TDI for a new set of parameters */
		void TDINewParams(double ** &tDatExt, double * NewParams);
		
		/*! \brief Compute TDI Fourier Transform for a new set of parameters */
		void FTTDINewParams(fftw_complex ** &fDatExt, double * NewParams);
		
		/*! \brief Count the total numbers of parameters for GWs */
		void CountNbGWsParams();
		
		/* \brief Add TDI generator */
		void AddTDI(char * generatorname);
		
		/* \brief Add TDI generator AE */
		void AddTDI_AEMLDC();
		
	};




#endif // __LISACODELIGHTCLASS_H

// end of LISACode-LISACodeLightClass.h