#!/bin/bash
#
# >run_alpha.sh <runlist>
#
# <runlist> is a file with a list of alpha runs. No .data suffix

source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

#export SSH_AUTH_SOCK=/tmp/ssh-qhSiH31025/agent.31025
#export SSH_AGENT_PID=31026

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
      bgx_limit 10 $CNIPOL_DIR/bin/asym --alpha --use-db -g --alpha-sources=${arr[1]} --sfx=new -r $meas_name
   fi
done

# Wait until all processes are finished
bgx_wait
