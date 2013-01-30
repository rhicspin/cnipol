#!/bin/bash

# To run this script do
#
# >eval `ssh-agent -s`
# >ssh-add
#
# Then modify the SSH_... variables in this script accordingly
#
# >nohup ./copy_results_pc2pc.sh run12
#

RUN_PERIOD=$1

LOG="/eic/u/dsmirnov/eic0005/copy_results_pc2pc_${RUN_PERIOD}.log"
CHECKINGPERIOD=3600 # in sec

#eval `ssh-agent -s` >> ${LOG} 2>&1
#export 

#eic0008
#export SSH_AUTH_SOCK=/tmp/ssh-Slbbvtp782/agent.782
#export SSH_AGENT_PID=783
#eic0009
export SSH_AUTH_SOCK=/tmp/ssh-QnbKyj9381/agent.9381
export SSH_AGENT_PID=9382
#eic0010
#export SSH_AUTH_SOCK=/tmp/ssh-TPtHZa4668/agent.4668
#export SSH_AGENT_PID=4669

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
