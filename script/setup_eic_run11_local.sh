#!/bin/bash

echo "Setting environmet variables:"

# ROOT variables
#if [ -z "$ROOTSYS" ]; then
   source /eicdata/eic0005/root/bin/thisroot.sh
#fi

# CERNLIB variables
if [ -z "$CERNLIB_DIR" ]; then
   export CERNLIB_DIR=/cern/2006b
   export PATH+=:$CERNLIB_DIR/bin
fi

# MySQL
export MYSQL_INCLUDE_DIR=/usr/include/mysql
export MYSQL_LIB_DIR=/usr/lib/mysql

export MYSQLPP_DIR=/eicdata/eic0005/mysql++
export MYSQLPP_INCLUDE_DIR=$MYSQLPP_DIR/lib
export MYSQLPP_LIB_DIR=$MYSQLPP_DIR

if [[ $LD_LIBRARY_PATH != *$MYSQL_LIB_DIR* ]]; then
   export LD_LIBRARY_PATH+=:$MYSQL_LIB_DIR
fi

if [[ $LD_LIBRARY_PATH != *$MYSQLPP_LIB_DIR* ]]; then
   export LD_LIBRARY_PATH+=:$MYSQLPP_LIB_DIR
fi

# CNIPOL variables
export CNIPOL_DIR=~/cnipol
export CNIPOL_DATA_DIR=/eicdata/eic0005/run11/data
export CNIPOL_RESULTS_DIR=/eicdata/eic0005/run11/root

if [[ $PATH != *$CNIPOL_DIR/bin* ]]; then
   export PATH+=:$CNIPOL_DIR/bin
fi

if [[ $LD_LIBRARY_PATH != *$CNIPOL_DIR/lib* ]]; then
   export LD_LIBRARY_PATH+=:$CNIPOL_DIR/lib
fi

# Older variables
export POLARIM_DIR=/eicdata/eic0005/run11
export CONFDIR=$POLARIM_DIR/config

#UTILS
export UTILS_LIB_DIR=~dsmirnov/rootmacros/utils
export UTILS_INC_DIR=~dsmirnov/rootmacros/

if [[ $LD_LIBRARY_PATH != *$UTILS_LIB_DIR* ]]; then
   export LD_LIBRARY_PATH+=:$UTILS_LIB_DIR
fi

# CDEV variables
if [ -z "$CDEV_DIR" ]; then
   export CDEV_DIR=/usr/local/cdev
   export CDEVDDL=${CNIPOL_DIR}/online/cdev/PolarClient.ddl
   export CDEV_NAME_SERVER=acnserver01.pbn.bnl.gov
   export LD_LIBRARY_PATH+=:${CDEV_DIR}/lib
fi

echo "\$ROOTSYS=$ROOTSYS"
echo "\$CERNLIB_DIR=$CERNLIB_DIR"
echo "\$CNIPOL_DIR=$CNIPOL_DIR"
echo "\$CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR"
echo "\$CNIPOL_RESULTS_DIR=$CNIPOL_RESULTS_DIR"
echo
echo "\$UTILS_LIB_DIR=$UTILS_LIB_DIR"
echo "\$UTILS_INC_DIR=$UTILS_INC_DIR"
echo
echo "\$MYSQL_INCLUDE_DIR=$MYSQL_INCLUDE_DIR"
echo "\$MYSQL_LIB_DIR=$MYSQL_LIB_DIR"
echo "\$MYSQLPP_INCLUDE_DIR=$MYSQLPP_INCLUDE_DIR"
echo "\$MYSQLPP_LIB_DIR=$MYSQLPP_LIB_DIR"
echo
echo "\$CDEV_DIR=$CDEV_DIR"
echo "\$CDEVDDL=$CDEVDDL"
echo "\$CDEV_NAME_SERVER=$CDEV_NAME_SERVER"
echo
echo "\$PATH=$PATH"
echo "\$LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
