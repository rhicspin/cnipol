#!/bin/bash

export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov
export CDEVSHOBJ=/usr/local/lib/cdev
export LD_LIBRARY_PATH=${CDEVSHOBJ}/Linux:${LD_LIBRARY_PATH}
export CDEVDDL=/usr/local/etc/PolarClient.ddl
BINDIR=/usr/local/polarim/bin

${BINDIR}/rpoldaemon $1 -l ${BINDIR}/rpoldaemon${1}.log -r ${BINDIR}/rpolmeasure.sh 
