#!/bin/bash

cd /usr/local/polarim/bin
#./rpoldaemon.sh -U >>/dev/null &
#exit $status 

HOSTNAME=`hostname -s`

if [ $HOSTNAME = "bluepc" ]; then
	#echo $HOSTNAME
	./rpoldaemon.sh -B >>/dev/null &
	exit $status
else
	if [ $HOSTNAME = "yellowpc" ]; then
			#echo $HOSTNAME
			./rpoldaemon.sh -Y >>/dev/null &
			exit $status
		else
			echo "Host not recognized. Start script on either bluepc or  yellowpc"
		fi
	fi
fi
