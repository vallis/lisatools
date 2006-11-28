
/* --- data structures --- */

/* Represents a list of parameters for a single schwarzschild binary */

typedef struct BBHParameters {
    char *Name;
    
    double EclipticLatitude; 
    double EclipticLongitude;                 
    double Polarization;
    double Mass1;
    double Mass2;
    double CoalescenceTime;
    double InitialAngularOrbitalPhase;
    double Distance;
    double Inclination;
    
} BBHParameters;

/* --- data-handling functions --- */

/* Returns a BBHParameters structure containing the parameters of the
   Schwarzschild Binary PlaneWave object in the XML file "filename". */

BBHParameters *getBBHParameters(char *filename);

/* Frees all storage associated with a GBParameters structure */

void freeBBHParameters(BBHParameters *binary);
