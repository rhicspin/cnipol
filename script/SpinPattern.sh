#!/bin/bash
# SpinPattern
# June 9, 2006
# I. Nakagawa
RUN=6;
LOGDIR=$ONLINEDIR/log;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " $0 [-xh][-f <runlist>]"
    echo    "    : make spin pattern database";
    echo    " "
    echo -e "   -f <runlist>              make spin pattern database for <runlist>"
    echo -e "   -r <run>                  For Run 5, 6, ... [def]:Run6";
    echo -e "   -h | --help               Show this help"
    echo -e "   -x                        Show example"
    echo    " "
    exit;
}


ShowExample(){

    echo    " "
    exit;

}


#############################################################################
#                           ExtractSpinPattern()                            #
#############################################################################
ExtractSpinPattern(){


if [ -f SpinPattern\_$RHICRUN.*.db ] ; then
    rm -f SpinPattern\_$RHICRUN.*.db;
fi
touch SpinPattern\_$RHICRUN.Yellow.db
touch SpinPattern\_$RHICRUN.Blue.db


NLINE=`wc $RUNLIST | gawk '{print $1}'`;
for (( i=1; i<=$NLINE ; i++ ))
  do 
      RunID=`line.sh $i $RUNLIST`;
      Fill=`echo $RunID | gawk '{printf("%4d",$Run)}'`
      Test=`echo $RunID $Fill | gawk '{RunN=($1-$2)*10; printf("%1d",RunN)}'`
      if [ $Test -eq 1 ] ; then
	  Beam="Yellow"
      else 
	  Beam="Blue"
      fi
      
      LOGFILE=$LOGDIR/$RunID.log;
      if [ -f $LOGFILE ] ; then
	  echo -e -n "$Fill " | tee -a SpinPattern\_$RHICRUN.$Beam.db
	  grep Bunches $LOGFILE  | gawk '{print $4}' | tee -a SpinPattern\_$RHICRUN.$Beam.db
      fi

    done


}

#############################################################################
#                                    Main                                   #
#############################################################################

while test $# -ne 0; do
  case "$1" in
  -f) shift ; RUNLIST=$1 ;;
  -r) shift ; RUN=$1 ;; 
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


RHICRUN=Run0$RUN;
ExtractSpinPattern;


