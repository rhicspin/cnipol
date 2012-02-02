#!/bin/bash
ExeRefreshPolError=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " $0 [-xh][-f <RunID>][--offline-error]"
    echo    "    : reflesh contents in existing Asym log file "
    echo    " "
    echo -e "   -f <RunID>                <RunID>"
    echo -e "   --offline-error           reflesh offline dP and dphi"
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
#                        RefreshPolError()                                  #
#############################################################################
RefreshPolError(){

    LOGFILE=log/$RUNID.log;
    NLINE=`wc $LOGFILE | gawk '{print $1}'`

    echo -e -n "$RUNID\n"

    header=`grep -n " Polarization (sinphi)" $LOGFILE | sed -e "s/:/ /" | gawk '{print $1-1}'`
    head -n $header $LOGFILE > tmp.log
    echo -e -n " Polarization (sinphi)       = " >> tmp.log
    export RUNID=$RUNID; 
    paw -q -b $MACRODIR/HID36230.kumac > /dev/null;
    cat dat/HID36230.dat | gawk '{printf("%10.4f%8.4f\n",$1/100,$2/100)}' >> tmp.log
    echo -e -n " Phase (sinphi)  [deg.]      = " >> tmp.log;
    cat dat/HID36230.dat | gawk '{printf("%10.4f%8.4f\n",$3,$4)}' >> tmp.log
    residual=`echo $NLINE $header | gawk '{print $1-$2-2}'`
    tail -n $residual $LOGFILE >> tmp.log

    mv tmp.log newlog/$RUNID.log;

}



#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  -f) shift ; RUNID=$1 ;; 
  --offline-error) ExeRefreshPolError=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


if [ $ExeRefreshPolError -eq 1 ] ; then
    RefreshPolError;
fi


