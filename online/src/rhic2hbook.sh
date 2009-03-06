#!/bin/bash

POLDIR=/usr/local/polarim
CONFDIR=$POLDIR/config
BINDIR=$POLDIR/bin
DATADIR=$CONFDIR/data
LOGDIR=$CONFDIR/log
HBOOKDIR=$CONFDIR/hbook
ANACMD=rhic2hbook

RUN=$1
MUX=1

export HBOOKFILE=$HBOOKDIR/$RUN.hbook
export PSFILE=$LOGDIR/$RUN.ps
export MACDIR=$POLDIR/sources/polar2008
export RUN=$RUN
export MUX=$MUX

$ANACMD -l -s $DATADIR/$RUN.data $HBOOKDIR/$RUN.hbook >$LOGDIR/an$RUN.log
$MACDIR/lr_spinpat.pl $RUN
$MACDIR/pvector.pl $RUN
pawX11 -n -b $MACDIR/onliplot.kumac > $LOGDIR/plot$RUN.log 
gv $PSFILE &

convert $PSFILE -trim $LOGDIR/$RUN.gif >> $LOGDIR/plot$RUN.log 2>&1
$BINDIR/sndpic $RING $LOGDIR/$RUN.gif >> $LOGDIR/plot$RUN.log 2>&1 

