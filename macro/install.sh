#!/bin/bash

# install paw macros:
if [ -d $MACRODIR ] ; then
	echo "install paw macros to $MACRODIR";
else
	echo "Create $MACRODIR";
	mkdir $MACRODIR
fi

install *.pl *.sh -C -v  --mode 777 $MACRODIR
install *.kumac *.C *.awk -C -v  --mode 664 $MACRODIR



