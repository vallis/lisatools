/****************************************************************************/
/*                                                                          */
/* TITLE: LISA Simulator Preprossesor Definitions                           */
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
#define pi 3.1415926535897932385


          /* ---------------  NATURAL CONSTANTS  ---------------- */ 

 /* Speed of light (m/s) */
#define c 2.99792458e8

 /* Newton's gravitational constant (mks) */
#define G 6.67259e-11

 /* Astronomical unit (meters) */
#define AU 1.49597870660e11

 /* Sidereal year (seconds) */
#define year 3.15581498e7

 /* Mass of the Sun (kg) */
#define Msun 1.9889e30
 
 /* Number of meters in a kpc */
#define kpc 3.0856675807e19

 /* Number of meters in a Gpc */
#define Gpc 3.0856675807e25


          /* --------------  DATA ANALYIS VALUES  --------------- */

 /* Power of 2 that gives the number of data points to be FFTed, 
    i.e. NFFT = 2^n */
#define npow 21
/* #define npow 18 */

 /* Number of data points to be FFTed */
#define NFFT 2097152
/* #define NFFT 262144 */

 /* Observation length */
#define T 31457280.0
/* #define T 3932160.0 */

 /* Increment of time between data sample points for the detector */
#define dt 15.0

/* Input Signal Padding */
#define Tpad 900.0

 /* Number of sections to divide the observation length into */
#define Ndiv 8


          /* -----------  LISA'S INITIAL CONDITIONS  ------------ */

 /* Initial azimuthal position of the guiding center in the ecliptic plane */
#define kappa 0.0

 /* Initial orientation of the LISA spacecraft */
#define lambda 0.0


          /* ------------  LISA SPECIFIC CONSTANTS  ------------- */

 /* Orbital radius of the guiding center */
#define Rgc (1.0*AU)

 /* Mean arm length of the LISA detector (meters) */
#define L 5.0e9

 /* Eccentricity of the LISA spacecraft orbits */
#define ecc (L/(2.0*sqrt(3.0)*Rgc))
