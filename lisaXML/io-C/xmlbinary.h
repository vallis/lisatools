
/* --- data structures --- */

/* Represents a list of parameters for a single galactic binary */

typedef struct GBParameters {
    char *Name;
    
    double EclipticLatitude; 
    double EclipticLongitude;                 
    double Polarization;
    double Amplitude;
    double Inclination;
    double InitialPhase;
    double Frequency;
    double FreqDeriv;
    
} GBParameters;

/* --- data-handling functions --- */

/* Returns a GBParameters structure containing the parameters of the
   GalacticBinary PlaneWave object in the XML file "filename". */

GBParameters *getGBParameters(char *filename);

/* Frees all storage associated with a GBParameters structure */

void freeGBParameters(GBParameters *binary);
