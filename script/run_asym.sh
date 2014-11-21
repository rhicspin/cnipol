#!/bin/bash
#
# $run_asym.sh <meas_list>
#
# where <meas_list> is a file with a list of measurements without the .data suffix

source $CNIPOL_DIR/script/bgx_asym.sh

FILE_MEASLIST=$1

exec < $FILE_MEASLIST

while read meas_id
do
   bgx_limit 10 $CNIPOL_DIR/build/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -r $meas_id
done

# Wait until all processes are finished
bgx_wait
