#!/bin/bash

source ~/cnipol/setup12.sh
source ~/run/bgx_asym.sh

array=(`find ${CNIPOL_DATA_DIR} -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -newer last.data -printf "%f\n"`)

touch ${CNIPOL_DATA_DIR}/last.data

for run_name in "${array[@]}"
do
	#echo my item : ${run_name%.data}
   bgx_limit 16 ${CNIPOL_DIR}/bin/asym -r ${run_name%.data} --calib --profile --use-db -g --copy --pmt --raw --pulser --asym --kinema
done

# Wait until all others are finished.
bgx_wait
