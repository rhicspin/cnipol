source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

#bgx_limit 3 sleep 10

#export SSH_AUTH_SOCK=/tmp/ssh-qhSiH31025/agent.31025
#export SSH_AGENT_PID=31026

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
   #bgx_limit 10 ./bin/asym --calib --profile --use-db -g --copy -r $run_name
   #bgx_limit 10 ./bin/asym --calib --profile --use-db -g --pmt --copy -r $run_name
   #bgx_limit 10 ./bin/asym --calib --profile --use-db -g --copy --disable-det=101101 --sfx=hama --raw -r $run_name
   bgx_limit 12 $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -g --copy -r $run_name 
   #bgx_limit 10 $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -r $run_name 
done

# Wait until all processes are finished
bgx_wait
