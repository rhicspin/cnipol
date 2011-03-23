#!/bin/bash

echo "Setting environmet variables:"

# ROOT variables
if [ -z "$ROOTSYS" ]; then
   source /afs/rhic.bnl.gov/opt/phenix/root-5.27.04/bin/thisroot.sh
fi

# CERNLIB variables
if [ -z "$CERNLIB_DIR" ]; then
	export CERNLIB_DIR=/cern/2006b
	export PATH+=:$CERNLIB_DIR/bin
fi

# CNIPOL variables
if [ -z "$CNIPOL_DIR" ]; then
	export CNIPOL_DIR=/data1/cnipol
	export PATH+=:$CNIPOL_DIR/bin
	export LD_LIBRARY_PATH+=:$CNIPOL_DIR/lib
fi

export CNIPOL_DATA_DIR=/data1/run11/data
export CNIPOL_RESULTS_DIR=/data1/run11/root

# Older variables
export POLARIM_DIR=/data1/run11
#export CNILIB=/home/dsmirnov/cnipol_install
#export CNIPOL_LIB_DIR=$CNIPOL_DIR/lib
#export CNIPOL_ONLINE_DIR=$CNIPOL_DIR/online
export DATADIR=$POLARIM_DIR/data
export CONFDIR=$POLARIM_DIR/config
export CALIBDIR=$POLARIM_DIR/share/config

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
echo "\$CDEV_DIR=$CDEV_DIR"
echo "\$CDEVDDL=$CDEVDDL"
echo "\$CDEV_NAME_SERVER=$CDEV_NAME_SERVER"
echo
echo "\$DATADIR=$DATADIR   <-- deprecated"
echo "\$CONFDIR=$CONFDIR   <-- deprecated"
echo "\$CALIBDIR=$CALIBDIR   <-- deprecated"
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
