#!/bin/csh

if ( ${#argv} != 2 ) then
echo "Not enough arguments: ./Galaxy.csh seed xml_root"
exit
endif

# make a random galaxy (takes several minutes)
./Galaxy_Maker $1 1

# make an XML key file linking to the source parameters
./Galaxy_key $2 $1

# compute the instrument response (takes a couple of hours)
./Fast_Response $1 

# make noise free xml and bin for LISA Simulator
./Fast_XML_LS $2 Data/Sources_LS.txt

# make noise free xml and bin for Synthetic LISA
./Fast_XML_SL $2 Data/Sources_SL.txt
