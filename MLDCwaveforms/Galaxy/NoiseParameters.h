/****************************************************************************/
/*                                                                          */
/* TITLE: LISA Noise Preprossesor Definitions                               */
/* AUTHORS: Louis J. Rubbo and Neil J. Cornish                              */
/* DATE: June 13, 2006                                                      */
/*                                                                          */
/*                                                                          */
/* ABSTRACT: This header file defines the values of the power spectral      */
/* densities that will be used to scale the random numbers drawn to         */
/* simulate the acceleration and shot noise on the LISA spacecraft.  The    */
/* shot noises are reported in units of m^2/Hz while the acceleration       */
/* noises are given in units of m^2/s^4/Hz.  In our notation Sxij is the    */
/* power spectral density as measured on spacecraft j when the instrument   */
/* is pointed towards spacecraft i.                                         */
/*                                                                          */
/****************************************************************************/


          /* -----------------  SPACECRAFT #1  ------------------ */

 /* Shot noise power spectral density */
#define Ss21 (4.0e-22)

 /* Acceleration noise power spectral density */
#define Sa21 (9.0e-30)

 /* Shot noise power spectral density */
#define Ss31 (4.0e-22)

 /* Acceleration noise power spectral density */
#define Sa31 (9.0e-30)


          /* -----------------  SPACECRAFT #2  ------------------ */

 /* Shot noise power spectral density */
#define Ss12 (4.0e-22)

 /* Acceleration noise power spectral density */
#define Sa12 (9.0e-30)

 /* Shot noise power spectral density */
#define Ss32 (4.0e-22)

 /* Acceleration noise power spectral density */
#define Sa32 (9.0e-30)


          /* -----------------  SPACECRAFT #3  ------------------ */

 /* Shot noise power spectral density */
#define Ss13 (4.0e-22)

 /* Acceleration noise power spectral density */
#define Sa13 (9.0e-30)

 /* Shot noise power spectral density */
#define Ss23 (4.0e-22)

 /* Acceleration noise power spectral density */
#define Sa23 (9.0e-30)
