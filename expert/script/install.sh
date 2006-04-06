#!/bin/bash


if [ -d $INSTALLDIR ] ; then
	echo "install executables to $INSTALLDIR"
else
	echo "Create $INSTALLDIR";
	mkdir $INSTALLDIR
fi

install *.sh -C -v  --mode 777 $INSTALLDIR


