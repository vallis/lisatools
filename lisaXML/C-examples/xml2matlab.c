/* lisatools/lisaXML/C-examples/xml2matlab.c --- Copyright (c) 2006 Michele Vallisneri

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
   
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE. */

#include "mex.h" 
#include "io-C/readxml.h"

/* MATLAB syntax: array = xml2matlab('file.xml')
   Compile with: mex xml2matlab.c ../io-C/readxml.c ../io-C/ezxml.c -I..

   This little MEX program will retrieve the first TDIObservable section in a lisaXML file
   as a MATLAB n x m array, where m is the number of observables on each column, and n
   is the number of samples. It will also print more information about the time series,
   such as the cadence, time offset, and name of the observables.

   It will not fail gracefully if anything goes wrong, but code to do that can be added
   easily. Code can also be added to return the time series information mentioned above.
   Please contact vallis@vallis.org if you have improvements to this code, or feature
   requests. */
 
static mxArray *parsefile(char *filename) {
    TimeSeries *timeseries;
    
    mxArray *newmatrix;
    double *newdata;

    int i,j;

    timeseries = getTDIdata(filename);

    mexPrintf("Reading TimeSeries '%s' from file '%s':\n",timeseries->Name,timeseries->FileName);
    mexPrintf("-> %d x %d values\n",timeseries->Length,timeseries->Records);
    mexPrintf("-> cadence = %g s, time offset = %g s\n",timeseries->TimeOffset,timeseries->Cadence);

    mexPrintf("-> columns = ");
    for(i=0;i<timeseries->Records;i++) {
        mexPrintf("%s",timeseries->Data[i]->Name);
        if(i<timeseries->Records-1) {
            mexPrintf(", ");
        } else {
            mexPrintf("\n");
        }
    }

    newmatrix = mxCreateDoubleMatrix(timeseries->Length,timeseries->Records,mxREAL); 
    newdata = mxGetPr(newmatrix); 

    for(i=0;i<timeseries->Records;i++) {
        for(j=0;j<timeseries->Length;j++) {
            newdata[i*(timeseries->Length)+j] = timeseries->Data[i]->data[j];
        }
    }
        
    freeTimeSeries(timeseries);
    
    return newmatrix;
}
 
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) { 
    int buflen;
    char *input_buf;
  
    if(nrhs != 1) { 
        mexErrMsgTxt("One input required."); 
    } else if(nlhs > 1) { 
        mexErrMsgTxt("Too many output arguments"); 
    } 
  
    if(mxIsChar(prhs[0]) != 1) 
        mexErrMsgTxt("Input must be a string.");
  
    if(mxGetM(prhs[0]) != 1) 
      mexErrMsgTxt("Input must be a row vector."); 

    buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;
    input_buf = mxCalloc(buflen, sizeof(char)); 
    mxGetString(prhs[0], input_buf, buflen);

    plhs[0] = parsefile(input_buf);
    
    return;
}
