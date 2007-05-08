#!/bin/bash
HOSTFILE=src/.compiled_host

if [ -d $INSTALLDIR ] ; then
	echo "install executables to $INSTALLDIR"
else
	echo "Create $INSTALLDIR";
	mkdir -p $INSTALLDIR
fi

# Check temporary output directory
if [ ! -d $TMPOUTDIR ] ; then
    mkdir -p $TMPOUTDIR
    chmod 777 $TMPOUTDIR
    echo "Create $TMPOUTDIR";
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

# This requires CDEV library. Compile only when it exist.
if [ -d /usr/local/lib/cdev ] ; then
    make -C src/cdev
    make -C src/cdev install
fi


# install scripts into $INSTALLDIR
cd script 
./install.sh
cd ..

# install scripts into $MACRODIR
cd macro
./install.sh
cd ..

# install physics database into $SHARE/phys
cd phys
./install.sh
cd ..

# make necessary directories for analysis
if [ ! -d $ASYMDIR ] ; then
    mkdir $ASYMDIR
fi
if [ ! -d $ASYMDIR/config ] ; then
    mkdir $ASYMDIR/config
fi
if [ ! -d $ASYMDIR/calib ] ; then
    mkdir $ASYMDIR/calib
fi
if [ ! -d $ASYMDIR/dlayer ] ; then
    mkdir $ASYMDIR/dlayer
fi
if [ ! -d $ASYMDIR/hbook ] ; then
    mkdir $ASYMDIR/hbook
fi
if [ ! -d $ASYMDIR/root ] ; then
    mkdir $ASYMDIR/root
fi
if [ ! -d $ASYMDIR/log ] ; then
    mkdir $ASYMDIR/log
fi
if [ ! -d $ASYMDIR/ps ] ; then
    mkdir $ASYMDIR/ps
fi
if [ ! -d $ASYMDIR/summary ] ; then
    mkdir $ASYMDIR/summary
fi




if [ $CNI_EXPERT ] ; then
    if [ -d expert ] ; then
        echo "### Installing Expert Tools. ###";
        cd expert
	./install.sh
        cd ..
    fi
fi

echo "Installation Completed."

