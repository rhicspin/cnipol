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
SINGLE_STRIP=1;
DUMMY=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`
    echo    " "
    echo    " $COMMAND [-xh][-f <file>][--distribute][--strip <st>]";
    echo    "    : make deadlayer analysis database ";
    echo    " "
    echo -e "   --distribute  Distribute blue/yellow & flattop/injection measurements."
    echo -e "   --strip <st>  Single strip number to be extracted [def]=$SINGLE_STRIP";
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
    echo    " ";
    echo    "2. extract single strip #2 and distribute to output files";
    echo    " "
    echo    "    mkDB.sh -f .cnipol_daemon_run.list --distribute --strip 2"
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
    Dldet1=0;
    RANGE_MIN=0;
    RANGE_MAX=0;
    t0Strip1=0;
    t0EStrip1=0;
    DlStrip1=0;
    DlEStrip1=0;
    t0_2par_Strip1=0;
    t0E_2par_Strip1=0;

}


#############################################################################
#                                  InitVariables()                          #
#############################################################################
ShowIndex(){

    printf "================================================================";
    printf "===============================================================\n";
    printf " RunID    ";
    printf " Dl_ave";
    printf " err";
    printf " Rate   ";
    printf " WCM " ;
    printf " SpeLumi ";
    printf " #of "
    printf " T0_ave";
    printf " DeltaT0";
    printf " Emin";
    printf " Emax";
    printf " Bnch";
    printf " Dl_ave   ";
    printf " Strip-$SINGLE_STRIP"
    printf " ==================> "
    printf " (2par)"
    printf "\n";
    printf "            ";
    printf "[ug/cm^2]";
    printf "";
    printf " [Hz]   ";
    printf " ave ";
    printf "        ";
    printf " Fill  ";
    printf " [ns]   ";
    printf " [ns]  ";
    printf " keV ";
    printf " keV";
    printf " Mode "
    printf " Det1   ";
    printf " t0    ";
    printf " t0E   ";
    printf " dl    ";
    printf " dlE   ";
    printf " t0    ";
    printf " t0E   ";
    printf "\n";
    printf "================================================================";
    printf "===============================================================\n";

}


#############################################################################
#                                DlayerAverage()                            #
#############################################################################
DlayerAverage(){

NLINE=`wc $RUNLIST | gawk '{print $1}'`

for (( i=1; i<=$NLINE ; i++ )) ;
  do
  InitVariables;
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
    if [ $Test -gt 25 ] ; then
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
	    READ_RATES=`grep 'Read Rate' $LOGFILE | gawk '{printf("%4.2f", $5)}'`
	    AVE_Dl=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`
	    AVE_Dl_ERROR=`grep "Deviation/strip=" $FITLOGFILE | gawk '{printf("%5.2f", $2)}'`
	    AVE_WCM=`grep 'WCM Average \[10^9' $LOGFILE | gawk '{printf("%7.1f", $6)}'`
	    FILL_BUNCH=`grep '# of Filled Bunch' $LOGFILE | gawk '{printf(" %3d", $6)}'`
	    SPECIFIC_LUMI=`grep 'Specific Luminosity' $LOGFILE | gawk '{printf("%6.3f",$6)}'`
	    TGT_POS=`grep @ $LOGFILE | gawk '{print $4}'`
	    AVE_T0=`grep " t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	    DELTA_T0=`grep " Delta_t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	    RANGE_MIN=`grep "Emin-Emax" $FITLOGFILE | gawk '{print $6}' | sed -e '{s/-/ /}'`
	    RANGE_MAX=`grep "Emin-Emax" $FITLOGFILE | gawk '{print $7}'`

	    #Single Strip 
	    cat $DlayerFile | gawk '{print $1+1}' > $TMPOUTDIR/dLayerAve.log;
	    LINE=`grep -n $SINGLE_STRIP $TMPOUTDIR/dLayerAve.log | head -n 1 | sed -e "s/:/ /" | gawk '{print $1}'`;
	    STRIP=`line.sh $LINE $DlayerFile | gawk '{print $1+1}'`;
	    if [ $LINE ] ; then
		Dldet1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$2)}'`;
		t0Strip1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$3)}'`;
		t0EStrip1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$4)}'`;
		DlStrip1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$5)}'`;
		DlEStrip1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$6)}'`;
		t0_2par_Strip1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$7)}'`;
		t0E_2par_Strip1=`line.sh $LINE $DlayerFile | gawk '{printf("%6.2f",$8)}'`;
	    fi

	    if [ $AVE_Dl ]&&[ $AVE_WCM ]  ; then
		echo -e -n "$RUNID $AVE_Dl $AVE_Dl_ERROR $READ_RATES "        | tee -a $OFILE 
		echo -e -n "$AVE_WCM $SPECIFIC_LUMI $FILL_BUNCH   "           | tee -a $OFILE
		echo -e -n "$AVE_T0 $DELTA_T0   $RANGE_MIN $RANGE_MAX "       | tee -a $OFILE
		echo -e -n "$AT_BUNCH $Dldet1 $t0Strip1 $t0EStrip1 "          | tee -a $OFILE
		echo -e -n "$DlStrip1 $DlEStrip1 $t0_2par_Strip1 $t0E_2par_Strip1"  | tee -a $OFILE
		echo -e -n "\n"                                               | tee -a $OFILE
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
  --strip) shift; SINGLE_STRIP=$1;;
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



