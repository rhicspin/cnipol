#!/bin/sh
#
# 
#

source ~/.login

set FILL_ID=$1

exportLoggerData -logger RHIC/BeamIons,RHIC/PowerSupplies/rot-ps,RHIC/PowerSupplies/snake-ps \
-cells "bluDCCTtotal,yelDCCTtotal,bi5-rot3-outer,yo5-rot3-outer,yi7-rot3-outer,bo7-rot3-outer,bo3-snk7-outer,yi3-snk7-outer" \
-timeformat "unix" -dataformat "%10.2f" -excluderowswithholes -start "ev-lumi" -stop "ev-lumi-off" \
-fill $FILL_ID | gawk 'NR%60!=0 {next} {print}'
