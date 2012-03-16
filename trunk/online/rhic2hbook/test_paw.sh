#RUN=14802.016
#RUN=14802.114
#RUN=14802.207
RUN=14802.307
POLDIR=/usr/local/polarim
LOGDIR=${POLDIR}/log
PSFILE=${LOGDIR}/${RUN}.ps
HBOOKDIR=${POLDIR}/hbook
HBOOKFILE=${HBOOKDIR}/${RUN}.hbook
BINDIR=${POLDIR}/bin
MACDIR=${BINDIR}/macro

export RUN LOGDIR PSFILE HBOOKFILE MACDIR # no other way to pass arguments to kumac...
pawX11 -n -b $MACDIR/onliplot.kumac
