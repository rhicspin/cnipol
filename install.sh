#!/bin/bash
HOSTFILE=src/.compiled_host

if [ -d $INSTALLDIR ] ; then
	echo "install executables to $INSTALLDIR"
else
	echo "Create $INSTALLDIR";
	mkdir $INSTALLDIR
fi


# This routine checks if the present executables are compiled in 
# a local machine. If not, then make clean to recompile.
if [ -f $HOSTFILE ] ; then
    COMPILED_HOST=`cat $HOSTFILE`;
    if [ $COMPILED_HOST != $HOSTNAME ] ; then
	make -C src clean
	make -C src/util clean
    fi
fi

# make source files and install into $INSTALLDIR
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


if [ $CNI_EXPERT ] ; then
    if [ -d expert ] ; then
        echo "### Installing Expert Tools. ###";
        cd expert
	./install.sh
        cd ..
    fi
fi

echo "Installation Completed."

