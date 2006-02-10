#!/bin/bash

if [ -d $INSTALLDIR ] ; then
	echo "install executables to $INSTALLDIR"
else
	echo "Create $INSTALLDIR";
	mkdir $INSTALLDIR
fi

make -C src 
make -C src install

cd script 
install *.pl *.sh -C -v  --mode 777 $INSTALLDIR 
cd ..


# install paw macros:
if [ -d $MACRODIR ] ; then
	echo "install paw macros to $MACRODIR";
else
	echo "Create $MACRODIR";
	mkdir $MACRODIR
fi

cd macro
install *.pl *.sh -C -v  --mode 777 $MACRODIR
install *.kumac *.C *.awk -C -v  --mode 664 $MACRODIR
cd ..

echo "Installation Completed"

