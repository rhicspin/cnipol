#!/bin/bash
#
# >run_alpha.sh <runlist>
#
# <runlist> is a file with a list of alpha runs. No .data suffix

source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

FILE_MEASLIST=$1

exec < $FILE_MEASLIST

while read meas_name
do
   arr=($meas_name)
   #echo ${arr[0]} --- ${arr[1]} ---
   if [ -z "${arr[1]}" ]; then
      #echo "polid NOT defined"
      bgx_limit 10 $CNIPOL_DIR/bin/asym --alpha --use-db -g -r $meas_name
   else
      #echo "polid defined" ${arr[1]}
      #bgx_limit 10 $CNIPOL_DIR/bin/asym --alpha --use-db -g --pol-id=${arr[1]} -r ${arr[0]}
      bgx_limit 10 $CNIPOL_DIR/bin/asym --alpha --use-db -g --copy --alpha-sources=${arr[1]} -r $meas_name
      #bgx_limit 10 $CNIPOL_DIR/bin/asym --alpha --alpha-sources=${arr[1]} -r $meas_name
   fi
done

# Wait until all processes are finished
bgx_wait
