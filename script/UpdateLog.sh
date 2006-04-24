#!/bin/bash

ExeUpdateStatus=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " $0 [-xh][-f <RunID>][--update]"
    echo    "    : refresh contents in existing Asym log file "
    echo    " "
    echo -e "   -f <RunID>                <RunID>"
    echo -e "   --update                  refresh status and comment"
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
#                 UpdateStatus() and UpdateComment()                         #
#############################################################################
UpdateStatus() {

    LOGFILE=log/$RUNID.log;

    $INSTALLDIR/RunDBReader -f $RUNID > firsttest.dat
    NEWSTATUS=`grep "RUN STATUS" firsttest.dat | gawk '{print $4}'`
    NEWCOMMENT=`grep "COMMENT" firsttest.dat | sed -e "s/ COMMENT      = //"`
    echo -e -n "$NEWSTATUS\n"
    echo -e -n "$NEWCOMMENT\n"


    NLINE=`wc $LOGFILE | gawk '{print $1}'`
    echo -e -n "$RUNID\n"

    header=`grep -n " RUN STATUS" $LOGFILE | sed -e "s/:/ /" | gawk '{print $1-1}'`
    head -n $header $LOGFILE > alanstmp.log
    echo -e -n " RUN STATUS   = $NEWSTATUS\n" >> alanstmp.log
    residual=$(( $NLINE - $header - 1 ));
    tail -n $residual $LOGFILE >> alanstmp.log
    echo -e -n "is now tagged as $NEWSTATUS \n"
}

UpdateComment() {
    NLINE=`wc alanstmp.log | gawk '{print $1}'`

    header=`grep -n '(END)    ===$' alanstmp.log | sed -e "s/:/ /" | gawk '{print $1-3}'`
    head -n $header alanstmp.log > alanstmp2.log
    echo -e -n " COMMENT      = $NEWCOMMENT\n" >> alanstmp2.log
    residual=$(( $NLINE - $header - 1 ));
    tail -n $residual alanstmp.log >> alanstmp2.log
    echo -e -n "and has comment $NEWCOMMENT \n"

#    mv alanstmp2.log log/$RUNID.log;
# ***do not remove prvious line until ready for implimentation***

    rm alanstmp.log
}

#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  -f) shift ; RUNID=$1 ;; 
  --update) ExeUpdateStatus=1 ;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invalid Option $1"
      help;;
  esac
  shift
done



if [ $ExeUpdateStatus -eq 1 ]; then
    UpdateStatus;
    if [ -n "$NEWCOMMENT" ]; then
	UpdateComment;
    else
#       mv alanstmp.log log/$RUNID.log
#***do not remove above line until ready for implimentation
    echo -e "removing firsttest.dat";
    rm firsttest.dat;
    fi

fi


