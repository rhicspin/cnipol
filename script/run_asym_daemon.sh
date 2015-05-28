#!/bin/bash

CHECKINGPERIOD=60 # in sec

source /direct/eic+u/gwebb/pCpol/cnipol/script/setup.sh 15 > /dev/null

CNIPOL_REMOTE_BLUE_DATA_DIR=/home/blue/20$RUN_ID/data
CNIPOL_REMOTE_YELLOW_DATA_DIR=/home/yellow/20$RUN_ID/data
CNIPOL_ONLINE_DIR=/home/cnipol/root
RUNLIST=/eicdata/eic0005/runXX/lists/run${RUN_ID}_all

while true;  do

    blue_file_list="$(rsync -av --include='*.data' --exclude='*' bluepc:${CNIPOL_REMOTE_BLUE_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2)"
    yellow_file_list="$(rsync -av --include='*.data' --exclude='*' yellowpc:${CNIPOL_REMOTE_YELLOW_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2)"

    file_list="$(echo -n "${blue_file_list}${yellow_file_list}" | egrep "\.data$" | sort)"

    echo Processing following files:
    echo "$file_list"
    echo "$file_list" >> ${RUNLIST}
    cat ${RUNLIST} | uniq > ${RUNLIST}.bak
    mv ${RUNLIST}.bak ${RUNLIST}

    echo "$file_list" | while read -r file_name; do
	if [[ ! -z $file_name ]]; then
	    run_name=${file_name%.data}
	    args="--update-db -g -r $run_name"
	    if [[ $run_name == *".alpha0" ]]; then
		args="--alpha $args"
	    fi
	    echo $CNIPOL_DIR/build/asym $args 
	    $CNIPOL_DIR/build/asym $args
	    echo  "root -b -q '/direct/eic+u/schmidke/polana/afterburner/pro/afterburner.C+($run_name)'"
	    root -b -q '/direct/eic+u/schmidke/polana/afterburner/pro/afterburner.C+('$run_name')' > /dev/null 
	    rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/${run_name} cnipol@eicsrv1.phy.bnl.gov:${CNIPOL_ONLINE_DIR} > /dev/null
	fi
    done
    sleep ${CHECKINGPERIOD}

done
