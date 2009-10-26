__version__ = '$Id: lisaxml.py 479 2007-07-26 16:19:50Z vallisneri $'

import pyRXP, xmlutils
import numpy

import convertunit

import sys, os, stat, shutil, glob
import urllib
import time, string, re

# if we have traits, import xmltraits.py
try:
    import enthought.traits.api
    from xmltraits import InteractiveMakeSource
except:
    pass

# TO DO:
#
# - added Checksum facility to the writing of external binary tables; however Stream objects don't automatically
#   get the attribute. Should they?
#
# - some MLDC utilities may use xmltype to check for source type (makebarycentric and fixemri2) see how it's used here
# - also the standard is that a PlaneWave becomes a SampledPlaneWave once it acquires a TimeSeries;
#   we should probably deal with this in PlaneWave
# - document Stream, Table, Array, TimeSeries, Observable
# - more doctests
#
# + later
# - add an intelligent PlaneWaveTable that knows how to add sources, make them, etc.
# - a print method for XSIL, Array, etc. elements (possibly piggyback on XML?)
# - support more Stream types (including different wordlengths)
# - default units
# - smart units?
# - more doctests
#
# + ideas
# - to map a returned Source into a synthlisa object, I can just modify
#   the synthlisa constructor to get what it needs... and then a "synthesize"
#   method can find out which constructor to call
# - let getobsc return a derived numpy object that knows about cadence,
#   observable names, etc.; then the lisaxml call TDIData can just 
#
# TO DO: to avoid the problem of broken links for aliases (e.g., Observable.Data), one can implement them
#        with getters and setters (Python property, right?)


SourceClassModules = {}

SourceClassModules['BlackHoleBinary'] = 'BBH'
SourceClassModules['SpinBlackHoleBinary'] = 'BBH'
SourceClassModules['FastSpinBlackHoleBinary'] = 'FastBBH'
SourceClassModules['ExtremeMassRatioInspiral'] = 'EMRI'
SourceClassModules['GalacticBinary'] = 'GalacticBinary'
SourceClassModules['Stochastic'] = 'Stochastic'
SourceClassModules['CosmicStringCusp'] = 'CosmicStringCusp'

SourceClassModules['PseudoRandomNoise'] = 'PseudoRandomNoise'

class XMLobject(object):
    """This class is used to manage attributes for XSIL objects: it keeps a list
    of attributes that are assigned directly (i.e., with a self.attrname = value
    assignment), so that they can later be written to XML as <Param> elements.
    Attributes that begin with '_' (including private attributes) or that end
    in '_Unit' or '_Type' are automatically excluded. The list is kept in the
    XMLobj.parameters attribute. Attributes that are deleted are also removed from the 
    list. One way to define an attribute while keeping it out of XMLobj.parameters is
    the direct-to-dict syntax XMLobj.__dict__['attrname'] = value (further
    assignments can use the standard self.attrname = value syntax, since only
    new attributes are added to XMLobj.parameters)"""
    
    def __init__(self):
        self.__dict__['parameters'] = []
    
    def __setattr__(self,attr,value):
        # thus, attributes that are initially assigned through the __dict__ interface
        # will never appear in self.parameters even if they are changed later 
        if (not attr in self.__dict__) and (not attr in self.parameters) and (not '_Unit' in attr) and (not '_Type' in attr) and (attr[0] != '_'):
            self.parameters.append(attr)
        
        # create the attribute with a value of None if the unit or type is set
        if ('_Unit' in attr) or ('_Type' in attr):
            baseattr = re.sub('_Unit|_Type','',attr)
            if (not baseattr in self.__dict__) and (not baseattr in self.parameters):
                self.parameters.append(baseattr)
                self.__dict__[baseattr] = None
        
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
    

class XSILobject(XMLobject,list):
    """This class represents a lisaXML XSIL object that may have <Param>eters,
    a Comment, (possibly) an Array and/or a Table, and XSIL children. The
    parameters are stored as named attributes (say, XSILobj.Frequency), the
    comment in XSILobj.Comment, the Array/Table in XSILobj.Array and/or
    XSILobj.Table, and the XSIL children represent the list content of the
    class (meaning that they can be accessed collectively as XSILobj[:], or
    individually as XSILobj[0], etc.).
    
    A reference to the first XSIL children of each Type is also stored as an
    attribute named by Type (e.g., XSILobj.SourceData or XSILobj.TimeSeries);
    however, this reference is for convenience only, and it will not be
    updated if and when list content is replaced or modified.
    """
    
    def __init__(self,xmltype='',name=''):
        """Create an XSILobject. Generally called from the constructors of
        inheriting objects, or from makeXSIL to make a generic XSILobject."""
        
        super(XSILobject,self).__init__()
        
        # avoid calling setattr
        self.__dict__['xmltype'] = self.__dict__['Type'] = xmltype
        self.__dict__['name'] = self.__dict__['Name'] = name
    
    def makeXSIL(node,xmlfile=None,self=None):
        """Create an XSILobject from a pyRXP-processed XML tree.
        This amounts to:
        
        - Constructing an XSILobject of the right type. Currently supported
          special cases include Source ('PlaneWave'), SourceTable
          ('PlaneWaveTable'), Observable ('TDIObservable'), and TimeSeries.
        
        - Setting the Name and Type of the object (aliased also to 'name' and
          'xmltype'.)
        
        - Assigning <Param> elements to XSILobject attributes.
        
        - Processing <Comment> elements by collating them into a single
          newline-separated Comment (accessible as XSILobj.Comment).
        
        - Processing <Array> and <Table> elements if defined inside this
          <XSIL> element. Note that only one of each is allowed; if given, the
          two are accessible as XSILobj.Array and XSILobj.Table.
        
        - Processing any XSIL children, which are stored in the list content
          of the XSILobject (meaning that the Python list interface can be
          used to access, modify, and remove them), and also in attributes
          name by Type (such as XSILobj.SourceData, XSILobj.TimeSeries), etc.
        
        - Last, if the special classes define the checkContent() method, it is
          called. This allows for such operations as checking the presence of
          all required <Param>s in Source, and adding shortcuts to the TDI
          Array columns in Observable.
        
        Doctests:
        
        >>> p = pyRXP.Parser(); p.TrustSDD = p.Validate = 0
        >>> t = xmlutils.TagWrapper(p(open('test.xml').read()))
        >>> x = XSILobject.makeXSIL(t)
        >>> x.GenerationDate, x.GenerationDate_Type
        ('2007-07-24T15:50:02PDT', 'ISO-8601')
        >>> x.ExtraParameter, x.ExtraParameter_Unit
        (3.1400000000000001, 'Second')
        >>> type(x[0]) == XSILobject, x[0].Type
        (True, 'LISAData')
        """
        
        # first verify that this is indeed an XSIL object
        assert node.tagName == 'XSIL', 'Not an XSIL element!'
        
        # get name and type
        objname = hasattr(node,'Name') and node.Name or ''
        objtype = hasattr(node,'Type') and node.Type or ''
        
        # create an object of the correct kind, or use what's passed to us...
        if self != None:
            pass
        elif objtype == 'PlaneWave' or objtype == 'SampledPlaneWave' or objtype == 'Noise':
            # we need the SourceType to allocate the right kind of Source object
            # use Source.makeSource also for objects (such as Noise) that are just
            # XSILobjects, not quite Sources
            for node2 in node:
                if node2.tagName == 'Param' and node2.Name == 'SourceType':
                    sourcetype = str(node2)
            
            try:
                self = Source.makeSource(sourcetype)
            except AttributeError:
                raise AttributeError, 'XSILobject.makeXSIL(): SourceType undefined for source %s' % objname
        elif objtype == 'TDIObservable':
            self = Observable()
        elif objtype == 'TimeSeries':
            self = TimeSeries()
        elif objtype == 'FrequencySeries':
            self = FrequencySeries()
        elif objtype == 'PlaneWaveTable':
            for node2 in node:
                if node2.tagName == 'Param' and node2.Name == 'SourceType':
                    sourcetype = str(node2)
            
            self = SourceTable(sourcetype)
        else:
            # This includes LISA!
            self = XSILobject()
        
        self.__dict__['Name'] = self.__dict__['name'] = objname
        self.__dict__['Type'] = self.__dict__['xmltype'] = objtype
        
        # now we loop through the children, and save all XSIL children
        # in the 'list' content of this class
        # (by contrast, Params are saved in standard attributes)
        
        self[:] = []
                
        # we assume (but do not enforce) only one Array, one Table, and one comment per XSIL
        # (one the first Array or Table will be used; comments will be consolidated)
        
        self.__dict__['Comment'] = ''
        
        # now get all the parameters, if any
        for node2 in node:
            if node2.tagName == 'Param':
                # we pass the Unit to reprvalue, if we have it, in case it can inform the
                # Python type of the attribute
                setattr(self,node2.Name,self.__reprvalue((str(node2),hasattr(node2,'Unit') and str(node2.Unit) or '')))
                
                if hasattr(node2,'Unit'):
                    setattr(self,node2.Name + '_Unit',str(node2.Unit))                    
                
                if hasattr(node2,'Type'):
                    setattr(self,node2.Name + '_Type',str(node2.Type))
            if node2.tagName == 'Array':
                if not hasattr(self,'Array'):
                    self.__dict__['Array'] = Array.makeArray(node2,xmlfile)
            if node2.tagName == 'Table':
                if not hasattr(self,'Table'):
                    self.__dict__['Table'] = Table.makeTable(node2,xmlfile)
            if node2.tagName == 'Comment':
                self.addComment(str(node2))
            if node2.tagName == 'XSIL':
                retobj = XSILobject.makeXSIL(node2,xmlfile)
                self.append(retobj)
                
                # we also assume (but do not enforce) only one LISAData, SourceData, and TDIData section
                # per XSIL file; only the first will be used...
                
                if not hasattr(self,retobj.Type):
                    self.__dict__[retobj.Type] = retobj
        
        # note: according to the DTD, XSIL may also contain Url, Time, Stream, Element, and XBFDif,
        # but we're not currently handling these
        
        # final parameter check for Source objects
        if hasattr(self,'checkContent'):
            self.checkContent()
        
        return self
    
    makeXSIL = staticmethod(makeXSIL)
    def addComment(self,comments):
        """Add a comment to this XSIL object. Further comments will be
        consolidated into a single string, interspersing newlines."""
        if comments:
            if self.Comment:
                self.Comment += ('\n' + comments)
            else:
                self.Comment = comments
    
    def XML(self,xmlfile,name=None,comments=''):
        """Create the RXP tuple representation for a a generic XSIL object.
        Will process parameters in the order in which they were defined,
        and it will call itself recursively to process XSIL, Array, and
        Table children."""
        
        xsildict = {}
        
        if name or (hasattr(self,'Name') and self.Name):
            xsildict['Name'] = name and name or self.Name
        
        if hasattr(self,'Type') and self.Type:
            xsildict['Type'] = self.Type
        
        self.addComment(comments)
        
        children = []
        
        # go through parameters
        for parname in self.parameters:
            paramdict = {'Name': parname}
            
            if hasattr(self,parname + '_Unit'):
                paramdict['Unit'] = getattr(self,parname + '_Unit')
            
            if hasattr(self,parname + '_Type'):
                paramdict['Type'] = getattr(self,parname + '_Type')
            
            children.append(('Param', paramdict, [getattr(self,parname)]))        
        
        # handle comments        
        if hasattr(self,'Comment') and self.Comment:
            children.append(('Comment', {}, [self.Comment]))
        
        # handle sub-elements
        if hasattr(self,'Array'):
            children.append(self.Array.XML(xmlfile))
        if hasattr(self,'Table'):
            children.append(self.Table.XML(xmlfile))
        for elem in self:
            children.append(elem.XML(xmlfile))
        
        # we're done...
        return ('XSIL', xsildict, children)
    
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
        
        raise ValueError, ('XSILobject.__reprvalue() [internal]: cannot interpret value %s as an int, float, or str' % param[0])
    
    def __call__(self,obj):
        """To reproduce the old API whereby sources and observables were added
        with syntax similar to lisaXMLobj.SourceData(newsource), etc., calling an XSIL
        object as a function has the effect of appending to its XSIL content.
        For instance, writeXMLobj.SourceData(source) will append to the SourceData
        section. The new preferred syntax is writeXMLobj.SourceData.append(source),
        etc., where SourceData, TDIData, etc. are treated as Python lists.
        
        >>> readXMLobj = readXML('test.xml')
        >>> bh = XSILobject('Empty','just an empty object')
        >>> readXMLobj.SourceData(bh)
        >>> readXMLobj.SourceData[2]
        <XSIL Empty 'just an empty object'>
        """
        
        assert isinstance(obj,XSILobject), 'Accepting only XSIL elements as XSIL content.'
        
        self.append(obj)
    
    def __repr__(self):
        """Display this object by showing its Type, Name, and the
        number of its XSIL children."""
        
        Type = self.Type and (" %s" % self.Type) or ''
        name = self.Name and (" '%s'" % self.Name) or ''
        children = len(self) and (' (%s ch.)' % len(self)) or ''
        
        return "<XSIL%s%s%s>" % (Type,name,children)
    


class Stream(object):
    MapStreamsFromDisk = False
    
    def __init__(self,data,filetype='Remote',encoding='Binary'):
        self.Data = data
        
        self.Type = filetype
        self.Encoding = encoding
        
        if isinstance(data,numpy.ndarray):            
            assert data.dtype == 'float64', 'Stream: currently support only arrays of double-precision floats'
        
            # get info about shape to format text output
            if len(data.shape) == 1:
                self.Length, self.Records = data.shape[0], 1
            elif len(data.shape) == 2:
                self.Length, self.Records = data.shape
            else:
                raise NotImplementedError, 'Stream(): data can currently be represented only as 1D or 2D numpy arrays.'
        elif type(data) == str:
            lines = data.split('\n')
            
            self.Records = len(lines[0].split())
            self.Length = len([line for line in lines if len(line.split()) == self.Records])
        else:
            raise NotImplementedError, 'Stream(): I do not know how to handle these Stream data.'
    
    def getChecksum(data,lfname):
        import zlib
        
        if data == None or isinstance(data,numpy.memmap):   # compute the CRC-32 in batches
            bfile = open(lfname,'r')
            checksum = 0
            while True:
                databuffer = bfile.read(262144)
                if databuffer == '': break
                checksum = zlib.crc32(databuffer,checksum)
            bfile.close()                        
        else:
            checksum = zlib.crc32(data)
        
        return checksum
    
    getChecksum = staticmethod(getChecksum)
    
    def makeStream(node,length,records,xmlfile,disableremote=False):
        content = str(node)
        
        # if this flag is set, we will not load the Stream content,
        # but we'll treat it as an external file to copy or link
        if node.Type == 'Remote' and disableremote == True:
            directory = os.path.dirname(xmlfile.filename)
            datafile = directory + '/' + content
            
            if 'http://' in directory or os.path.isfile(datafile):
                return Stream(datafile,node.Type,node.Encoding)
            elif os.path.isfile(content):
                return Stream(content,node.Type,node.Encoding)
            else:
                raise IOError, 'Stream.makeStream(): cannot find Stream file!'
        
        # otherwise let's go on and parse the file
        
        # load data from file
        if node.Type == 'Remote':
            directory = os.path.dirname(xmlfile.filename)
        
            if 'http://' in directory:
                loadfile = urllib.urlopen(directory + '/' + content,'r')
            else:
                try:
                    if not directory:
                        raise
                        
                    loadfile = open(directory + '/' + content,'r')
                except:
                    # if we cannot find the binary relative to the xml,
                    # try relative to the working directory
                    loadfile = open(content,'r')
            
            if 'Binary' in node.Encoding:
                if Stream.MapStreamsFromDisk == True:
                    lfname = loadfile.name; loadfile.close()
                    
                    if xmlfile.intent == 'r':
                        filemode = 'c'
                    elif os.path.isfile(lfname):
                        filemode = 'r+'
                    else:
                        filemode = 'w+'
                    
                    data = numpy.memmap(filename=lfname,dtype='d',mode=filemode,shape=(length,records))
                else:
                    data = numpy.fromfile(loadfile,'double',length * records)
                # previously data = numpy.fromstring(loadfile.read(8 * length * records),'double')
                
                if hasattr(node,'Checksum'):
                    datachecksum = Stream.getChecksum(data,loadfile.name)   # (getting name is OK even if file is closed)
                    filechecksum = int(node.Checksum)
                    
                    if (datachecksum & 0xffffffffL) != (filechecksum & 0xffffffffL):
                        print 'Stream.makeStream(): failed checksum for binary file %s' % loadfile.name
                        raise IOError
                
                # change endianness if needed
                if ( ('BigEndian' in node.Encoding and sys.byteorder == 'little') or
                     ('LittleEndian' in node.Encoding and sys.byteorder == 'big') ):
                    data.byteswap(True)
            elif 'Text' in node.Encoding:
                content = loadfile.read()
            else:
                raise NotImplementedError, 'Stream.makeStream(): currently only remote binary/text and local text Streams are supported.'
            
            loadfile.close()    # (no harm in closing twice)
        elif node.Type == 'Local' and 'Text' in node.Encoding:
            # we have the data already in 'content'
            pass
        else:
            raise NotImplementedError, 'Stream.makeStream(): currently only remote binary/text and local text Streams are supported.'
            
        # process text data
        if 'Text' in node.Encoding:
            if hasattr(node,'Delimiter'):
                for delchar in node.Delimiter:
                    content = string.join(content.split(delchar),' ')
            
            data = numpy.array(map(float,content.split()),'d')
            
        # reshape the data array
        try:
            data = numpy.reshape(data,(length,records))
        except:
            raise IOError, "Stream.makeStream(): Can't reshape %s (%s) to %s x %s." % (len(data),type(data),length,records)
        
        return Stream(data,node.Type,node.Encoding)
    
    makeStream = staticmethod(makeStream)
    def XML(self,xmlfile):
        if self.Type == 'Remote':
            if 'Binary' in self.Encoding:
                filename = xmlfile.nextbinfile()
            
                if isinstance(self.Data,numpy.ndarray):
                    bfile = open(filename,'w')
                    self.Data.tofile(bfile) # previously open(filename,'w').write(writebuffer)
                    bfile.close()
                    
                    # compute the CRC-32 from the file just written, in batches, to avoid MemoryErrors
                    checksum = Stream.getChecksum(None,filename)
                    
                    bfile.close()
                elif type(self.Data) == str:
                    if os.path.abspath(self.Data) != os.path.abspath(filename):
                        if os.path.isfile(filename):    # delete the target binary file if it exists already
                            os.unlink(filename)         # will work also for symbolic links
                        
                        if 'http://' in self.Data:                          # download remote file to the correct local filename
                            urllib.urlretrieve(self.Data,filename)
                        elif os.stat(self.Data)[stat.ST_SIZE] < 50000000:   # copy files below 50M, symlink otherwise
                            shutil.copy(self.Data,filename)
                        else:
                            os.symlink(os.path.abspath(self.Data),filename)
                    
                    if hasattr(self,'Checksum'):
                        checksum = self.Checksum
                    else:
                        checksum = None
                else:
                    raise NotImplementedError, 'Stream.XML(): I do not know how to handle these Stream data.'
                
                attrlist = {'Type': 'Remote',
                            'Encoding': sys.byteorder == 'big' and 'Binary,BigEndian' or 'Binary,LittleEndian'}
                
                if checksum != None:
                    attrlist['Checksum'] = str(checksum)
                
                stream = ('Stream',attrlist,[os.path.basename(filename)])
            elif 'Text' in self.Encoding:               # TO DO: remove repetitive code between 'Binary' and 'Text'...
                filename = xmlfile.nexttxtfile()
                
                if type(self.Data) == numpy.ndarray:
                    fileobj = open(filename,'w')
                
                    linepattern = '%s ' * self.Records + '\n'
                
                    if self.Records == 1:
                        for i in range(0,self.Length):
                            fileobj.write(linepattern % self.Data[i])
                    else:
                        for i in range(0,self.Length):
                            fileobj.write(linepattern % tuple(self.Data[i,:]))
                elif type(self.Data) == str:
                    if os.path.abspath(self.Data) != os.path.abspath(filename):
                        if os.path.isfile(filename):    # delete the target binary file if it exists already
                            os.unlink(filename)         # will work also for symbolic links
                                                    
                        if 'http://' in self.Data:                          # download remote file to the correct local filename
                            urllib.urlretrieve(self.Data,filename)
                        elif os.stat(self.Data)[stat.ST_SIZE] < 50000000:   # copy files below 50M, symlink otherwise
                            shutil.copy(self.Data,filename)
                        else:
                            os.symlink(os.path.abspath(self.Data),filename)
                else:
                    raise NotImplementedError, 'Stream.XML(): I do not know how to handle these Stream data.'
                    
                stream = ('Stream',
                          {'Type': 'Remote', 'Encoding': 'Text'},
                          [os.path.basename(filename)])
        elif self.Type == 'Local' and 'Text' in self.Encoding:
            if type(self.Data) == str:
                textdata = self.Data
            else:
                textdata = ''
                linepattern = '%s ' * self.Records + '\n'
            
                if self.Records == 1:
                    for i in range(0,self.Length):
                        textdata += linepattern % self.Data[i]
                else:
                    for i in range(0,self.Length):
                        textdata += linepattern % tuple(self.Data[i,:])
            
            stream = ('Stream',
                      {'Type': 'Local', 'Encoding': 'Text', 'Delimiter': ' '},
                      [textdata] )
        else:
            # TO DO: currently support only remote binary and local text
            raise NotImplementedError, 'Stream.XML(): currently only remote binary and local text Streams are supported.'
        
        return stream
    

class Table(XMLobject):
    DisableRemoteTableLoad = False
    
    def __init__(self,data,length,records,name='',filetype='Remote',encoding='Binary'):
        """
        # create a Table and save it to disk...
        >>> a = numpy.array([[101,102,103,104],[201,202,203,204]],'d')
        >>> t = Table(a,2,4,name='TestTable')
        >>> t.ParameterOne, t.ParameterTwo, t.ParameterThree, t.ParameterFour = None, None, None, None
        >>> fw = lisaXML('test5.xml','w')
        >>> fw.append(t)
        >>> fw.close()
        
        # load it back and check against it
        >>> fr = lisaXML('test5.xml','r')
        >>> fr.Table.Length, fr.Table.Records
        (2, 4)
        >>> fr.Table.Data
        array([[ 101.,  102.,  103.,  104.],
               [ 201.,  202.,  203.,  204.]])
        >>> fr.Table.parameters
        ['ParameterOne', 'ParameterTwo', 'ParameterThree', 'ParameterFour']
        >>> fr.Table.ParameterOne
        array([ 101.,  201.])
        
        # try with an inline table
        >>> t2 = Table(a,2,4,name='TestTable',filetype='Local',encoding='Text')
        >>> fw = lisaXML('test6.xml','w')
        >>> fw.append(t2)
        >>> fw.close()
        
        # clean up test files
        >>> os.system('rm test5.xml test5-0.bin test6.xml')
        0
        """
        
        super(Table,self).__init__()
        
        self.__dict__['Name'] = name
        
        self.__dict__['Length'] = length
        self.__dict__['Records'] = records
        
        self.__dict__['Stream'] = None
        
        if isinstance(data,numpy.ndarray):
            self.Stream = Stream(data,filetype,encoding)
        elif type(data) == tuple or type(data) == list:
            self.Stream = Stream(numpy.array(data),filetype,encoding)
        elif type(data) == Stream:
            self.Stream = data
        else:
            raise NotImplementedError, 'Table: cannot process data as passed'
                    
        # TO DO: more checks on data here!
    
    def makeTable(node,xmlfile):
        """Doctest for DisableRemoteTableLoad:
        
        # create
        >>> t = Table(numpy.array([[101,102],[201,202]],'d'),2,2,name='TestTable')
        >>> t.ParameterOne, t.ParameterTwo = None, None
        >>> lisaXML('test7.xml','w').append(t)
        
        # load table, process remote file
        >>> t = lisaXML('test7.xml','r').Table
        >>> t.Data, t.parameters
        (array([[ 101.,  102.],
               [ 201.,  202.]]), ['ParameterOne', 'ParameterTwo'])
        
        # do not process remote file
        >>> Table.DisableRemoteTableLoad = True
        >>> f2 = lisaXML('test7.xml','r')
        >>> t2 = f2.Table
        >>> t2.Data, t2.parameters
        ('test7-0.bin', ['ParameterOne', 'ParameterTwo'])
        
        # check writing
        >>> lisaXML('test8.xml','w').append(t2)
        >>> os.system('diff test7-0.bin test8-0.bin')
        0
        
        # writing to the same file
        >>> f2.write()
        
        # restore flag and remove temp files
        >>> Table.DisableRemoteTableLoad = False
        >>> os.system('rm test7.xml test7-0.bin test8.xml test8-0.bin')
        0
        """
        tablename = hasattr(node,'Name') and node.Name or ''
        
        # gather the table dimensions
        dimensions = {}
        
        for node2 in node:
            if node2.tagName == 'Dim':
                dimensions[node2.Name] = int(float(str(node2)))
        
        # if the number of records is not given (naughty you!), we'll count the columns:
        dimensions['Records'] = len([True for node2 in node if node2.tagName == 'Column'])
        
        # now look for the stream
        for node2 in node:
            if node2.tagName == 'Stream':
                stream = Stream.makeStream(node2,dimensions['Length'],dimensions['Records'],xmlfile,disableremote=Table.DisableRemoteTableLoad)
                break
        
        self = Table(stream,dimensions['Length'],dimensions['Records'],name=tablename)
        
        # next, gather the Columns        
        for node2 in node:
            if node2.tagName == 'Column':
                setattr(self,node2.Name,None)
                
                if hasattr(node2,'Unit'):
                    setattr(self,node2.Name + '_Unit',node2.Unit)
                
                if hasattr(node2,'Type'):
                    setattr(self,node2.Name + '_Type',node2.Type)
                else:
                    setattr(self,node2.Name + '_Type','double')
                
        assert len(self.parameters) == dimensions['Records'], 'Table.makeTable(): XML file declares and defines a different number of Columns.'
        
        self.__dict__['Data'] = self.Stream.Data
        
        if not Table.DisableRemoteTableLoad:        
            # assign the columns of the table to the Table attributes
            for i in range(len(self.parameters)):
                setattr(self,self.parameters[i],self.Data[:,i])
        
        return self
    
    makeTable = staticmethod(makeTable)
    def XML(self,xmlfile):
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
        
        assert numpy.all([col[1]['Type'] == 'double' for col in columns]) == True, "Table.XML(): Sorry, currently I can only handle Type='double' columns."
                
        return ('Table',{},
                columns + [ ('Dim', {'Name': 'Length' }, [self.Length]),
                            ('Dim', {'Name': 'Records'}, [self.Records]),
                            self.Stream.XML(xmlfile) ] )
    

class Array(object):
    def __init__(self,data,length,records,name='',atype='double',unit='',filetype='Remote',encoding='Binary'):
        self.Name = name
        self.Type = atype
        self.Unit = unit
        
        self.Length = length
        self.Records = records
        
        if isinstance(data,numpy.ndarray):
            self.Stream = Stream(data,filetype,encoding)
        elif type(data) == tuple or type(data) == list:
            self.Stream = Stream(numpy.array(data).transpose(),filetype,encoding)
        elif type(data) == Stream:
            self.Stream = data
        else:
            raise NotImplementedError, 'Array: cannot process data as passed'
        
        self.checkContent()
        # TO DO: more checks on data here!
    
    def makeArray(node,xmlfile):
        arrayname = hasattr(node,'Name') and node.Name or ''
        arraytype = hasattr(node,'Type') and node.Type or 'double'
        arrayunit = hasattr(node,'Unit') and node.Unit or ''
        
        assert arraytype == 'double', 'Array.makeArray(): currently support only double arrays'
        
        dimensions = {}
        
        for node2 in node:
            if node2.tagName == 'Dim':
                dimensions[node2.Name] = int(float(str(node2)))
        
        for node2 in node:  
            if node2.tagName == 'Stream':
                # TO DO: currently enforce a single Stream for each Array
                stream = Stream.makeStream(node2,dimensions['Length'],dimensions['Records'],xmlfile)
                break
        
        self = Array(stream,dimensions['Length'],dimensions['Records'],name=arrayname,atype=arraytype,unit=arrayunit)
        
        # TO DO: currently we're not implementing Element
        
        return self
    
    makeArray = staticmethod(makeArray)
    def XML(self,xmlfile):
        arraydict = {}
        
        for attr in ('Name','Type','Unit'):
            if hasattr(self,attr) and getattr(self,attr):
                arraydict[attr] = getattr(self,attr)
        
        return ('Array',arraydict,
                [ ('Dim', {'Name': 'Length' }, [self.Length]),
                  ('Dim', {'Name': 'Records'}, [self.Records]),
                  self.Stream.XML(xmlfile) ] )
    
    def checkContent(self):
        if not isinstance(self.Stream.Data,numpy.memmap):
            self.Arrays = [self.Stream.Data[:,col] for col in range(self.Records)]
        
            # see if we can parse the column names
            columnnames = [s.strip(' ') for s in self.Name.split(',')]
            if len(columnnames) == self.Records:
                for col in range(len(columnnames)):
                    if not hasattr(self,columnnames[col]):
                        self.__dict__[columnnames[col]] = self.Arrays[col]
    


# TO DO: should move this on-demand loading code to Stream, and figure out how to access it from here
# TO DO: support Streams that are not newline-separated

class SourceTable(XSILobject):
    def __init__(self,sourcetype='',name='',table=None):
        super(SourceTable,self).__init__('PlaneWaveTable',name)
        
        if sourcetype:
            self.SourceType = sourcetype
                
        if table and type(table) == Table:
            self.__dict__['Table'] = table
            
            if not self.Name:
                # inherit the name of the Table, if given...
                self.__dict__['Name'] = self.__dict__['name'] = self.Table.Name
    
    # I'm not happy with the semantics here... do we have any other generators in lisaxml2?
    def makeTableSources(self):
        if not hasattr(self,'Table'):
            raise StopIteration
        
        stream = self.Table.Stream
        
        if stream.Type == 'Remote' and stream.Encoding == 'Text' and Table.DisableRemoteTableLoad == True:
            if 'http://' in stream.Data:
                tablerows = urllib.urlopen(stream.Data)
            else:
                tablerows = open(stream.Data,'r')
            
            # TO DO: note that currently gzip does not work for urllib objects...
            #        ...will need to download the file locally, which is a shame
            if len(stream.Data) > 3 and stream.Data[-3:] == '.gz':
                import gzip
                tablerows = gzip.GzipFile(fileobj=tablerows,mode='r')
            
            delim = getattr(stream,'Delimiter',' ')
        elif stream.Type == 'Local' or DisableRemoteTableLoad == False:
            tablerows = stream.Data
        else:
            print 'SourceTable.makeTableSource(): wrong table format, or cannot find table data'
            raise IOError        
        
        index = 0
        for row in tablerows:
            if type(row) == str:
                row = map(float,row.split(delim))
            
            newsrc = Source.makeSource(self.Type)
            
            if self.Name:
                newsrc.Name = '%s-%s' % (self.Name,index)
            else:
                newsrc.Name = 'Table source #%s' % index
            
            for par,val in zip(self.Table.parameters,row):
                setattr(newsrc,par,val)
            
            for par in self.Table.parameters:
                parunit = par + '_Unit'
                
                if hasattr(self.Table,parunit):
                    setattr(newsrc,parunit,getattr(self.Table,parunit))
            
            index = index + 1
            
            yield newsrc
        
        raise StopIteration
    
    def getTableSources(self):
        return list(self.makeTableSources())
    


# TODO: it may be good to consolidate some of this code with TimeSeries
class FrequencySeries(XSILobject):
    def __init__(self,data=None,name='',unit='',Cadence=None,FrequencyOffset=None):
        super(FrequencySeries,self).__init__('FrequencySeries',name)
        
        if data != None:
            if type(data) in (tuple,list):
                length, records = len(data[0]), len(data)
            elif type(data) == numpy.ndarray:
                if len(data.shape) == 1:
                    length, records = data.shape[0], 1
                elif len(data.shape) == 2:
                    length, records = data.shape
                else:
                    raise NotImplementedError, 'Sorry, I can work only with 1D or 2D numpy arrays.'
            else:
                raise TypeError, 'Sorry, I cannot parse these FrequencySeries data.'
        
            self.__dict__['Array'] = Array(data,length,records,name,unit)
        
            if Cadence != None:
                self.Cadence = Cadence
                self.Cadence_Unit = 'Hertz'
            
            if FrequencyOffset != None:
                self.FrequencyOffset = FrequencyOffset
                self.FrequencyOffset_Unit = 'Hertz'
    
    def checkContent(self):
        if hasattr(self,'Array'):
            if not hasattr(self,'Bandwidth') and hasattr(self,'Cadence'):
                self.Bandwidth = self.Array.Length * self.Cadence
                self.Bandwidth_Unit = self.Cadence_Unit
            
            if hasattr(self.Array,'Arrays'):
                columnnames = [s.strip(' ') for s in self.Array.Name.split(',')]
                if len(columnnames) == self.Array.Records:
                    for col in range(len(columnnames)):
                        if not hasattr(self,columnnames[col]):
                            self.__dict__[columnnames[col]] = self.Array.Arrays[col]
    

class TimeSeries(XSILobject):
    """TimeSeries alternate, inheriting from XSILobject."""
    
    def __init__(self,data=None,name='',unit='',Cadence=None,TimeOffset=None):
        super(TimeSeries,self).__init__('TimeSeries',name)
        
        if data != None:
            if type(data) in (tuple,list):
                length, records = len(data[0]), len(data)
            elif isinstance(data,numpy.ndarray):
                if len(data.shape) == 1:
                    length, records = data.shape[0], 1
                elif len(data.shape) == 2:
                    length, records = data.shape
                else:
                    raise NotImplementedError, 'Sorry, I can work only with 1D or 2D numpy arrays.'
            else:
                raise TypeError, 'Sorry, I cannot parse these TimeSeries data.'
        
            self.__dict__['Array'] = Array(data,length,records,name,unit)
        
            if Cadence != None:
                self.Cadence = Cadence
                self.Cadence_Unit = 'Second'
        
            if TimeOffset != None:
                self.TimeOffset = TimeOffset
                self.TimeOffset_Unit = 'Second'
    
    def checkContent(self):
        if hasattr(self,'Array'):
            if not hasattr(self,'Duration') and hasattr(self,'Cadence'):
                self.Duration = self.Array.Length * self.Cadence
                self.Duration_Unit = self.Cadence_Unit
            
            if hasattr(self.Array,'Arrays'):
                columnnames = [s.strip(' ') for s in self.Array.Name.split(',')]
                if len(columnnames) == self.Array.Records:
                    for col in range(len(columnnames)):
                        if not hasattr(self,columnnames[col]):
                            self.__dict__[columnnames[col]] = self.Array.Arrays[col]
    

class Observable(XSILobject):
    def __init__(self,name='',timeseries=None,frequencyseries=None,datatype=None):
        """Creates an Observable object."""
        
        super(Observable,self).__init__('TDIObservable',name)
        
        if timeseries != None and isinstance(timeseries,TimeSeries):
            self.__dict__['TimeSeries'] = timeseries
            self.append(timeseries)
            
            if not self.Name:
                # inherit the name of the TimeSeries, if given...
                self.__dict__['Name'] = self.__dict__['name'] = self.TimeSeries.Name
        
        if frequencyseries != None and isinstance(frequencyseries,FrequencySeries):
            self.__dict__['FrequencySeries'] = frequencyseries
            self.append(frequencyseries)
            
            if not self.Name:
                # inherit the name of the TimeSeries, if given...
                self.__dict__['Name'] = self.__dict__['name'] = self.FrequencySeries.Name
        
        if datatype != None:
            self.DataType = datatype
        else:
            # try to guess it...
            isf = ('Xf' in self.Name) or ('Yf' in self.Name) or ('Zf' in self.Name)
            iss = ('Xs' in self.Name) or ('Ys' in self.Name) or ('Zs' in self.Name)
        
            if isf and (not iss):
                self.DataType = 'FractionalFrequency'
            elif iss and (not isf):
                self.DataType = 'Strain'
    
    def __setattr__(self,attr,value):
        if attr in ['TimeSeries','FrequencySeries']:
            # setting TimeSeries or FrequencySeries directly removes the previous value from the list
            if hasattr(self,attr):
                self.remove(getattr(self,attr))
            
            self.__dict__[attr] = value
            self.append(value)
        else:
            super(Observable,self).__setattr__(attr,value)
    
    def checkContent(self):
        if hasattr(self,'TimeSeries') and hasattr(self.TimeSeries,'Array') and hasattr(self.TimeSeries.Array,'Arrays'):
            ar = self.TimeSeries.Array
            columnnames = [s.strip(' ') for s in ar.Name.split(',')]
            if len(columnnames) == ar.Records:
                for col in range(len(columnnames)):
                    self.__dict__[columnnames[col]] = ar.Arrays[col]
        
        if hasattr(self,'FrequencySeries') and hasattr(self.FrequencySeries,'Array') and hasattr(self.FrequencySeries.Array,'Arrays'):
            ar = self.FrequencySeries.Array
            columnnames = [s.strip(' ') for s in ar.Name.split(',')]
            if len(columnnames) == ar.Records:
                for col in range(len(columnnames)):
                    self.__dict__[columnnames[col]] = ar.Arrays[col]
    

class Source(XSILobject):
    """This class represents a lisaXML GW Source object. It inherits from
    XSILobject and has the same attribute/parameter semantics: all
    Source 'Param' elements are represented as attributes of the Python object
    (such as mysource.Frequency), with units stored in aptly named attributes
    (such as mysource.Frequency_Unit).
    
    # creating a new source
    >>> import BBH
    >>> mysystem = BBH.BlackHoleBinary('A BH binary')
    >>> mysystem.Mass1 = 1.0e6
    >>> mysystem.Mass2 = 0.5e6
    >>> outputXML = lisaXML('test5.xml','w')
    >>> outputXML.SourceData.append(mysystem)
    >>> outputXML.close()
    Traceback (most recent call last):
    AttributeError: Source.XML(): missing parameter EclipticLatitude in object <XSIL PlaneWave 'A BH binary'>
    >>> outputXML.opened = 0   # otherwise close() will be called again by __del__
    
    # loading a source from file    
    >>> inputXML = lisaXML('test.xml','r')
    >>> mysystem = inputXML.SourceData[1]
    >>> mysystem, mysystem.Mass1, mysystem.Mass1_Unit
    (<XSIL PlaneWave 'SMBH binary 1.2.1'>, 2897164.7966700001, 'SolarMass')
    
    # clean up test files
    >>> os.system('rm test5.xml')
    0
    
    NOTE ABOUT REQUIRED PARAMETERS
        
    Source.outputlist contains the list of the required parameters that must
    be provided in order for the object to be written to XML; in this list, 
    each parameter is specified by a tuple
    
    ('ParName', 'DefaultParUnit', DefaultValue, 'parameter description')
    
    - If DefaultValue is None, then the parameter must be set explicitly
    (e.g., "bbh.Mass1 = 1e6") before the Source can be written to XML.
    
    - If the Unit is not set explicitly (e.g., "bbh.Mass1_Unit='kg'")
    then the parameter will be taken to be specified w.r.t. 'DefaultParUnit'.
    
    - If the units are not as requested, the package lisaxml.convertunit
    will attempt to some trivial unit conversions.
    
    NOTE ABOUT SOURCE ATTRIBUTES
    
    Any attribute that is defined by user code for a source with a standard
    self.xxx = ... assignment will be included among the Params saved to XML.
    To avoid this behavior, use one of the following workarounds:
    
    - preassign the attribute (to None, for instance) in the __init__ constructor
    for the Source;
    
    - define the attribute as private, by prefixing its name with '__'
    
    - use the direct-to-dict assignment self.__dict__['attributename'] = ... 
    
    NOTE ABOUT WAVEFORM GENERATION
    
    For MLDC purposes, all Source objects should support the function call
    
    waveforms(samples,timestep,initialtime)
    
    (where timestep and initialtime are in seconds) to return hp and hc
    time series as numpy arrays. Currently waveforms assumes that
    all parameters have their default units (and no check is made).
    Conversion to the default units can be attempted by calling
    Source.checkParams()
        
    # all Source objects support this call to get hp and hc polarizations
    # as numpy arrays
    (hp,hc) = mysystem.waveforms(samples,timestep,initialtime)
    """
    
    outputlist = ()    
    def __init__(self,sourcetype='',name=''):
        """Creates a generic Source object. Generally this is called
        implicitly from specific constructors such as BBH and EMRI,
        but it can be used to create generic user-defined lisaXML
        Source objects."""
        
        super(Source,self).__init__('PlaneWave',name)
        
        self.SourceType = sourcetype
    
    def makeSource(sourcetype):
        """[Internal] Returns a Source object of the right type, looking at the
        SourceClassModule dictionary and importing the correct Python
        extension module."""
        
        try:
            module = __import__(SourceClassModules[sourcetype])
            self = getattr(module,sourcetype)()
        except KeyError:
            # print 'Source.makeSource(): unknown object type %s, I will make it a generic source' % sourcetype
            self = Source(sourcetype)
        except ImportError:
            print 'Source.makeSource(): cannot import module %s for object type %s, I will make it a generic source' % (SourceClassModules[sourcetype],sourcetype)
            self = Source(sourcetype)
        
        # here we used to erase the list of attributes assigned in __init__, but this obliterated SourceType, which ain't nice
        # self.parameters = []
        
        return self
    
    makeSource = staticmethod(makeSource)    
    def checkContent(self):
        """[Internal] Verify that this Source has all the required parameters, and
        convert them to standard units."""
        
        for arg in self.outputlist:
            # see if argument was assigned to object            
            if hasattr(self,arg[0]):
                argvalue = getattr(self,arg[0])
                
                if hasattr(self,arg[0] + '_Unit'):
                    argvalue = (argvalue,getattr(self,arg[0] + '_Unit'))
            else:
                # if not, try to convert argument from other arguments                  
                argvalue = convertunit.convertParam(self,arg)
                
                if argvalue == None and arg[2] != None:
                    # if that does not work, try to use default value
                    argvalue = arg[2]
                else:
                    raise AttributeError, 'Source.XML(): missing parameter %s in object %s' % (arg[0],self)
            
            # check if the parameter already has a unit            
            if isinstance(argvalue,tuple):
                # convert it to the standard unit if necessary
                if argvalue[1] != arg[1]:
                    argvalue = convertunit.convertUnit(argvalue[0],argvalue[1],arg[1],arg[0])
            else:
                # assume the standard unit otherwise
                argvalue = (argvalue,arg[1])
            
            # (note: convertParam and convertUnit return argument with unit as tuple)
            setattr(self,arg[0],argvalue[0])
            setattr(self,arg[0] + '_Unit',argvalue[1])
        
        # leave the other (optional) parameters alone...
        
        # also, if I have a TimeSeries, then I am a SampledPlaneWave
        if hasattr(self,'TimeSeries'):
            self.__dict__['xmltype'] = self.__dict__['Type'] = 'SampledPlaneWave'
    
    def XML(self,xmlfile,name=None,comments=''):
        """[Internal] Generate XML for this Source; calls the standard XSILobject.XML,
        but only aftering running checkContent."""
        
        # verify that we have all required parameters and convert them to standard units
        self.checkContent()
        
        # any included TimeSeries will be handled as standard XSILcontent
        return XSILobject.XML(self,xmlfile,name,comments)    
    

# obsolete (but left for backward compatibility)
class LISA(XSILobject):
    """This class inherits from XSILobject, and it represents the pseudo-LISA
    geometry. LISA objects are read from and added to lisaXMLobj.LISAData, which
    behaves like a list.
    
    It is also possible to use the legacy API, getLISAgeometry()
    
    # output
    >>> lisaXMLobj = lisaXML('test4.xml','w')
    >>> lisa = LISA('My MLDC PseudoLISA')
    >>> lisa.TimeOffset      = 0; lisa.TimeOffset_Unit      = 'Second'
    >>> lisa.InitialPosition = 0; lisa.InitialPosition_Unit = 'Radian'
    >>> lisa.InitialRotation = 0; lisa.InitialRotation_Unit = 'Radian'
    >>> lisa.Armlength = 16.6782; lisa.Armlength_Unit       = 'Second'
    >>> lisaXMLobj.LISAData(lisa)
    >>> lisaXMLobj.close()
    
    # input
    >>> lisaXMLobj = lisaXML('test4.xml','r')
    >>> lisa = lisaXMLobj.getLISAgeometry()
    >>> lisa, lisa.Armlength
    (<XSIL PseudoLISA 'My MLDC PseudoLISA'>, 16.6782)
    
    # clean up test files
    >>> os.system('rm test4.xml')
    0
    """
    
    def __init__(self,name=''):
        super(LISA,self).__init__('PseudoLISA',name)


class writeXML(object):
    """A basic XML-writing class. Used by lisaXML for output."""
    
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
            # always put 'Name' first, and then 'Type'
            
            if 'Name' in attrs.keys():
                string += ' Name="' + attrs['Name'] +'"'
            
            if 'Type' in attrs.keys():
                string += ' Type="' + attrs['Type'] +'"'
            
            for attr in attrs.keys():
                if attr not in ('Name','Type'):
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
    
    def printstream(self,stream):
        if stream:
            print >> self.f, stream,
        
            if stream[-1] != '\n':
                print >> self.f
    
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
                    # hack: don't indent Streams
                    if rxpexp[0] == 'Stream':
                        self.printstream(elem)
                    else:
                        self.content(elem)
            
            self.closetag(rxpexp[0])
        else:
            # I am a singleton
            self.singletag(rxpexp[0],rxpexp[1])
    


def readXML(filename,validate=False):
    """This is just a utility function that calls lisaXML(filename,'r'),
    provided to implement the old readXML API."""
    
    return lisaXML(filename,intent='r',validate=validate)

class lisaXML(XSILobject):
    """This is the Python object representation of a lisaXML file.
    This class can be used to load and parse lisaXML files, modify them,
    and write them back to disk. It can also be used to create lisaXML
    files from scratch.
    
    The lisaXML files are represented as a set of nested Python XSILobject
    structures, that have attributes corresponding to XSIL Param's, Array,
    and Table elements, and that contain (just as Python lists) any enclosed
    XSIL elements. To be concrete: the object returned by lisaXML represents
    the 'root' XSIL element, which has the file metadata as its attributes, 
 
    >>> readXMLobj = lisaXML('test.xml','r')
    >>> readXMLobj
    <lisaXML file 'test.xml' (LISAData, SourceData, TDIData)>
    >>> readXMLobj.Author
    'Michele Vallisneri'
    >>> readXMLobj.GenerationDate, readXMLobj.GenerationDate_Type
    ('2007-07-24T15:50:02PDT', 'ISO-8601')
    
    and the sections of the lisaXML file as its XSIL children:
    
    >>> readXMLobj[:]
    [<XSIL LISAData (1 ch.)>, <XSIL SourceData (2 ch.)>, <XSIL TDIData (1 ch.)>]
    
    These may be accessed by name, and contain (respectively) PseudoLISA
    descriptors, PlaneWave objects, and Observable objects. 
    
    >>> readXMLobj.LISAData
    <XSIL LISAData (1 ch.)>
    >>> readXMLobj.LISAData[0]
    <XSIL PseudoLISA 'Standard MLDC LISA'>
    >>> readXMLobj.SourceData[0]
    <XSIL PlaneWave 'Galactic binary 1.1.1a'>
    
    This list semantics supports the addition of new children
    (e.g., additional sources, or TDI observables) to the relevant lisaXML
    section:
    
    >>> readXMLobj.SourceData
    <XSIL SourceData (2 ch.)>
    >>> bh = Source('Empty','Just an empty object')
    >>> readXMLobj.SourceData.append(bh)
    >>> readXMLobj.SourceData
    <XSIL SourceData (3 ch.)>
    >>> readXMLobj.SourceData[2] 
    <XSIL PlaneWave 'Just an empty object'>
    
    See Source, Observable, and LISA for more info.
    """
    
    def __init__(self,filename,intent='w',author='',date='',comments='',validate=False):
        """Initialize a lisaXML object for input/output.
        
        - If intent='w', a new lisaXML file is created with metadata
          [author,comments] (date is currently set from the system clock, so
          this argument has no effect).
        
        - If intent='r', read and parse lisaXML file; in this case 'filename' may
          begin with 'http://' to point to a remote file on the web.
          Setting 'validate' to true enables the validation of the XML with any
          DTD statement that it may contain. The metadata will be disregarded.
          
        - If intent='rw', file will be read/parsed, and saved on closure* with
          any modifications (* when the reference to the file falls out of scope,
          or the file is deleted).
          
        Note that even 'r'-intent files may be written by calling write() explicitly.
        
        For intent='r', the metadata will be disregarded, as will be 'validate'
        for intent='w'.
            
        Doctests:
        
        # opening file for input    
        >>> readXMLobj = lisaXML('test.xml','r')
        >>> readXMLobj.Comment
        'lisaXML 1.0 [M. Vallisneri, June 2006]'
        
        # adding a comment and writing with a different filename
        >>> readXMLobj.addComment('An additional comment!')
        >>> readXMLobj.Comment
        'lisaXML 1.0 [M. Vallisneri, June 2006]\\nAn additional comment!'
        >>> readXMLobj.write('test2.xml')
        
        # loading a file from the web
        >>> readXMLobj2 = lisaXML('http://www.vallis.org/test.xml','r')
        
        # creating a file from scratch (and reading it back as a check)
        >>> writeXMLobj = lisaXML('test3.xml','w',author='Michele',comments='Not much here')
        >>> writeXMLobj.close()
        >>> lisaXML('test3.xml','r').Comment
        'Not much here'
        
        # clean up test files
        >>> os.system('rm test2.xml test3.xml')
        0
        """
        
        super(lisaXML,self).__init__()
        
        self.__dict__['filename'] = filename
        self.__dict__['directory'] = os.path.dirname(filename)
        
        self.__dict__['intent'] = intent
        
        if 'r' in intent:
            p = pyRXP.Parser()
        
            # read from the web or from local file
            if 'http://' in filename:
                lines = urllib.urlopen(filename).read()
            
                # TO DO: could use better recognition of 404-file non found
                if '<?xml' not in lines:
                    raise IOError, 'lisaXML(): I think I am having trouble getting this file!'
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
                raise TypeError, 'lisaXML(): Not a LISA XSIL file!'
        
            tw = xmlutils.TagWrapper(tree)
        else:
            tw = xmlutils.TagWrapper(('XSIL', {}, [], None))
            
        XSILobject.makeXSIL(tw,self,self=self)
        
        # assign meta data if given
        if author:
            self.Author = author 
        if date:
            print >> sys.stderr, "lisaXML() [warning]: Sorry, the new API does not allow for setting the date explicitly! I'll do it for you from the clock."
        
        self.addComment(comments)
        
        # set up the standard lisaXML sections if not present
        
        for sec in ('LISAData','SourceData','TDIData','NoiseData'):
            if not hasattr(self,sec):
               newsec = XSILobject(sec)
               
               self.append(newsec)
               self.__dict__[sec] = newsec
        
        self.__dict__['opened'] = 1
    
    def __del__(self):
        if self.opened == 1:
            self.close()
    
    def close(self):
        """Closes the file if still open, after writing it to disk if 'w'
        was part of the initial intent."""
        
        if self.opened == 1 and 'w' in self.intent:
            self.write()        
        
        self.opened = 0
    
    def write(self,filename='',author='',comments=''):
        """Write the lisaXML file to disk using the 'filename'
        given, and replacing/augmenting the metadata if
        'author' and 'comments' are given."""
        
        # update metadata        
        if author:
            self.Author = author
        
        self.GenerationDate = time.strftime('%Y-%m-%dT%H:%M:%S%Z',time.localtime())
        self.GenerationDate_Type = 'ISO-8601'
        
        self.addComment(comments)
            
        if filename == '':
            filename = self.filename
        
        if not re.match('.*\.xml$',filename):
            filename += '.xml'
        
        # clean up the standard sections if empty
        # note: cannot delete list items conditionally while enumerating the list... it yields unexpected behavior in Python
        #       so we first select the items to be removed 
        for child in filter(lambda c: isinstance(c,XSILobject) and c.Type in ('LISAData','SourceData','TDIData','NoiseData') and len(c) == 0,self):
            delattr(self,child.Type)
            self.remove(child)
        
        # reset bin and txt file counters
        self.__dict__['binfilecounter'] = 0
        self.__dict__['txtfilecounter'] = 0
        self.__dict__['writefilename'] = filename
                
        # now write!
        writexml = writeXML(filename)
        
        writexml.content('<?xml version="1.0"?>')
        writexml.content('<!DOCTYPE XSIL SYSTEM "http://www.vallis.org/lisa-xml.dtd">')        
        writexml.content('<?xml-stylesheet type="text/xsl" href="lisa-xml.xsl"?>')
        
        writexml.outputrxp(self.XML(self))
        
        writexml.close()
        
        # remove all bin and txt files (at least the first 100)        
        while True:
            nextbin = self.nextbinfile()        
            if os.path.isfile(nextbin):
                os.unlink(nextbin)
            else:
                break
        
        while True:
            nexttxt = self.nexttxtfile()        
            if os.path.isfile(nexttxt):
                os.unlink(nexttxt)
            else:
                break
    
    def getMetaData(self):
        """Returns Author, GenerationDate, Comment (may be empty strings if
           not provided). The preferred interface is to access the lisaXML
           instance attributes Author, GenerationDate, and Comment directly."""
           
        return self.Author, self.GenerationDate, self.Comment
    
    def getLISAGeometry(self):
        """Return the first LISA object defined in the XML (in the LISAData
        section). It's better to access LISAData[0] (i.e., the first PseudoLISA
        element in the LISAData section of the lisaXML file) directly.
           
        Doctests:
         
        >>> readXMLobj = lisaXML('test.xml','r')
        >>> lisa = readXMLobj.LISAData[0]
        >>> lisa.Name
        'Standard MLDC LISA'
        >>> lisa.Armlength, lisa.Armlength_Unit
        (16.6782, 'Second')
        """
        
        if len(self.LISAData) > 0:
            return self.LISAData[0]
        else:
            return None
    
    def getLISASources(self):
        """Return a list of Source objects defined in the XML. It's better to access
        the SourceData attribute directly.
        
        Doctests:
        
        >>> readXMLobj = lisaXML('test.xml','r')
        >>> sources = readXMLobj.SourceData
        >>> len(sources)
        2
        >>> sources[0].name, sources[0].SourceType, type(sources[0])
        ('Galactic binary 1.1.1a', 'GalacticBinary', <class 'GalacticBinary.GalacticBinary'>)
        >>> sources[0].Frequency, sources[0].Frequency_Unit
        (0.0010627314430000001, 'Hertz')
        >>> sources[1].name, sources[1].SourceType, type(sources[1])
        ('SMBH binary 1.2.1', 'BlackHoleBinary', <class 'BBH.BlackHoleBinary'>)
        >>> sources[1].CoalescenceTime, sources[1].CoalescenceTime_Unit
        (13374027.9811, 'Second')
        """
        
        return self.SourceData
    
    def getTDIObservables(self):
        """Return a list of the Observable objects defined in the XML. It'
        better to access the TDIData attribute directly.
        
        Doctests:
        
        >>> readXMLobj = lisaXML('test.xml','r')
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
        
        return self.TDIData
    
    def nextbinfile(self):
        """[Internal] Yields the filename of the next binary file to be used for Streams."""
        
        filename = re.sub('\.xml$','-' + str(self.binfilecounter) + '.bin',self.writefilename)
        self.binfilecounter += 1
        return filename
    
    def nexttxtfile(self):
        """[Internal] Yields the filename of the next binary file to be used for Tables."""
        
        filename = re.sub('\.xml$','-' + str(self.txtfilecounter) + '.txt',self.writefilename)
        self.txtfilecounter += 1
        return filename
    
    def __repr__(self):
        """[Internal] The print-out representation for lisaXML files."""
        
        # enumerate non-empty sections
        secs = string.join([child.Type for child in self if len(child) > 0],', ')
        
        return "<lisaXML file '%s'%s>" % (self.filename,secs)    
    
    # deprecated:
    def getLISAgeometry(self):
        """[Deprecated] See getLISAGeometry()"""
        
        return self.getLISAGeometry()
    


def _test():
    import doctest
    doctest.testmod()

if __name__ == "__main__":
    _test()
