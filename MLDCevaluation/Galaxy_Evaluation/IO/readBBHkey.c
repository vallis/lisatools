#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "IO/readxml.h"
#include "IO/xmlbinary.h"
#include "IO/xmlbbh.h"
#include "IO/lisaxml.h"

int main(int argc, char* argv[])
{

  BBHParameters *BBHparams;

  if(argc !=2)
   {
    fprintf(stderr,"./BBHread_key key.xml\n");
    exit;
   }

  
  BBHparams = getBBHParameters(argv[1]);

  printf("Inclination = %f\n", BBHparams->Inclination);
  printf("Initial Orbital Phase = %f\n", BBHparams->InitialAngularOrbitalPhase); 
  printf("Polarization = %f\n", BBHparams->Polarization);                              
  printf("Luminosity Distance = %e\n", BBHparams->Distance);
  printf("Mass 1 = %e\n", BBHparams->Mass1);
  printf("Mass 2 = %e\n", BBHparams->Mass2);
  printf("Ecliptic Latitude = %f\n", BBHparams->EclipticLatitude);
  printf("Ecliptic Longitude = %f\n", BBHparams->EclipticLongitude);
  printf("CoalescenceTime = %e\n", BBHparams->CoalescenceTime);

}
