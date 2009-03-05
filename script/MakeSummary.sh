#!/bin/bash

SUMMARYDIR=$ASYMDIR/summary


LinkFile(){

cd $SUMMARYDIR

if [ ! -f  $FILE\_Blue.dat ] ; then
    ln -s $FILE\_Blue_all.dat $FILE\_Blue.dat;
fi

if [ ! -f  $FILE\_Yellow.dat ] ; then
    ln -s $FILE\_Yellow_all.dat $FILE\_Yellow.dat;
fi

}

LinkFileDlayer(){

cd $SUMMARYDIR

if [ ! -f  $FILE\_Blue_$MODE.dat ] ; then
    ln -s $FILE\_Blue_$MODE\_all.dat $FILE\_Blue_$MODE.dat;
fi

if [ ! -f  $FILE\_Yellow_$MODE.dat ] ; then
    ln -s $FILE\_Yellow_$MODE\_all.dat $FILE\_Yellow_$MODE.dat;
fi

}


LINK_FILE(){
    FILE="OfflinePol";
    LinkFile;
    FILE="ErrorDet";
    LinkFile;
    FILE="dLayer"; MODE="FTP";
    LinkFileDlayer;
    FILE="dLayer"; MODE="INJ";
    LinkFileDlayer;
}


MAIN(){

mkDB.sh --online --yellow | tee summary/Online_Yellow.dat ; mkDB.sh --online --blue | tee summary/Online_Blue.dat
dLayerAve.sh -f .cnipol_daemon_run.list --distribute | tee /tmp/cnipol/dLayerAve.log
root -b -q $MACRODIR/DlayerMonitor.C
mkDB.sh --blue > summary/OfflinePol_Blue_all.dat ; mkDB.sh --yellow > summary/OfflinePol_Yellow_all.dat
root -b -q $MACRODIR/OfflinePol.C
mkDB.sh --error-detector --blue > summary/ErrorDet_Blue_all.dat ; mkDB.sh --error-detector --yellow > summary/ErrorDet_Yellow_all.dat
root -b /usr/local/bin/macro/ErrorDetector.C
}



cd $ASYMDIR;
LINK_FILE;

MAIN;


