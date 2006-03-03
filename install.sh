#!/bin/bash

if [ -d $INSTALLDIR ] ; then
	echo "install executables to $INSTALLDIR"
else
	echo "Create $INSTALLDIR";
	mkdir $INSTALLDIR
fi

make -C src 
make -C src install

make -C src/util
make -C src/util install

# install scripts into $INSTALLDIR
cd script 
./install.sh
cd ..


# install scripts into $MACRODIR
cd macro
./install.sh
cd ..

echo "Installation Completed"

