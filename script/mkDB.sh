#!/bin/bash
#make run database
#I.Nakagawa
#March, 11, 2006
ANALYZED_RUN_LIST="$ASYMDIR/analyzed_run.list";
ONLINE_DB="$DATADIR/OnlinePol.dat";
ExeAnalyzedRunList=0 ;
ExeMakeDatabase=1;
ExclusiveMode=0;
ExeOnlineNevents=0;
ExeOnlineDatabase=0;
ExeDlayerConfig=0;
ExeErrorDetector=0;
ExpertMode=0;
FROM_FILL=7537;
if [ $RHICRUN ] ; then
    if [ $RHICRUN -eq 5 ] ; then
	FROM_FILL=6600;
    fi
fi
TILL_FILL=9000;
LOGDIR=$ASYMDIR/log;
DISTRIBUTION=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " mkDB.sh [-xh][-F <Fill#>][--fill-from <Fill#>][--fill-till <Fill#>]"
    echo    "         [-a --analyzed-run-list][-X --expert][-f <runlis>][--blue][--yellow]";
    echo    "         [--online][--online-nevents][--dlayer-config][--error-detector]";
    echo    "    : make pC offline/offline analysis database "
    echo    " "
    echo -e "   -a --analyzed-run-list    Make analyized runlist file [def]:$ANALYZED_RUN_LIST";
    echo -e "   -F <Fill#>                Show list <Fill#>"
    echo -e "   --fill-from <Fill#>       Make list from <Fill#>";
    echo -e "   --fill-till <Fill#>       Make list till <Fill#>";
    echo -e "   -f <runlist>              Show list for runs listed in <runlist>";
    echo -e "   --blue                    Show only blue data ";
    echo -e "   --yellow                  Show only yellow data ";
    echo -e "   --exclusive               Show only data analyized";
    echo -e "   --dlayer-config           Show database for configulation files loaded by Asym";
    echo -e "   --online-nevents          Show online nevents"
    echo -e "   --online                  Show online parameters"
    echo -e "   --error-detector          Show error detector results";
    echo -e "   -X --expert               Show list in expert mode";
    echo -e "   -h | --help               Show this help"
    echo -e "   -x                        Show example"
    echo    " "
    exit;
}

 
ShowExample(){
 
    echo    " "
    echo    "1. make database from fill#7575 till 7590"
    echo    " "
    echo    "    mkDB.sh --fill-from 7575 --fill-till 7590"
    echo    " "
    echo    "2. make analyzed runlist = $ANALYZED_RUN_LIST"
    echo    " "
    echo    "    mkDB.sh -a"
    echo    " "
    echo    "3. make exclusive database for blue from <runlist> and dump to <file>"
    echo    " "
    echo    "    mkDB.sh --blue -f <runlist> --exclusive | tee <file>"
    echo    " "
    echo    "4. make configulation file database loaded in Asym.";
    echo    " "
    echo    "    mkDB.sh --blue -f <runlist> --exclusive --dlayer-config"
    echo    " "
    exit;

}


#############################################################################
#                       MakeAnalyzedRunList()                               #
#############################################################################
MakeAnalyzedRunList(){

  if [ ! -d $TMPOUTDIR ]; then
      mkdir $TMPOUTDIR;
  fi

  TMPLIST="$TMPOUTDIR/tmp.list";
  if [ -f $TMPLIST ] ; then
      rm -f $TMPLIST;
  fi
  touch $TMPLIST;

  for a in alanH alanD jeffW itaru daemn koich; 
  do 

    f="$ASYMDIR/analyzed_runlist.$a"
    if [ -f $f ] ; then
	NLINE=`wc $f | gawk '{print $1}'`
	for (( i=1; i<=$NLINE; i++ )) ; 
	  do
	  RunID=`$INSTALLDIR/line.sh $i $f` 
	  FILL=`echo $RunID | gawk '{printf("%4d",$RUNID)}'`;
	  Test=`echo $RunID $FILL | gawk '{RunN=($1-$2)*10; printf("%1d",RunN)}'`
	  if [ $Test -eq 1 ]&&[ $DISTRIBUTION -eq 2 ] ; then
	      Beam="Yellow"
	      echo -e -n "$RunID $a\n"  >> $TMPLIST
	  elif [ $Test -eq 0 ]&&[ $DISTRIBUTION -eq 1 ] ; then
	      Beam="Blue"
	      echo -e -n "$RunID $a\n"  >> $TMPLIST
	  elif [ $DISTRIBUTION -eq 0 ] ; then
	      echo -e -n "$RunID $a\n"  >> $TMPLIST
	  fi
	  
	done

    fi

  done


  # sorting 
  sort $TMPLIST > $ANALYZED_RUN_LIST;
  mv -f $ANALYZED_RUN_LIST $TMPLIST;

  #eliminate double counting same RunID
  NLINE=`wc $TMPLIST | gawk '{print $1}'`;
  for (( i=1; i<=$NLINE; i++ )); do
      run=`line.sh $i $TMPLIST | gawk '{print $1}'`;
      if [ $i -eq 1 ] ; then 
	  line.sh $i $TMPLIST >>  $ANALYZED_RUN_LIST;
      else 
	  if [ $run != $prev ] ; then
	      line.sh $i $TMPLIST >> $ANALYZED_RUN_LIST;
	  fi
      fi
      prev=$run;
  done


  if [ $ExeMakeDatabase != 1 ] ; then
      cat $ANALYZED_RUN_LIST;
  fi

  rm -f $TMPLIST;

}

#############################################################################
#                                ShowIndex()                                #
#############################################################################
ShowIndex(){

    printf "=================================================================================================";
    printf "============================\n";
    printf " RunID ";
    printf "       Online";
    printf "  Status  ";
    printf "Anal  "
    printf "Beam "
    printf "      Date/Time ";
    printf "         Offline";
    printf "    Phase [deg] ";
    printf " chi2 ";
    printf " A_N ";
    printf " Target";
    printf " Rate";
    printf " SpLm";
    printf "   on-off";
    printf "\n";
    printf "          ";
    printf "   P     dP ";
    printf "       "
    printf " crew ";
    printf "[GeV]                       ";
    printf "   P     dP";
    printf "     phi  dphi";
    printf " /dof ";
    printf "  ave ";
    printf "       ";
    printf "[MHz]"
    printf "  RMS"
    printf "   Error"
    printf "\n";
    printf "=================================================================================================";
    printf "============================\n";


}



ShowIndexOnline(){

    printf "===============================================================================================\n";
    printf " RunID     ";
    printf " Date/Time   ";
    printf " Bunch";
    printf " BZDelay ";
    printf " Threshoulds";
    printf " Nevents ";
    printf " A_N  ";
    printf " Dl(ave) ";
    printf " config file ";
    printf " \n";
    printf " \t";
    printf " \t";
    printf " \t";
    printf " \t";
    printf " \t     ";
    printf " [keV] ";
    printf " [M]   ";
    printf " (ave)";
    printf " [ug/cm2] ";
    printf " \n";
    printf "===============================================================================================\n";


}

ShowDlayerConfigIndex(){

    printf "=====================================================================================\n";
    printf " RunID  ";
    printf " ConfigID";
    printf " Dlayer";
    printf " Error";
    printf " Rate";
    printf " \n";
    printf "=====================================================================================\n";


}

ShowErrorDetectorIndex(){

    printf "=====================================================================================\n";
    printf " RunID    #    #Bad   Bad  Error   Max    #Bad     Mass      \n"; 
    printf "        bunch  bunch  Rate  Code   Dev    strip   MaxChi2    \n";
    printf "=====================================================================================\n";

}


#############################################################################
#                              ErrorDetector                                #
#############################################################################

ErrorDetector(){

    
   PROBLEM_BUNCH=-1;
   PROBLEM_STRIP=-1;

   LOGFILE=$ASYMDIR/log/$RunID.log;
   if [ -f $LOGFILE ] ; then
       NBUNCH=`grep '# of Filled Bunch          ' $LOGFILE | gawk '{printf("%3d",$6)}'`;
       BUNCH_ERR_CODE=`grep 'Bunch error code     ' $LOGFILE | gawk '{printf("%4s",$5)}'`;
       MAX_SPELUMI_DEV=`grep ' Max SpeLumi deviation from average' $LOGFILE | gawk '{printf("%5.1f",$7)}'`;
       BAD_BUNCH_RATE=`grep 'Problemeatic Bunches Rate' $LOGFILE | gawk '{printf("%5.1f",$6)}'`;
       PROBLEM_BUNCH=`grep 'Number of Problemeatic Bunches' $LOGFILE | gawk '{printf("%2d",$6)}'`;
       PROBLEM_STRIP=`grep 'Number of Problematic Strips' $LOGFILE | gawk '{printf("%2d",$6)}'`;
       MAX_MASS_CHI2=`grep 'Maximum Mass fit chi-2' $LOGFILE | gawk '{printf("%7.2f",$6)}'`;    
#       MAX_MASS_CHI2_STRIP=`grep 'Maximum Mass fit chi-2' $LOGFILE | gawk '{printf("%7.2f",$7)}'`;    
       UNRECOG_STRIP=`grep 'Unrecognized Problematic Strips' $LOGFILE | sed -e '{s/ Unrecognized Problematic Strips     ://}'`;
       if [ ! $NBUNCH ] ; then
	   NBUNCH=-1;
       fi
       if [ ! $PROBLEM_BUNCH ] ; then
	   PROBLEM_BUNCH=-1;
       fi
       if [ ! $PROBLEM_STRIP ] ; then
	   PROBLEM_STRIP=-1;
       fi
       if [ ! $MAX_MASS_CHI2 ] ; then
	   MAX_MASS_CHI2=-1;
       fi
       if [ ! $BUNCH_ERR_CODE ] ; then
	   BUNCH_ERR_CODE=-1;
       fi
       if [ ! $MAX_SPELUMI_DEV ] ; then
	   MAX_SPELUMI_DEV=-1;
       fi
       if [ ! $BAD_BUNCH_RATE ] ; then
	   BAD_BUNCH_RATE=-1;
       fi
   fi

   if [ $PROBLEM_BUNCH -ge 1 ] ; then
       echo -e -n "$RunID";
       echo -e -n "  $NBUNCH   $PROBLEM_BUNCH   $BAD_BUNCH_RATE  $BUNCH_ERR_CODE $MAX_SPELUMI_DEV ";
       echo -e -n "  $PROBLEM_STRIP    $MAX_MASS_CHI2 ";
       echo -e -n "\n";
#   echo -e -n "  $UNRECOG_STRIP";
   fi

}





#############################################################################
#                              OnlinePolarization                           #
#############################################################################

GetOnlinePolarization(){

    OnlineP=`OnlinePol.sh -f $RunID --A_NCorrection | gawk '{printf("%7.1f",$1)}'`;
    OnlinedP=`OnlinePol.sh -f $RunID | gawk '{printf("%5.1f",$2)}'`;

}


GetOnlinePolFromFile(){

    OnlineP=`grep $RunID $ONLINE_DB | gawk '{printf("%7.1f",$2)}'`
    OnlinedP=`grep $RunID $ONLINE_DB | gawk '{printf("%5.1f",$3)}'`

}



OnlineNevents(){

   echo -e -n "$RunID";
   ONLINE_LOG=$ONLINEDIR/log/$RunID.log;
   grep ">>>" $ONLINE_LOG | tail -n 1 | gawk '{printf(" %2d ",$10/1e6)}';
   echo -e -n "\n";

}

OnlineDatabase(){

   ONLINE_LOG=$ONLINEDIR/log/$RunID.log;
   ONLINE_ANLOG=$ONLINEDIR/log/an$RunID.log;
   ONLINE_CONFIG_DLAYERAVE=$ONLINEDIR/config/dLayerAverage.list;

   MONTH=`grep '>>>>' $ONLINE_LOG | tail -n 1 | gawk '{print $3}'`;
   DATE=`grep '>>>>' $ONLINE_LOG | tail -n 1 | gawk '{print $4}'`;
   TIME=`grep '>>>>' $ONLINE_LOG | tail -n 1 | gawk '{print $5}'`;
   NEvents=`grep ">>>" $ONLINE_LOG | tail -n 1 | gawk '{printf(" %2d ",$10/1e6)}'`;
   CONFIG_FILE=`grep 'Reading calibration parameters' $ONLINE_LOG | tail -n 1 | gawk '{print $8}'`;
   TRIG_THRESHOLD_E=`grep 'Trigger threshold for enegry' $ONLINE_LOG | tail -n 1 | gawk '{print $6}'`;
   TRIG_THRESHOLD=`grep 'TrigThreshold:' $ONLINE_LOG | gawk '{print $1}' | tail -n 1 | sed -e 's/TrigThreshold://'`;
   BZ_DELAY=`grep 'TrigThreshold:' $ONLINE_LOG | gawk '{print $2}' | tail -n 1 | sed -e 's/BZDelay://'`
   AT_BUNCH=`grep 'AT Bunch:' $ONLINE_LOG | gawk '{print $2}' | tail -n 1 | sed -e 's/Bunch://'`;
   A_N=`grep 'Average analyzing power' $ONLINE_ANLOG | tail -n 1 | gawk '{print $7}' | sed -e '{s/NAN/0/}'`;

   dlAve=0;
   if [ $CONFIG_FILE ] ; then
       grep $CONFIG_FILE $ONLINE_CONFIG_DLAYERAVE > /dev/null
       if [ $? -eq 0 ]; then
	   dlAve=`grep $CONFIG_FILE $ONLINE_CONFIG_DLAYERAVE | gawk '{print $2}'`;
       fi
   fi
   
   echo -e -n "$RunID";
   printf " %s %2d %s " $MONTH $DATE $TIME;
   printf " %3d "     $AT_BUNCH;
   printf " %5d "     $BZ_DELAY;
   printf " %5d "     $TRIG_THRESHOLD;
   printf " %5.0f "   $TRIG_THRESHOLD_E;
   printf " %5.1f "   $NEvents;
   printf " %6.4f "   $A_N;
   printf " %5.1f "   $dlAve;
   printf " %s  "     $CONFIG_FILE;
   echo -e -n "\n";


}


#############################################################################
#                          dLyaerConfig()                                   #
#############################################################################
dLayerConfig(){

    if [ -f $TMPOUTDIR/mkDB.log ]; then
	rm -f $TMPOUTDIR/mkDB.log;
    fi

    echo -e -n "$RunID ";
    basename `grep CONFIG $LOGFILE | gawk '{print $3}'` 2> /dev/null | sed -e 's/.config.dat//' | gawk '{printf("%s",$1)}' | tee $TMPOUTDIR/mkDB.log;
    CONFIG_ID=`cat $TMPOUTDIR/mkDB.log`;
    if [ ! $CONFIG_ID ] ; then
	echo -e -n "0000 0 0 0" ;
    fi


    DLAYERDIR=$ASYMDIR/dlayer
    FITLOGFILE=$DLAYERDIR/$CONFIG_ID.fit.log;
    LOGFILE=$ASYMDIR/douts/$CONFIG_ID.dl.log;

    if [ -f $FITLOGFILE ]&&[ -f $LOGFILE ]  ; then
	READ_RATES=`grep 'Read Rate' $LOGFILE | gawk '{printf("%4.2f", $5*1e-6)}'`
	AVE_Dl=`grep "dlave =" $FITLOGFILE | gawk '{printf("%6.2f",$3)}'`
	AVE_Dl_ERROR=`grep "Deviation/strip=" $FITLOGFILE | gawk '{printf("%5.2f", $2)}'`
	if [ $AVE_Dl ] ; then
	    echo -e -n " $AVE_Dl $AVE_Dl_ERROR $READ_RATES "        
	else
	    echo -e -n " 0 0 0 "        
	fi
    fi

    echo -e -n "\n";

}

#############################################################################
#                                grepit()                                   #
#############################################################################
grepit(){

    echo -e -n "$RunID";
    GetOnlinePolarization;
#    GetOnlinePolFromFile;
    printf "$OnlineP $OnlinedP";

    # check RUN_STATUS entry in logfile. If RUN_STATUS isn't there, asign "----"
    RUN_STATUS=`grep 'RUN STATUS' $LOGFILE |  gawk '{printf(" %s ",$4)}'`
    if [ $RUN_STATUS ] ; then
	if [ $RUN_STATUS == 'Suspicious' ] ; then
	    RUN_STATUS="Susp";
	elif [ $RUN_STATUS == 'Recovered' ] ; then
	    RUN_STATUS="Rcvd";
	fi
    else
	RUN_STATUS="----";
    fi


    # If Junk then carrige return
    if [ $RUN_STATUS == 'Junk' ] ; then
	echo -e -n " $RUN_STATUS\n";
    else

    # If No Crew defined then carrige return
    if [ ! $CREW ] ; then
	CREW="-----";
    fi


	printf "  %4s  %5s" $RUN_STATUS $CREW;
	grep 'Beam Energy :' $LOGFILE | gawk '{printf(" %4d",$4)}'
	arg=`grep 'End Time:' $LOGFILE | grep -v 'Scaler' | sed -e 's/End Time:/ /' | sed -e 's/200[4-9]//'`
	echo -e -n "$arg";
	grep 'Polarization (sinphi)     ' $LOGFILE | gawk '{printf(" %6.1f %5.1f",$4*100,$5*100)}'
#	    grep 'Average Polarization' $LOGFILE | gawk '{printf(" %6.1f%7.1f",$4*100,$5*100)}'
	grep 'Phase' $LOGFILE | gawk '{printf(" %7.1f %5.1f",$5,$6)}'
	grep 'chi2/d.o.f (sinphi fit)     ' $LOGFILE | gawk '{printf(" %4.2f",$5)}'
	grep 'Analyzing Power Average     ' $LOGFILE | gawk '{printf(" %6.4f",$5)}'
	grep 'Target                      ' $LOGFILE | gawk '{printf(" %c",$3)}'      
	grep 'Target Operation            ' $LOGFILE | sed -e 's/fixed/fixd/' | gawk '{printf(" %s",$4)}' 
	grep 'Event Rate' $LOGFILE | gawk '{printf(" %4.2f",$5/1e6)}'
	grep 'Specific Luminosity         ' $LOGFILE | gawk '{printf(" %5.3f",$6)}'
	OfflineP=`grep 'Polarization (sinphi)' $LOGFILE | gawk '{printf(" %6.1f ",$4*100)}'`
	echo $OnlineP $OfflineP | gawk '{printf(" %6.2f",$1-$2)}'

	grep 'Polarization (sinphi) alt  ' $LOGFILE | gawk '{printf(" %6.1f%5.1f",$5*100, $6*100)}'

	if [ $ExpertMode -eq 1 ] ; then
	    basename `grep CONFIG $LOGFILE | gawk '{print $3}'` 2> /dev/null | gawk '{printf(" %s",$1)}';
#	    grep 'MIGRAD' $LOGFILE | sed -e 's/STATUS=//' | gawk '{printf(" %6s",$4)}' ; 
#	    grep 'MATRIX' $LOGFILE | gawk '{printf(" %6s %s",$6,$7)}' ; 
        fi
	echo -e -n "\n";
    fi

    
}





#############################################################################
#                              MakeDatabase()                               #
#############################################################################
MakeDatabase(){
PROCESS=1;

for f in `cat $DATADIR/raw_data.list` ;
  do
      RunID=$f
      Fill=`echo $RunID | gawk '{printf("%4d",$1)}'`
      Test=`echo $RunID $Fill | gawk '{RunN=($1-$2)*10; printf("%1d",RunN)}'`
      if [ $Test -eq 1 ]&&[ $DISTRIBUTION -eq 2 ] ; then
	  PROCESS=1;
      elif [ $Test -eq 0 ]&&[ $DISTRIBUTION -eq 1 ] ; then
	  PROCESS=1;
      fi

      if [ $PROCESS -eq 1 ] ; then

	  if [ $Fill -ge $FROM_FILL ]&&[ $Fill -le $TILL_FILL ] ; then
	      if [ $ExeOnlineDatabase -eq 1 ]; then 
		  OnlineDatabase;
	      else 
		  CREW=`grep $RunID $ANALYZED_RUN_LIST | gawk '{print $2}'`
		  grep $RunID $ANALYZED_RUN_LIST  > /dev/null; 
		  if [ $? == 0 ] ; then
		      LOGFILE=$LOGDIR/$RunID.log
		      if [ -f $LOGFILE ] ; then
			  if [ $ExeOnlineNevents -eq 1 ] ; then
			      OnlineNevents;
			  elif [ $ExeDlayerConfig -eq 1 ] ; then
			      dLayerConfig;
			  elif [ $ExeErrorDetector -eq 1 ] ; then
			      ErrorDetector;
			  else
			      grepit;
			  fi
		      else 
			  echo -e -n "$RunID $LOGFILE missing\n";
		      fi
		  elif [ $ExclusiveMode == 0 ] ; then
		      echo -e -n "$RunID";
#		      GetOnlinePolarization;
		      GetOnlinePolFromFile;
		      printf "$OnlineP $OnlinedP  N/A-";
		      echo -e -n "\n";
		  fi
	      fi

	  fi

	  if [ $DISTRIBUTION -ne 0 ] ; then
	      PROCESS=0;
	  fi

      fi

done

}


#############################################################################
#                                   Switch                                  #
#############################################################################
# Check if one has a permission to create analyzed_run.list file in default
# directory. If not, the analyzed_run.list will be created in the current
# directory.
CheckWritePermission(){
    touch $ASYMDIR 2>/dev/null;
    if [ $? -eq 1 ] ; then
	ANALYZED_RUN_LIST="./analyzed_run.list"
    fi

}


#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  --analyzed-run-list) ExeAnalyzedRunList=1 ; ExeMakeDatabase=0;;
  -a) ExeAnalyzedRunList=1 ; ExeMakeDatabase=0;;
  -F) shift ; FROM_FILL=$1 ;TILL_FILL=$1 ;;
  --fill-from) shift ; FROM_FILL=$1;;
  --fill-till) shift ; TILL_FILL=$1;;
  -f) shift ; ANALYZED_RUN_LIST=$1; ExeAnalyzedRunList=0 ;;
  --online-nevents) ExeOnlineNevents=1;;
  --online)         ExeOnlineDatabase=1; ExeAnalyzedRunList=0 ;;
  --exclusive) ExclusiveMode=1;;
  --blue)   DISTRIBUTION=1;;
  --yellow) DISTRIBUTION=2;;
  --dlayer-config) ExeDlayerConfig=1;;
  --error-detector) ExeErrorDetector=1; ExclusiveMode=1;;
  -X | --expert) ExpertMode=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done



if [  $ExeAnalyzedRunList -eq 1 ] ; then
    CheckWritePermission;  # check the write permission of analyzed_run.list.
    MakeAnalyzedRunList;
fi
if [ $ExeMakeDatabase -eq 1 ] ; then
    if [ $ExeOnlineDatabase -eq 1 ]; then
	ShowIndexOnline;
    elif [ $ExeDlayerConfig -eq 1 ]; then
	ShowDlayerConfigIndex;
    elif [ $ExeErrorDetector -eq 1 ]; then
	ShowErrorDetectorIndex;
    else
	ShowIndex;
    fi
    MakeDatabase;
fi;

