#!/bin/bash
# run deadlayer fit / Asym calculation in background
# March, 15, 2006
# I. Nakagawa


CNI_DAEMON_RUNLIST=$ASYMDIR/.cnipol_daemon_run.list;
DLAYERDIR=$ASYMDIR/dlayer
HBOOKDIR=$ASYMDIR/hbook
LOGDIR=$ASYMDIR/log
FROM_FILL=7538;
TILL_FILL=9000;
SLEEP_TIME=1800;

# deadlayer fit environments
CNI_DAEMON_DLAYER_STUDY=$ASYMDIR/.cnipol_daemon_dlayer_iteration;
ExeDlayerFit=0;
MAX_ITERATION=4;
TOLERANCE=1; 

#Asym environments
ExeRunAsym=0;


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
    echo    " cnipol_daemon.sh [-xh][-F <Fill#>][--fill-from <Fill#>][--fill-till <Fill#>]"
    echo    "                  [--dlayer-fit][-s --sleep <time>][--max-iteration <int>]"; 
    echo    "                  [--run-Asym]";
    echo    "    : search for new run which has not been analyized and then run analysis program "
    echo    " "
    echo -e "   -F <Fill#>                Show list <Fill#>"
    echo -e "   --run-Asym                run Asym"
    echo -e "   --delayer-fit             run deadlayer fit"
    echo -e "   --max-iteration <int>     Maximum iteration for deadlayer fit [def]:$MAX_ITERATION";
    echo -e "   --toleratnce <[%]>        Tolerance in [%] to be converged. [def]:$TOLERANCE";
    echo -e "   --fill-from <Fill#>       Make list from <Fill#> [def]:$FROM_FILL";
    echo -e "   --fill-till <Fill#>       Make list till <Fill#> [def]:$TILL_FILL";
    echo -e "   -s --sleep <time>         Sleep <time> in sec [def]:$SLEEP_TIME";
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
    echo    "2. run Asym program for Fill#7575"
    echo    " "
    echo    "    cnipol_daemon.sh -F 7575 --run-Asym"
    echo    " "
    exit;

}


#############################################################################
#                             RunDlayer()                                   #
#############################################################################
RunDlayer(){

    FITLOGFILE=$DLAYERDIR/$RunID.fit.log;

    echo $RunID  ;
    echo -e -n "$RunID " >> $CNI_DAEMON_DLAYER_STUDY;

    # First iteration without -b option
    dLayer.pl -f $RunID 
    mkConfig.pl -f $RunID
    AVE_Dl_1=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
    AVE_T0_1=`grep " t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;

    # Loop for further iteration with -b option
    for (( i=2; i<=$MAX_ITERATION; i++ )) ; do 

	dLayer.pl -f $RunID -b -F ./config/$RunID.config.dat
	mkConfig.pl -f $RunID ;

	AVE_Dl_2=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	AVE_T0_2=`grep " t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	echo -e -n "$AVE_Dl_2 $AVE_Dl_1" | converge >> $CNI_DAEMON_DLAYER_STUDY;
	echo -e -n "$AVE_T0_2 $AVE_T0_1" | converge >> $CNI_DAEMON_DLAYER_STUDY;

	# Check if results are converged or not
	TEST1=`echo -e -n "$AVE_Dl_2 $AVE_Dl_1" | converge -t $TOLERANCE` 
	TEST2=`echo -e -n "$AVE_T0_2 $AVE_T0_1" | converge -t $TOLERANCE` 

	# Following 3 lines are for debugging
	echo -e -n "Iteration: $i DL= $AVE_Dl_1 $AVE_Dl_2 \n";
	echo -e -n "Iteration: $i T0= $AVE_T0_1 $AVE_T0_2 \n";
	echo -e "TEST Results : $TEST1 $TEST2";

	if [ $TEST1 -eq 1 ]&&[ $TEST2 -eq 1 ] ; then

	    # OK, both deadlayer and t0 are converged
	    mkConfig.pl -f $RunID -p ;
	    echo -e -n "\n" >> $CNI_DAEMON_DLAYER_STUDY;
	    echo -e "Ok $RunID deadlayer fit Converged, Move on to run";
	    break;

	else

	    if [ $i -eq $MAX_ITERATION ] ; then
		echo -e -n " Didn't Converge after $i Iteration\n" >> $CNI_DAEMON_DLAYER_STUDY;
		break;
	    fi

	    # update reference dl/t0 results
	    AVE_Dl_1=$AVE_Dl_2;
	    AVE_T0_1=$AVE_T0_2;

	fi
    done

}



#############################################################################
#                             RunAsym()                                     #
#############################################################################
RunAsym(){

    Asym -f $RunID -o $RunID.hbook | tee $LOGDIR/$RunID.log ;
    mv $RunID.hbook $HBOOKDIR

}


#############################################################################
#                             CNIPOL_DAEMON()                               #
#############################################################################
CNIPOL_DAEMON(){

if [ ExeDlayerFit==1 ]; then
    HOST=`echo $HOSTNAME | sed -e 's/.rhic.bnl.gov//'`
    CNI_DAEMON_DLAYER_STUDY=$CNI_DAEMON_DLAYER_STUDY.$HOST;
    echo -e -n "Deadlayer Iteration Monitor file: $CNI_DAEMON_DLAYER_STUDY\n";
fi


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
	    echo -e -n "\n Analyzing $RunID \n"
	    if [ $ExeDlayerFit==1 ]; then
		    RunDlayer;
		    echo $RunID >> $CNI_DAEMON_RUNLIST;
	    fi
	    if [ $ExeRunAsym==1 ]; then
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
  -F)              shift ; FROM_FILL=$1 ;TILL_FILL=$1 ;;
  --fill-from)     shift ; FROM_FILL=$1;;
  --fill-till)     shift ; TILL_FILL=$1;;
  --run-Asym)              ExeRunAsym=1;;
  --dlayer-fit)            ExeDlayerFit=1;;
  --max-iteration) shift ; MAX_ITERATION=$1 ;;
  --tolerance)     shift ; TOLERANCE=$1 ;;
  -s | --sleep)    shift ; SLEEP_TIME=$1 ;;
  -x)              shift ; ShowExample ;;
  -h | --help)              help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


CNIPOL_DAEMON;

