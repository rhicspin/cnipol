#!/bin/sh

SERVICE="rsync"
RESULT=`ps -a | sed -n /${SERVICE}/p`

if [ "${RESULT:-null}" = null ]; then
   echo "rsync not running. Executing"
	#echo "rsync -a /eicdata/eic0005/run11/root/* bluepc:/usr/local/polarim/root/"
	#echo "rsync -a /eicdata/eic0005/run09/root/* bluepc:/usr/local/polarim/root/"
	echo "rsync -a eic0005:/eicdata/eic0005/run09/root/* /usr/local/polarim/root/"
	echo "rsync -a eic0005:/eicdata/eic0005/run11/root/* /usr/local/polarim/root/"
else
   echo "running"
fi

