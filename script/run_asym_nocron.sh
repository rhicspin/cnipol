#!/bin/bash

LOG="/eicdata/eic0005/run12/run_asym_nocron.log"
CHECKINGPERIOD=1800 # in sec

#source /eicdata/eic0005/cnipol/setup.sh >> /dev/null 2>&1
source /eicdata/eic0005/cnipol/setup12.sh >> ${LOG} 2>&1
#source /eicdata/eic0005/cnipol/script/run_asym/bgx_asym.sh >> ${LOG} 2>&1

export SSH_AUTH_SOCK=/tmp/ssh-qhSiH31025/agent.31025
export SSH_AGENT_PID=31026


while [ 1 ];
do
   echo "---" >> ${LOG}
   date >> ${LOG}
   #RESULT=`ps -a | sed -n /${SERVICE}/p`

   array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -mmin -70 ! -mmin -30 -printf "%f\n"`)

   echo Found new files: ${array[@]} >> ${LOG}
	#echo $SSH_AUTH_SOCK, $SSH_AGENT_PID >> ${LOG}

   #if [ "${RESULT:-null}" = null ]; then
   #  echo "rsync not running. Executing"
   #    #echo "rsync -a /eicdata/eic0005/run11/root/* bluepc:/usr/local/polarim/root/"
   #    #echo "rsync -a /eicdata/eic0005/run09/root/* bluepc:/usr/local/polarim/root/"
   #    nohup rsync -av eic0005:/eicdata/eic0005/run09/root/* /usr/local/polarim/root/ &
   #    nohup rsync -av eic0005:/eicdata/eic0005/run11/root/* /usr/local/polarim/root/ &
   #else
   #   echo "running"
   #fi

   for run_name in "${array[@]}"
   do
      #echo my item : ${run_name%.data}
      #echo bgx_limit 16 ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw --pulser --asym --kinema
      #bgx_limit 16 ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw --pulser --asym --kinema >> ${LOG} 2>&1
      echo ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw-ext --asym --kinema >> ${LOG}
      ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw-ext --asym --kinema >> ${LOG} 2>&1 &
      sleep 2
   done

   sleep $CHECKINGPERIOD

done
