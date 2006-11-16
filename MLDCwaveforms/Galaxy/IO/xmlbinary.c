/* lisatools/io/readxml.c --- Copyright (c) 2006 Neil Cornish

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

#include <stdlib.h>
#include <assert.h>

#include "ezxml.h"
#include "readxml.h"
#include "xmlbinary.h"

GBParameters *getGBParameters(char *filename) {
    ezxml_t tree, section, source, param, series;
    const char *type, *name, *elat, *elon, *pol, *amp, *incl, *iphase, *freq;

    GBParameters *first = 0;

    tree = ezxml_parse_file(filename);
    assert(tree);

    for(section = ezxml_child(tree, "XSIL"); section; section = section->next) {
        type = ezxml_attr(section, "Type");
        
        if(!strcmp(type,"SourceData")) {
            for(source = ezxml_child(section,"XSIL"); source; source = source->next) { 
                assert(!strcmp(ezxml_attr(source,"Type"),"PlaneWave"));
                /* Expected PlaneWave here... */

                name = ezxml_attr(source,"Name");

                for(param = ezxml_child(source,"Param"); param; param = param->next) {
                    if(!strcmp(ezxml_attr(param,"Name"),"Polarization")) {
                        pol = ezxml_txt(param);
                        assert(pol);
                    }

                    if(!strcmp(ezxml_attr(param,"Name"),"Inclination")) {
                        incl = ezxml_txt(param);
                        assert(incl);
                    }

                    if(!strcmp(ezxml_attr(param,"Name"),"EclipticLatitude")) {
                        elat = ezxml_txt(param);
                        assert(elat);
                    }
            
                    if(!strcmp(ezxml_attr(param,"Name"),"EclipticLongitude")) {
                        elon = ezxml_txt(param);
                        assert(elon);
                    }
                        
                    if(!strcmp(ezxml_attr(param,"Name"),"Amplitude")) {
                        amp = ezxml_txt(param);
                        assert(amp);
                    }

                    if(!strcmp(ezxml_attr(param,"Name"),"InitialPhase")) {
                        iphase = ezxml_txt(param);
                        assert(iphase);
                    }

                    if(!strcmp(ezxml_attr(param,"Name"),"Frequency")) {
                        freq = ezxml_txt(param);
                        assert(freq);
                    }
                }

                first = malloc(sizeof(GBParameters));
                assert(first != 0);

                first->Name = stripcopy(name);

                first->EclipticLatitude  = strtod(elat,NULL); 
                first->EclipticLongitude = strtod(elon,NULL);                 
                first->Polarization      = strtod(pol,NULL);
                first->Amplitude         = strtod(amp,NULL);
                first->Inclination       = strtod(incl,NULL);
                first->InitialPhase      = strtod(iphase,NULL);
                first->Frequency         = strtod(freq,NULL);

            }
        }
    }

    ezxml_free(tree);
    
    return first;
}


void freeGBParameters(GBParameters *binary) {
    free(binary->Name);
    
    free(binary);
}
