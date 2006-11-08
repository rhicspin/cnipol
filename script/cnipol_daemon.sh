#!/bin/bash
# run deadlayer fit / Asym calculation in background
# March, 15, 2006
# I. Nakagawa

#cnipol_daemon in to_be_analyzed_list mode;
TO_BE_ANALYZED_LIST=$ASYMDIR/.cnipol_daemon_to_be_analyzed_asym.list;
ANALYZE_FROM_FILE=0;

RUNLIST=$DATADIR/raw_data.list;
CNI_DAEMON_RUNLIST=$ASYMDIR/.cnipol_daemon_run.list;
DLAYERDIR=$ASYMDIR/dlayer
HBOOKDIR=$ASYMDIR/hbook
LOGDIR=$ASYMDIR/log
FROM_FILL=6500;
TILL_FILL=9000;
SLEEP_TIME=1800;

# deadlayer fit environments
CNI_DAEMON_DLAYER_ERANGE=$ASYMDIR/.cnipol_daemon_dlayer_erange;
CNI_DAEMON_DLAYER_STUDY=$ASYMDIR/.cnipol_daemon_dlayer_iteration;
ExeDlayerFitSimple=0;
ExeDlayerFit=0;
MAX_ITERATION=6;
TOLERANCE=1; 

#Asym environments
ANALYZED_RUNLIST_DAEMON=$ASYMDIR/analyzed_runlist.daemn;
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
    echo    "                  [--run-Asym][--runlist <runlist>][--analyze-from-list]";
    echo    "    : search for new run from <runlist> which has not been analyized and then run"
    echo    "      analysis program."
    echo    " "
    echo -e "   --analyze-from-list       analyzed from <file> without checking .cnipol_daemon_run.list."
    echo -e "                             [def <file>]=$TO_BE_ANALYZED_LIST";
    echo -e "   --runlist <runlist>       data run list file [def]:$RUNLIST";
    echo -e "   -F <Fill#>                run program for runs in <Fill#>"
    echo -e "   --fill-from <Fill#>       run program for runs from <Fill#> [def]:$FROM_FILL";
    echo -e "   --fill-till <Fill#>       run program for runs till <Fill#> [def]:$TILL_FILL";
    echo -e "   --run-Asym                run Asym"
    echo -e "   --delayer-fit             run deadlayer fit"
    echo -e "   --max-iteration <int>     Maximum iteration for deadlayer fit [def]:$MAX_ITERATION";
    echo -e "   --toleratnce <[%]>        Tolerance in [%] to be converged. [def]:$TOLERANCE";
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
    echo    "3. run deadlayer fit without iteration."
    echo    " "
    echo    "    cnipol_daemon.sh --dlayer-fit --iteration 0."
    echo    " "
    exit;

}


#############################################################################
#                             RunDlayer()                                   #
#############################################################################
RunDlayer(){

    FITLOGFILE=$DLAYERDIR/$RunID.fit.log;

    echo $RunID  ;

    if [ $MAX_ITERATION -eq 0 ] ; then

	echo "No iteration";
	dLayerCal.pl -f $RunID -b ;
	mkConfig.pl -f $RunID ;
	mkConfig.pl -f $RunID -p ;

    else 

    echo -e -n "$RunID " >> $CNI_DAEMON_DLAYER_STUDY;
    echo -e -n "$RunID " >> $CNI_DAEMON_DLAYER_ERANGE;
    # First iteration without -b option
    dLayer.pl -f $RunID -i
    mkConfig.pl -f $RunID
    AVE_Dl_1=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
    AVE_T0_1=`grep " t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
    grep "Energy Range" $FITLOGFILE >> $CNI_DAEMON_DLAYER_ERANGE;

    # Loop for further iteration with -b option
    for (( i=2; i<=$MAX_ITERATION; i++ )) ; do 

	dLayer.pl -f $RunID -i -b -F ./config/$RunID.config.dat
	mkConfig.pl -f $RunID ;

	AVE_Dl_2=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	AVE_T0_2=`grep " t0 average=" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`;
	echo -e -n "$AVE_Dl_2 $AVE_Dl_1" | converge >> $CNI_DAEMON_DLAYER_STUDY;
	echo -e -n "$AVE_T0_2 $AVE_T0_1" | converge >> $CNI_DAEMON_DLAYER_STUDY;
	grep "Energy Range" $FITLOGFILE >> $CNI_DAEMON_DLAYER_ERANGE;

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
    
    # if [ $MAX_ITERATION -eq 0 ] 
    fi  

}



#############################################################################
#                             RunAsym()                                     #
#############################################################################
RunAsym(){
    
    NEVENTS=`OnlinePol.sh -f $RunID --nevents -k`;
    if [ $NEVENTS -lt 50 ] ; then
	echo -e "\n";
	echo -e "[$RunID]@" | tee $TMPOUTDIR/rundb.log;
	echo -e "\tRUN_STATUS*=Junk;@" | tee -a $TMPOUTDIR/rundb.log;
	echo -e "\tCOMMENT*=\"Number of events < 50k.\";@" | tee -a $TMPOUTDIR/rundb.log;
	echo -e "\n";
#	rundb_updater.pl
	rundb_inserter.sh 
    fi
    echo -e "Deadlayer File Check : dLayerChecker -f $RunID";
    dLayerChecker -f $RunID;
    if [ $? -eq 0 ] ; then
	dLayerChecker -z $RunID
#	rundb_updater.pl 
	rundb_inserter.sh 
    fi

    nice -n 19 Asym -f $RunID -b -o hbook/$RunID.hbook | tee log/$RunID.log;	
    mv $RunID.root root/.;
    echo $RunID >> $ANALYZED_RUNLIST_DAEMON

}


#############################################################################
#                             CNIPOL_DAEMON()                               #
#############################################################################
CNI_Operation(){

    if [ $ExeDlayerFit -eq 1 ]; then
	RunDlayer;
	echo $RunID >> $CNI_DAEMON_RUNLIST;
    fi
    if [ $ExeRunAsym -eq 1 ]; then
	RunAsym;
	echo $RunID >> $CNI_DAEMON_RUNLIST;
    fi

}


#############################################################################
#                   CNIPOL_DAEMON_FROM_LIST()                               #
#############################################################################
CNIPOL_DAEMON_FROM_LIST(){
TMPLIST=$TMPOUTDIR/cnipol_daemon_tmp.list;

if [ $ExeDlayerFit -eq 1 ] ; then
    echo "Running --analyze-from-list mode is not ready to run with --dlayer-fit operation";
    exit;
fi

while [ 1 ] ; 
  do 

  NLINE=`wc $TO_BE_ANALYZED_LIST | gawk '{print $1}'`;
  for RunID in `cat $TO_BE_ANALYZED_LIST`;
  do 
    if [ -f $DATADIR/$RunID.data ] ; then
	echo -e -n "Analyzing $RunID ...";
	CNI_Operation;
	grep -v $RunID $TO_BE_ANALYZED_LIST > $TMPLIST;
	mv -f $TMPLIST > $TO_BE_ANALYZED_LIST;
    fi
  done

  echo "sleeping $SLEEP_TIME [s] ..."
  sleep $SLEEP_TIME

  #end-of-while loop
  done;
    
}




#############################################################################
#                             CNIPOL_DAEMON()                               #
#############################################################################
CNIPOL_DAEMON(){
echo "Mission: DeadlayerFit=$ExeDlayerFit, Asym=$ExeRunAsym";


if [ $ExeDlayerFit -eq 1 ]; then
    HOST=`echo $HOSTNAME | sed -e 's/.rhic.bnl.gov//'`
    CNI_DAEMON_DLAYER_STUDY=$CNI_DAEMON_DLAYER_STUDY.$HOST;
    echo -e -n "Deadlayer Iteration Monitor file: $CNI_DAEMON_DLAYER_STUDY\n";
fi


while [ 1 ] ; 
  do 

  for f in `cat $RUNLIST` ;
#  for f in `ls $DATADIR/????.???.data` ;
    do 
    RunID=`basename $f | sed -e 's/\.data//'`; 
    fill=`echo $RunID | sed -e 's/\./ /' | gawk '{print $1}'`;
    run=`echo $RunID | sed -e 's/\./ /' | gawk '{print $2}'`;
    echo -e -n "searching for next unanalyzied run $RunID ...\r"
    if [ $fill -ge $FROM_FILL ]&&[ $fill -le $TILL_FILL ]  ; then
	grep $RunID $CNI_DAEMON_RUNLIST > /dev/null;
	if [ $? == 1 ] ; then
	    CNI_Operation;
	    echo -e -n "\n Analyzing $RunID \n"
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
  --analyze-from-list)        ANALYZE_FROM_FILE=1;;
  --runlist)       shift ; RUNLIST=$1 ;;
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



if [ $ANALYZE_FROM_FILE -eq 1 ] ; then
    CNIPOL_DAEMON_FROM_LIST;
else
    CNIPOL_DAEMON;
fi

