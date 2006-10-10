/* lisatools/lisaXML/C-examples/xml2ascii.c --- Copyright (c) 2006 Michele Vallisneri

   $Id$

   In the directory lisatools/lisaXML/C-examples, compile with
   gcc xml2ascii.c ../io-C/readxml.c ../io-C/ezxml.c -I.. -o xml2ascii */

#include <stdio.h>
#include <stdlib.h>

#include "io-C/readxml.h"

int main(int argc,char **argv) {
    TimeSeries *timeseries;

    FILE *outfile;

    int i,j;

    if(argc < 2) {
        printf("Usage: %s source.xml [output.txt]\n",argv[0]);
        exit(0);
    }

    if(argc < 3) {
        outfile = stdout;
    } else {
        outfile = fopen(argv[2],"w");
    }

    timeseries = getTDIdata(argv[1]);

    fprintf(stderr,"-> Read TimeSeries '%s' from file '%s':\n",timeseries->Name,timeseries->FileName);
    fprintf(stderr,"-> %d x %d values (%s)\n",timeseries->Length,timeseries->Records,timeseries->Name);
    
    for(i=0;i<timeseries->Records;i++) {
        fprintf(stderr,"-> column %d: %s\n",i,timeseries->Data[i]->Name);
    }

    if(argc > 2) {
        fprintf(outfile,"\n----------\n");
    }

    for(j=0;j<timeseries->Length;j++) {
        for(i=0;i<timeseries->Records;i++) {
            fprintf(outfile,"%.16g",timeseries->Data[i]->data[j]);
            
            if(i+1 < timeseries->Records) {
                fprintf(outfile,"\t");
            } else {
                fprintf(outfile,"\n");
            }
        }
    }

    freeTimeSeries(timeseries);
    
    if(argc > 2) {
        fclose(outfile);
    }
}
