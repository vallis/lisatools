/* lisatools/io/lisaxml.c --- Copyright (c) 2006 Michele Vallisneri

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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define XMLSTDINDENT 4

typedef struct {
    FILE *file;
    int indent;
} writeXML;


writeXML *XMLopen(char *filename) {
    writeXML *xmlfile;
    
    xmlfile = (writeXML*)malloc(sizeof(writeXML));
    
    xmlfile->file = fopen(filename, "w");
    xmlfile->indent = 0;
    
    return xmlfile;
}

void XMLclose(writeXML *xmlfile) {
    fclose(xmlfile->file);
    free(xmlfile);
}

void XMLindent(writeXML *xmlfile) {
    int i;
    
    for(i=0;i<xmlfile->indent;i++)
        fprintf(xmlfile->file," ");
}

void XMLopentag(writeXML *xmlfile,char *tag,char *attrtemplate,...) {
    va_list attrs;

    XMLindent(xmlfile);

    if(strlen(attrtemplate) > 0) {
        fprintf(xmlfile->file,"<%s ",tag);

        va_start(attrs,attrtemplate);
        vfprintf(xmlfile->file,attrtemplate,attrs);
        va_end(attrs);    
        
        fprintf(xmlfile->file,">\n");
    } else {
        fprintf(xmlfile->file,"<%s>\n",tag);    
    }

    xmlfile->indent += XMLSTDINDENT;    
}

void XMLclosetag(writeXML *xmlfile,char *tag) {
    xmlfile->indent -= XMLSTDINDENT;

    XMLindent(xmlfile);

    fprintf(xmlfile->file,"</%s>\n",tag);
}

void XMLsingletag(writeXML *xmlfile,char *tag,char *attrtemplate,...) {
    va_list attrs;

    XMLindent(xmlfile);

    if(strlen(attrtemplate) > 0) {
        fprintf(xmlfile->file,"<%s ",tag);
    
        va_start(attrs,attrtemplate);
        vfprintf(xmlfile->file,attrtemplate,attrs);
        va_end(attrs);    
        
        fprintf(xmlfile->file,"/>\n");
    } else {
        fprintf(xmlfile->file,"<%s>\n",tag);
    }
}

void XMLcontentstring(writeXML *xmlfile,char *content) {
    XMLindent(xmlfile);
    
    fprintf(xmlfile->file,"%s\n",content);
}

void XMLcontentdouble(writeXML *xmlfile,double content) {
    XMLindent(xmlfile);
    
    fprintf(xmlfile->file,"%.16g\n",content);
}

/* the following are shorthand for XMLopentag, XMLcontent..., XMLclosetag
   for the Param element */

void XMLparamstring(writeXML *xmlfile,char *name,char *unit,char *value) {
    XMLindent(xmlfile);
    
    fprintf(xmlfile->file,"<Param Name=\"%s\" Unit=\"%s\">",name,unit);
    fprintf(xmlfile->file,"%s",value);
    fprintf(xmlfile->file,"</Param>\n");
}

void XMLparamdouble(writeXML *xmlfile,char *name,char *unit,double value) {
    XMLindent(xmlfile);
    
    fprintf(xmlfile->file,"<Param Name=\"%s\" Unit=\"%s\">",name,unit);
    fprintf(xmlfile->file,"%.16g",value);
    fprintf(xmlfile->file,"</Param>\n");
}

void XMLdimlong(writeXML *xmlfile,char *name,long value) {
    XMLindent(xmlfile);
    
    fprintf(xmlfile->file,"<Dim Name=\"%s\">",name);
    fprintf(xmlfile->file,"%d",value);
    fprintf(xmlfile->file,"</Dim>\n");
}

void XMLarray(writeXML *xmlfile,char *name,char *type,char *unit,long length,int records,char *stype,char *code,char *file) {
    XMLopentag(xmlfile,"Array","Name=\"%s\" Type=\"%s\" Unit=\"%s\"",name,type,unit);

        XMLdimlong(xmlfile,"Length",length);
        XMLdimlong(xmlfile,"Records",records);
    
        XMLopentag(xmlfile,"Stream","Type=\"%s\" Encoding=\"%s\"",stype,code);
            XMLcontentstring(xmlfile,file);
        XMLclosetag(xmlfile,"Stream");

    XMLclosetag(xmlfile,"Array");
}

void getISOtime(char *buffer) {
    time_t clocktime;
    struct tm mytime;

    time(&clocktime);
    localtime_r(&clocktime,&mytime);            
    strftime(buffer,256,"%Y-%m-%dT%H:%M:%S%Z",&mytime);
}
