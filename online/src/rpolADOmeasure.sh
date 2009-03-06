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

#export PATH=$PATH:/usr/local/cern/root:/usr/local/cern/root/lib:/usr/bin:/usr/local/bin
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cern/root/lib

umask 0002

POLDIR=/usr/local/polarim
#POLDIR=`pwd`
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
MUX=$6
MODE=$7

declare -i EXIT_MODE
declare -i IRC
declare -i ENERGY
declare -i MUX

export RUN=$RUN
export MUX=$MUX

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
    if [ $RING == "b" ]; then
    # BLUE at FLATTOP larger tshift MOVES banana DOWN.
	  TSHIFT=2.0
    fi
  fi
else
# Injection TSHIFT = Flattop TSHIFT + 7
  if [ $RING == "y" ]; then
  # YELLOW at INJECTION
	TSHIFT=7.0
  else
    if [ $RING == "b" ]; then
    # BLUE at INJECTION
	  TSHIFT=9.0
    fi
  fi
fi

# Select ini file based on ring and mux settings

if [ $RING == "y" ] ; then
  if [ $MUX -eq 1 ]; then
    CNF="yellow1"
  else
    CNF="yellow2"
  fi
fi

if [ $RING == "b" ]; then
  if [ $MUX -eq 1 ]; then
    CNF="blue1"
  else
    CNF="blue2"
  fi
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
if [ $MODE == "r" ] ; then
    CONFIG=$CONFDIR/${CNF}_sc.ini
fi

if [ $MODE != "t" ]; then
    if [ $MODE == "e" ]; then
	$POLCMD -P -n -I -v200 -T $TSHIFT -t $TIME -e $EVENTS -f $DATADIR/e$RUN.data -l $LOGDIR/e$RUN.log -i $CONFIG -m $MODE >> $LOGDIR/${RING}-err.log 2>&1&
    else
	if [ $MODE == "r" ]; then
	    $POLCMD -P -n -I -v200 -T $TSHIFT -t 2 -e 0 -N $EVENTS -f $DATADIR/$RUN.data -l $LOGDIR/$RUN.log -i $CONFIG -m $MODE >> $LOGDIR/${RING}-err.log 2>&1&
	else
	    $POLCMD -P -n -I -v200 -T $TSHIFT -t $TIME -e $EVENTS -f $DATADIR/$RUN.data -l $LOGDIR/$RUN.log -i $CONFIG -m $MODE >> $LOGDIR/${RING}-err.log 2>&1&
	fi
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
	   # export RUN=$RUN

	    if [ $MODE == "e" ]; then
		export PSFILE=$LOGDIR/e$RUN.ps
        $EMITCMD -f $DATADIR/e$RUN.data -o $ROOTDIR/e$RUN.root > $LOGDIR/eana$RUN.log
        chmod ug+w $LOGDIR/eana$RUN.log

		convert $PSFILE -trim $LOGDIR/e$RUN.gif >> $LOGDIR/eana$RUN.log 2>&1
		$BINDIR/sndeplot $RING $LOGDIR/e$RUN.gif >> $LOGDIR/eana$RUN.log 2>&1
	#printenv >> $LOGDIR/eana$RUN.log 2>&1
	    else
	    export MACDIR=$BINDIR/macro
	    #export PATH=$PATH:/usr/local/cern/bin/:/usr/bin/:/usr/local/bin/:
	    #export CERN=/usr/local/cern
		if [ $MODE == "r" ]; then
			echo "Analyzing RAMP measurement" >> $LOGDIR/$RUN.log 2>&1
		    $ANACMD -l -s -N -1 $DATADIR/$RUN.data $HBOOKDIR/$RUN.hbook >$LOGDIR/ramp$RUN.log
		else
		    $ANACMD -l -s $DATADIR/$RUN.data $HBOOKDIR/$RUN.hbook >$LOGDIR/an$RUN.log
		    export HBOOKFILE=$HBOOKDIR/$RUN.hbook
		    export PSFILE=$LOGDIR/$RUN.ps
		    $MACDIR/lr_spinpat.pl $RUN
		    $MACDIR/pvector.pl $RUN
		    pawX11 -n -b $MACDIR/onliplot.kumac >> $LOGDIR/plot$RUN.log 2>&1 
		    convert $PSFILE -trim $LOGDIR/$RUN.gif >> $LOGDIR/plot$RUN.log 2>&1
		    $BINDIR/sndpic $RING $LOGDIR/$RUN.gif >> $LOGDIR/plot$RUN.log 2>&1 
	#printenv >>$LOGDIR/an$RUN.log 2>&1 
		fi
	    fi
	fi
    fi

fi

if [ x$DISPLAY != "x" ] ; then
    kill -SIGTERM %2 >/dev/null 2>&1
fi
