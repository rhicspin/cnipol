#!/bin/bash

#export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov
export CDEV_NAME_SERVER=acnserver01.pbn.bnl.gov
#export CDEVSHOBJ=/usr/local/CDEV/store/X86/lib
export CDEVSHOBJ=/usr/local/lib/cdev
export LD_LIBRARY_PATH=${CDEVSHOBJ}/Linux:${LD_LIBRARY_PATH}
export CDEVDDL=/usr/local/etc/PolarClient.ddl
POLDIR=/usr/local/polarim/bin

${POLDIR}/rpolado $1 -d -l ${POLDIR}/rpolADOdemon${1}.log -r ${POLDIR}/rpolADOmeasure.sh 
