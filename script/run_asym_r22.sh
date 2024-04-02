#!/bin/bash

CHECKINGPERIOD=60 # in sec

source /star/u/zchang/run22/cnipol/script/setup.sh 22 > /dev/null

MY_HOME_DIR=/star/u/zchang
MY_REMOTE_HOME_DIR=/home/cfsd/zchang

CNIPOL_REMOTE_BLUE_DATA_DIR=/home/blue/20$RUN_ID/data
CNIPOL_REMOTE_YELLOW_DATA_DIR=/home/yellow/20$RUN_ID/data
CNIPOL_ONLINE_DIR=/gpfs02/eic/eichome/cnipol/root
RUNLIST=/gpfs02/eic/cnipol/runXX/lists/run${RUN_ID}_all

while true;  do
    if [ -e ${MY_HOME_DIR}/ASYMLOCK ]; then
       exit;
    else
      touch ${MY_HOME_DIR}/ASYMLOCK
    fi
    
    # check bluepc tunnel
    date > ${MY_HOME_DIR}/TSTTUNBLU
    rsync -av ${MY_HOME_DIR}/TSTTUNBLU bluepc:${MY_REMOTE_HOME_DIR}/.
    rm ${MY_HOME_DIR}/TSTTUNBLU
    rsync -av bluepc:${MY_REMOTE_HOME_DIR}/TSTTUNBLU ${MY_HOME_DIR}/.

    # check yellpc tunnel
    date > ${MY_HOME_DIR}/TSTTUNYEL
    rsync -av ${MY_HOME_DIR}/TSTTUNYEL yellpc:${MY_REMOTE_HOME_DIR}/.
    rm ${MY_HOME_DIR}/TSTTUNYEL
    rsync -av yellpc:${MY_REMOTE_HOME_DIR}/TSTTUNYEL ${MY_HOME_DIR}/.

    blue_file_list="$(rsync -av --min-size=100k --include='*.data' --exclude='*' bluepc:${CNIPOL_REMOTE_BLUE_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2)"
    yellow_file_list="$(rsync -av --min-size=100k --include='*.data' --exclude='*' yellpc:${CNIPOL_REMOTE_YELLOW_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2)"

    file_list="$(echo -n "${blue_file_list}${yellow_file_list}" | egrep "\.data$" | sort)"

    echo Processing following files:
    echo "$file_list"
    echo "$file_list" >> ${RUNLIST}
    cat ${RUNLIST} | sort | uniq > ${RUNLIST}.bak
    mv ${RUNLIST}.bak ${RUNLIST}

    echo "$file_list" | while read -r file_name; do
	if [[ ! -z $file_name ]]; then
	    run_name=${file_name%.data}
	    args="--update-db -g -r $run_name"
	    #args="-g -r $run_name"
	    if [[ $run_name == *".alpha0" ]]; then
		args="--alpha $args"
	    fi
	    echo $CNIPOL_DIR/build/asym $args 
	    $CNIPOL_DIR/build/asym $args
	    rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/${run_name} ${CNIPOL_ONLINE_DIR} > /dev/null
	fi
    done
    rm ${MY_HOME_DIR}/ASYMLOCK
    sleep ${CHECKINGPERIOD}
#    exit
done
