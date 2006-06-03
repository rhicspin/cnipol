#!/bin/bash
#dLayerAve.sh
#Makes deadlayer database
#Feb.24,2006
#I. Nakagawa

DISTRIBUTION=0;
BASEDIR=$ASYMDIR
ExeDlayerAverage=1;
DLAYERDIR=$ASYMDIR/dlayer
RUNLIST=$ASYMDIR/.runlist
DUMMY=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`
    echo    " "
    echo    " $COMMAND [-xh][-f <file>][--distribute]"
    echo    "    : make deadlayer analysis database "
    echo    " "
    echo -e "   --distribute  Distribute blue/yellow & flattop/injection measurements."
    echo -e "   -f <file>     Make list from runlist <file>";
    echo -e "   -h | --help   Show this help"
    echo -e "   -x            Show example"
    echo    " "
    exit;
}

 
ShowExample(){
 
    echo    " "
    echo    "1. make deadlayer database from runlist=.runlist";
    echo    " "
    echo    "    mkDB.sh -f .runlist"
    echo    " "
    exit;

}


#############################################################################
#                                  InitVariables()                          #
#############################################################################
InitVariables(){
    
    EVENT_RATES=0;
    READ_RATES=0;
    AVE_Dl=0;
    AVE_Dl_ERROR=0;
    AVE_WCM=0;
    FILL_BUNCH=0;
    SPECIFIC_LUMI=0;
    TGT_POS=0;
    AVE_T0=0;
    DELTA_T0=0;

}


#############################################################################
#                                  InitVariables()                          #
#############################################################################
ShowIndex(){

    printf "=============================================================";
    printf "===============\n"
    printf " RunID    ";
    printf " Dl_ave";
    printf " err";
    printf " Rate   ";
    printf " WCM " ;
    printf " SpeLumi ";
    printf " #of "
    printf " T0_ave";
    printf " DeltaT0";
    printf "\n";
    printf "            ";
    printf "[ug/cm^2]";
    printf "";
    printf " [Hz]   ";
    printf " ave ";
    printf "        ";
    printf " Fill  ";
    printf " [ns]   ";
    printf " [ns]";
    printf "\n";
    printf "=============================================================";
    printf "===============\n"

}


#############################################################################
#                                DlayerAverage()                            #
#############################################################################
DlayerAverage(){

NLINE=`wc $RUNLIST | gawk '{print $1}'`

for (( i=1; i<=$NLINE ; i++ )) ;
  do
    RUNID=`line.sh $i $RUNLIST  | gawk '{print $1}'`
    FILL=`echo $RUNID | gawk '{printf("%4d",$RUNID)}'`;
    Test=`echo $RUNID $FILL | gawk '{RunN=($1-$2)*10; printf("%1d",RunN)}'`
    if [ $Test -eq 1 ] ; then
	Beam="Yellow"
    else
	Beam="Blue"
    fi

    DlayerFile=$DLAYERDIR/$RUNID.temp.dat;
    FITLOGFILE=$DLAYERDIR/$RUNID.fit.log;
    LOGFILE=$BASEDIR/douts/$RUNID.dl.log;
    ONLINE_LOG=$ONLINEDIR/log/$RUNID.log;

    Test=`grep 'Beam Energy :' $LOGFILE | gawk '{printf(" %4d",$4)}'`;
    if [ $Test -gt 30 ] ; then
	Mode="FTP";
    else
	Mode="INJ";
    fi

    if [ $DISTRIBUTION -eq 1 ] ; then
	OFILE=$ASYMDIR/summary/dLayer_$Beam\_$Mode\_all.dat;
    fi

    AT_BUNCH=0;
    if [ -f $ONLINE_LOG ] ; then
	   AT_BUNCH=`grep 'AT Bunch:' $ONLINE_LOG | gawk '{print $2}' | sed -e 's/Bunch://' | head -n 1`;
	   if [ ! $AT_BUNCH ] ; then
	       AT_BUNCH=0;
           fi
    fi


    if [ -f $DlayerFile ] ; then

	    EVENT_RATES=`grep 'Event Rate'  $LOGFILE | gawk '{printf("%4.2f", $4*1e-6)}'`
	    READ_RATES=`grep 'Read Rate' $LOGFILE | gawk '{printf("%4.2f", $5*1e-6)}'`
	    AVE_Dl=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`
	    AVE_Dl_ERROR=`grep "Deviation/strip=" $FITLOGFILE | gawk '{printf("%5.2f", $2)}'`
	    AVE_WCM=`grep 'WCM Average  ' $LOGFILE | gawk '{printf("%7.1f", $4)}'`
	    FILL_BUNCH=`grep '# of Filled Bunch' $LOGFILE | gawk '{printf(" %3d", $6)}'`
	    SPECIFIC_LUMI=`grep 'Specific Luminosity' $LOGFILE | gawk '{printf("%6.3f",$6)}'`
	    TGT_POS=`grep @ $LOGFILE | gawk '{print $4}'`
	    AVE_T0=`grep " t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	    DELTA_T0=`grep " Delta_t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;

	    if [ $AVE_Dl ]&&[ $AVE_WCM ]  ; then
		echo -e -n "$RUNID $AVE_Dl $AVE_Dl_ERROR $READ_RATES "   | tee -a $OFILE 
		echo -e -n "$AVE_WCM $SPECIFIC_LUMI $FILL_BUNCH   "      | tee -a $OFILE
		echo -e -n "$AVE_T0 $DELTA_T0  $AT_BUNCH $DUMMY $DUMMY " | tee -a $OFILE
		echo -e -n "$DUMMY $DUMMY $DUMMY $DUMMY $DUMMY "         | tee -a $OFILE
		echo -e -n "\n"                                          | tee -a $OFILE
	    fi

    fi

  done


}


#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  --distribute) DISTRIBUTION=1;;
  -f) shift ; RUNLIST=$1 ;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


InitVariables;

if [ $DISTRIBUTION -eq 1 ] ; then
    rm -f $ASYMDIR/summary/dLayer_*_*_all.dat &> /dev/null;
    if [ ! -d $ASYMDIR/summary ] ; then
	mkdir $ASYMDIR/summary
    fi
fi

ShowIndex;
if [ $ExeDlayerAverage == 1 ] ; then
    DlayerAverage;
fi



