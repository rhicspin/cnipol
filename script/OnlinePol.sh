#!/bin/bash

ExeOnlinePol=1;
ExeNevents=0;
UnitMillion=0;
UnitKiro=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`;
    echo    " "
    echo    " $COMMAND [-xh][-f <RunID>][--nevents][-M]"
    echo    "    : Calculate Online Polarization & show online records"
    echo    " "
    echo -e "   -f <RunID>     Calculate Online Polarization of <RunID>";
    echo -e "   --nevents      Print Online # of Events ";
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
#                               GetOnlinePolariztion                                   #
#############################################################################
GetOnlinePolarization(){

 TMPOUTDIR=/tmp/cnipol;
 if [ ! -d $TMPOUTDIR ]; then
     mkdir $TMPOUTDIR;
 fi

 $MACRODIR/pvector.pl $RunID
 paw -b $MACRODIR/pvect_simple.kumac &> /dev/null;
 echo -e -n "$RunID" > $TMPOUTDIR/tmp.log
 cat $TMPOUTDIR/tmp.dat | gawk '{printf("%6.1f %5.1f\n",$1,$2)}' 
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
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  -f) shift ; RunID=$1 ;;
  --nevents) ExeNevents=1; ExeOnlinePol=0;;
  -M) UnitMillion=1;;
  -k) UnitKiro=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done

if [ ! $RunID ] ; then
    echo -e -n "Erorr: RunID is not defined";
    help;
else
    export RUN=$RunID
fi

if [ $ExeOnlinePol -eq 1 ] ; then
    GetOnlinePolarization;
fi

if [ $ExeNevents -eq 1 ] ; then
    OnlineNevents;
fi

