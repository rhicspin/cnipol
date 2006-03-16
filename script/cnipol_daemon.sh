#!/bin/bash
# run deadlayer fit / Asym calculation in background
# March, 15, 2006
# I. Nakagawa


CNI_DAEMON_RUNLIST=$ASYMDIR/.cnipol_daemon_run.list;
HBOOKDIR=$ASYMDIR/hbook
LOGDIR=$ASYMDIR/log
FROM_FILL=7605;
TILL_FILL=9000;
ExeDlayerFit=0;
SLEEP_TIME=3;

# check runlist
if [ -f $CNI_DAEMON_RUNLIST ] ; then
   echo "Updates exising $CNI_DAEMON_RUNLIST ...";
else
   echo "Create $CNI_DAEMON_RUNLIST ...";
   touch $CNI_DAEMON_RUNLIST ;
fi


#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " cnipol_daemon.sh [-xh][--dlayer-fit][-F <Fill#>][--fill-from <Fill#>][--fill-till <Fill#>]"
    echo    "    : search for new run which has not been analyized and then run analysis program "
    echo    " "
    echo -e "   -F <Fill#>                Show list <Fill#>"
    echo -e "   --delayer-fit             run deadlayer fit"
    echo -e "   --fill-from <Fill#>       Make list from <Fill#> [def]:$FROM_FILL";
    echo -e "   --fill-till <Fill#>       Make list till <Fill#> [def]:$TILL_FILL";
    echo -e "   -h | --help               Show this help"
    echo -e "   -x                        Show example"
    echo    " "
    exit;
}

 
ShowExample(){
 
    echo    " "
    echo    "1. run deadlayer analysis in background from fill#7575 till 7590"
    echo    " "
    echo    "    cnipol_daemon.sh --fill-from 7575 --fill-till 7590 --dlayer-fit"
    echo    " "
    echo    " "
    exit;

}


#############################################################################
#                             RunDlayer()                                   #
#############################################################################
RunDlayer(){

    echo $RunID ;
    dLayer.pl -f $RunID 
    mkConfig.pl -f $RunID
    dLayer.pl -f $RunID -b -F ./config/$RunID.config.dat

}


#############################################################################
#                             RunAsym()                                     #
#############################################################################
RunAsym(){

    Asym -f $RunID -b -o $RunID.hbook | tee $LOGDIR/$RunID.log ;
    mv $RunID.hbook $HBOOKDIR

}


#############################################################################
#                             CNIPOL_DAEMON()                               #
#############################################################################
CNIPOL_DAEMON(){

while [ 1 ] ; 
  do 

  for f in `ls $DATADIR/????.???.data` ;
    do 
    RunID=`basename $f | sed -e 's/\.data//'`; 
    fill=`echo $RunID | sed -e 's/\./ /' | gawk '{print $1}'`;
    run=`echo $RunID | sed -e 's/\./ /' | gawk '{print $2}'`;
    echo -e -n "searching for next unanalyzied run $RunID ...\r"
    if [ $fill -ge $FROM_FILL ]&&[ $fill -le $TILL_FILL ]  ; then
	grep $RunID $CNI_DAEMON_RUNLIST > /dev/null;
	if [ $? == 1 ] ; then
	    if [ $ExeDlayerFit==1 ]; then
		    echo -e -n "\n Analyzing $RunID \n"
		    RunDlayer;
		    echo $RunID >> $CNI_DAEMON_RUNLIST;
	    fi
	else 
	    echo -e -n "\n $RunID has already been analyized. Skip. \n";
	fi
	
    fi  
 
  done;

echo "sleeping $SLEEP_TIME [s] ..."
sleep $SLEEP_TIME

#end-of-while loop
done;

}



#############################################################################
#                                    Main                                   #
#############################################################################

while test $# -ne 0; do
  case "$1" in
  -F) shift ; FROM_FILL=$1 ;TILL_FILL=$1 ;;
  --fill-from) shift ; FROM_FILL=$1;;
  --fill-till) shift ; TILL_FILL=$1;;
  --dlayer-fit) ExeDlayerFit=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


CNIPOL_DAEMON;

