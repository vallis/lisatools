/****************************************************************************/
/*                                                                          */
/* TITLE: LISA Simulator Preprocessor Definitions                           */
/* AUTHORS: Louis J. Rubbo and Neil J. Cornish                              */
/* DATE: June 13, 2006                                                      */
/*                                                                          */
/*                                                                          */
/* ABSTRACT: These preprocessor macro constants are values that are shared  */
/* through out The LISA Simulator codes.  The natural constants are taken   */
/* from the Particle Physics Booklet.                                       */
/*                                                                          */
/****************************************************************************/


          /* -------------  MATHEMATICAL CONSTANTS  ------------- */

 /* Set the value of pi */
static const double pi=3.1415926535897932385;


          /* ---------------  NATURAL CONSTANTS  ---------------- */ 

 /* Speed of light (m/s) */
static const double c=2.99792458e8;

 /* Newton's gravitational constant (mks) */
static const double G=6.67259e-11;

 /* Astronomical unit (meters) */
static const double AU=1.49597870660e11;

 /* Sidereal year (seconds) */
static const double year=3.15581498e7;

 /* Mass of the Sun (kg) */
static const double Msun=1.9889e30;
     
 /* Number of meters in a kpc */
static const double kpc=3.0856675807e19;

 /* Number of meters in a Gpc */
static const double Gpc=3.0856675807e25;


          /* --------------  DATA ANALYIS VALUES  --------------- */

 /* Power of 2 that gives the number of data points to be FFTed, i.e. NFFT = 2^n */
static const int npow=25;

 /* Number of data points to be FFTed */
static const long NFFT=33554432;

 /* Observation length */
static const double T=62914560.0;

 /* Increment of time between data sample points for the detector */
static const double dt=1.875;

/* Input Signal Padding */
static const double Tpad=900.0;

 /* Number of sections to divide the observation length into */
static const int Ndiv=16;


          /* -----------  LISA'S INITIAL CONDITIONS  ------------ */

 /* Initial azimuthal position of the guiding center in the ecliptic plane */
static const double kappa=0.0;

 /* Initial orientation of the LISA spacecraft */
static const double lambda=0.0;


          /* ------------  LISA SPECIFIC CONSTANTS  ------------- */

 /* Orbital radius of the guiding center (1 AU) */
static const double Rgc=1.49597870660e11;

 /* Mean arm length of the LISA detector (meters) */
static const double L=5.0e9;

 /* Eccentricity of the LISA spacecraft orbits (L/(2.0*sqrt(3.0)*Rgc)) */
static const double ecc=0.0096483704387377977;
