#!/bin/bash

source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

export SSH_AUTH_SOCK=/tmp/ssh-qhSiH31025/agent.31025
export SSH_AGENT_PID=31026

FILE_MEASLIST=$1

exec < $FILE_MEASLIST

while read meas_name
do
   #bgx_limit 10 ./bin/asym -r $meas_name --calib --profile --use-db -g --copy
   #bgx_limit 10 ./bin/asym -r $meas_name --calib --profile --use-db --copy
   #bgx_limit 10 ./bin/asym -r $meas_name --calib --profile --use-db --pmt --raw-ext --asym --kinema -g --copy 
   #bgx_limit 10 ./bin/asym -r $meas_name --calib --profile --use-db -g --copy --raw --asym --disable-det=101101 --sfx=hama
   #bgx_limit 10 $CNIPOL_DIR/bin/asym -r $meas_name --calib --profile --use-db --raw-ext --asym --pmt --kinema -g
   bgx_limit 10 $CNIPOL_DIR/bin/asym -r $meas_name --calib --profile --use-db --raw-ext --asym --pmt --kinema
done

# Wait until all processes are finished
bgx_wait
