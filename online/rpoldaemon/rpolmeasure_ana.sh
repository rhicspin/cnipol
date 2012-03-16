#!/bin/bash
#
# RHIC pC polarimeters main script
#       I. Alekseev & D. Svirida 2000-2011
#
# This script runs real measurement, not supposed to be run manually.
#
# Must be run with 3 argumets:
# $1 - polarimeter CDEV name: like polarimeter.yel1
# $2 - measurement command: like "Emit Run". Can be of two words and we process the first letter only.
# $3 - generated run number: like 12345.201. We use 99999.999 for test run.
#
# catches SIGINT and exits gracefully   (Stop)
# catches SIGTERM and exits without reprocessing the data       (Cancel)
#
# Checks return code and sends error mesage(s) and failed.gif
# to mcr application.
#
# Jan 25, 2012 - Dmitri Smirnov
#    - Added the type of measurement option (-T) to the command
#
# Feb 6, 2012 - Dmitri Smirnov
#    - The summary plots are now save also in png format
#

# Enable job control
set -m
# Set file creation mode to rwxrwxr-x
umask 0002
# this is return code of rhicpol after wait
declare -i IRC

# Set directories etc
export POLDIR=/usr/local/polarim
export CNIPOL_DIR=/usr/local/cnipol_trunk
export CONFDIR=$CNIPOL_DIR/config
export BINDIR=$CNIPOL_DIR/bin
export DATADIR=$POLDIR/data
export LOGDIR=$POLDIR/log
export HBOOKDIR=$POLDIR/hbook
export MACDIR=$BINDIR/macro
export ROOTDIR=$POLDIR/root

POLCMD=$BINDIR/rhicpol
ANACMD=$BINDIR/rhic2hbook
EMITCMD=$BINDIR/emitscan

# set our parameters
POLARIM=$1
MODE=$2
RUN=$3

# Select ini file based on the polarimeter
CNF=${CONFDIR}/${POLARIM}.ini

# Set data file
DATA=${DATADIR}/${RUN}.data

# Set log files
LOG=${LOGDIR}/${RUN}.log
ALOG=${LOGDIR}/an${RUN}.log
PSFILE=${LOGDIR}/${RUN}.ps
PSFILEE=${LOGDIR}/${RUN}_E.ps
ROOTFILE=${ROOTDIR}/${RUN}.root
HBOOKFILE=${HBOOKDIR}/${RUN}.hbook
ERRLOG=${LOGDIR}/${POLARIM}-err.log

# General options - pulse prog and verbose level
OPT="-P -v200 -Tcdev"


# SIGTERM handler (Cancel)
myhardexit() {
    kill -SIGTERM %1 >>/dev/null 2>&1
    exit
}

# SIGINT handler (Stop)
mysoftexit() {
    kill -SIGINT %1 >>/dev/null 2>&1
}

myignore() {
    echo "AAA" >> /dev/null
}

mywait() {
    for (( ;; )) ; do
        wait %1
        IRC=$?
        if (( $IRC < 128 )); then
            break
        fi
    done
}

mysendpict() {
    if [ -f $2 ] ; then
        convert $2 -trim ${2/.ps/.gif}
        #$BINDIR/sndpic $POLARIM $1 ${2/.ps/.gif}
        if [ $1 == "plotData" ] ; then
           echo "Creating png files..."
           convert ${2/.ps/.gif}[0] ${2/.ps/}.a.png
           convert ${2/.ps/.gif}[1] ${2/.ps/}.b.png
        else
           echo "Creating png files..."
           convert ${2/.ps/.gif} ${2/.ps/}.e.png
        fi
    else
        echo
        #$BINDIR/sndpic $POLARIM $1 $BINDIR/failed.gif
    fi
}

# Traps on the signals
trap mysoftexit SIGINT
trap myhardexit SIGTERM


# Run rhicpol and analysis according to the mode
case $MODE in
    Run* | Data* )
        # Regular run
        $POLCMD $OPT -l $LOG -i $CNF -f $DATA -d $POLARIM -c "$MODE $POLARIM" >> $ERRLOG 2>&1 &
        mywait
        trap myignore SIGINT    # ignore Stop on analysis stage
        if [ $IRC -eq 0 ]; then # analyze data if the measurement was OK
            echo "Starting rhic2hbook..." >> $ALOG
            $ANACMD -l -s $POLARIM $DATA $HBOOKFILE >> $ALOG 2>&1
            echo "Starting lr_spinpat.pl..." >> $ALOG
            $MACDIR/lr_spinpat.pl $RUN
            echo "Starting pvector.pl..." >> $ALOG
            $MACDIR/pvector.pl $RUN
            echo "Starting pawX11..." >> $ALOG
            export RUN LOGDIR PSFILE HBOOKFILE MACDIR   # no other way to pass arguments to kumac...
            pawX11 -n -b $MACDIR/onliplot.kumac >> $ALOG 2>&1
            echo "Starting online_polar.pl..." >> $ALOG
            $MACDIR/online_polar.pl $RUN
            echo "Starting sendpict..." >> $ALOG
            mysendpict plotData $PSFILE >> $ALOG 2>&1
            # We will also leave in the background the process to analyze this
            # measurement for bunch per bunch emittance
            (   echo "Starting emitscan..." >> $ALOG; \
                $EMITCMD -f $DATA -o $ROOTFILE -p $PSFILEE -d $POLARIM >> $ALOG 2>&1 ; \
                echo "Starting sendpict 2 ..." >> $ALOG; \
                mysendpict emitPlot $PSFILEE >> $ALOG 2>&1 ) &
        fi
        ;;
    Test* )
        # Test run (target scan for instance)
        # this is histOnly mode (-M) and we put data and log files to /dev/null
        $POLCMD $OPT -M -l /dev/null -i $CNF -f /dev/null -d $POLARIM >> $ERRLOG 2>&1 &
        mywait
        trap myignore SIGINT    # ignore Stop on analysis stage
        if [ $IRC -eq 0 ]; then # analyze data if the measurement was OK
            $EMITCMD -f $DATA -o $ROOTFILE -p $PSFILE -d $POLARIM >> $ALOG 2>&1
            mysendpict emitPlot $PSFILE >> $ALOG 2>&1
        fi
        ;;
    Pol* )
        # Ramp measurement
        $POLCMD $OPT -M -R -l $LOG -i $CNF -f $DATA -d $POLARIM -c "$MODE $POLARIM" >> $ERRLOG 2>&1 &
        mywait
        trap myignore SIGINT    # ignore Stop on analysis stage
        if [ $IRC -eq 0 ]; then # analyze data if the measurement was OK
            $ANACMD -l -s $POLARIM -N -1 $DATA $HBOOKFILE >> $ALOG 2>&1
            export RUN PSFILE HBOOKFILE POLARIM # no other way to pass arguments to kumac...
            pawX11 -n -b $MACDIR/rampplot.kumac >> $ALOG 2>&1
            echo "Starting sendpict..." >> $ALOG
            mysendpict plotData $PSFILE >> $ALOG 2>&1
        fi
        ;;
    * )
        echo "FATAL: Unknown run mode=$MODE." >> $ERRLOG
        exit 127
        ;;
esac
