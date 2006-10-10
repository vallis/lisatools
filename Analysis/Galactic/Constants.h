/****************************************************************************/
/*                                                                          */
/* TITLE: Preprocessor Macros For The Galactic Background                   */
/* AUTHOR: Louis J. Rubbo                                                   */
/* DATE: March 16, 2004                                                     */
/*                                                                          */
/* ABSTRACT: This header file contains a number of constants used           */
/* throughout the making of the galactic backgound realizations.            */
/*                                                                          */
/****************************************************************************/



          /* --------------  MATHEMATICAL CONSTANTS  -------------- */

 /* Set the value of pi */
#define pi 3.141592653589793


          /* ----------------  NATURAL CONSTANTS  ----------------- */

 /* Speed of light (m/s) */
#define c 299792458.0

 /* Number of seconds in a sidereal year */
#define year 3.15581498e7

#define fm 3.16875357503e-8

 /* Newton's gravitational constant (mks) */
#define G 6.67259e-11

 /* Astronomical unit (meters) */
#define AU 1.49597870660e11

 /* Number of meters in a parsec */
#define pc 3.0856775807e16

 /* Number of meters in a kiloparsec */
#define kpc 3.0856775807e19

 /* Distance bewteen the Sun and the center of the galaxy (kpc) */
#define Rsun 8.5

 /* Mass of the Sun (kg) */
#define Msun 1.9889e30


          /* ----------------  DETECTOR CONSTANTS  ---------------- */

 /* Initial azimuthal position of the guiding center */
#define kappa 0.0

 /* Initial orientation of the LISA constellation */
#define lambda 0.0

 /* Orbital radius of the guiding center */
#define Rgc (1.0*AU)

 /* Mean arm length of the LISA detector (meters) */
#define L 5.0e9

 /* Eccentricity of the LISA spacecraft orbits */
#define ecc (L/(2.0*sqrt(3.0)*Rgc))

 /* Photon shot noise power */
#define Sps 4.0e-22
 
 /* Acceleration noise power */
#define Sacc 9.0e-30
