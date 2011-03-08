#!/bin/bash

cd /usr/local/polarim/bin
#./rpoldaemon.sh -U >>/dev/null &
#exit $status 

HOSTNAME=`hostname -s`

if [ $HOSTNAME = "bluepc" ]; then
	#echo $HOSTNAME
	./rpoldaemon.sh -U >>/dev/null &
	exit $status
else
	if [ $HOSTNAME = "insideip12" ]; then
		#echo $HOSTNAME
		./rpoldaemon.sh -D >>/dev/null &
		exit $status
	else
		if [ $HOSTNAME = "yellowpc" ]; then
			#echo $HOSTNAME
			./rpoldaemon.sh -D >>/dev/null &
			exit $status
		else
			echo "Host not recognized. Start script on either bluepc, insideip12, or yellowpc"
		fi
	fi
fi
