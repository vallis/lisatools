/*
Copyright (C) 2000, 2005  S. Babak

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
#define CVSHEADER "$Header: /afs/aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/base/include/Constants.hh,v 1.3 2006/04/21 18:48:34 stba Exp $" 
*/

#ifndef CONSTANTSHH
#define CONSTANTSHH
#include <string>
namespace LISAWP{
  extern const double LISAWP_E;     /* e */
  extern const double LISAWP_LOG2E;  /* log_2 e */
  extern const double LISAWP_LOG10E; /* log_10 e */
  extern const double LISAWP_LN2;    /* log_e 2 */
  extern const double LISAWP_LN10;   /* log_e 10 */
  extern const double LISAWP_SQRT2;  /* sqrt(2) */
  extern const double LISAWP_SQRT1_2;  /* 1/sqrt(2) */
  extern const double LISAWP_GAMMA;   /* gamma */
  /* Assuming we're not near a black hole or in Tennessee... */

  extern const double LISAWP_PI;      /* pi */
  extern const double LISAWP_TWOPI;   /* 2*pi */
  extern const double LISAWP_PI_2;    /* pi/2 */
  extern const double LISAWP_PI_4;    /* pi/4 */
  extern const double LISAWP_1_PI;    /* 1/pi */
  extern const double LISAWP_2_PI;    /* 2/pi */
  extern const double LISAWP_2_SQRTPI;  /* 2/sqrt(pi) */
  extern const double LISAWP_PI_180;  /* pi/180 */
  extern const double LISAWP_180_PI;  /* 180/pi */
  extern const double LISAWP_C_SI;    /* Speed of light in vacuo, m s^-1 */
  extern const double LISAWP_EPSILON0_SI; /* Permittivity of free space, C^2 N^-1 m^-2 */
  extern const double LISAWP_MU0_SI;      /* Permeability of free space, N A^-2 */

  // PHYSICAL CONSTANTS
  extern const double LISAWP_GEARTH_SI;    /* Standard gravity, m s^-2 */
  extern const double LISAWP_PATM_SI;      /* Standard atmosphere, Pa */
  extern const double LISAWP_G_SI;         /* Gravitational externant, N m^2 kg^-2 */
  extern const double LISAWP_H_SI;         /* Planck externant, J s */
  extern const double LISAWP_HBAR_SI;      /* Reduced Planck externant, J s */
  extern const double LISAWP_MPL_SI;       /* Planck mass, kg */
  extern const double LISAWP_LPL_SI;       /* Planck length, m */
  extern const double LISAWP_TPL_SI;       /* Planck time, s */
  extern const double LISAWP_K_SI;         /* Boltzmann externant, J K^-1 */
  extern const double LISAWP_R_SI;         /* Ideal gas externant, J K^-1 */
  extern const double LISAWP_MOL;          /* Avogadro externant, dimensionless */
  extern const double LISAWP_BWIEN_SI;     /* Wien displacement law externant, m K */
  extern const double LISAWP_SIGMA_SI;     /* Stefan-Boltzmann externant, W m^-2 K^-4 */
  extern const double LISAWP_AMU_SI;       /* Atomic mass unit, kg */
  extern const double LISAWP_MP_SI;        /* Proton mass, kg */
  extern const double LISAWP_ME_SI;        /* Electron mass, kg */
  extern const double LISAWP_QE_SI;        /* Electron charge, C */
  extern const double LISAWP_ALPHA;        /* Fine structure externant, dimensionless */
  extern const double LISAWP_RE_SI;        /* Classical electron radius, m */
  extern const double LISAWP_LAMBDAE_SI;   /* Electron Compton wavelength, m */
  extern const double LISAWP_AB_SI;        /* Bohr radius, m */
  extern const double LISAWP_MUB_SI;       /* Bohr magneton, J T^-1 */
  extern const double LISAWP_MUN_SI;       /* Nuclear magneton, J T^-1 */
  extern const double LISAWP_REARTH_SI;    /* Earth equatorial radius, m */
  extern const double LISAWP_AWGS84_SI;    /* Semimajor axis of WGS-84 Reference Ellipsoid, m */
  extern const double LISAWP_BWGS84_SI;    /* Semiminor axis of WGS-84 Reference Ellipsoid, m */
  extern const double LISAWP_MEARTH_SI;    /* Earth mass, kg */
  extern const double LISAWP_IEARTH;       /* Earth inclination (2000), radians */
  extern const double LISAWP_EEARTH;       /* Earth orbital eccentricity */
  extern const double LISAWP_RSUN_SI;      /* Solar equatorial radius, m */
  extern const double LISAWP_MSUN_SI;      /* Solar mass, kg */
  extern const double LISAWP_MRSUN_SI ;    /* Geometrized solar mass, m */
  extern const double LISAWP_MTSUN_SI;     /* Geometrized solar mass, s */
  extern const double LISAWP_LSUN_SI;      /* Solar luminosity, W */
  extern const double LISAWP_AU_SI;        /* Astronomical unit, m */
  extern const double LISAWP_PC_SI;        /* Parsec, m */
  extern const double LISAWP_YRTROP_SI;    /* Tropical year (1994), s */
  extern const double LISAWP_YRSID_SI;     /* Sidereal year (1994), s */
  extern const double LISAWP_DAYSID_SI;    /* Mean sidereal day, s */
  extern const double LISAWP_LYR_SI;       /* ``Tropical'' lightyear (1994), m */
  extern const double LISAWP_H0FAC_SI;     /* Hubble externant prefactor, s^-1 */
  extern const double LISAWP_H0_SI;        /* Approximate Hubble externant, s^-1 */
  /* Hubble externant H0 = h0*HOFAC, where h0 is around 0.65 */
  extern const double LISAWP_RHOCFAC_SI;   /* Critical density prefactor, J m^-3 */
  extern const double LISAWP_RHOC_SI;      /* Approximate critical density, J m^-3 */
  /* Critical density RHOC = h0*h0*RHOCFAC, where h0 is around 0.65 */
  extern const double LISAWP_TCBR_SI;      /* Cosmic background radiation temperature, K */
  extern const double LISAWP_VCBR_SI;      /* Solar velocity with respect to CBR, m s^-1 */
  extern const double LISAWP_RHOCBR_SI;    /* Energy density of CBR, J m^-3 */
  extern const double LISAWP_NCBR_SI;      /* Number density of CBR photons, m^-3 */
  extern const double LISAWP_SCBR_SI;      /* Entropy density of CBR, J K^-1 m^-3 */
  extern const double LISAWP_STAS;         /* My birthday :) */
} // end of namespace

#endif

