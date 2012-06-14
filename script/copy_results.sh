#!/bin/bash

LOG="/eic/u/dsmirnov/eic0005/run12/copy_results.log"
CHECKINGPERIOD=600 # in sec

#eval `ssh-agent -s` >> ${LOG} 2>&1
#export 
export SSH_AUTH_SOCK=/tmp/ssh-lxuXX32554/agent.32554
export SSH_AGENT_PID=32555


while [ 1 ];
do
   echo "---" >> ${LOG}
   date >> ${LOG}

   #rsync --stats --bwlimit=1000 -rlpgoDv /eicdata/eic0005/run12/root/* bluepc:/usr/local/polarim/root/ >> ${LOG} 2>&1
   #rsync --stats --bwlimit=5000 -a /eicdata/eic0005/run11/root/* bluepc:/usr/local/polarim/root/ >> ${LOG} 2>&1
   #sleep $CHECKINGPERIOD
   rsync --stats --bwlimit=10000 -a /eicdata/eic0005/run12/root/* bluepc:/usr/local/polarim/root/ >> ${LOG} 2>&1

   sleep $CHECKINGPERIOD
done



#!/bin/sh

#nohup rsync -av eic0005:/eicdata/eic0005/run09/root/* /usr/local/polarim/root/ &

#SERVICE="rsync"
#CHECKINGPERIOD=60                  # in sec
#
#while [ 1=1 ];
#do
#    RESULT=`ps -a | sed -n /${SERVICE}/p`
#    if [ "${RESULT:-null}" = null ]; then
#      echo "rsync not running. Executing"
#    	#echo "rsync -a /eicdata/eic0005/run11/root/* bluepc:/usr/local/polarim/root/"
#    	#echo "rsync -a /eicdata/eic0005/run09/root/* bluepc:/usr/local/polarim/root/"
#    	nohup rsync -av eic0005:/eicdata/eic0005/run09/root/* /usr/local/polarim/root/ &
#    	nohup rsync -av eic0005:/eicdata/eic0005/run11/root/* /usr/local/polarim/root/ &
#    else
#       echo "running"
#    fi
#    sleep $CHECKINGPERIOD
#
#done

#MAILSTOP=0                         # switcher for mail sending
#CHECKINGPERIOD=20                  # in sec
#MYEMAIL="me@myemail.com"           # where to send info
#
#while [ 1=1 ];
#do
#    if [ ! "$(pidof httpd)" ]
#    then
#    else
#    fi
#    sleep $CHECKINGPERIOD
