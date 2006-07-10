#!/bin/bash

PHYSDIR=$SHAREDIR/phys

# Create directory if it doesn't exist
if [ ! -d $PHYSDIR ] ; then
	echo "Create $PHYSDIR";
	mkdir $PHYSDIR
fi

install *.dat -C -v  --mode 664 $PHYSDIR






