#!/bin/bash

if [ -n "$1" ]; then RUN_ID=$1; else RUN_ID="13"; fi

RUN_ID=$(printf '%02d' $RUN_ID)

echo "Setting environment variables for RUN_ID=$RUN_ID:"

# ROOT
source /eicdata/eic0005/root/bin/thisroot.sh
export ROOTSYS

# CERNLIB (required for online programs)
export CERNLIB_DIR=/cern/2006b
export PATH+=:$CERNLIB_DIR/bin

# MySQL
export MYSQL_LIB_DIR=/usr/lib/mysql
export MYSQLPP_LIB_DIR=/eicdata/eic0005/mysql++

export LD_LIBRARY_PATH+=:$MYSQL_LIB_DIR
export LD_LIBRARY_PATH+=:$MYSQLPP_LIB_DIR

# CNIPOL
export CNIPOL_DIR=~/cnipol
export CNIPOL_DATA_DIR=/eicdata/eic0005/run$RUN_ID/data
export CNIPOL_RESULTS_DIR=/eicdata/eic0005/run$RUN_ID/root

export PATH+=:$CNIPOL_DIR/build
export LD_LIBRARY_PATH+=:$CNIPOL_DIR/build

# UTILS
export UTILS_LIB_DIR=$CNIPOL_DIR/build/contrib/root-helper/utils/

export LD_LIBRARY_PATH+=:$UTILS_LIB_DIR

# CDEV
export CDEV_DIR=/usr/local/cdev
export CDEVDDL=${CNIPOL_DIR}/online/cdev/PolarClient.ddl
export CDEV_NAME_SERVER=acnserver01.pbn.bnl.gov

export LD_LIBRARY_PATH+=:${CDEV_DIR}/lib


echo "\$ROOTSYS=$ROOTSYS"
echo "\$CERNLIB_DIR=$CERNLIB_DIR"
echo "\$CNIPOL_DIR=$CNIPOL_DIR"
echo "\$CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR"
echo "\$CNIPOL_RESULTS_DIR=$CNIPOL_RESULTS_DIR"
echo
echo "\$UTILS_LIB_DIR=$UTILS_LIB_DIR"
echo
echo "\$MYSQL_LIB_DIR=$MYSQL_LIB_DIR"
echo "\$MYSQLPP_LIB_DIR=$MYSQLPP_LIB_DIR"
echo
echo "\$CDEV_DIR=$CDEV_DIR"
echo "\$CDEVDDL=$CDEVDDL"
echo "\$CDEV_NAME_SERVER=$CDEV_NAME_SERVER"
echo
echo "\$PATH=$PATH"
echo "\$LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
