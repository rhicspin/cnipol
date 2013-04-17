#!/bin/bash

# To run this script in background do
#
# >eval `ssh-agent -s`
# >ssh-add
#
# Then modify the SSH_... variables in this script accordingly
#
# >nohup ./copy_results_pc2pc.sh run12
#

#source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

LOG="/usr/local/polarim/log/run_asym_nocron.log"
#LOG="/eicdata/eic0005/run13/run_asym_nocron.log"
CHECKINGPERIOD=60 # in sec

# pc2pc-phy
export SSH_AUTH_SOCK=/tmp/ssh-CnylBq9610/agent.9610
export SSH_AGENT_PID=9611
# eic0005
#export SSH_AUTH_SOCK=/tmp/ssh-lKLHL25172/agent.25172
#export SSH_AGENT_PID=25173

source /usr/local/cnipol/setup.sh >> ${LOG} 2>&1
#source /eicdata/eic0005/cnipol/setup13.sh >> ${LOG} 2>&1

# overwrite the data location
export CNIPOL_DATA_DIR=/usr/local/polarim/data_buffer
#export CNIPOL_DATA_DIR=/eicdata/eic0005/run13/data_buffer
echo "overwrite data location" >> ${LOG}
echo "\$CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR" >> ${LOG}

while true;
do
   echo "Checking for files..." >> ${LOG}
   array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -printf "%f\n"`)

   if [ -z "${array}" ]
   then
      echo "list empty. sleep for $CHECKINGPERIOD. copy new files from online" >> ${LOG}
      sleep $CHECKINGPERIOD
      rsync -q --remove-source-files -av bluepc:${CNIPOL_DATA_DIR}/ ${CNIPOL_DATA_DIR}/
      continue
   fi

   echo "---" >> ${LOG}
   date >> ${LOG}
   echo Found new files: ${array[@]} >> ${LOG}
   #echo "---"
   #date
   #echo Found new files: ${array[@]}

   for run_name in "${array[@]}"
   do
      #echo processed item : ${run_name%.data} and removed
      echo $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -g -r ${run_name%.data} >> ${LOG}
      $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -g -r ${run_name%.data} >> ${LOG} 2>&1
      #bgx_limit 10 $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -r ${run_name%.data} >> ${LOG}
   done

   # Wait until all processes are finished
   #bgx_wait

   for run_name in "${array[@]}"
   do
	   echo ${run_name} >> /eicdata/eic0005/runXX/lists/run13_all
      mv ${CNIPOL_DATA_DIR}/${run_name} ${CNIPOL_DATA_DIR}/
		ssh -Y acnlina "~/cnipol_trunk/script/get_fill_info.sh 17236" > ttt
   done

done
