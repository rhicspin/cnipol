#!/bin/bash
# Run regular polarimeter run, not supposed to be run manually
# catches SIGINT and exits gracefully
# catches SIGTERM and exits without reprocessing the data

# exports not needed since are now defined in /etc/profile.d/cdev.sh
#
#export CDEV_NAME_SERVER=acnlin89.pbn.bnl.gov
#export CDEVSHOBJ=/usr/local/lib/cdev/
#export LD_LIBRARY_PATH=${CDEVSHOBJ}/Linux:${LD_LIBRARY_PATH}
#export CDEVDDL=/usr/local/etc/PolarClient.ddl

POLDIR=/usr/local/polarim
CONFDIR=$POLDIR/config
BINDIR=$POLDIR/bin
DATADIR=$CONFDIR/data
LOGDIR=$CONFDIR/log
HBOOKDIR=$CONFDIR/hbook
ROOTDIR=$CONFDIR/root
POLCMD=$BINDIR/rhicpol
ANACMD=$BINDIR/rhic2hbook
EMITCMD=$BINDIR/emitscan

RUN=$1
TIME=$2
EVENTS=$3
ENERGY=$4
RING=$5
MODE=$6

declare -i EXIT_MODE
declare -i IRC
declare -i ENERGY
EXIT_MODE=0
IRC=0
set -m

myhardexit() {
    kill -SIGTERM %1 >/dev/null 2>&1 
    EXIT_MODE=99;
}

mysoftexit() {
    kill -SIGINT %1 >/dev/null 2>&1 
    EXIT_MODE=1;
}

childexit() {
    EXIT_MODE=2;
}

targetout() {
# only send SIGUSR1 to the parent process == polarimeter daemon
    kill -SIGUSR1 $PPID >/dev/null 2>&1 
    EXIT_MODE=0;
}

trap targetout SIGUSR1
trap childexit SIGCHLD
trap mysoftexit SIGINT
trap myhardexit SIGTERM

if [ $ENERGY -gt 25 ]; then
    if [ $RING == "y" ]; then
# YELLOW at FLATTOP
	TSHIFT=-3.0
    else
# BLUE at FLATTOP
	TSHIFT=2.0
    fi
else
    if [ $RING == "y" ]; then
# YELLOW at INJECTION
	TSHIFT=7.0
    else
# Injection TSHIFT ~ Flattop TSHIFT + 7
# BLUE at INJECTION
	TSHIFT=9.0
    fi
fi

if [ $RING == "y" ]; then
    CNF="yellow"
fi

if [ $RING == "b" ]; then
    CNF="blue"
fi

# config file for different modes
if [ $MODE == "s" ] ; then
    CONFIG=$CONFDIR/${CNF}_sc.ini
fi
if [ $MODE == "t" ] ; then
    CONFIG=$CONFDIR/${CNF}_sc.ini
fi 
if [ $MODE == "d" ]; then
    CONFIG=$CONFDIR/${CNF}_at.ini
fi 
if [ $MODE == "e" ]; then
    CONFIG=$CONFDIR/${CNF}_at.ini
fi 
    
if [ $MODE != "t" ]; then
    if [ $MODE == "e" ]; then
	$POLCMD -P -n -I -v200 -T $TSHIFT -t $TIME -e $EVENTS -f $DATADIR/e$RUN.data -l $LOGDIR/e$RUN.log -i $CONFIG -m $MODE >> $LOGDIR/${RING}-err.log 2>&1&
    else
	$POLCMD -P -n -I -v200 -T $TSHIFT -t $TIME -e $EVENTS -f $DATADIR/$RUN.data -l $LOGDIR/$RUN.log -i $CONFIG -m $MODE >> $LOGDIR/${RING}-err.log 2>&1&
    fi
fi

if [ $MODE == "t" ]; then
    $POLCMD -P -v200 -T $TSHIFT -t 20000 -e 0 -f $DATADIR/${RING}tscan_tmp.data -l $LOGDIR/${RING}tscan_tmp.log -i $CONFIG -m $MODE >> $LOGDIR/${RING}-err.log 2>&1&
fi

JOB=`jobs -p`

if [ x$DISPLAY != "x" -a $MODE != "t" ] ; then
    xterm -e tail -F $LOGDIR/$RUN.log $LOGDIR/an$RUN.log &
fi

if [ x$DISPLAY != "x" -a $MODE == "t" ] ; then
    xterm -e tail -F $LOGDIR/${RING}tscan_tmp.log &
fi

if [ x$JOB != "x" ]; then

    for ((;;)); do
	wait $JOB >/dev/null 2>&1
	IRC=$?
	if [ $EXIT_MODE -gt 0 ]; then
	    break;
	fi
    done
    
    for ((;;)); do
	wait $JOB >/dev/null 2>&1
	if [ $? -ne 127 ]; then
	    IRC=$?
	fi
	if [ $EXIT_MODE == 2 ]; then
	    break;
	fi
    done

    if [ $MODE != "t" ]; then
    if [ $MODE == "e" ]; then
        echo "IRC is $IRC (0, 130, or 142 is good)" >> $LOGDIR/e$RUN.log 2>&1
    fi
	if [ $IRC -eq 0 ] || [ $IRC -eq 130 ] || [ $IRC -eq 142 ]; then
	    export RUN=$RUN
	    export MACDIR=$BINDIR/macro
	    export PATH=$PATH:/usr/local/cern/bin/:/usr/bin/:/usr/local/bin/:
	    export CERN=/usr/local/cern
	    if [ $MODE == "e" ]; then
		export HBOOKFILE=$HBOOKDIR/e$RUN.hbook
		export PSFILE=$LOGDIR/e$RUN.ps
######
# Use new emitscan analysis program 3/1/2008
######
        $EMITCMD -f $DATADIR/e$RUN.data -o $ROOTDIR/e$RUN.root > $LOGDIR/eana$RUN.log
#		$EMITCMD -f $DATADIR/e$RUN.data -o $HBOOKDIR/e$RUN.hbook > $LOGDIR/eana$RUN.log
#		pawX11 -n -b $MACDIR/emitplot.kumac >> $LOGDIR/eana$RUN.log 2>&1
		convert $PSFILE -trim $LOGDIR/e$RUN.gif >> $LOGDIR/eana$RUN.log 2>&1
		$BINDIR/sndeplot $RING $LOGDIR/e$RUN.gif >> $LOGDIR/eana$RUN.log 2>&1
	    else
		$ANACMD -l -s $DATADIR/$RUN.data $HBOOKDIR/$RUN.hbook >$LOGDIR/an$RUN.log
		export HBOOKFILE=$HBOOKDIR/$RUN.hbook
		export PSFILE=$LOGDIR/$RUN.ps
		$MACDIR/lr_spinpat.pl $RUN
		$MACDIR/pvector.pl $RUN
		pawX11 -n -b $MACDIR/onliplot.kumac >> $LOGDIR/plot$RUN.log 2>&1 
		convert $PSFILE -trim $LOGDIR/$RUN.gif >> $LOGDIR/plot$RUN.log 2>&1
		$BINDIR/sndpic $RING $LOGDIR/$RUN.gif >> $LOGDIR/plot$RUN.log 2>&1 
	    fi
	fi
    fi

fi

if [ x$DISPLAY != "x" ] ; then
    kill -SIGTERM %2 >/dev/null 2>&1
fi
