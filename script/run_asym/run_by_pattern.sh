source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

#bgx_limit 3 sleep 10

export SSH_AUTH_SOCK=/tmp/ssh-qhSiH31025/agent.31025
export SSH_AGENT_PID=31026

RUN_PATTERN=$1

ORIG_WD=`pwd`

cd $CNIPOL_DATA_DIR

RUN_LIST=(`ls $1`)

cd $ORIG_WD

echo "Will process ${#RUN_LIST[@]} runs"

for file_name in ${RUN_LIST[@]}
do
   run_name=${file_name%.data} 
   echo -n "$run_name "
done

echo

for file_name in ${RUN_LIST[@]}
do
   run_name=${file_name%.data} 
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db -g --copy
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db -g --pmt --copy
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db -g --copy --disable-det=101101 --sfx=hama --raw
   #bgx_limit 16 $CNIPOL_DIR/bin/asym -r $run_name --calib --profile --use-db --raw --pulser --asym -g --copy --pmt --kinema
   bgx_limit 16 $CNIPOL_DIR/bin/asym -r $run_name --calib --profile --use-db --raw-ext --pulser --asym -g --pmt --kinema
   #bgx_limit 16 $CNIPOL_DIR/bin/asym -r $run_name --calib --profile --use-db --raw --pulser --asym -g --pmt --kinema
done

# Wait until all processes are finished
bgx_wait
