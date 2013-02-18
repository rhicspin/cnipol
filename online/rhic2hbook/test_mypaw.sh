#RUN=14802.016
#RUN=14802.114
#RUN=14802.207
#RUN=14802.307
RUN=$1

POLDIR=/usr/local/polarim
LOGDIR=${POLDIR}/log
#PSFILE=${LOGDIR}/${RUN}.ps
HBOOKDIR=${POLDIR}/hbook
HBOOKFILE=${HBOOKDIR}/${RUN}.hbook
BINDIR=${POLDIR}/bin
#MACDIR=${BINDIR}/macro

MACDIR=/usr/local/cnipol_trunk/online/rhic2hbook
PSFILE=test_mypaw.ps

$MACDIR/pvector.pl $RUN

export RUN LOGDIR PSFILE HBOOKFILE MACDIR # no other way to pass arguments to kumac...

$MACDIR/pvector.pl $RUN

pawX11 -n -b $MACDIR/onliplot.kumac

gv $PSFILE &

