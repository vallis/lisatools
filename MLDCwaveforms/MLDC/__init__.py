from sys import stderr
from glob import glob
from re import sub

# does not seem to work correctly right now...

for p in glob(__path__[0] + '/*.py'):
    if not '__init__.py' in p:
        modulename = sub('.*/(.*)\.py','MLDC.\\1',p)
        
        try:
            exec(modulename + ' = __import__(modulename)')
        except:
            print >> stderr, "Problems importing %s" % modulename
            raise
