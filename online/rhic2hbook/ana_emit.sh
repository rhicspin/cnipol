#!/bin/bash
#
# Apr  9, 2012 - Dmitri Smirnov
#    - Polarimeter id can be recognized automatically for standard run notation ?????.P??
#


RUN=$1
POLARIM=$2

POLDIR=/usr/local/polarim
export LOGDIR=${POLDIR}/log
export ROOTDIR=${POLDIR}/root
PSFILE=${LOGDIR}/${RUN}.ps
#PSFILE=./${RUN}.ps
ALOG=${LOGDIR}/an${RUN}.log
export HBOOKDIR=${POLDIR}/hbook
HBOOKFILE=${HBOOKDIR}/${RUN}.hbook
ROOTFILE=${ROOTDIR}/${RUN}.root
export BINDIR=${CNIPOL_DIR}/bin
export MACDIR=${BINDIR}/macro
export DATADIR=${POLDIR}/data
DATA=${DATADIR}/${RUN}.data
EMITCMD=$BINDIR/emitscan
ANACMD=$BINDIR/rhic2hbook


if [ -z "$POLARIM" ]; then
   if [ "${RUN:6:1}" = "0" ]; then
      POLARIM="polarimeter.blu1"
   elif [ "${RUN:6:1}" = "1" ]; then
      POLARIM="polarimeter.yel1"
   elif [ "${RUN:6:1}" = "2" ]; then
      POLARIM="polarimeter.blu2"
   elif [ "${RUN:6:1}" = "3" ]; then
      POLARIM="polarimeter.yel2"
   else
      echo "POLARIM not defined"
   fi
fi


echo $RUN $POLARIM

echo "\$MACDIR=$MACDIR"
echo "\$LOGDIR=$LOGDIR"


mysendpict() {
    if [ -f $2 ] ; then
        convert $2 -trim ${2/.ps/.gif}
        $BINDIR/sndpic $POLARIM $1 ${2/.ps/.gif}
        convert ${2/.ps/.gif}[0] ${2/.ps/}.a.png
        convert ${2/.ps/.gif}[1] ${2/.ps/}.b.png
    else
        $BINDIR/sndpic $POLARIM $1 $BINDIR/failed.gif
    fi
}

echo "Starting emitscan..." >> $ALOG
$EMITCMD -f $DATA -o $ROOTFILE -p $PSFILE -d $POLARIM >> $ALOG 2>&1
#echo "Starting sendpict..." >> $ALOG
#mysendpict emitPlot $PSFILE >> $ALOG 2>&1
