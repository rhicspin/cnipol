#!/bin/bash

if [ -n "$1" ]; then 
  export RUN_ID=$1
else
  export RUN_ID="22"
fi

echo "Setting environment variables for RUN_ID=${RUN_ID}"

export CNIPOL_DB_NAME="cnipol"
export CNIPOL_DB_HOST="eicsrv1.sdcc.bnl.gov"
export CNIPOL_DB_USER="cnipol"
export CNIPOL_DB_PASSWORD="(n!P0l"
#echo $CNIPOL_DB_PASSWORD

# CNIPOL
export CNIPOL_DIR=/star/u/zchang/run22/cnipol
export CNIPOL_DATA_DIR=/gpfs02/eic/cnipol/run$RUN_ID/data
export CNIPOL_RESULTS_DIR=/gpfs02/eic/cnipol/run$RUN_ID/root

export PATH=$PATH\:$CNIPOL_DIR/build
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH\:$CNIPOL_DIR/build

# UTILS
export UTILS_LIB_DIR=$CNIPOL_DIR/build/contrib/root-helper/utils/

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH\:$UTILS_LIB_DIR

# CDEV
#export CDEV_DIR=/usr/local/cdev
#export CDEVDDL=${CNIPOL_DIR}/online/cdev/PolarClient.ddl
#export CDEV_NAME_SERVER=acnserver01.pbn.bnl.gov
#echo "CERNLIB_DIR=$CERNLIB_DIR"

#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH\:${CDEV_DIR}/lib
#echo "\$CDEV_DIR=$CDEV_DIR"
#echo "\$CDEVDDL=$CDEVDDL"
#echo "\$CDEV_NAME_SERVER=$CDEV_NAME_SERVER"
echo
echo "ROOTSYS=$ROOTSYS"
echo "CNIPOL_DIR=$CNIPOL_DIR"
echo "CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR"
echo "CNIPOL_RESULTS_DIR=$CNIPOL_RESULTS_DIR"
echo
echo "UTILS_LIB_DIR=$UTILS_LIB_DIR"
echo
echo "PATH=$PATH"
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
