#!/bin/bash
#
# $run_asym_alpha.sh <meas_list>
#
# where <meas_list> is a file with a list of alpha calibration measurements
# without the .data suffix

source $CNIPOL_DIR/script/bgx_asym.sh

FILE_MEASLIST=$1

exec < $FILE_MEASLIST

while read meas_id
do
   arr=($meas_id)
   if [ -z "${arr[1]}" ]; then
      #echo "polid NOT defined"
      bgx_limit 10 $CNIPOL_DIR/build/asym --alpha --use-db -r $meas_id
   else
      #echo "polid defined:" ${arr[1]}
      bgx_limit 10 $CNIPOL_DIR/build/asym --alpha --use-db --no-update-db --pol-id=${arr[1]} -r ${arr[0]}
   fi
done

# Wait until all processes are finished
bgx_wait
