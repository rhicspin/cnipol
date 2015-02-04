#!/bin/bash
#
# $run_asym.sh <meas_list>
#
# where <meas_list> is a file with a list of measurements without the .data suffix

source $CNIPOL_DIR/script/bgx_asym.sh


while read meas_id
do
   bgx_limit 10 $CNIPOL_DIR/build/asym --update-db -g -r $meas_id

done < listofMeasId_run13.txt

# Wait until all processes are finished
bgx_wait
