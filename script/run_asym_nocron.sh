#!/bin/bash

LOG="/eicdata/eic0005/run12/run_asym_nocron.log"
CHECKINGPERIOD=1800 # in sec

source /eicdata/eic0005/cnipol_v1.8.5/setup.sh >> /dev/null 2>&1
source /eicdata/eic0005/cnipol_v1.8.5/script/run_asym/bgx_asym.sh >> ${LOG} 2>&1


while [ 1 ];
do
   date >> ${LOG}
   #RESULT=`ps -a | sed -n /${SERVICE}/p`

   array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -mmin -50 ! -mmin -20 -printf "%f\n"`)

   echo Found new files: ${array[@]} >> ${LOG}

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
      echo bgx_limit 16 ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw --pulser --asym --kinema
      bgx_limit 16 ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw --pulser --asym --kinema >> ${LOG} 2>&1
   done

   sleep $CHECKINGPERIOD

done

