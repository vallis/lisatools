from enthought.traits.api import HasTraits, Float, String
# from enthought.traits.ui.api import *

defaultvalues = {'Radian': 0.0,
                 '1': 1.0,
                 'Hertz': 1e-3,
                 'Second': 0,
                 'Parsec': 1e9,
                 'SolarMass': 1e6}

def InteractiveMakeSource(classorinstance,sourcename=''):
    if type(classorinstance) == type:
        # we're working with a class; hope it's a Source
        classtype = classorinstance
    else:
        # assume we're working with an instance; hope it's a Source
        # (ask for forgiveness later)
        classtype = classorinstance.__class__
        
        # allow name override
        if not sourcename:
            sourcename = classorinstance.name
    
    class TraitedSource(HasTraits):
        SourceName = String(sourcename,desc='source name',label='name of %s [String]' % classtype.__name__)
        
        for parameter in classtype.outputlist:
            if hasattr(classorinstance,parameter[0]):
                defval = getattr(classorinstance,parameter[0])
            else:
                try:
                    defval = defaultvalues[parameter[1]]
                except:
                    defval = parameter[2]                
            
            if hasattr(classorinstance,parameter[0] + '_Unit'):
                unitval = getattr(classorinstance,parameter[0] + '_Unit')
            else:
                unitval = parameter[1]
                
            # set the input label
            parlabel = '%s [%s]' % (parameter[0], unitval)
            
            # create the traits object
            exec('%s = Float(defval,desc=parameter[3],label=parlabel)' % parameter[0])
    
    traitsource = TraitedSource()
    traitsource.configure_traits()

    if type(classorinstance) == type:
        newsource = classtype(traitsource.SourceName)
    else:
        newsource = classorinstance
    
    for parameter in classtype.outputlist:
        setattr(newsource,parameter[0],getattr(traitsource,parameter[0]))
        setattr(newsource,parameter[0] + '_Unit',parameter[1])
    
    return newsource