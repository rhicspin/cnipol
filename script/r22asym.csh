#!/bin/csh
set MY_HOME_DIR=/star/u/zchang

if ( -e ${MY_HOME_DIR}/ASYMLOCK ) then
    exit;
else
    touch ${MY_HOME_DIR}/ASYMLOCK
#    echo ${MY_HOME_DIR}/ASYMLOCK
endif

#set CHECKINGPERIOD=60 # in sec

source /star/u/zchang/.cshrc
source /star/u/zchang/.login

source /star/u/zchang/run22/cnipol/script/r22setup.csh 22 > /dev/null

set MY_REMOTE_HOME_DIR=/home/cfsd/zchang

set CNIPOL_REMOTE_BLUE_DATA_DIR=/home/blue/20$RUN_ID/data
set CNIPOL_REMOTE_YELLOW_DATA_DIR=/home/yellow/20$RUN_ID/data
set CNIPOL_ONLINE_DIR=/gpfs02/eic/eichome/cnipol/root
set RUNLIST=/gpfs02/eic/cnipol/runXX/lists/run${RUN_ID}_all

#echo PATH=$PATH
#echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
    
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

set blue_file_list="`rsync -av --min-size=500k --include='*.data' --exclude='*' bluepc:${CNIPOL_REMOTE_BLUE_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2`"
#echo "blue: $blue_file_list"
set yellow_file_list="`rsync -av --min-size=500k --include='*.data' --exclude='*' yellpc:${CNIPOL_REMOTE_YELLOW_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2`"
#echo "yellow: $yellow_file_list"
set file_list=`echo -n ${blue_file_list}${yellow_file_list} | grep -oP '\S*.data(?=\s|$)'`

if ("$file_list" != "") then
	echo Processing following files:
	echo "$file_list"
	foreach file ($file_list)
        	echo $file >> ${RUNLIST}
        	set run_name=`echo $file | grep -oP "\S*(?=.data)"`
		set args="--update-db -g -r $run_name"
        	if ( $run_name =~ *alpha0* ) then
            		set args="--alpha $args"
                else
            		set args="$args"
        	endif
		echo $CNIPOL_DIR/build/asym $args 
		$CNIPOL_DIR/build/asym $args
		#rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/${run_name} ${CNIPOL_ONLINE_DIR} > /dev/null
 		mkdir -p ${CNIPOL_ONLINE_DIR}/${run_name}/images
	        cp -rf ${CNIPOL_RESULTS_DIR}/${run_name}/images ${CNIPOL_ONLINE_DIR}/${run_name}/. > /dev/null
	        cp ${CNIPOL_RESULTS_DIR}/${run_name}/stdoe.log ${CNIPOL_ONLINE_DIR}/${run_name} > /dev/null
	        cp ${CNIPOL_RESULTS_DIR}/${run_name}/config_calib.dat ${CNIPOL_ONLINE_DIR}/${run_name} > /dev/null
	        cp ${CNIPOL_RESULTS_DIR}/${run_name}/runconfig.php ${CNIPOL_ONLINE_DIR}/${run_name} > /dev/null
	end
	cat ${RUNLIST} | sort | uniq > ${RUNLIST}.bak
	mv ${RUNLIST}.bak ${RUNLIST}
endif

rm ${MY_HOME_DIR}/ASYMLOCK
#sleep ${CHECKINGPERIOD}

