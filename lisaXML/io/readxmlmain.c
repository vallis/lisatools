#include <stdio.h>

#include "readxml.h"

int main(int argc,char **argv) {
    TimeSeries *timeseries;
    LISASource *lisasources, *lisasource;
    
    int i;

    timeseries = getTDIdata("data/gb.xml");

    printf("'%s': %d x %d doubles (every %g seconds); ",timeseries->Name,
                                                        timeseries->Records,
                                                        timeseries->Length,
                                                        timeseries->Cadence);

    printf("data in '%s'\n",timeseries->FileName);
    
    for(i=0;i<timeseries->Records;i++)
        printf("'%s': %g (first) %g (last)\n",timeseries->Data[i]->Name,
                                              timeseries->Data[i]->data[0],
                                              timeseries->Data[i]->data[timeseries->Data[i]->Length-1]);

    freeTimeSeries(timeseries);
    
    lisasources = getLISASources("data/bbh.xml");
    lisasource = lisasources;
        
    while(lisasource) {
        printf("'%s': %d doubles (every %g seconds); \n",lisasource->Name,
                                                         lisasource->TimeSeries->Length,
                                                         lisasource->TimeSeries->Cadence);

        for(i=0;i<lisasource->TimeSeries->Records;i++)
            printf("'%s': %g (first) %g (last)\n",lisasource->TimeSeries->Data[i]->Name,
                                                  lisasource->TimeSeries->Data[i]->data[0],
                                                  lisasource->TimeSeries->Data[i]->data[lisasource->TimeSeries->Data[i]->Length-1]);
  
        lisasource = lisasource->NextSource;
    }
    
    freeLISASources(lisasources);
}
