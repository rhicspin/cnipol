#!/bin/sh

SERVICE="rsync"
CHECKINGPERIOD=3600                  # in sec

while [ 1=1 ];
do
    RESULT=`ps -a | sed -n /${SERVICE}/p`
    if [ "${RESULT:-null}" = null ]; then
      echo "rsync not running. Executing"
    	#echo "rsync -a /eicdata/eic0005/run11/root/* bluepc:/usr/local/polarim/root/"
    	#echo "rsync -a /eicdata/eic0005/run09/root/* bluepc:/usr/local/polarim/root/"
    	nohup rsync -av eic0005:/eicdata/eic0005/run09/root/* /usr/local/polarim/root/ &
    	nohup rsync -av eic0005:/eicdata/eic0005/run11/root/* /usr/local/polarim/root/ &
    else
       echo "running"
    fi
    sleep $CHECKINGPERIOD

done

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
