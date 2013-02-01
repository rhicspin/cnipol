#!/bin/sh
#
# 
#

FILE_FILLS=$1

exec < $FILE_FILLS

while read fill_id
do

exportLoggerData -logger RHIC/BeamIons,RHIC/PowerSupplies/rot-ps,RHIC/PowerSupplies/snake-ps \
-cells "bluDCCTtotal,yelDCCTtotal,bi5-rot3-outer,yo5-rot3-outer,yi7-rot3-outer,bo7-rot3-outer,bo3-snk7-outer,yi3-snk7-outer" \
-timeformat "unix" -dataformat "%10.2f" -excluderowswithholes -start "ev-lumi" -stop "ev-lumi-off" -fill $fill_id | gawk 'NR%60!=0 {next} {print}' > cdev_$fill_id

done
