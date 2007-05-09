#!/bin/bash

# install paw macros:
if [ -d $MACRODIR ] ; then
	echo "Install root/paw macros to $MACRODIR";
else
	echo "Create $MACRODIR";
	mkdir $MACRODIR
fi

install *.pl *.sh -C -v  --mode 777 $MACRODIR
install *.kumac *.C *.h *.awk -C -v  --mode 666 $MACRODIR



