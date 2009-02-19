#!/bin/bash


if [ ! -d $CNILIB ] ; then
    echo "Create $CNILIB" ;
    mkdir -p $CNILIB;
fi

if [ ! -d $SHAREDIR ] ; then
    echo "Create $SHAREDIR" ;
    mkdir -p $SHAREDIR;
fi


# Install Calibration Data Files
if [ ! -d $CALIBDIR ] ; then
    echo "Create $SHAREDIR" ;
    mkdir -p $CALIBDIR;
fi

tar xvzf Run06_calib.tgz -C $CALIBDIR

# Install Configuration Files
if [ ! -d $CONFDIR ] ; then
    echo "Create $CONFDIR" ;
    mkdir -p $CONFDIR;
fi

#tar xvzf Run06_config.tgz -C $CONFDIR
tar xvzf run08_config.tgz -C $CONFDIR


# Install DeadLayer Files
if [ ! -d $SHAREDIR/dlayer ] ; then
    echo "Create $SHAREDIR/dlayer" ;
    mkdir -p $SHAREDIR/dlayer;
fi

tar xvzf Run06_dlayer.tgz -C $SHAREDIR/dlayer

echo "Installation completed."


