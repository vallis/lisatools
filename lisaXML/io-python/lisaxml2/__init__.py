"""
This Python module provides reading and writing functionality for lisaXML
files. Reading lisaXML files is achieved through the "readXML" class, while
writing is achieved through the "lisaXML" class (see their documentation).

The following is a nonexhaustive list of classes that represent various
lisaXML data elements (see their documentation):

- "Source" represents a GW source, with its parameters (and their units)
  stored as attributes. Source may also include a "TimeSeries" attribute for
  GW sources specified as arrays of polarizations. "Source" is generally
  instantiated through one of its derived classes, such as "BBH" or "EMRI".

- "Observable" represents a set of TDI observables, and includes a TimeSeries
  attribute that actually contains numpy arrays.

- "TimeSeries" represents a time series or set of time series with the same
  length, cadence, and time offset. It serves as storage for either Observable
  or Source.

- "LISA" represents a LISA geometry object.
"""

from lisaxml import *
