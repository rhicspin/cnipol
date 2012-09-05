#!/bin/bash

RUN_PERIOD=$1

LOG="/eic/u/dsmirnov/eic0005/copy_results_pc2pc_${RUN_PERIOD}.log"
CHECKINGPERIOD=3600 # in sec

#eval `ssh-agent -s` >> ${LOG} 2>&1
#export 
export SSH_AUTH_SOCK=/tmp/ssh-lxuXX32554/agent.32554
export SSH_AGENT_PID=32555


while [ 1 ];
do
   echo "---" >> ${LOG}
   date >> ${LOG}

   #sleep $CHECKINGPERIOD
   #rsync --stats --bwlimit=100000 -a /eicdata/eic0005/run09/root/* /eicdata/eic0005/run11/root/* /eicdata/eic0005/run12/root/* pc2pc-phy:/usr/local/polarim/root/ >> ${LOG} 2>&1
   rsync --stats --bwlimit=100000 --exclude=*.root -a /eicdata/eic0005/${RUN_PERIOD}/root/* cnipol@pc2pc-phy:/usr/local/polarim/root/ >> ${LOG} 2>&1
	#tar -c /eicdata/eic0005/run12/root/16* | ssh pc2pc-phy 'tar -xvf - -C /usr/local/polarim/root/' >> ${LOG} 2>&1

   sleep $CHECKINGPERIOD
done
