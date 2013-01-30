#!/bin/bash

echo "Setting environmet variables:"

# ROOT variables
if [ -z "$ROOTSYS" ]; then
	source /usr/local/cern/root/bin/thisroot.sh
fi

# CERNLIB variables
if [ -z "$CERNLIB_DIR" ]; then
	export CERNLIB_DIR=/usr/local/cern/2006
	export PATH+=:$CERNLIB_DIR/bin
fi

# MySQL
export MYSQL_INCLUDE_DIR=/usr/include/mysql
export MYSQL_LIB_DIR=/usr/lib64/mysql

export MYSQLPP_INCLUDE_DIR=/usr/local/mysql++/lib
export MYSQLPP_LIB_DIR=/usr/local/mysql++

if [[ $LD_LIBRARY_PATH != *$MYSQL_LIB_DIR* ]]; then
   export LD_LIBRARY_PATH+=:$MYSQL_LIB_DIR
fi

if [[ $LD_LIBRARY_PATH != *$MYSQLPP_LIB_DIR* ]]; then
   export LD_LIBRARY_PATH+=:$MYSQLPP_LIB_DIR
fi

# CNIPOL variables
export CNIPOL_DIR=/usr/local/cnipol
export CNIPOL_DATA_DIR=/usr/local/polarim/data
export CNIPOL_RESULTS_DIR=/usr/local/polarim/root

if [[ $PATH != *$CNIPOL_DIR/bin* ]]; then
	export PATH+=:$CNIPOL_DIR/bin
fi

if [[ $LD_LIBRARY_PATH != *$CNIPOL_DIR/lib* ]]; then
	export LD_LIBRARY_PATH+=:$CNIPOL_DIR/lib
fi

# Older variables
export POLARIM_DIR=/usr/local/polarim
#export CNILIB=/home/dsmirnov/cnipol_install
#export CNIPOL_LIB_DIR=$CNIPOL_DIR/lib
#export CNIPOL_ONLINE_DIR=$CNIPOL_DIR/online
#export DATADIR=$POLARIM_DIR/data
export CONFDIR=$CNIPOL_DIR/config
#export CALIBDIR=$POLARIM_DIR/share/config

#UTILS
export UTILS_LIB_DIR=~dsmirnov/rootmacros/utils
export UTILS_INC_DIR=~dsmirnov/rootmacros/

if [[ $LD_LIBRARY_PATH != *$UTILS_LIB_DIR* ]]; then
	export LD_LIBRARY_PATH+=:$UTILS_LIB_DIR
fi

# CDEV variables
export CDEV_DIR=/usr/local/cdev
export CDEV=$CDEV_DIR
export CDEVSHOBJ=${CDEV}/lib
export CDEVDDL=${CNIPOL_DIR}/online/cdev/PolarClient.ddl
export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov

if [[ $LD_LIBRARY_PATH != *$CDEVSHOBJ* ]]; then
	export LD_LIBRARY_PATH+=:$CDEVSHOBJ
fi

# CDEV variables
if [ -z "$CDEV_DIR" ]; then
	export CDEV_DIR=/usr/local/cdev
	export CDEVDDL=${CNIPOL_DIR}/online/cdev/PolarClient.ddl
	export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov
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
#echo "\$DATADIR=$DATADIR     <-- deprecated"
echo "\$CONFDIR=$CONFDIR     <-- deprecated"
#echo "\$CALIBDIR=$CALIBDIR   <-- deprecated"
echo
echo "\$PATH=$PATH"
echo "\$LD_LIBRARY_PATH=$LD_LIBRARY_PATH"

# working directory
#export ASYMDIR=$HOME/offline
#export ASYMDIR=$CNIPOL_DIR/offline

# temporary output directory
#export TMPOUTDIR=/tmp/`whoami`

# installation destination
#export INSTALLDIR=$ASYMDIR/bin
#export MACRODIR=$ASYMDIR/macro
#    for developers use followings instead of above two lines
#       export INSTALLDIR=$SHAREDIR/bin
#       export MACRODIR=$SHAREDIR/macro
#       export CNI_EXPERT=1;

#export LD_LIBRARY_PATH=/usr/local/cdev/lib/Linux:/usr/local/cern/lib:/usr/lib
