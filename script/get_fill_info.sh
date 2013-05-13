#!/bin/sh
#
# Various markers to mix:
#
# -start "ev-lumi" -stop "ev-lumi-off" 
# -start "softev-physics-on" -stop "softev-physics-off" \
#

#source ~/.login

export PATH=/usr/bin:/bin:/sbin:/usr/sbin:/usr/dt/bin:/usr/proc/bin:/usr/local/bin:/usr/controls/bin:/usr/controls/scripts:/usr/X11R6/bin:/usr/local/share/sybase150/OCS-15_0/bin:/usr/local/src/Frame2html/bin:/usr/local/src/Frame2html/src:.

export LD_LIBRARY_PATH=/usr/X11R6/lib:/lib:/usr/lib:/usr/local/lib:/usr/local/share:/usr/local/share/lib:/usr/local/ActiveTcl/lib:/usr/local/share/sybase150/OCS-15_0/lib:/usr/local/share/sybase/OCS-12_5/lib:/ride/release/X86/lib

FILL_ID=$1

#exportLoggerData -logger RHIC/BeamIons,RHIC/PowerSupplies/rot-ps,RHIC/PowerSupplies/snake-ps \
#-cells "bluDCCTtotal,yelDCCTtotal,bi5-rot3-outer,yo5-rot3-outer,yi7-rot3-outer,bo7-rot3-outer,bo3-snk7-outer,yi3-snk7-outer" \
#-timeformat "unix" -dataformat "%10.2f" -excluderowswithholes -start "ev-lumi" -stop "ev-lumi-off" \
#-fill $FILL_ID | gawk 'NR%60!=0 {next} {print}'

exportLoggerData -logger RHIC/BeamIons,RHIC/PowerSupplies/rot-ps,RHIC/PowerSupplies/snake-ps \
-cells "bluDCCTtotal,yelDCCTtotal,bi5-rot3-outer,yo5-rot3-outer,yi7-rot3-outer,bo7-rot3-outer,bo3-snk7-outer,yi3-snk7-outer" \
-timeformat "unix" -dataformat "%10.2f" -excluderowswithholes -start "ev-lumi" -stop "softev-physics-off" \
-fill $FILL_ID | gawk 'NR%60!=0 {next} {print}'
