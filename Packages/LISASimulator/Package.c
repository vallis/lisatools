/****************************************************************************/
/*                                                                          */
/* TITLE: Package                                                           */
/* AUTHOR: Neil J. Cornish                                                  */
/* DATE: June 22, 2006                                                      */
/* VERSION: 2.1.1                                                           */
/*                                                                          */
/* gcc -O2 -o Package Package.c IO/lisaxml.c IO/ezxml.c IO/xmlbinary.c      */
/*   IO/readxml.c -lm                                                       */
/*                                                                          */
/****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "IO/lisaxml.h"
#include "IO/readxml.h"
#include "LISAconstants.h"

void KILL(char*);

int main(int argc,char **argv) {
    writeXML *myxml;
    char buffer[256];
    char Name[50];
    char recordname[50];
    char inputname[50];
    char outputname[50];
    char binaryname[50];
    char ErrorMessage[100];
    char sourcename[50];
    char tagin[50];
    char tag[50];
    size_t ns;
    double t, tmp;
    double *X_TDI, *Y_TDI, *Z_TDI;
    long i, k;
    FILE* sourcefile;
    FILE* file;
    FILE* record;

  if (argc < 4)
  {
    printf("The correct call of this program is:\n\n");
    printf("   ./Package name sources.txt noise_seed\n");
    KILL("Please try again.\n");
  }

  if ((sourcefile = fopen(argv[2],"r")) == NULL)
  {
    sprintf(ErrorMessage, "Cannot open the source file\n");
    KILL(ErrorMessage);
  }

  k = atoi(argv[3]);

  if(k < 0) k *= -1;

    strcpy(Name,argv[1]);
    sprintf(outputname, "XML/%s.xml", Name);
    sprintf(binaryname, "Binary/%s.bin", Name);


  X_TDI = (double*) malloc (NFFT*sizeof(double));
  Y_TDI = (double*) malloc (NFFT*sizeof(double));
  Z_TDI = (double*) malloc (NFFT*sizeof(double));

  if (!X_TDI || !Y_TDI || !Z_TDI) KILL("Out of memory.\n");

  for (i = 0 ; i < NFFT ; i++)
  {
    X_TDI[i] = 0.0;
    Y_TDI[i] = 0.0;
    Z_TDI[i] = 0.0;
  }

  /* we will keep a record of what sources and noise went into the package */
  sprintf(recordname, "Data/%s.txt", Name);
  record = fopen(recordname,"w");


  /* add up the signals from all the sources */

  /* flip the sign to correct from Rubbo, Cornish & Poujade convention to
     the Cornish & Hellings convention which also agrees with the JPL
     group and Synthetic LISA */

  while (!feof(sourcefile))
     {
      fscanf(sourcefile, "%s\n", &tag);

      if(*tag != '\0')
	{

      printf("reading in source %s\n", tag);

      fprintf(record,"%s\n", tag);

      sprintf(sourcename, "Binary/X_%s.bin", tag);
      if ((file = fopen(sourcename,"rb")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the file %s\n", sourcename);
        KILL(ErrorMessage);
       }
      for (i = 0 ; i < NFFT ; i++)
       {
        fread(&tmp, sizeof(double), 1, file);
        X_TDI[i] -= tmp;
       }
      fclose(file);

      sprintf(sourcename, "Binary/Y_%s.bin", tag);
      if ((file = fopen(sourcename,"rb")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the file %s\n", sourcename);
        KILL(ErrorMessage);
       }
      for (i = 0 ; i < NFFT ; i++)
       {
        fread(&tmp, sizeof(double), 1, file);
        Y_TDI[i] -= tmp;
       }
      fclose(file);

      sprintf(sourcename, "Binary/Z_%s.bin", tag);
      if ((file = fopen(sourcename,"rb")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the file %s\n", sourcename);
        KILL(ErrorMessage);
       }
      for (i = 0 ; i < NFFT ; i++)
       {
        fread(&tmp, sizeof(double), 1, file);
        Z_TDI[i] -= tmp;
       }
      fclose(file);

	}

     }

    fclose(sourcefile);

  

  /* add in the noise */

      fprintf(record,"Noise %d\n", k);

      fclose(record);

      /* noise seed = 0 means no instrument noise is added */

   if(k != 0)
     {

      printf("reading in noise files %d\n", k);

      sprintf(sourcename, "Binary/XNoise_%d.bin", k);
      if ((file = fopen(sourcename,"rb")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the file %s\n", sourcename);
        KILL(ErrorMessage);
       }
      for (i = 0 ; i < NFFT ; i++)
       {
        fread(&tmp, sizeof(double), 1, file);
        X_TDI[i] += tmp;
       }
      fclose(file);

      sprintf(sourcename, "Binary/YNoise_%d.bin", k);
      if ((file = fopen(sourcename,"rb")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the file %s\n", sourcename);
        KILL(ErrorMessage);
       }
      for (i = 0 ; i < NFFT ; i++)
       {
        fread(&tmp, sizeof(double), 1, file);
        Y_TDI[i] += tmp;
       }
      fclose(file);

      sprintf(sourcename, "Binary/ZNoise_%d.bin", k);
      if ((file = fopen(sourcename,"rb")) == NULL)
       {
        sprintf(ErrorMessage, "Cannot open the file %s\n", sourcename);
        KILL(ErrorMessage);
       }
      for (i = 0 ; i < NFFT ; i++)
       {
        fread(&tmp, sizeof(double), 1, file);
        Z_TDI[i] += tmp;
       }
      fclose(file);

     }

      

    /* now write all the data channels and the times to a single file */

      file = fopen(binaryname,"wb");

      t = dt/2.0;

      for (i = 0 ; i < NFFT ; i++)
       {
        fwrite(&t, sizeof(double), 1, file);
        fwrite(&X_TDI[i], sizeof(double), 1, file);
        fwrite(&Y_TDI[i], sizeof(double), 1, file);
        fwrite(&Z_TDI[i], sizeof(double), 1, file);
        t += dt;
       }

      fclose(file);
      free(X_TDI);
      free(Y_TDI);
      free(Z_TDI);

      /* now write the xml wrapper */

    myxml = XMLopen(outputname);

    /* Standard beginning of the file */

    XMLcontentstring(myxml,"<?xml version=\"1.0\"?>");
    XMLcontentstring(myxml,"<!DOCTYPE XSIL SYSTEM \"http://www.vallis.org/lisa-xml.dtd\">");

    XMLcontentstring(myxml,"<?xml-stylesheet type=\"text/xsl\" href=\"http://www.vallis.org/lisa-xml.xsl\"?>");

    XMLopentag(myxml,"XSIL","");

        /* Prolog */

        XMLopentag(myxml,"Param","Name=\"Author\"");
            XMLcontentstring(myxml,"Neil Cornish");
        XMLclosetag(myxml,"Param");
    
        XMLopentag(myxml,"Param","Name=\"GenerationDate\" Type=\"ISO-8601\"");
            getISOtime(buffer);
            XMLcontentstring(myxml,buffer);
        XMLclosetag(myxml,"Param");

        XMLopentag(myxml,"Comment","");
            XMLcontentstring(myxml,"This file produced by LISA Simulator v. 2.1.1 Version Date June 22, 2006");
            XMLcontentstring(myxml,"(c) 2006 Neil Cornish, Montana State University");
        XMLclosetag(myxml,"Comment");

        /* LISAData section */

        XMLopentag(myxml,"XSIL","Type=\"LISAData\"");

        XMLopentag(myxml,"XSIL","Name=\"LISA\" Type=\"PseudoLISA\"");

        XMLparamdouble(myxml,"TimeOffset","Seconds",0.0);

        XMLparamdouble(myxml,"InitialPosition","Radian",kappa);

        XMLparamdouble(myxml,"InitialRotation","Radian",lambda);

        XMLparamdouble(myxml,"Armlength","Seconds",L/c);

        XMLclosetag(myxml,"XSIL");

        XMLclosetag(myxml,"XSIL");

        /* TDI Data section */

        XMLopentag(myxml,"XSIL","Type=\"TDIData\"");
            XMLopentag(myxml,"XSIL","Name=\"%s\" Type=\"TDIObservable\"",Name);

	    XMLcontentstring(myxml,"<Param Name=\"DataType\">");
	    XMLcontentstring(myxml,"Strain</Param>");

            XMLopentag(myxml,"XSIL","Name=\"%s\" Type=\"TimeSeries\"","t,Xs,Ys,Zs");

                XMLparamdouble(myxml,"TimeOffset","Second",0.0);
                XMLparamdouble(myxml,"Cadence","Second",dt);
                XMLparamdouble(myxml,"Duration","Second",T);

                    if (testbyteorder() == BIGENDIAN) {
	   	    XMLarray(myxml,"t,Xs,Ys,Zs","double","1",NFFT,4,"Remote","Binary,BigEndian",binaryname);
                    } else {
                   XMLarray(myxml,"t,Xs,Ys,Zs","double","1",NFFT,4,"Remote","Binary,LittleEndian",binaryname);
                    }

            XMLclosetag(myxml,"XSIL");

            XMLclosetag(myxml,"XSIL");
            
        XMLclosetag(myxml,"XSIL");

    XMLclosetag(myxml,"XSIL");

    XMLclose(myxml);

    exit(0);
}

void KILL(char* Message)
{
  printf("\a\n");
  printf(Message);
  printf("Terminating the program.\n\n\n");
  exit(1);

 
  return;
}
