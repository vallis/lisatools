__version__ = '$Id$'

import pyRXP
import xmlutils

import numpy

import convertunit

import sys
import os
import os.path
import glob
import urllib
import stat
import shutil
import time
import string
import re

# if we have traits, import xmltraits.py

try:
    import enthought.traits.api
    from xmltraits import InteractiveMakeSource
except:
    pass

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
SourceClassModules['SpinBlackHoleBinary'] = 'BBH'
SourceClassModules['ExtremeMassRatioInspiral'] = 'EMRI'
SourceClassModules['GalacticBinary'] = 'GalacticBinary'

# could use something here to return a default unit if it is not set...
# but we would have to look in the source-specific list as well as
# a general list...

class XMLobject(object):
    """This is the base class for objects represented in lisaXML. It keeps
    track of what attributes are set for an object, and adds them to
    XMLobject.parameters unless the attribute name ends in _Unit or _Type. The
    idea is that all the attributes in XMLobject.parameters will eventually be
    written to XML as <Param> elements. When attributes are deleted they are
    also removed from XMLobject.parameters. To add an attribute while keeping
    it out of XMLobject.parameters, one can use the syntax
    XMLobject.__dict__['attributename'] = valueself. This class provides also
    a function parstr to print out an attribute/parameter with its Unit, if it
    is set."""
    
    def __init__(self):
        self.__dict__['parameters'] = []
    
    def __setattr__(self,attr,value):
        # thus, attributes that are initially assigned through the __dict__ interface
        # will never appear in self.parameters even if they are changed later 
        if (not attr in self.__dict__) and (not attr in self.parameters) and (not '_Unit' in attr) and (not '_Type' in attr):
            self.parameters.append(attr)
        
        self.__dict__[attr] = value
    
    def __delattr__(self,attr):
        del self.__dict__[attr]
        
        if attr in self.parameters:
            self.parameters.remove(attr)
    
    def parstr(self,attr):
        value = getattr(self,attr)
        
        try:
            unit = getattr(self,attr+'_Unit')
        except AttributeError:
            unit = 'default'
        
        return "%s (%s)" % (value,unit)
    

class Table(XMLobject):    
    """This class represents a lisaXML Table object. It is constructed with
    the name and record length of a stream. Attributes are supposed to be set,
    in order, representing the <Param> elements that corresponding to columns
    in the stream. Currently writing out the XML for a Table object copies the
    stream if the file is smaller than 5Mb, and symlinks it otherwise."""
    
    def __init__(self,streamname,streamlength):
        """Creates a Table object."""
        
        super(Table,self).__init__()
        
        self.__dict__['StreamName'] = streamname
        self.__dict__['StreamLength'] = streamlength
    
    def XML(self,textfile):
        """Returns a nested-tuple representation of a Table object."""
        columns = []
        
        for par in self.parameters:
            coldict = {'Name': par}
            
            if hasattr(self,par + '_Unit'):
                coldict['Unit'] = getattr(self,par + '_Unit')
            
            if hasattr(self,par + '_Type'):
                coldict['Type'] = getattr(self,par + '_Type')
            else:
                coldict['Type'] = 'double'
            
            # Column is a singleton element
            columns.append( ('Column', coldict, None) )
            
        dims = [ ('Dim', {'Name': 'Length'},  [self.StreamLength]),
                 ('Dim', {'Name': 'Records'}, [str(len(self.parameters))]) ]
        
        # initially support only Remote/Text
        
        if os.path.abspath(textfile) != self.StreamName:
            try:
                # delete textfile if it exists
                try:
                    os.unlink(textfile)
                except:
                    pass
            
                # copy files below 50M, symlink otherwise
                if os.stat(self.StreamName)[stat.ST_SIZE] < 50000000:
                    shutil.copy(self.StreamName,textfile)
                else:
                    os.symlink(self.StreamName,textfile)
            except IOError:
                raise IOError, 'Table.XML(): I have a problem copying data file %s to %s' % (self.StreamName,textfile)
            
        stream = ('Stream', {'Type': 'Remote', 'Encoding': 'Text'}, [os.path.basename(textfile)])
        
        return ('Table', {}, columns + dims + [stream])
    

class PlaneWaveTable(XMLobject):
    """This class represents a lisaXML PlaneWaveTable, which embeds (as
    PlaneWaveTable.Table) a table of source parameters, but carries additional
    parameters, such as SourceType."""
    
    def __init__(self,name='',textfilename=''):
        """Creates a PlaneWaveTable object."""
        
        super(PlaneWaveTable,self).__init__()
        
        self.__dict__['name'] = name
        # this is a bit of a hack... it allows passing the name of the final text data file
        self.__dict__['textfilename'] = textfilename
    
    def XML(self,xmlfile,name=None,comments=''):
        """Returns a nested-tuple representation of a PlaneWaveTable object."""
        
        xsildict = {}
        params = []
        
        xsildict['Type'] = 'PlaneWaveTable'
        xsildict['Name'] = name and name or self.name
        
        if hasattr(self,'SourceType'):
            params.append( ('Param',{'Name': 'SourceType', 'Type': 'String'},[self.SourceType]) )
        
        if hasattr(self,'Table'):
            # need to pass filename for copy
            if self.textfilename:
                params.append(self.Table.XML(self.textfilename))
            else:
                params.append(self.Table.XML(re.sub('\.xml$','',xmlfile.filename) + '-' + str(xmlfile.textfiles) + '.txt'))
            
            xmlfile.textfiles += 1
        
        return ('XSIL', xsildict, params)
    
    def getSources(self):
        """Is a generator that instantiates sources from the Table, one by
        one..."""
        
        tablename  = self.name
        
        sourcetype = self.SourceType
        try:
            module = __import__(SourceClassModules[sourcetype])
            retobj = getattr(module,sourcetype)
        except KeyError:
            raise NotImplementedError, 'PlaneWaveTable.getSources(): unknown object type %s' % sourcetype
        
        sourceparameters = self.Table.parameters
        # for the moment we'll assume that all parameters have their default units
        # of course, this is dangerous...
        
        counter = 1
        for line in open(self.Table.StreamName,'r'):
            newobject = retobj(name='Object #%d from table %s' % (counter,tablename))
            
            # skip line if we don't have enough parameters
            lineparameters = line.split()
            if len(sourceparameters) != len(lineparameters):
                break
            
            for (p,v) in zip(sourceparameters,lineparameters):
                # should be more sophisticated in treating type here...
                value = v
                
                for t in (int,float,str):
                    try:
                        value = t(v)
                    except ValueError:
                        pass
                    else:
                        break
                        
                setattr(newobject,p,value)
            
            counter = counter + 1
            yield newobject
        
        return
    

class TimeSeries(XMLobject):
    """This class derives from XMLobject, and it represents a TimeSeries
    holding numpy arrays for an Observable or Source object."""
    
    def __init__(self,arrays,name='',unit='',Cadence=None,TimeOffset=None):
        """Create a TimeSeries object. "arrays" must be a numpy 1D array, or a list
        or tuple of numpy arrays of the same length. "name" must be given as a 
        comma separated list of observable names. The attributes TimeOffset and
        Cadence must also be set before the TimeSeries can be output.
        See also lisaxml.Observable."""
        
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
        
        if Cadence != None:
            self.Cadence = Cadence
        
        if TimeOffset != None:
            self.TimeOffset = TimeOffset
        
    
    def XML(self,filename,encoding='Binary'):
        """Returns a nested-tuple representation of a TimeSeries object."""
        
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
        
        # use default units for TimeOffset and Cadence
                
        if not hasattr(self,'TimeOffset_Unit'):
            self.TimeOffset_Unit = 'Second'
        if not hasattr(self,'Cadence_Unit'):
            self.Cadence_Unit = 'Second'
        
        # watch the definition of duration (it is just length * timestep) 
                
        TimeSeries = ('XSIL',
                      {'Name': self.name, 'Type': 'TimeSeries'},
                      [ ('Param', {'Name': 'TimeOffset', 'Unit': self.TimeOffset_Unit}, [self.TimeOffset]),
                        ('Param', {'Name': 'Cadence',    'Unit': self.Cadence_Unit},    [self.Cadence]),
                        ('Param', {'Name': 'Duration',   'Unit': self.Cadence_Unit},    [self.Length * self.Cadence]),
                        Array ] )
        
        return TimeSeries
    

class Observable(XMLobject):
    """This class derives from XMLobject, and it represents a set of TDI
    observable time series with the same cadence, time offset, and length.
    Observable.TimeSeries is an instance of TimeSeries containing and
    describing the data.
        
    --- Output ---
    The following is an example of setting up an Observable object to write it
    to a lisaXML file object (using TDIData()). Suppose you have numpy arrays
    of your (Synthetic LISA) data in t, Xf, Yf, Zf, and that their cadence
    (in seconds) is in 'cad', their time offset (in seconds) is in 'timeoff':
    
    ts = lisaxml.TimeSeries([t,X,Y,Z],'t,Xf,Yf,Zf',Cadence=cad,TimeOffset=timeoff)
    to = lisaxml.Observable(TimeSeries=ts,DataType='FractionalFrequency')
        
    lisaXMLinstance.TDIData(tdiobs)
        
    --- Input ---
    Observable.TimeSeries is also available upon reading from a readXML file
    object (using TDIData, which returns a list of Observable
    objects). In that case the individual numpy arrays can be accessed as
    attributes of TimeSeries.
        
    # get the first Observable object
    tdiobs = readXMLinstance.TDIData[0]
        
    length     = tdiobs.TimeSeries.Length
    timestep   = tdiobs.TimeSeries.Cadence
    timeoffset = tdiobs.TimeSeries.TimeOffset
        
    t  = tdiobs.t
    Xf = tdiobs.Xf
    Yf = tdiobs.Yf
    Zf = tdiobs.Zf"""
    
    def __init__(self,name='',TimeSeries=None,DataType=None):
        """Creates an Observable object."""
        
        super(Observable,self).__init__()
        
        self.__dict__['name'] = name
        
        if TimeSeries:
            self.__dict__['TimeSeries'] = TimeSeries
            
            if not self.name:
                # inherit the name of the TimeSeries, if given...
                self.__dict__['name'] = self.TimeSeries.name
            
        if DataType:
            self.DataType = DataType
        else:
            # try to guess it...
            isf = ('Xf' in self.name) or ('Yf' in self.name) or ('Zf' in self.name)
            iss = ('Xs' in self.name) or ('Ys' in self.name) or ('Zs' in self.name)
        
            if isf and (not iss):
                self.DataType = 'FractionalFrequency'
            elif iss and (not isf):
                self.DataType = 'Strain'
        
    
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
    """This class represents a lisaXML GW Source object. It inherits from
    XMLobject and has the same attribute/parameter semantics; in addition, it
    carries "name" and "xmltype" parameters. The first is written to XML as
    the "Name" of a PlaneWave element; the second is included among the
    PlaneWave parameters as <Param Name="SourceType">. "xmltype" is usually
    automatically set when instantiating Source from a derived object, such as
    "BBH" or "EMRI". Symmetrically, when a PlaneWave element is read from XML,
    it will instantiate an object of type given by SourceType.
        
    Source.outputlist contains the list of parameters that will be written
    to XML; each parameter is specified by a tuple
        ('ParName', 'DefaultParUnit', DefaultValue, 'parameter description')
    if DefaultValue is None, then the parameter must be set explicitly (e.g.,
    "bbh.Mass1 = 1e6") before the Source can be written to XML. If the Unit
    is not set explicitly (e.g., "bbh.Mass1_Unit='kg'") then the parameter
    will be taken to be specified w.r.t. 'DefaultParUnit'. The package
    lisaxml.convertunit will attempt to some trivial unit conversions.
        
    All Source objects should support the function call
        waveforms(samples,timestep,initialtime)
    (where timestep and initialtime are in seconds) to return hp and hc
    time series as numpy arrays. [Note: waveforms currently assumes that
    all parameters have their default units.]
        
    --- Output ---
    import BBH
        
    # BBH derives from Source
    mysystem = BBH.BlackHoleBinary('Name of my binary')
        
    mysystem.Mass1 = 1.0e6  # m1 in SolarMass
    mysystem.Mass2 = 0.5e6  # m2 in SolarMass
    ...
        
    outputXML = lisaxml.lisaXML('output.xml')
    outputXML.SourceData(mysystem)
    outputXML.close()
        
    --- Input ---
        
    inputXML = lisaxml.readXML('input.xml')
    # this returns a list, from which we take only the first Source
    mysystem = inputXML.getLISASources()[0]
    inputXML.close()
        
    # mysystem is now a "BBH" object
    mass1 = mysystem.Mass1
    ...
        
    # all Source objects support this call to get hp and hc polarizations
    # as numpy arrays
    (hp,hc) = mysystem.waveforms(samples,timestep,initialtime)"""
    
    def __init__(self,sourcetype,name=''):
        """Creates a generic Source object. Generally this is called
        implicitly from specific constructors such as BBH and EMRI."""
        
        super(Source,self).__init__()
            
        # avoid calling setattr
        self.__dict__['xmltype'] = sourcetype
        self.__dict__['name'] = name
        # TO DO: name would be better as 'Name'
    
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
        
        if hasattr(self,'Comment') and self.Comment:
            self.Comment = self.Comment + '\n' + comments
        else:
            self.__dict__['Comment'] = comments
        
        if self.Comment:
            params.append(('Comment', {}, [self.Comment]))
        
        # we're done...
        
        return ('XSIL', xsildict, params)
    

class XSILobject(XMLobject):
    """This class represents a lisaXML object with parameters (such as LISA),
    but without the special properties of Source. [In a future implementation,
    Source may inherit from XSILobject.]"""
    
    def __init__(self,xmltype,name=''):
        """Create an XSILobject. Generally called from the constructors of
        inhering objects."""
        
        super(XSILobject,self).__init__()
        
        # avoid calling setattr
        self.__dict__['xmltype'] = xmltype
        self.__dict__['name'] = name
    
    def XML(self,xmlfile,name=None,comments=''):
        """Add a generic XML object to an XML output file."""
        
        xsildict = {}
        
        xsildict['Type'] = self.xmltype
        xsildict['Name'] = name and name or self.name
        
        # no unit conversion, nothing!
        
        params = []
        
        for parname in self.parameters:
            if hasattr(self,parname + '_Unit'):
                params.append(('Param', {'Name': parname, 'Unit': getattr(self,parname + '_Unit')}, [getattr(self,parname)]))
            else:
                params.append(('Param', {'Name': parname}, [getattr(self,parname)]))
        
        # we're done...
        
        return ('XSIL', xsildict, params)
    

class LISA(XSILobject):
    """This class inherits from XSILobject, and it represents the pseudo-LISA
    geometry. For instance, the standard MLDC pseudo-LISA is created with
        
    lisa = lisaxml.LISA('Standard MLDC PseudoLISA')
    lisa.TimeOffset      = 0; lisa.TimeOffset_Unit      = 'Second'
    lisa.InitialPosition = 0; lisa.InitialPosition_Unit = 'Radian'
    lisa.InitialRotation = 0; lisa.InitialRotation_Unit = 'Radian'
    lisa.Armlength = 16.6782; lisa.Armlength_Unit       = 'Second'
        
    LISA objects are read with readXML.getLISAgeometry() and written
    with lisaXML.LISAData().
        
    Doctests: see readXML.getLISAGeometry().
    """
    
    def __init__(self,name=''):
        super(LISA,self).__init__('PseudoLISA',name)


class writeXML(object):
    """The basic class underlying lisaXML, which supports basic formatted-XML
    output."""
    
    def __init__(self,filename):
        """Opens an XML file for writing."""
        
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
        """Closes the file if still open."""
        
        if self.opened == 1:
            self.close()
    
    def close(self):
        """Closes the file if still open."""
        
        if self.opened == 1:
            self.f.close()
            self.opened = 0
    
    # handling of XML indentation
    stdindent = 4
    indent = ""  
    def incind(self):
        """Increase the current indentation level."""
        
        self.indent += " " * self.stdindent
    
    def decind(self):
        """Reduce the current indentation level."""
        
        self.indent = self.indent[0:len(self.indent)-self.stdindent]
    
    def iprint(self,s):
        """Print a string at the current indentation level, and adds a newline
        if not already included in the string."""
        
        if self.opened != 0 and len(s) > 0:
            print >> self.f, self.indent + s,
            if s[-1] != '\n':
                print >> self.f
    
    def doattrs(self,attrs):
        """Handles the attributes of an element, putting Name first if it's
        there."""
        
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
        """Open an XML element; take dictionary for attributes.""" 
        
        string = '<' + tag + self.doattrs(attrs) + '>'
        
        self.iprint(string)
        self.incind()
    
    def singletag(self,tag,attrs):
        """Do an XML singleton; take dictionary for attributes."""
        
        string = '<' + tag + self.doattrs(attrs) + '/>'
        
        self.iprint(string)
    
    def coupletag(self,tag,attrs,thevalue):
        """Do inline XML open/close tag."""
        
        string = '<' + tag + self.doattrs(attrs) + '>'     
        string += str(thevalue)
        string += '</' + tag + '>'        
        
        self.iprint(string)
    
    def closetag(self,tag):
        """Close an XML element."""
        
        string = '</' + tag + '>'
        
        self.decind()
        self.iprint(string)    
    
    def content(self,thevalue):
        """Output XML characters, adding indentation."""
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
    """The Python object representation of an output lisaXML file"""
    
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
        
        # remove all bin and txt files (at least the first 100)
        
        basename = re.sub('\.xml$','',self.filename)
        
        for binfile in (glob.glob(basename + '-[0-9].bin') + glob.glob(basename + '-[0-9][0-9].bin')):
            if os.path.isfile(binfile):
                os.unlink(binfile)
                
        for txtfile in (glob.glob(basename + '-[0-9].txt') + glob.glob(basename + '-[0-9][0-9].txt')):
            if os.path.isfile(txtfile):
                os.unlink(txtfile)
        
        # for later use
        
        self.binaryfiles = 0
        self.textfiles = 0
        
        self.theLISAData = []
        self.theNoiseData = []
        self.theTDIData = []
    
    def SourceData(self,source,name='',comments=''):
        """Add a source (e.g., BBH, EMRI, GalacticBinary) to a lisaXML file object.
        See lisaxml.Source."""
        
        self.theSourceData.append(source.XML(self,name,comments))
    
    def TDIData(self,observable,name='',comments=''):
        """Add a TDI Observable object to the lisaXML file. See lisaxml.Observable."""
        
        self.theTDIData.append(observable.XML(self,name,comments))
    
    def LISAData(self,lisa,name='',comments=''):
        """Add a LISAData entry describing the LISA geometry. See lisaxml.LISA."""
        
        self.theLISAData.append(lisa.XML(self,name,comments))
    
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
        
        # add a lisaXML statement
        
        lisaxmlstring = 'lisaXML 1.0 [M. Vallisneri, June 2006]'
        
        if lisaxmlstring not in self.comments:
            if self.comments:
                self.comments += '\n\n'
            
            self.comments += lisaxmlstring
            
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
    
    def doComment(self,comments):
        """Add Comment element (internal function)"""
        
        return ('Comment', {}, [comments])
    


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

# remote http requests are enabled only for the main xml and binary files

# ticket: ordering of columns may not be applied when loading table of parameters

class readXML(object):
    """This is the Python object representation of an input lisaXML file. This
       class parses lisaXML files and provides access to their contained LISA,
       Source, and TDI data. This is obtained by accessing class attributes:
       
       exampleXML = lisaxml.readXML('example.xml')
       
       lisa        = exampleXML.LISAData   # returns a LISA object
       sources     = exampleXML.SourceData # returns a list of Source objects
       observables = example.TDIData       # returns a list of Observable objects
       
       Note that the lists may be lists of a single element, which is then
       accessed by sources[0] or observables[0].
       
       In addition, the attributes Author, Date, and Comment may be used to access
       the file metadata. If more than one comment is present in the XML, they will be
       collated into a single string."""
    
    def __init__(self,filename,validate=False):
        """Initialize a readXML object. 'filename' may point to a local file, or
        (if it begins with http://) to a remote file to be downloaded from the web.
        Setting 'validate' to true enables the validation of the XML with any DTD
        statement that it may contain.
         
        Doctests:
         
        >>> readXMLobj = readXML('test.xml')
        >>> readXMLobj.Author
        'Michele Vallisneri'
        >>> readXMLobj.GenerationDate, readXMLobj.GenerationDate_Type
        ('2007-07-20T15:47:29PDT', 'ISO-8601')
        >>> readXMLobj.Comment
        'lisaXML 1.0 [M. Vallisneri, June 2006]\\nA second test comment.'
        >>> readXMLobj.ExtraParameter, readXMLobj.ExtraParameter_Unit
        (3.1400000000000001, 'Second')
        >>> readXMLobj = readXML('http://www.vallis.org/test.xml')
        """
        
        p = pyRXP.Parser()
        
        # read from the web or from local file
        if 'http://' in filename:
            lines = urllib.urlopen(filename).read()
            
            # TO DO: could use better recognition of 404-file non found
            if '<?xml' not in lines:
                raise IOError
        else:
            lines = open(filename).read()
        
        try:
            if not validate:
                # disable validation
                raise
        
            tree = p(lines)
        except:
            # [TO DO: catch this exception more specifically]
            # validation was disabled, or pyRXP failed, probably on DTD
            # so try calling pyRXP without validation
            
            p.TrustSDD = 0
            p.Validate = 0
        
            tree = p(lines)
        
        if tree[0] != 'XSIL':
            print 'Not a LISA XSIL file!'
            raise TypeError
        
        # save the location of the file and its parsed tree representation
        self.directory = os.path.dirname(filename)
        self.tw = xmlutils.TagWrapper(tree)
        
        # gather meta data from the file
        self.Author, self.GenerationDate, self.Comment = '', '', ''
        
        # TO DO: this could be refactored by making readXML inherit from XMLobject
        for node in self.tw:
            if node.tagName == 'Param':
                if not hasattr(self,node.Name) or getattr(self,node.Name) == '':
                    setattr(self,node.Name,self.__reprvalue((str(node),hasattr(node,'Unit') and str(node.Unit) or '')))
                    
                    if hasattr(node,'Unit'):
                        setattr(self,node.Name + '_Unit',str(node.Unit))                    
                    
                    if hasattr(node,'Type'):
                        setattr(self,node.Name + '_Type',str(node.Type))                        
            elif node.tagName == 'Comment':
                if self.Comment:
                    self.Comment += ('\n' + str(node))
                else:
                    self.Comment = str(node)
    
    def close(self):
        """It is not necessary to close the readXML object; 'del' can be used to
           destroy it if it is necessary to reclaim memory."""
        pass
    
        
    def getMetaData(self):
        """Returns Author, GenerationData, Comment (may be empty strings if
           not provided). The preferred interface is to access the readXML
           instance attributes Author, GenerationData, and Comment directly.
           Some notes:
           
           - If more than one Comment element is present in the XML, they are
             concatenated in a single newline-separated string.
           
           - If the Type of GenerationData is given, it is available as
             the readXML attribute GenerationData_Type.
             
           - If other Param elements are present at the root level of the XML
             file, they are assigned to the readXML attribute of the same Name,
             unless that attribute is already being used for system purposes."""
        
        return self.Author, self.GenerationData, self.Comment
    
    def getLISAGeometry(self):
        """Return the first LISA object defined in the XML. It's better to access
           the LISAData attribute directly.
           
           Doctests:
           
           >>> readXMLobj = readXML('test.xml')
           >>> lisa = readXMLobj.LISAData
           >>> lisa.name
           'Standard MLDC LISA'
           >>> lisa.Armlength, lisa.Armlength_Unit
           (16.6782, 'Second')
           """
        # use the first LISA found...
        for node in self.tw:
            if node.tagName == 'XSIL':
                if node.Type == 'LISAData':
                    for node2 in node:
                        if node2.tagName == 'XSIL' and node2.Type == 'PseudoLISA':
                                return self.__processObject(node2)
        
        return None
    
    LISAData = property(getLISAGeometry)
    def getLISASources(self):
        """Return a list of Source objects defined in the XML. It's better to access
           the LISASource attribute directly.
           
           Doctests:
           
           >>> readXMLobj = readXML('test.xml')
           >>> sources = readXMLobj.SourceData
           >>> len(sources)
           2
           >>> sources[0].name, sources[0].xmltype, type(sources[0])
           ('Galactic binary 1.1.1a', 'GalacticBinary', <class 'GalacticBinary.GalacticBinary'>)
           >>> sources[0].Frequency, sources[0].Frequency_Unit
           (0.0010627314430000001, 'Hertz')
           >>> sources[1].name, sources[1].xmltype, type(sources[1])
           ('SMBH binary 1.2.1', 'BlackHoleBinary', <class 'BBH.BlackHoleBinary'>)
           >>> sources[1].CoalescenceTime, sources[1].CoalescenceTime_Unit
           (13374027.9811, 'Second')
        """
         
        result = []
        
        for node in self.tw:
            if node.tagName == 'XSIL':
                if node.Type == 'SourceData':
                    for node2 in node:
                        if node2.tagName == 'XSIL':
                            if node2.Type in ('PlaneWave','SampledPlaneWave','PlaneWaveTable'):
                                r = self.__processObject(node2)
                                
                                result.append(r)
        
        return result
    
    SourceData = property(getLISASources)
    def getTDIObservables(self):
        """Return a list of the Observable objects defined in the XML. It's
           better to access the TDIData attribute directly...
           
           Doctests:
           
           >>> readXMLobj = readXML('test.xml')
           >>> tdiobs = readXMLobj.TDIData
           >>> len(tdiobs)
           1
           >>> tdiobs[0].name, tdiobs[0].DataType, type(tdiobs[0]) == Observable
           ('t,Xf,Yf,Zf', 'FractionalFrequency', True)
           >>> tdiobs[0].TimeSeries.Cadence, tdiobs[0].TimeSeries.TimeOffset
           (15, 0)
           >>> tdiobs[0].t
           array([ 101.,  201.,  301.,  401.])
           >>> tdiobs[0].Xf
           array([ 102.,  202.,  302.,  402.])
           """
        
        result = []
        
        for node in self.tw:
            if node.tagName == 'XSIL':
                if node.Type == 'TDIData':
                    for node2 in node:
                        if node2.tagName == 'XSIL' and node2.Type == 'TDIObservable':
                            r = self.__processObject(node2)
                            
                            result.append(r)
        
        return result
    
    TDIData = property(getTDIObservables)
    
    # internal (TO DO: may want to define these as __private!):
    def __getParam(self,node):
        """[Internal] Return parameter value as (str,unit) or (str,) tuple."""
        
        try:
            return [str(node),node.Unit]
        except AttributeError:
            return [str(node),None]
    
    def __reprvalue(self,param):
        """[Internal] Turn a ('value','unit') pair into something we can work with!
           Will try (int,float,str) in order, unless Unit/Type is given as string."""
        
        if param[1] == 'String':
            return str(param[0])
        
        for t in (int,float,str):
            try:
                value = t(param[0])
            except ValueError:
                pass
            else:
                return value
        
        raise ValueError
    
    def __processTimeSeries(self,node):
        """[Internal] Reads array objects from file for a TimeSeries object."""
        
        dim = {}
        
        for node2 in node:
            if node2.tagName == 'Array':
                for node3 in node2:
                    if node3.tagName == 'Dim':
                        dim[node3.Name] = int(str(node3))
                    elif node3.tagName == 'Stream':
                        encoding = node3.Encoding
                        stype    = node3.Type
                        
                        content = str(node3)
        
        length  = dim['Length']
        records = dim['Records']
        
        if stype == 'Remote' and 'Binary' in encoding:
            # assume length of doubles is 8 (generic?)
            readlength = 8 * length * records 
        
            if 'http://' in self.directory:
                binaryfile = urllib.urlopen(self.directory + '/' + content,'r')
            else:
                try:
                    if not self.directory:
                        raise
                        
                    binaryfile = open(self.directory + '/' + content,'r')
                except:
                    # if we cannot find the binary relative to the xml,
                    # try relative to the working directory
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
            readbuffer = numpy.array(map(float,content.split()),'d')
        else:
            # support only remote binary and local text
            raise NotImplementedError
        
        if records == 1:
            return (readbuffer,)
        else:
            readbuffer = numpy.reshape(readbuffer,(length,records))
            # return list for compatibility with Python 2.3
            return [readbuffer[:,i] for i in range(records)]
            # previously: return tuple(readbuffer[:,i] for i in range(records))
    
    def __processObject(self,node):
        """[Internal] Makes an XMLObject from XML data. Currently handles
           PlaneWave, SampledPlaneWave, PlaneWaveTable, TDIObservable, PseudoLISA."""
        
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
        objectparamlist = []
        
        # get the type and all other parameters
        
        for node2 in node:
            if node2.tagName == 'Param':
                if node2.Name == 'SourceType':
                    sourcetype = self.__getParam(node2)[0]
                else:
                    objectparams[node2.Name] = self.__getParam(node2)
                    # need this to set parameters in the order they're given...
                    objectparamlist.append(node2.Name)
        
        # create the object, importing the right module
        
        if objecttype in ('PlaneWave','SampledPlaneWave'):
            try:
                module = __import__(SourceClassModules[sourcetype])
                retobj = getattr(module,sourcetype)(objectname)
            except KeyError:
                raise NotImplementedError, 'readXML.processObject(): unknown object type %s for object %s' % (sourcetype,objectname)
            except ImportError:
                print 'readXML.processObject(): cannot import module %s for object type %s, I will make it a generic source' % (SourceClassModules[sourcetype],sourcetype)
                retobj = Source(sourcetype,objectname)                            
        elif objecttype == 'PlaneWaveTable':
            retobj = PlaneWaveTable(objectname)
            retobj.SourceType = sourcetype
        elif objecttype == 'TDIObservable':
            retobj = Observable(objectname)
        elif objecttype == 'PseudoLISA':
            retobj = LISA(objectname)
        else:
            raise NotImplementedError, 'readXML.processObject(): unknown object type %s for object %s' % (objecttype,objectname)
        
        # now assign attributes
        # units go in AttributeName_Unit
        
        for param in objectparamlist:
            try:
                setattr(retobj,param,self.__reprvalue(objectparams[param]))
                setattr(retobj,param + '_Unit',objectparams[param][1])
            except ValueError:
                raise ValueError, 'readXML.processObject(): cannot interpret value %s for parameter %s in object %s' % (objectparams[param],param,objectname)
        
        # look for TimeSeries (use only the first one found, if any)
        
        for node2 in node:
            if node2.tagName == 'XSIL' and node2.Type == 'TimeSeries':      
                retobj.TimeSeries = TimeSeries(self.__processTimeSeries(node2),node2.Name)
                
                for node3 in node2:
                    if node3.tagName == 'Param':
                        objectparam = self.__getParam(node3)
                            
                        setattr(retobj.TimeSeries,node3.Name,self.__reprvalue(objectparam))
                        setattr(retobj.TimeSeries,node3.Name + '_Unit',objectparam[1])
                
                # if TimeSeries name is understandable (comma-separated, etc.) and names
                # do not conflict with existing attributes, assign aliases to array columns
        
                columnnames = [s.strip(' ') for s in retobj.TimeSeries.name.split(',')]
                
                if ( len(columnnames) == retobj.TimeSeries.Records and
                     len(columnnames) == len(retobj.TimeSeries.Arrays) ):
                    
                    for col in range(len(columnnames)):
                        # for convenience, also to the outlying Observable or SampledPlaneWave object
                        if not hasattr(retobj,columnnames[col]):
                            retobj.__dict__[columnnames[col]] = retobj.TimeSeries.Arrays[col]
                        
                        if not hasattr(retobj.TimeSeries,columnnames[col]):
                            retobj.TimeSeries.__dict__[columnnames[col]] = retobj.TimeSeries.Arrays[col]
            
                break
        
        # look for Table (use only the first one found, if any)
        
        for node2 in node:
            if node2.tagName == 'Table':
                # get Dim and Stream name
                for node3 in node2:
                    if node3.tagName == 'Dim':
                        if node3.Name == 'Length':
                            streamlength = int(str(node3))
                        elif node3.Name == 'Records':
                            tabledim = int(str(node3))
                    elif node3.tagName == 'Stream':
                        streamname = os.path.abspath(self.directory) + '/' + str(node3)
                        
                        try:
                            if node3.Type != 'Remote' or node3.Encoding != 'Text':
                                raise NotImplementedError, 'readXML.processObject(): Stream can only be Remote/Text'
                        except AttributeError:
                            raise AttributeError, 'readXML.processObject(): Stream must specify Type and Encoding'
                
                try:
                    retobj.Table = Table(streamname,streamlength)
                except NameError:
                    raise AttributeError, 'readXML.processObject(): missing Stream file in %s %s' % (objecttype,objectname)
                
                # now get all the columns
                for node3 in node2:
                    if node3.tagName == 'Column':
                        setattr(retobj.Table,node3.Name,None)
                        
                        if hasattr(node3,'Unit'):
                            setattr(retobj.Table,node3.Name + '_Unit',node3.Unit)
                        
                        if hasattr(node3,'Type'):
                            setattr(retobj.Table,node3.Name + '_Type',node3.Type)
                
                try:
                    if len(retobj.Table.parameters) != tabledim:
                        raise ValueError, 'readXML.processObject(): Table %s declares the wrong number of columns (%s vs %s)' % (objectname,tabledim,len(retobj.parameters))
                except:
                    pass
                    
                break
        
        # look for comment
        
        for node2 in node:
            if node2.tagName == 'Comment':
                if hasattr(retobj,'Comment') and retobj.Comment:
                    retobj.Comment = retobj.Comment + '\n' + str(node2)
                else:
                    retobj.__dict__['Comment'] = str(node2)
        
        return retobj
    
    
    # deprecated:
    def getLISAgeometry(self):
        """[Deprecated] see getLISAGeometry()"""
        
        return self.getLISAGeometry()
    


def _test():
    import doctest
    doctest.testmod()

if __name__ == "__main__":
    _test()
