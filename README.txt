Welcome to lisatools! If you've successfully checked out this archive from the googlecode SVN, you’re almost ready to run the master installer script in lisatools. Before you do that, you will need to decide where the compiled lisatools libraries should be installed (for instance, /usr/local, or /home/yourname), and set some environment variables. For instance,

export LISATOOLS=/home/yourname
export PYTHONPATH=$LISATOOLS/lib/python2.X/site-packages:$PYTHONPATH
export PATH=$LISATOOLS/bin:$PATH

on bash, or

setenv LISATOOLS /home/yourname
setenv PYTHONPATH $LISATOOLS/lib/python2.X/site-packages:$PYTHONPATH
setenv PATH $LISATOOLS/bin:$PATH

on tcsh. In these commands, replace "python2.X" with the version of Python on your system (just run "python" to see what you have; exit with CTRL-D).
These paths will need to be set whenever you begin a lisatools session, so you can work the commands above into your .profile or .tcshrc.

Great! We’re all set. Cd into lisatools, and run

python master-install.py --prefix=$LISATOOLS

This will install:

    * numpy 1.0 (a Python array library)
    * pyRXP 1.11 (a Python XML library)
    * SWIG 1.3.31 (a C/Python interface generator)
    * Michele’s lisaxml (a lisaXML parsing library)
    * Stas’s BBH and EMRI signal generation packages (C++ codes with Python interfaces)
    * Neil’s Fast Galaxy code (C codes and precompiled executables)
    * Michele’s Synthetic LISA
    * Neil’s LISA Simulator (two separate versions, compiled for one-year and two-year datasets, will be installed in $LISATOOLS/lisasimulator-1year and  $LISATOOLS/lisasimulator-2year respectively)

In fact, the master-install.py script will check if all these packages are already available in the system, and install them only if they are not. There are two exceptions, however:

- SWIG will be installed if an older version is detected
- the script will look for GSL (the GNU Scientific Library) under $LISATOOLS. If it does not find it, it will ask you to specify its location with a command line argument --gsl=GSLDIR (for instance, --gsl=/usr/local, if the GSL libraries are in /usr/local/lib). If you don't have it or wish to reinstall it under $LISATOOLS, the script will install v1.8 if you give it the --installgsl option.

You should be all set! If something went wrong, the script will complain in a way that suggests a solution. If not, you should complain to vallis@vallis.org.
