%module FastBinary
%{
#include "FastBinary.h"
%}

// the following three lines allow the automatic conversion of a numpy array
// into a pointer to an array of doubles and the long length of the array
// each pair of these that appears in the library header file (e.g., BBHChallenge1.hh)
// needs to be named explicitly

%include numpy_typemaps.i
%typemap(in) (double *XLS,long XLSlen) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *XSL,long XSLlen) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *YLS,long YLSlen) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *YSL,long YSLlen) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *ZLS,long ZLSlen) = (double *numpyarray,long numpyarraysize);
%typemap(in) (double *ZSL,long ZSLlen) = (double *numpyarray,long numpyarraysize);

// the following include defines all the calls that need to be wrapped by SWIG
// the include at the top serves only to provide definitions for this one;
// since it is between braces %{ %}, its content goes straight into
// the CPP wrapper file

%include "FastBinary.h"

// and here we can add some Python to the interface code

%pythoncode %{
import lisaxml
# import lisaxml2 as lisaxml

import numpy
import FrequencyArray
import math

lisaxml.SourceClassModules['FastGalacticBinary'] = 'FastBinary'

# constants...

year = 3.15581498e7 # year in seconds
T = 6.2914560e7     # two years in seconds

fstar = 0.00954269032 # LISA transfer frequency

# define the interface class

class FastGalacticBinary(lisaxml.Source):
    outputlist = ( ('Frequency',                  'Hertz',     None, 'GW frequency'),
                   ('FrequencyDerivative',        'Hertz/s',   None, 'GW frequency derivative'),
                   ('EclipticLatitude',           'Radian',    None, 'standard ecliptic latitude'),
                   ('EclipticLongitude',          'Radian',    None, 'standard ecliptic longitude'),
                   ('Amplitude',                  '1',         None, 'dimensionless GW amplitude'),
                   ('Inclination',                'Radian',    None, 'standard source inclination'),
                   ('Polarization',               'Radian',    None, 'standard source polarization'),
                   ('InitialPhase',               'Radian',    None, 'GW phase at t = 0') )
    
    FastBinaryCache = {}
    
    def __init__(self,name=''):
        super(FastGalacticBinary, self).__init__('FastGalacticBinary',name)
        
    def fourier(self):
        if T/year <= 1.0:
            mult = 1
        elif T/year <= 2.0:
            mult = 2
        elif T/year <= 4.0:
            mult = 4
        else:
            mult = 8
        
        if self.Frequency > 0.1:
            N = 1024*mult
        elif self.Frequency > 0.03:
            N = 512*mult
        elif self.Frequency > 0.01:
            N = 256*mult
        elif self.Frequency > 0.001:
            N = 64*mult
        else:
            N = 32*mult            
        
        Sm = AEnoise(self.Frequency) / (4.0 * math.sin(self.Frequency/fstar) * math.sin(self.Frequency/fstar))
        Acut = self.Amplitude * math.sqrt(T/Sm)
        
        M = int(math.pow(2.0,1 + int(math.log(Acut)/math.log(2.0))));
        
        # corrected per Neil's 2009-07-28 changes to Fast_Response3.c
        # logic is a little funny
        
        if M < N:
            M = N
        elif N < M:
            N = M
        
        if M > 8192:
            M = 8192
            N = M
        
        if (N,M) not in self.FastBinaryCache:
            self.FastBinaryCache[(N,M)] = FastResponse(N,M)
            
        fastbin = self.FastBinaryCache[(N,M)]
        
        # TO DO: allow user to choose LS or SL
        XLS = numpy.zeros(2*M,'d'); XSL = numpy.zeros(2*M,'d')
        YLS = numpy.zeros(2*M,'d'); YSL = numpy.zeros(2*M,'d')
        ZLS = numpy.zeros(2*M,'d'); ZSL = numpy.zeros(2*M,'d')        
        
        fastbin.Response(self.Frequency,self.FrequencyDerivative,0.5*math.pi - self.EclipticLatitude,self.EclipticLongitude,
                         self.Amplitude,self.Inclination,self.Polarization,self.InitialPhase,
                         XLS,XSL,YLS,YSL,ZLS,ZSL)
        
        # whatever's returned by Response, is then added to a frequency array as in
        # 
        # q = (long)(f*T);
        # 
        # for(i=1; i<=M; i++) {                             for(i=1; i<NFFT/2; i++) {
        #     k = (q + i - 1 - M/2);                            out[i][0] = XfLS[2*i];      // real part in 2, 4, 6...
        #                                                       out[i][1] = -XfLS[2*i+1];   // imaginary part
    	#     XfSL[2*k] += XSL[2*i-1]; // real in 1, 3...   }
    	#     XfSL[2*k+1] += XSL[2*i]; // -imag                    
        # }                                                 out[0][0] = out[0][1] = 0;        
        #
        # so i = 1 goes into k = q - M/2, hence the frequency of the first sample is f - (M/2) df 
        
        retX, retY, retZ = map(lambda a: FrequencyArray.FrequencyArray(a[::2] - 1j * a[1::2],   # again, empirically found...
                                                                       dtype=numpy.complex128,
                                                                       kmin = int(self.Frequency*T) - M/2,df = 1.0/T),
                               (XSL, YSL, ZSL))
        
        # for some reason Neil computes the conjugate of the FFT? or is it a question of how he uses FFTW?
        
        # figure out how to do the FFTW fftw_plan_dft_c2r_1d FFT
        # plan_reverse = fftw_plan_dft_c2r_1d(NFFT, out, in, FFTW_ESTIMATE);
        # so: out seems to be a vector of positive-frequency components (complex fftw_complex types), of length NFFT/2 + 1
        #     in is a vector of real time-domain components, of length NFFT
        #     note that in and out are reversed in Neil's call, but c2r calls are always FFTW_BACKWARD
        
        return (retX,retY,retZ)
    
    # this still needs changing...
    def waveforms(self,samples,deltat,inittime):
        bbh = BBHChallenge1(self.Mass1,self.Mass2)
        
        bbh.SetInitialOrbit(self.CoalescenceTime,self.InitialAngularOrbitalPhase)
        
        # note that the last parameter ComputeInspiral is the final time...
        # so to get "samples" values we need to step back one deltat
         
        bbh.ComputeInspiral(inittime,deltat,inittime + deltat*(samples-1))
        
        # note that BBHChallenge uses a "ThetaInclination", not a "IotaInclination"
        
        bbh.SetObserver(math.pi-self.Inclination,self.Distance)
        
        hp = numpy.empty(samples,'d')
        hc = numpy.empty(samples,'d')
        
        wavelen = bbh.ComputeWaveform(self.TruncationTime,self.TaperApplied,hp,hc)
        
        hp[wavelen:] = 0.0
        hc[wavelen:] = 0.0
        
        return (hp,hc)
    
%}
