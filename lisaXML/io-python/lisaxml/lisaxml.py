__version__='$Id$'

import pyRXP
import xmlutils

import numpy

import convertunit

import sys
import os.path
import time
import re

# somehow I will have to compile this directory of modules by source...

# if (say) MLDC defines SourceClassModules and then imports BBH,
# it's enough to 

# to import files with __import__, can then get their names from module.__name__
# and reassign the module to that name... perhaps
# module = __import__(modulename)
# exec(module.__name__ + '= module') or exec(modulename + '= module')
# or even
# exec(modulename + ' = __import__(modulename)')
# this seems most concise...
# ...but I don't like using exec...

SourceClassModules = {}

SourceClassModules['BlackHoleBinary'] = 'BBH'
SourceClassModules['ExtremeMassRatioInspiral'] = 'EMRI'

# could use something here to return a default unit if it is not set...
# but we would have to look in the source-specific list as well as
# a general list...

class XMLobject(object):
    def __init__(self):
        self.__dict__['parameters'] = []
    
    def __setattr__(self,attr,value):
        self.__dict__[attr] = value
        
        if (not attr in self.parameters) and (not '_Unit' in attr):
            self.parameters.append(attr)
    

class TimeSeries(XMLobject):
    def __init__(self,arrays,name='',unit=''):
        super(TimeSeries,self).__init__()
        
        # if we get only one array, make a tuple        
        if not isinstance(arrays,list) and not isinstance(arrays,tuple):
            self.Arrays = (arrays,)
        else:
            self.Arrays = arrays
        
        self.Records = len(self.Arrays)
        self.Length  = len(self.Arrays[0])
        
        self.__dict__['name'] = name
        self.__dict__['unit'] = unit
    
    def XML(self,filename,encoding='Binary'):
        buffer = numpy.zeros([self.Length,self.Records],'d')
        
        for i in range(self.Records):
            buffer[:,i] = self.Arrays[i]
        
        if encoding == 'Binary':
            bfile = open(filename,'w')
            bfile.write(buffer.tostring())              
            bfile.close()
            
            Stream = ('Stream',
                      {'Type': 'Remote', 'Encoding': sys.byteorder == 'big' and 'Binary,BigEndian' or 'Binary,LittleEndian'},
                      [os.path.basename(filename)])
        else:
            textdata = ''
            linepattern = '%le ' * self.Records + '\n'
            for i in range(0,self.Length):
                textdata += linepattern % tuple(buffer[i,:])
            
            Stream = ('Stream',
                      {'Type': 'Local', 'Encoding': 'Text', 'Delimiter': ' '},
                      [textdata] )
        
        Array = ('Array',
                 {'Name': self.name, 'Type': 'double', 'Unit': self.unit},
                 [ ('Dim', {'Name': 'Length' }, [self.Length]),
                   ('Dim', {'Name': 'Records'}, [self.Records]),
                   Stream ] )
        
        # watch the definition of duration (it is just length * timestep) 
        
        TimeSeries = ('XSIL',
                      {'Name': self.name, 'Type': 'TimeSeries'},
                      [ ('Param', {'Name': 'TimeOffset', 'Unit': 'Second'}, [self.TimeOffset]),
                        ('Param', {'Name': 'Cadence',    'Unit': 'Second'}, [self.Cadence]),
                        ('Param', {'Name': 'Duration',   'Unit': 'Second'}, [self.Length * self.Cadence]),
                        Array ] )
        
        return TimeSeries
    

class Observable(XMLobject):
    def __init__(self,name=''):
        super(Observable,self).__init__()
        
        self.__dict__['name'] = name
    
    def XML(self,xmlfile,name=None,comments=''):
        """Add a TDI observable section to an XML output file."""
        
        xsildict = {}
        params = []
        
        xsildict['Type'] = 'TDIObservable'
        xsildict['Name'] = name and name or self.name
        
        if hasattr(self,'DataType'):
            params.append( ('Param',{'Name': 'DataType'},[self.DataType]) )
        
        # handle TimeSeries
        
        if hasattr(self,'TimeSeries'):
            params.append(self.TimeSeries.XML(re.sub('\.xml$','',xmlfile.filename) + '-' + str(xmlfile.binaryfiles) + '.bin'))
            
            xmlfile.binaryfiles += 1
        
        return ('XSIL', xsildict, params)
    

class Source(XMLobject):
    def __init__(self,sourcetype,name=''):
        super(Source,self).__init__()
        
        # avoid calling setattr
        self.__dict__['xmltype'] = sourcetype
        self.__dict__['name'] = name
    
    def parstr(self,attr):
        value = getattr(self,attr)
        
        try:
            unit = getattr(self,attr+'_Unit')
        except AttributeError:
            unit = 'default'
        
        return "%s (%s)" % (value,unit)    
    
    def XML(self,xmlfile,name=None,comments=''):
        """Add a Source-like object to an XML output file."""
        
        xsildict = {}
        params = []
        
        # handle the source name and type
        # name passed as argument, if any, will override constructor name
        
        xsildict['Type'] = 'PlaneWave'
        xsildict['Name'] = name and name or self.name
        
        params.append( ('Param',{'Name': 'SourceType'},[self.xmltype]) )
        
        # get a list of all arguments, and attribute units
        # run through outputlist, and try to make all of them
        
        # remember, args in outputlist are tuples (argname,unit,defaultvalue)
        
        for arg in self.outputlist:
            # see if argument was assigned to object
            # if not, try to convert argument from other arguments  
            # if that does not work, try to use default value
            
            if hasattr(self,arg[0]):
                argvalue = getattr(self,arg[0])
                
                if hasattr(self,arg[0] + '_Unit'):
                    argvalue = (argvalue,getattr(self,arg[0] + '_Unit'))
            else:
                argvalue = convertunit.convertParam(self,arg)
                
                if argvalue == None and arg[2] != None:
                    argvalue = arg[2]
                else:
                    raise AttributeError, 'Source.XML(): missing parameter %s in object %s' % (arg[0],self)
            
            # now check if the parameter already has a unit
            # and convert it to the standard unit if necessary
            # if not, set it from the default unit
            
            # (note: convertParam and convertUnit return argument with unit as tuple)
            
            if isinstance(argvalue,tuple):
                if argvalue[1] != arg[1]:
                    argvalue = convertunit.convertUnit(argvalue[0],argvalue[1],arg[1],arg[0])
            else:
                argvalue = (argvalue,arg[1])
            
            params.append(('Param', {'Name': arg[0], 'Unit': argvalue[1]}, [argvalue[0]]))
        
        # handle extra parameters (those not in outputlist), but only if Unit was given explicitly!
        
        for parname in self.parameters:
            if not parname in [arg[0] for arg in self.outputlist] and hasattr(self,parname + '_Unit'):
                params.append(('Param', {'Name': parname, 'Unit': getattr(self,parname + '_Unit')}, [getattr(self,parname)]))
        
        # handle TimeSeries
        
        if hasattr(self,'TimeSeries'):
            xsildict['Type'] = 'SampledPlaneWave'
            
            params.append(self.TimeSeries.XML(re.sub('\.xml$','',xmlfile.filename) + '-' + str(xmlfile.binaryfiles) + '.bin'))
            
            xmlfile.binaryfiles += 1
        
        # handle comments
        
        if comments:
            params.append(self.doComment(comments))
        
        # we're done...
        
        return ('XSIL', xsildict, params)
    

class writeXML(object):
    def __init__(self,filename):
        if filename:
            # add ".xml" to filename if necessary
            if not re.match('.*\.xml$',filename):
                filename += '.xml'
            
            self.f = open(filename,'w')
            self.opened = 1
        else:
            self.f = sys.stdout
            self.opened = -1
    
    def __del__(self):
        if self.opened == 1:
            self.close()
    
    def close(self):
        if self.opened == 1:
            self.f.close()
            self.opened = 0
    
    # handling of XML indentation
    stdindent = 4
    indent = ""  
    def incind(self):
        self.indent += " " * self.stdindent
    
    def decind(self):
        self.indent = self.indent[0:len(self.indent)-self.stdindent]
    
    def iprint(self,s):
        if self.opened != 0 and len(s) > 0:
            print >> self.f, self.indent + s,
            if s[-1] != '\n':
                print >> self.f
    
    def doattrs(self,attrs):
        string = ''
        
        if len(attrs) > 0:
            # always put 'Name' first
            
            if 'Name' in attrs.keys():
                string += ' Name="' + attrs['Name'] +'"'
            
            for attr in attrs.keys():
                if attr != 'Name':
                    string += ' ' + attr + '="' + str(attrs[attr]) + '"'
        
        return string
    
    def opentag(self,tag,attrs):
        """Open an XML element; take dictionary for attributes""" 
        
        string = '<' + tag + self.doattrs(attrs) + '>'
        
        self.iprint(string)
        self.incind()
    
    def singletag(self,tag,attrs):
        """Do an XML singleton; take dictionary for attributes"""
        
        string = '<' + tag + self.doattrs(attrs) + '/>'
        
        self.iprint(string)
    
    def coupletag(self,tag,attrs,thevalue):
        """Do inline XML open/close tag"""
        
        string = '<' + tag + self.doattrs(attrs) + '>'     
        string += str(thevalue)
        string += '</' + tag + '>'        
        
        self.iprint(string)
    
    def closetag(self,tag):
        """Close an XML element"""
        
        string = '</' + tag + '>'
        
        self.decind()
        self.iprint(string)    
    
    def content(self,thevalue):
        """Output XML characters"""
        # try to keep indentation for multiline content
        
        for line in str(thevalue).split('\n'):
            # look here for content serialization concerns
            self.iprint(line)
    
    def outputrxp(self,rxpexp):
        """Output RXP tuple-based expression"""
        
        if rxpexp[2]:
            # have children!
            
            self.opentag(rxpexp[0],rxpexp[1])
            
            for elem in rxpexp[2]:
                if type(elem) in (tuple,list):
                    self.outputrxp(elem)
                else:
                    self.content(elem)
                
            self.closetag(rxpexp[0])
        else:
            # I am a singleton
            self.singletag(rxpexp[0],rxpexp[1])
    

class lisaXML(writeXML):
    def __init__(self,filename,author='',date='',comments=''):
        """Create lisaXML file with metadata [author,date,comments];
           date should be given as ISO-8601."""
        
        self.filename = filename    
        writeXML.__init__(self,self.filename)
        
        self.author = author
        
        if date:
            self.date = date    # ??? a user-provided date should be validated before using it
        else:
            self.date = time.strftime('%Y-%m-%dT%H:%M:%S%Z',time.localtime())
        
        self.comments = comments
        self.theSourceData = []
        
        # for later use
        
        self.binaryfiles = 0
        self.theLISAData = []
        self.theNoiseData = []
        self.theTDIData = []
    
    def doComment(self,comments):
        return ('Comment', {}, [comments])
    
    def SourceData(self,source,name='',comments=''):
        """Add a SourceData entry describing a synthlisa source to
           a lisaXML file object."""
        
        self.theSourceData.append(source.XML(self,name,comments))
    
    def TDIData(self,observable,name='',comments=''):
        """Add a TDIData entry describing a set of TDI observables."""
        
        self.theTDIData.append(observable.XML(self,name,comments))
    
    def close(self):
        """Write the XML file to disk. This happens also on destruction of
        the lisaXML object."""
        
        if self.opened == 0:
            print "lisaXML::close(): File is closed already"
            raise IOError
        
        self.content('<?xml version="1.0"?>')
        self.content('<!DOCTYPE XSIL SYSTEM "http://www.vallis.org/lisa-xml.dtd">')
        
        self.content('<?xml-stylesheet type="text/xsl" href="lisa-xml.xsl"?>')
        # self.content('<?xml-stylesheet type="text/xsl" href="http://www.vallis.org/lisa-xml.xsl"?>')
        
        self.opentag('XSIL',{})
        
        self.coupletag('Param',{'Name': 'Author'},self.author)
        self.coupletag('Param',{'Name': 'GenerationDate', 'Type': 'ISO-8601'},
                               self.date)
        
        if self.comments:
            self.comments += '\n\n'
        
        self.comments += 'lisaXML 1.0 [M. Vallisneri, June 2006]'
                
        self.outputrxp(self.doComment(self.comments))
        
        if self.theLISAData:
            self.opentag('XSIL',{'Type': 'LISAData'})
            
            for object in self.theLISAData:
                self.outputrxp(object)
            
            self.closetag('XSIL')
        
        if self.theNoiseData:
            self.opentag('XSIL',{'Type': 'NoiseData'})
            
            for object in self.theNoiseData:
                self.outputrxp(object)
            
            self.closetag('XSIL')
        
        if self.theSourceData:
            self.opentag('XSIL',{'Type': 'SourceData'})
            
            for object in self.theSourceData:
                self.outputrxp(object)
            
            self.closetag('XSIL')
        
        # do the TDIdata objects (first supported)
        
        if self.theTDIData:
            self.opentag('XSIL',{'Type': 'TDIData'})
            
            for object in self.theTDIData:
                self.outputrxp(object)
            
            self.closetag('XSIL')
        
        self.closetag('XSIL')
        
        # do the actual writing
        
        writeXML.close(self)
    

# now to map a returned Source into a synthlisa object, I can just modify
# the synthlisa constructor to get what it needs... and then a "synthesize"
# method can find out which constructor to call

# getLISASources could return the synthlisa object, if synthlisa is available...
# otherwise it would return only an XMLobject

# but can I use multiple inheritance from SWIG?

# also, let getobsc return a derived numpy object that knows about cadence,
# observable names, etc.; then the lisaxml call TDIData can just 

# I can have LISA and Noise objects inherit from XMLobject, then it should
# be easy to deal with them...

class readXML:
    def __init__(self,filename):
        p = pyRXP.Parser()        
        
        f = open(filename)
        lines = f.read()
        f.close()
        
        try:
            tree = p(lines)
        except pyRXP.error:
            print "XML validation error! (Or perhaps I couldn't access the DTD)."
            print "I'll try to use the file anyway by removing the DTD..."
            
            lines = re.sub('<!DOCTYPE XSIL SYSTEM ".*">','',lines)
            tree = p(lines)
        
        if tree[0] != 'XSIL':
            print 'Not a LISA XSIL file!'
            raise TypeError
        
        self.directory = os.path.dirname(filename)
        
        self.tw = xmlutils.TagWrapper(tree)
    
    def close(self):
        pass
    
    def getTime(self,node):
        try:
            # keep Time as string, get Type if provided
            return (str(node),node.Type)
        except AttributeError:
            return (str(node),)
    
    def getParam(self,node):
        try:
            return [str(node),node.Unit]
        except AttributeError:
            return [str(node),None]
    
    def getDim(self,node):
        return int(str(node))
    
    def getLISASources(self):
        result = []
        
        for node in self.tw:
            if node.tagName == 'XSIL':
                if node.Type == 'SourceData':
                    for node2 in node:
                        if node2.tagName == 'XSIL':
                            # do not do 'SampledPlaneWave' yet
                            if node2.Type == 'PlaneWave' or node2.Type == 'SampledPlaneWave':
                                r = self.processObject(node2)
                                
                                result.append(r)
        
        return result
    
    def getTDIObservables(self):
        result = []
        
        for node in self.tw:
            if node.tagName == 'XSIL':
                if node.Type == 'TDIData':
                    for node2 in node:
                        if node2.tagName == 'XSIL' and node2.Type == 'TDIObservable':
                            r = self.processObject(node2)
                            
                            result.append(r)
        
        return result
    
    def reprvalue(self,param):
        """Turn a string into something we can work with!"""
        if param[1] == 'String':
            return param[0]
        
        for t in (int,float,str):
            try:
                value = t(param[0])
            except ValueError:
                pass
            else:
                return value
        
        raise ValueError
    
    def processTimeSeries(self,node):
        dim = {}
        
        for node2 in node:
            if node2.tagName == 'Array':
                for node3 in node2:
                    if node3.tagName == 'Dim':
                        dim[node3.Name] = self.getDim(node3)
                    elif node3.tagName == 'Stream':
                        encoding = node3.Encoding
                        stype    = node3.Type
                        
                        content = str(node3)
        
        length  = dim['Length']
        records = dim['Records']
        
        if stype == 'Remote' and 'Binary' in encoding:
            # assume length of doubles is 8 (generic?)
            readlength = 8 * length * records 
        
            if self.directory:
                try:
                    binaryfile = open(self.directory + '/' + content,'r')
                except:
                    binaryfile = open(content,'r')
            else:
                binaryfile = open(content,'r')
        
            readbuffer = numpy.fromstring(binaryfile.read(readlength),'double')
            binaryfile.close()
            
            if ( ('BigEndian' in encoding and sys.byteorder == 'little') or
                 ('LittleEndian' in encoding and sys.byteorder == 'big') ):
                readbuffer = readbuffer.byteswap()
        elif stype == 'Local' and 'Text' in encoding: 
            delimiter = node3.Delimiter
            
            for delchar in delimiter:
                content = string.join(content.split(delchar),' ')
                
                # there may be a more efficient way to initialize an array
                readbuffer = numpy.array(map(float,datastring.split()),'d')
        else:
            # support only remote binary and local text
            raise NotImplementedError
        
        if records == 1:
            return (readbuffer,)
        else:
            readbuffer = numpy.reshape(readbuffer,(length,records))
            return tuple(readbuffer[:,i] for i in range(records))
    
    def processObject(self,node):
        # get the name of the object
        
        try:
            objectname = node.Name
        except AttributeError:
            objectname = ''
        
        try:
            objecttype = node.Type
        except AttributeError:
            objecttype = None
        
        objectparams = {}
        
        # get the type and all other parameters
        
        for node2 in node:
            if node2.tagName == 'Param':
                if node2.Name == 'SourceType':
                    sourcetype = self.getParam(node2)[0]
                else:
                    objectparams[node2.Name] = self.getParam(node2)
        
        # create the object, importing the right module
        
        if objecttype in ('PlaneWave','SampledPlaneWave'):
            try:
                module = __import__(SourceClassModules[sourcetype])
                retobj = getattr(module,sourcetype)(objectname)
            except KeyError:
                raise NotImplementedError, 'readXML.processObject(): unknown object type %s for object %s' % (sourcetype,objectname)                                
        elif objecttype == 'TDIObservable':
            retobj = Observable(objectname)
        else:
            raise NotImplementedError, 'readXML.processObject(): unknown object type %s for object %s' % (objecttype,objectname)
        
        # now assign attributes
        # units go in AttributeName_Unit
        
        for param in objectparams:
            try:
                setattr(retobj,param,self.reprvalue(objectparams[param]))
                setattr(retobj,param + '_Unit',objectparams[param][1])
            except ValueError:
                raise ValueError, 'readXML.processObject(): cannot interpret value %s for parameter %s in object %s' % (objectparams[param],param,objectname)
        
        # look for TimeSeries (use only the first one found)
        
        for node2 in node:
            if node2.tagName == 'XSIL' and node2.Type == 'TimeSeries':      
                retobj.TimeSeries = TimeSeries(self.processTimeSeries(node2),node2.Name)
                
                for node3 in node2:
                    if node3.tagName == 'Param':
                        objectparam = self.getParam(node3)
                            
                        setattr(retobj.TimeSeries,node3.Name,self.reprvalue(objectparam))
                        setattr(retobj.TimeSeries,node3.Name + '_Unit',objectparam[1])
                
                # if TimeSeries name is understandable (comma-separated, etc.) and names
                # do not conflict with existing attributes, assign aliases to array columns
        
                columnnames = list(s.strip(' ') for s in retobj.TimeSeries.name.split(','))
        
                if ( len(columnnames) == retobj.TimeSeries.Records and
                     len(columnnames) == len(retobj.TimeSeries.Arrays) ):
                    
                    for col in range(len(columnnames)):
                        if not hasattr(retobj,columnnames[col]):
                            retobj.__dict__[columnnames[col]] = retobj.TimeSeries.Arrays[col]
                        
                        if not hasattr(retobj.TimeSeries,columnnames[col]):
                            retobj.TimeSeries.__dict__[columnnames[col]] = retobj.TimeSeries.Arrays[col]
        
        return retobj
    
