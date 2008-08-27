#!/bin/bash

export WORKDIR="LinearDeDx";
export INTEGRATED_DATA_ARCHIVE="IntegratedDeDx.tgz";
export INTEGRATED_DATA_DIR="Integ_dat_moded";

if [ ! -d $ASYMDIR ] ; then 
    mkdir $ASYMDIR
fi

if [ ! -d $ASYMDIR/$WORKDIR ] ; then
    echo "create directory $ASYMDIR/$WORKDIR"
    mkdir $ASYMDIR/$WORKDIR
fi

# install LinearFit macros
install *.C -v --mode 666 $ASYMDIR/$WORKDIR

# install integrated data file
install -C -v $INTEGRATED_DATA_ARCHIVE $ASYMDIR/$WORKDIR --mode 666
# Unarchive data file if data directory doesn't exist
if [ ! -d $ASYMDIR/$WORKDIR/$INTEGRATED_DATA_DIR ] ; then
    cd $ASYMDIR/$WORKDIR
    tar xvzf $INTEGRATED_DATA_ARCHIVE
fi


