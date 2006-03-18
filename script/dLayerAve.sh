#!/bin/bash
#dLayerAve.sh
#Makes deadlayer database
#Feb.24,2006
#I. Nakagawa

BASEDIR=$ASYMDIR
ExeDlayerAverage=1;
DLAYERDIR=$ASYMDIR/dlayer
RUNLIST=$ASYMDIR/.runlist
DUMMY=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " mkDB.sh [-xh][-f <file>]"
    echo    "    : make deadlayer analysis database "
    echo    " "
    echo -e "   -f <file>                 Make list from runlist <file>";
    echo -e "   -h | --help               Show this help"
    echo -e "   -x                        Show example"
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

}


#############################################################################
#                                  InitVariables()                          #
#############################################################################
ShowIndex(){

    printf "=============================================================\n";
    printf " RunID    ";
    printf " Dl_ave";
    printf " err";
    printf " Rate ";
    printf "\n";
    printf "            ";
    printf "[ug/cm^2]";
    printf "";
    printf " [Hz]";
    printf "\n";
    printf "=============================================================\n";

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
    if [ -f $DlayerFile ] ; then

	    EVENT_RATES=`grep 'Event Rate'  $LOGFILE | gawk '{printf("%4.2f", $4*1e-6)}'`
	    READ_RATES=`grep 'Read Rate' $LOGFILE | gawk '{printf("%4.2f", $5*1e-6)}'`
	    AVE_Dl=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`
	    AVE_Dl_ERROR=`grep "Deviation/strip=" $FITLOGFILE | gawk '{printf("%5.2f", $2)}'`
	    AVE_WCM=`grep 'WCM Average  ' $LOGFILE | gawk '{printf("%7.2f", $4)}'`
	    FILL_BUNCH=`grep '# of Filled Bunch' $LOGFILE | gawk '{printf(" %3d", $6)}'`
	    SPECIFIC_LUMI=`grep 'Specific Luminosity' $LOGFILE | gawk '{printf("%6.3f",$6)}'`
	    TGT_POS=`grep @ $LOGFILE | gawk '{print $4}'`

	    echo -e -n "$RUNID $AVE_Dl $AVE_Dl_ERROR $READ_RATES " 
	    echo -e -n "$AVE_WCM $SPECIFIC_LUMI $FILL_BUNCH "
	    echo -e -n "$DUMMY $DUMMY $DUMMY $DUMMY $DUMMY "
	    echo -e -n "$DUMMY $DUMMY $DUMMY $DUMMY $DUMMY "
	    echo -e -n "\n";

    fi

  done


}


#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  -f) shift ; RUNLIST=$1 ;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


InitVariables;

ShowIndex;
if [ $ExeDlayerAverage == 1 ] ; then
    DlayerAverage;
fi



