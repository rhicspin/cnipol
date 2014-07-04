#!/bin/bash

LOG="/usr/local/polarim/log/run_asym.log"

date >> ${LOG} 2>&1

source /usr/local/cnipol_trunk/setup.sh >> /dev/null 2>&1
source /usr/local/cnipol_trunk/script/bgx_asym.sh >> ${LOG} 2>&1

#array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -mmin -3 -newer ${CNIPOL_DATA_DIR}/last.data -printf "%f\n"`)
array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -mmin -40 ! -mmin -10 -printf "%f\n"`)

#touch -d "+10 minutes" ${CNIPOL_DATA_DIR}/last.data

for run_name in "${array[@]}"
do
	#echo my item : ${run_name%.data}
   bgx_limit 1 ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --pmt --raw --pulser --asym --kinema >> ${LOG} 2>&1
done

# Wait until all others are finished.
bgx_wait
