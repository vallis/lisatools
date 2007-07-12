The Galactic Binary Pipeline
============================
The scripts and codes presented in this pipeline were cut from the
v2.1.1 distribution of THE LISA SIMULATOR.  The pipeline will
create time series files and lisaXML files which will be suitable for
input into either THE LISA SIMULATOR or SYNTHETIC LISA.


Pipeline input files
====================
To run the MLDC binary pipeline, you must create two base files which
will serve as the initial data for the pipeline: "gb.txt" and
"sources.txt".

"gb.txt": This is a file which enumerates the parameters of the galactic
binaries of interest.  The columns in the file should be:

   * Col 1: Binary identifier (name or other
   identifying tag for the binary)

   * Col 2: ecliptic LATITUDE (in radians)

   * Col 3: ecliptic LONGITUDE (in radians)

   * Col 4: principle polarization angle (in radians)

   * Col 5: binary amplitude

   * Col 6: inclination angle (in radians)

   * Col 7: GW frequency of the binary (in Hz)

   * Col 8: orbital phase (in radians)

Several caveats to note with this file's preparation:

   >> Column 2 is ECLIPTIC LATITUDE, ranging from
   +90degrees at the north ecliptic pole to -90degrees at the
   ecliptic south pole.

   >> The amplitude in Column 5 can be computed from standard binary
   parameters such as masses m_1 and m_2, the orbital
   separation r between binary components, and the distance D
   to the binary as ${\cal A}$ = 2 (G^2/c^4) m_1 m_2/(r D).
   In some instances the orbital period P is known rather than the
   orbital separation r.  In these instances, an application of
   Kepler's Third Law is in order: G M_tot = (2 \pi/P)^2  r^3.


"sources.txt": This is a file which simply has a list of the binary
names which should be processed from "gb.txt".

   * Col 1: Binary identifier (name or other identifying tag
     for the binary)

The column in this file is identical to column 1 in "gb.txt" if you
want all the sources to be processed.

Pipeline codes
==============
There are two codes included in this pipeline, along with two scripts 
to compile and run the pipeline.  To use the pipeline, you simply
have to run the two scripts.  The two scripts are:

   * gbPipelineCompile: Compiles the two codes in
    preparation for processing the list of binaries.
    
   * gbPipelineRun : Runs the two codes to process the
    list of binaries into barycenter waveforms and lisaXML files.

Note that if you are creating data streams which have altered cadences
or observation times, those elements will need to be changed in the
"LISAconstants.h" file and the pipeline will need to be
recompiled.  Similar changes will also need to be made in the
simulator you choose to use for creating the LISA signal output.

Pipeline output
================
This pipeline produces h+ and hx for each
source at the Solar Barycenter and saves the time-series in
the Binary subdirectory. The xml wrapper for each time-series
is written to the XML subdirectory.  These files will need to be
provided to the LISA simulation software to produce a final LISA
signal output suitable for data analysis research.
