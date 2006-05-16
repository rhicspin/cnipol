#!/bin/bash

ExeOnlinePol=1;
ExeNevents=0;
UnitMillion=0;
UnitKiro=0;
ONLINE_RUNLIST=$DATADIR/raw_data.list;
ONLINE_DB=$DATADIR/OnlinePol.dat;
SLEEP_INTERVAL=1800;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`;
    echo    " "
    echo    " $COMMAND [-xh][-f <RunID>][--nevents][--daemon][-s -sleeep <s>][-M]"
    echo    "    : Calculate Online Polarization & show online records"
    echo    " "
    echo -e "   -f <RunID>     Calculate Online Polarization of <RunID>";
    echo -e "   --nevents      Print Online # of Events ";
    echo -e "   --deamon       Dynamicly update online Pol data file $ONLINE_DB"; 
    echo -e "   -s --sleep <s> Sleep interval for Daemon mode in sec. [Def]:$SLEEP_INTERVAL";               
    echo -e "   -M             Nevents in Million unit ";
    echo -e "   -k             Nevents in kiro unit ";
    echo -e "   -h | --help    Show this help"
    echo -e "   -x             Show example"
    echo    " "
    exit;
}

 
ShowExample(){
 
    echo    " "
    echo    " $0 -f 7759.002";
    echo    " "
    exit;

}



#############################################################################
#                                CheckRunID                                 #
#############################################################################
CheckRunID(){

    if [ ! $RunID ] ; then
	echo -e -n "Erorr: RunID is not defined";
	help;
    else
	export RUN=$RunID
    fi

}


#############################################################################
#                               GetOnlinePolariztion                                   #
#############################################################################
GetOnlinePolarization(){

 TMPOUTDIR=/tmp/cnipol;
 if [ ! -d $TMPOUTDIR ]; then
     mkdir $TMPOUTDIR;
 fi

 $MACRODIR/pvector.pl $RunID
 paw -b $MACRODIR/pvect_simple.kumac &> /dev/null;
 echo -e -n "$RunID" > $TMPOUTDIR/tmp.log;
 cat $TMPOUTDIR/tmp.dat | gawk '{printf("%7.1f %5.1f\n",$1,$2)}' 
 rm -f $TMPOUTDIR/tmp.dat
 rm -f $TMPOUTDIR/pvect.dat

}



#############################################################################
#                                    Main                                   #
#############################################################################
OnlineNevents(){

#   echo -e -n "$RunID";
   ONLINE_LOG=$ONLINEDIR/log/$RunID.log;
   if [ $UnitMillion -eq 1 ] ; then
       grep ">>>" $ONLINE_LOG | tail -n 1 | gawk '{printf(" %2d ",$10/1e6)}';
   elif [ $UnitKiro -eq 1 ] ; then 
       grep ">>>" $ONLINE_LOG | tail -n 1 | gawk '{printf(" %d ",$10/1e3)}';
   else 
       grep ">>>" $ONLINE_LOG | tail -n 1 | gawk '{printf(" %d ",$10)}';
   fi
   echo -e -n "\n";

}

#############################################################################
#                                  ExeDaemon                                #
#############################################################################
ExeDaemon(){

  while [ 1 ] ; do 

    NLINE=`wc $ONLINE_RUNLIST | gawk '{print $1}'`
    for (( i=1; i<=$NLINE; i++ )) ; do
	RunID=`line.sh $i $ONLINE_RUNLIST`;
	CheckRunID;
	grep $RunID $ONLINE_DB;
	if [ $? -eq 1 ] ; then
	    echo -e -n "$RunID" | tee -a  $ONLINE_DB ;
	    GetOnlinePolarization | tee -a $ONLINE_DB ;
        fi

    done

    echo -e "sleeping $SLEEP_INTERVAL [sec]";
    sleep $SLEEP_INTERVAL;

  done

}
	    



#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  -f) shift ; RunID=$1 ;;
  --nevents) ExeNevents=1; ExeOnlinePol=0;;
  --daemon) DAEMON=1;;
  -s | -sleep) shift ; SLEEP_INTERVAL=$1;;
  -M) UnitMillion=1;;
  -k) UnitKiro=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done

if [ $DAEMON -eq 1 ] ; then
    if [ ! -f $ONLINE_DB ] ; then
	touch $ONLINE_DB
    fi
    ExeDaemon ;
else 
    
    CheckRunID;

    if [ $ExeOnlinePol -eq 1 ] ; then
	GetOnlinePolarization;
    fi
    
    if [ $ExeNevents -eq 1 ] ; then
	OnlineNevents;
    fi

fi

