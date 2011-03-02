#!/bin/bash

echo "Setting environmet variables:"

source /afs/rhic.bnl.gov/opt/phenix/root-5.27.04/bin/thisroot.sh

# ROOT variables
if [ -z "$ROOTSYS" ]; then
	source /usr/local/cern/root/bin/thisroot.sh
	echo "\$ROOTSYS=$ROOTSYS"
else
	echo "\$ROOTSYS already set"
fi

# CERNLIB variables
if [ -z "$CERNLIB_DIR" ]; then
	export CERNLIB_DIR=/cern/2006b
	export PATH=$PATH:$CERNLIB_DIR/bin
	echo "\$CERNLIB_DIR=$CERNLIB_DIR"
else
	echo "\$CERNLIB_DIR already set"
fi

#export CNIDIR=~/cnipol/cnipol/trunk
#export CNIDIR=/usr/local/cnipol

# CNIPOL variables
if [ -z "$CNIPOL_DIR" ]; then
	export CNIPOL_DIR=~/cnipol
	#export CNIPOL_DIR=/usr/local/cnipol
	export PATH=$PATH:$CNIPOL_DIR/bin
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CNIPOL_DIR/lib
fi

#export CNIPOL_DIR=~/cnipol/cnipol/trunk
export POLARIM_DIR=/data1/run11

#export CNILIB=/home/dsmirnov/cnipol_install
#export CNIPOL_LIB_DIR=$CNIPOL_DIR/lib
#export CNIPOL_ONLINE_DIR=$CNIPOL_DIR/online

export DATADIR=$POLARIM_DIR/data
#export SHARE_DIR=$CNIPOL_DIR/share
export CONFIGDIR=$POLARIM_DIR/config
export CONFDIR=$POLARIM_DIR/config
#export CALIB_DIR=$SHAREDIR/calib
export CALIBDIR=/home/dsmirnov/cnipol_install/share/config

#export CNI_RESULTS_DIR=$SHAREDIR/results
#export CNIPOL_RESULTS_DIR=~/results
export CNIPOL_RESULTS_DIR=$POLARIM_DIR/root

# CDEV variables

if [ -z "$CDEV_DIR" ]; then
	export CDEV_DIR=/usr/local/cdev
	export CDEVDDL=${CNIPOL_DIR}/online/cdev/PolarClient.ddl
	export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${CDEV_DIR}/lib
fi

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

#export LD_LIBRARY_PATH
