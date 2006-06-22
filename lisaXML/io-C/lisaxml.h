/* lisatools/io/lisaxml.h --- Copyright (c) 2006 Michele Vallisneri

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

/* This structure represents a lisaXML file */

typedef struct {
    FILE *file;
    int indent;
} writeXML;

/* Open a lisaXML file... */

writeXML *XMLopen(char *filename);

/* Close it (reopening will erase contents). */

void XMLclose(writeXML *xmlfile);

/* Used internally, but basically writes out enough spaces to
   indent to the current indentation level. */

void XMLindent(writeXML *xmlfile);

/* Write an "open element" combination to a lisaXML file, such as

   <Param Name="Inclination" Unit="Radian">
   
   and then indent.
   
   Here "Param" is the tag, and the element attributes ("Name" and
   "Unit") are passed using a printf-like style:
   
   XMLopentag(xmlfile,"Param","Name=\"%s\" Unit=\"%s\"",
                      "Inclination","Radian");

   Of course you can also do
   
   XMLopentag(xmlfile,"Param","Name=\"Inclination\" Unit=\"Radian\"")   

   If the element has no attributes, simply pass "" for attrtemplate. */ 

void XMLopentag(writeXML *xmlfile,char *tag,char *attrtemplate,...);

/* Write a "close element" combination, such as "</Param>". */

void XMLclosetag(writeXML *xmlfile,char *tag);

/* Write a "singleton" combination, such as "<br/>" (can have    
   attributes, with syntax as for XMLopentag). */

void XMLsingletag(writeXML *xmlfile,char *tag,char *attrtemplate,...);

/* The following two calls write content (respectively a string or a
   double) at the current level of indentation. Useful between an
   XMLopentag and an XMLclosetag. */

void XMLcontentstring(writeXML *xmlfile,char *content);

void XMLcontentdouble(writeXML *xmlfile,double content);

/* the following are shorthand for XMLopentag, XMLcontent, and
   XMLclosetag for the Param element. Param Name, Unit, and the
   value are passed explicitly. If Unit is not provided, or if
   Type is needed, use XMLopentag, etc., or write your own shorthand. */

void XMLparamstring(writeXML *xmlfile,char *name,char *unit,char *value);

void XMLparamdouble(writeXML *xmlfile,char *name,char *unit,double value);

/* shorthand for XMLopentag, XMLcontent, XMLclosetag for the Dim element */

void XMLdimlong(writeXML *xmlfile,char *name,long value);

/* shorthand for XMLopentag, XMLcontent, XMLclosetag for the Array element */

void XMLarray(writeXML *xmlfile,char *name,char *type,char *unit,long length,int records,char *stype,char *code,char *file);

/* Fills the string pointed to by buffer with the current local
   ISO-8601 time. Does not check buffer overflow. */

void getISOtime(char *buffer);
