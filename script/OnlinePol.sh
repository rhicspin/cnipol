#!/bin/bash


ExeDlayerAverage=0;
ExeOnlinePol=1;
ExeNevents=0;
UnitMillion=0;
UnitKiro=0;
ONLINE_CONFIGDIR=$ONLINEDIR/config;
ONLINE_RUNLIST=$DATADIR/raw_data.list;
ONLINE_DB=$DATADIR/OnlinePol.dat;
SLEEP_INTERVAL=1800;
DAEMON=0;
A_NCorrection=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`;
    echo    " "
    echo    " $COMMAND [-xh][-f <RunID>][--nevents][--daemon][-s -sleeep <s>][-M][--A_NCorrection]"
    echo    "          [--dlayer-average]"
    echo    "    : Calculate Online Polarization & show online records / make deadlyaer average in online config."
    echo    " "
    echo -e "   -f <RunID>       Calculate Online Polarization of <RunID>";
    echo -e "   --nevents        Print Online # of Events ";
    echo -e "   --deamon         Dynamicly update online Pol data file $ONLINE_DB"; 
    echo -e "   -s --sleep <s>   Sleep interval for Daemon mode in sec. [Def]:$SLEEP_INTERVAL";               
    echo -e "   -M               Nevents in Million unit ";
    echo -e "   -k               Nevents in kiro unit ";
    echo -e "   --A_NCorrection  A_N correction for before Fill#6884";
    echo -e "   --dlayer-average Make deadlyaer average for online config files in $ONLINE_CONFIGDIR";
    echo -e "   -h | --help      Show this help"
    echo -e "   -x               Show example"
    echo    " "
    exit;
}

 
ShowExample(){
 
    COMMAND=`basename echo`;    
    $0    " ";
    echo    " 1. Get online polarization for RunID=7759.002";
    echo    " $COMMAND -f 7759.002";
    echo    " ";
    echo    " 2. Running in daemon mode";
    echo    " $COMMAND --daemon";
    echo    " ";
    echo    " 3. A_N Correction for Fill<6884 (only for flattops). ";
    echo    " $COMMAND -f 6883 --A_NCorrection";
    echo    " ";
    echo    " 4. Create deadlayer summary list in $ONLINE_CONFIGDIR/dLayerAverage.list";
    echo    " $COMMAND --dlayer-average | tee $ONLINE_CONFIGDIR/dLayerAverage.list";
    echo    " ";
    exit;

}


#############################################################################
#                             DlayerAverage()                               #
#############################################################################
DlayerAverage(){

   
   for f in `ls $ONLINE_CONFIGDIR/*.dat`; 
     do
	NLINE=`wc $f | gawk '{print $1-5}'`;
	cat $f | tail -n $NLINE | sed -e 's/= //' | gawk '{print $7}' > $TMPOUTDIR/tmp.dat;

	if [ -f $TMPOUTDIR/Avearge.dat ] ; then
	    rm -f $TMPOUTDIR/Avearge.dat;
	fi

	for (( i=1; i<=$NLINE; i++ )) ; 
	  do 
	    echo -e -n "$i " >> $TMPOUTDIR/Avearge.dat;
	    line.sh $i $TMPOUTDIR/tmp.dat | gawk '{printf("%8.1f", $1)}'>> $TMPOUTDIR/Avearge.dat;
	    echo -e -n " 1 \n" >>$TMPOUTDIR/Avearge.dat;
	  done

	file=`basename $f`;
	dl_ave=`WeightedMean -f $TMPOUTDIR/Avearge.dat | gawk '{printf("%8.1f",$1)}'`;
	echo " $file $dl_ave ";

   done

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

 if [ -f $ONLINEDIR/log/an$RunID.log ] ; then
     $MACRODIR/pvector.pl $RunID
     OnlineP=`tail -n 1 $TMPOUTDIR/pvect.dat | gawk '{printf("%7.1f\n",$1)}'`; 
     dOnlineP=`tail -n 1 $TMPOUTDIR/pvect.dat | gawk '{printf("%7.1f\n",$2)}'`; 
     #tail -n 1 $TMPOUTDIR/pvect.dat | gawk '{printf("%7.1f %5.1f\n",$1,$2)}' 

     # This is a correction for A_N for Flattop 
     Fill=`echo $RunID | gawk '{printf("%d",$1)}'`;
     if [ $A_NCorrection -eq 1 ] && [ $Fill -lt 6884 ] ; then
	 OnlineP=`echo $OnlineP | gawk '{printf("%7.1f",$1*1.2)}'`;
     fi
     printf "%7.1f %5.1f\n" $OnlineP $dOnlineP;
 else 
     printf "%7.1f %5.1f\n" 0 0;
 fi
# rm -f $TMPOUTDIR/pvect.dat;

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

if [ ! -f $ONLINE_DB ] ; then
    touch $ONLINE_DB;
fi

    
  while [ 1 ] ; do 

    NLINE=`wc $ONLINE_RUNLIST | gawk '{print $1}'`
    for (( i=1; i<=$NLINE; i++ )) ; do
	RunID=`line.sh $i $ONLINE_RUNLIST`;
	CheckRunID;
	grep $RunID $ONLINE_DB;
	if [ $? -eq 1 ] ; then
	    echo -e -n "$RunID"   | tee -a $ONLINE_DB ;
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
  --dlayer-average) ExeDlayerAverage=1;;
  --A_NCorrection) A_NCorrection=1;;
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


if [ $ExeDlayerAverage -eq 1 ] ; then
    DlayerAverage;
    exit;
fi


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

