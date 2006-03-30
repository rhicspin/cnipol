#!/bin/bash
#make run database
#I.Nakagawa
#March, 11, 2006
ANALYZED_RUN_LIST="$ASYMDIR/analyzed_run.list";
ExeAnalyzedRunList=1 ;
ExeMakeDatabase=1;
ExclusiveMode=0;
ExpertMode=0;
FROM_FILL=7537;
TILL_FILL=9000;
LOGDIR=$ASYMDIR/log;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " mkDB.sh [-xha][-F <Fill#>][--fill-from <Fill#>][--fill-till <Fill#>]"
    echo    "         [--analyzed-run-list][-X --expert]";
    echo    "    : make pC offline analysis database "
    echo    " "
    echo -e "   -a --analyzed-run-list    Make analyized runlist";
    echo -e "   -F <Fill#>                Show list <Fill#>"
    echo -e "   --fill-from <Fill#>       Make list from <Fill#>";
    echo -e "   --fill-till <Fill#>       Make list till <Fill#>";
    echo -e "   --exclusive               Show only data analyized";
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
    echo    "2. make analyzed runlist"
    echo    " "
    echo    "    mkDB.sh -a"
    echo    " "
    exit;

}


#############################################################################
#                       MakeAnalyzedRunList()                               #
#############################################################################
MakeAnalyzedRunList(){

  TMPLIST="tmp.list";

  if [ -f $TMPLIST ] ; then
      rm -f $TMPLIST;
  fi
  touch $TMPLIST;

  for a in alanH alanD jeffW itaru ; 
  do 

    f="$ASYMDIR/analyzed_runlist.$a"
    NLINE=`wc $f | gawk '{print $1}'`
    for (( i=1; i<=$NLINE; i++ )) ; 
       do
       RunID=`$INSTALLDIR/line.sh $i $f` 
       echo -e -n "$RunID $a\n"  >> $TMPLIST
       done
      
  done


  if [ $ExeMakeDatabase == 1 ] ; then
      sort $TMPLIST > $ANALYZED_RUN_LIST;
  else
      sort $TMPLIST | tee $ANALYZED_RUN_LIST;
  fi

  rm -f $TMPLIST;


}

#############################################################################
#                                grepit()                                   #
#############################################################################
ShowIndex(){

    printf "=================================================================================================";
    printf "====================\n";
    printf " RunID ";
    printf "      Online";
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
    printf "\n";
    printf "          ";
    printf "    P   dP ";
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
    printf "\n";
    printf "=================================================================================================";
    printf "====================\n";


}


GetOnlinePolarization(){

 $MACRODIR/pvector.pl $RunID
 paw -b $MACRODIR/pvect_simple.kumac &> /dev/null
 OnlineP=`cat tmp.dat | gawk '{printf("%6.1f",$1)}'`;
 OnlinedP=`cat tmp.dat | gawk '{printf("%5.1f",$2)}'`;
 rm -f tmp.dat
 rm -f pvect.dat

}


#############################################################################
#                                grepit()                                   #
#############################################################################
grepit(){

    echo -e -n "$RunID";
    GetOnlinePolarization;
    printf "$OnlineP $OnlinedP";



    # check RUN_STATUS entry in logfile. If RUN_STATUS isn't there, asign "----"
    RUN_STATUS=`grep 'RUN STATUS' $LOGFILE |  gawk '{printf(" %s ",$4)}'`
    if [ $RUN_STATUS ] ; then
	if [ $RUN_STATUS == 'Suspicious' ] ; then
	    RUN_STATUS="Susp";
	fi
    else
	RUN_STATUS="----";
    fi


    # If Junk then carrige return
    if [ $RUN_STATUS == 'Junk' ] ; then
	echo -e -n " $RUN_STATUS\n";
    else


	printf "  %4s  %5s" $RUN_STATUS $CREW;
	grep 'Beam Energy :' $LOGFILE | gawk '{printf(" %4d",$4)}'
	arg=`grep 'End Time:' $LOGFILE | sed -e 's/End Time:/ /' | sed -e 's/2006//'`
	echo -e -n "$arg";
	grep 'Polarization (sinphi)' $LOGFILE | gawk '{printf(" %6.1f %5.1f",$4*100,$5*100)}'
#	    grep 'Average Polarization' $LOGFILE | gawk '{printf(" %6.1f%7.1f",$4*100,$5*100)}'
	grep 'Phase' $LOGFILE | gawk '{printf(" %7.1f %5.1f",$5,$6)}'
	grep 'chi2/d.o.f (sinphi fit)     ' $LOGFILE | gawk '{printf(" %4.1f",$5)}'
	grep 'Analyzing Power Average     ' $LOGFILE | gawk '{printf(" %6.4f",$5)}'
	grep 'Target                      ' $LOGFILE | gawk '{printf(" %c",$3)}'      
	grep 'Target Operation            ' $LOGFILE | sed -e 's/fixed/fixd/' | gawk '{printf(" %s",$4)}' 
	grep 'Event Rate' $LOGFILE | gawk '{printf(" %4.2f",$5/1e6)}'
	grep 'Specific Luminosity         ' $LOGFILE | gawk '{printf(" %5.3f",$6)}'
	if [ $ExpertMode -eq 1 ] ; then
	    grep 'MIGRAD' $LOGFILE | sed -e 's/STATUS=//' | gawk '{printf(" %6s",$4)}' ; 
        fi
	echo -e -n "\n";
    fi

    
}





#############################################################################
#                                grepit()                                   #
#############################################################################
MakeDatabase(){

for f in `ls $DATADIR/????.???.data` ;
  do
      RunID=`basename $f | sed -e 's/.data//'`
      Fill=`echo $RunID | gawk '{printf("%4d",$1)}'`
#      echo -e -n "$RunID $Fill\n"
      if [ $Fill -ge $FROM_FILL ]&&[ $Fill -le $TILL_FILL ] ; then
	  CREW=`grep $RunID $ANALYZED_RUN_LIST | gawk '{print $2}'`
	  grep $RunID $ANALYZED_RUN_LIST  > /dev/null; 
	  if [ $? == 0 ] ; then
	      LOGFILE=$LOGDIR/$RunID.log
	      if [ -f $LOGFILE ] ; then
		  grepit;
	      else 
		  echo -e -n "$RunID $LOGFILE missing\n";
	      fi
	  elif [ $ExclusiveMode == 0 ] ; then
	      echo -e -n "$RunID\n";
	  fi
      fi
done

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
  --exclusive) ExclusiveMode=1;;
  -X | --expert) ExpertMode=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done



if [  $ExeAnalyzedRunList -eq 1 ] ; then
    MakeAnalyzedRunList;
fi
if [ $ExeMakeDatabase -eq 1 ] ; then
    ShowIndex;
    MakeDatabase;
fi;

