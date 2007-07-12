/*
Copyright (C) 2001  S. Babak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/******************  CVS info ************************ 
#define CVSTAG "$Name:  $"
#define CVSHEADER "$Header: /afs/aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/base/src/Constants.cc,v 1.3 2006/04/21 18:40:08 stba Exp $" 
*/
#include "Constants.hh"

namespace LISAWP{
  const double LISAWP_E =         2.7182818284590452353602874713526625L;  /* e */
  const double LISAWP_LOG2E =     1.4426950408889634073599246810018922L;  /* log_2 e */
  const double LISAWP_LOG10E =    0.4342944819032518276511289189166051L;  /* log_10 e */
  const double LISAWP_LN2 =       0.6931471805599453094172321214581766L;  /* log_e 2 */
  const double LISAWP_LN10 =      2.3025850929940456840179914546843642L;  /* log_e 10 */
  const double LISAWP_SQRT2 =     1.4142135623730950488016887242096981L;  /* sqrt(2) */
  const double LISAWP_SQRT1_2 =   0.7071067811865475244008443621048490L;  /* 1/sqrt(2) */
  const double LISAWP_GAMMA =     0.5772156649015328606065120900824024L;  /* gamma */
  /* Assuming we're not near a black hole or in Tennessee... */
  
  const double LISAWP_PI =        3.1415926535897932384626433832795029L;  /* pi */
  const double LISAWP_TWOPI =     6.2831853071795864769252867665590058L;  /* 2*pi */
  const double LISAWP_PI_2 =      1.5707963267948966192313216916397514L;  /* pi/2 */
  const double LISAWP_PI_4 =      0.7853981633974483096156608458198757L;  /* pi/4 */
  const double LISAWP_1_PI =      0.3183098861837906715377675267450287L;  /* 1/pi */
  const double LISAWP_2_PI =      0.6366197723675813430755350534900574L;  /* 2/pi */
  const double LISAWP_2_SQRTPI =  1.1283791670955125738961589031215452L;  /* 2/sqrt(pi) */
  const double LISAWP_PI_180 =    1.7453292519943295769236907684886127e-2L; /* pi/180 */
  const double LISAWP_180_PI =    57.295779513082320876798154814105170L; /* 180/pi */
  const double LISAWP_C_SI =      299792458.0; /* Speed of light in vacuo, m s^-1 */
  
const double LISAWP_EPSILON0_SI = 8.8541878176203898505365630317107503e-12L;
  /* Permittivity of free space, C^2 N^-1 m^-2 */
  const double LISAWP_MU0_SI =      1.2566370614359172953850573533118012e-6L;
  /* Permeability of free space, N A^-2 */
  

  // PHYSICAL CONSTANTS
  
  
  const double LISAWP_GEARTH_SI =   9.80665; /* Standard gravity, m s^-2 */
  const double LISAWP_PATM_SI =     101325; /* Standard atmosphere, Pa */
  const double LISAWP_G_SI =        6.67259e-11;    /* Gravitational constant, N m^2 kg^-2 */
  const double LISAWP_H_SI =        6.6260755e-34;  /* Planck constant, J s */
  const double LISAWP_HBAR_SI =     1.05457266e-34; /* Reduced Planck constant, J s */
  const double LISAWP_MPL_SI =      2.17671e-8;     /* Planck mass, kg */
  const double LISAWP_LPL_SI =      1.61605e-35;    /* Planck length, m */
  const double LISAWP_TPL_SI =      5.39056e-44;    /* Planck time, s */
  const double LISAWP_K_SI =        1.380658e-23;   /* Boltzmann constant, J K^-1 */
  const double LISAWP_R_SI =        8.314511;       /* Ideal gas constant, J K^-1 */
  const double LISAWP_MOL  =        6.0221367e23;   /* Avogadro constant, dimensionless */
  const double LISAWP_BWIEN_SI =    2.897756e-3;    /* Wien displacement law constant, m K */
  const double LISAWP_SIGMA_SI =    5.67051e-8;  /* Stefan-Boltzmann constant, W m^-2 K^-4 */
  const double LISAWP_AMU_SI =      1.6605402e-27;  /* Atomic mass unit, kg */
  const double LISAWP_MP_SI =       1.6726231e-27;  /* Proton mass, kg */
  const double LISAWP_ME_SI =       9.1093897e-31;  /* Electron mass, kg */
  const double LISAWP_QE_SI =       1.60217733e-19; /* Electron charge, C */
  const double LISAWP_ALPHA =       7.297354677e-3; /* Fine structure constant, dimensionless */
  const double LISAWP_RE_SI =       2.81794092e-15; /* Classical electron radius, m */
  const double LISAWP_LAMBDAE_SI =  3.86159323e-13; /* Electron Compton wavelength, m */
  const double LISAWP_AB_SI =       5.29177249e-11; /* Bohr radius, m */
  const double LISAWP_MUB_SI =      9.27401543e-24; /* Bohr magneton, J T^-1 */
  const double LISAWP_MUN_SI =      5.05078658e-27; /* Nuclear magneton, J T^-1 */
  const double LISAWP_REARTH_SI =   6.378140e6;      /* Earth equatorial radius, m */
  const double LISAWP_AWGS84_SI =   6.378137e6;      /* Semimajor axis of WGS-84 Reference Ellipsoid, m */
  const double LISAWP_BWGS84_SI =   6.356752314e6;   /* Semiminor axis of WGS-84 Reference Ellipsoid, m */
  const double LISAWP_MEARTH_SI =   5.97370e24;      /* Earth mass, kg */
  const double LISAWP_IEARTH =      0.409092804;     /* Earth inclination (2000), radians */
  const double LISAWP_EEARTH =      0.0167;          /* Earth orbital eccentricity */
  const double LISAWP_RSUN_SI =     6.960e8;         /* Solar equatorial radius, m */
  const double LISAWP_MSUN_SI =     1.98892e30;      /* Solar mass, kg */
  const double LISAWP_MRSUN_SI =    1.47662504e3;    /* Geometrized solar mass, m */
  const double LISAWP_MTSUN_SI =    4.92549095e-6;   /* Geometrized solar mass, s */
  const double LISAWP_LSUN_SI =     3.846e26;        /* Solar luminosity, W */
  const double LISAWP_AU_SI =       1.4959787066e11; /* Astronomical unit, m */
  const double LISAWP_PC_SI =       3.0856775807e16; /* Parsec, m */
  const double LISAWP_YRTROP_SI =   31556925.2;      /* Tropical year (1994), s */
  const double LISAWP_YRSID_SI  =   31558149.8;      /* Sidereal year (1994), s */
  const double LISAWP_DAYSID_SI =   86164.09053;     /* Mean sidereal day, s */
  const double LISAWP_LYR_SI =      9.46052817e15;   /* ``Tropical'' lightyear (1994), m */
  const double LISAWP_H0FAC_SI =    3.2407792903e-18; /* Hubble constant prefactor, s^-1 */
  const double LISAWP_H0_SI =       2e-18;            /* Approximate Hubble constant, s^-1 */
  /* Hubble constant H0 = h0*HOFAC, where h0 is around 0.65 */
  const double LISAWP_RHOCFAC_SI =  1.68860e-9;   /* Critical density prefactor, J m^-3 */
  const double LISAWP_RHOC_SI =     7e-10;         /* Approximate critical density, J m^-3 */
  /* Critical density RHOC = h0*h0*RHOCFAC, where h0 is around 0.65 */
  const double LISAWP_TCBR_SI =     2.726;   /* Cosmic background radiation temperature, K */
  const double LISAWP_VCBR_SI =     3.695e5; /* Solar velocity with respect to CBR, m s^-1 */
  const double LISAWP_RHOCBR_SI =   4.177e-14; /* Energy density of CBR, J m^-3 */
  const double LISAWP_NCBR_SI =     4.109e8;   /* Number density of CBR photons, m^-3 */
  const double LISAWP_SCBR_SI =     3.993e-14; /* Entropy density of CBR, J K^-1 m^-3 */
  const double LISAWP_STAS =        1911.1974; 

} // end of namespace
