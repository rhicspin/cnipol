#!/bin/bash
#
# $run_asym.sh <meas_list>
#
# where <meas_list> is a file with a list of measurements without the .data suffix

source $CNIPOL_DIR/script/bgx_asym.sh
CNIPOL_ONLINE_DIR=/usr/local/polarim/root

while read meas_id
do
   bgx_limit 10 $CNIPOL_DIR/build/asym --update-db -g -r $meas_id
done < tempMeaslist_run15.txt

# Wait until all processes are finished
bgx_wait
