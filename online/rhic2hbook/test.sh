
source /usr/local/cnipol_trunk/setup.sh
source $CNIPOL_DIR/online/setup_online.sh

#RUN=14890.311
#RUN=15080.309
#RUN=15080.007
#RUN=15122.101
#RUN=15150.006
#RUN=15172.007
RUN=$1

#POLARIM=polarimeter.blu1
#POLARIM=polarimeter.yel1
POLARIM=$2

POLDIR=/usr/local/polarim
export LOGDIR=${POLDIR}/log
PSFILE=${LOGDIR}/${RUN}.ps
#PSFILE=./${RUN}.ps
ALOG=${LOGDIR}/an${RUN}.log
export HBOOKDIR=${POLDIR}/hbook
HBOOKFILE=${HBOOKDIR}/${RUN}.hbook
export BINDIR=${CNIPOL_DIR}/bin
export MACDIR=${BINDIR}/macro
export DATADIR=${POLDIR}/data
DATA=${DATADIR}/${RUN}.data
EMITCMD=$BINDIR/emitscan
ANACMD=$BINDIR/rhic2hbook

echo $RUN $POLARIM

echo "\$MACDIR=$MACDIR"
echo "\$LOGDIR=$LOGDIR"

mysendpict() {
    if [ -f $2 ] ; then
        convert $2 -trim ${2/.ps/.gif}
        #$BINDIR/sndpic $POLARIM $1 ${2/.ps/.gif}
        convert ${2/.ps/.gif}[0] ${2/.ps/}.a.png
        convert ${2/.ps/.gif}[1] ${2/.ps/}.b.png
    #else
        #$BINDIR/sndpic $POLARIM $1 $BINDIR/failed.gif
    fi
}


$ANACMD -l -s $POLARIM $DATA $HBOOKFILE >> $ALOG 2>&1
echo "Starting lr_spinpat.pl..." >> $ALOG
$MACDIR/lr_spinpat.pl $RUN
echo "Starting pvector.pl..." >> $ALOG
$MACDIR/pvector.pl $RUN
echo "Starting pawX11..." >> $ALOG
export RUN LOGDIR PSFILE HBOOKFILE MACDIR       # no other way to pass arguments to kumac...
pawX11 -n -b $MACDIR/onliplot.kumac >> $ALOG 2>&1
echo "Starting sendpict..." >> $ALOG
mysendpict plotData $PSFILE >> $ALOG 2>&1
