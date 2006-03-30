#!/bin/bash
BASEDIR=/home/e950/offline
ASYMDIR=`pwd`

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " $0 [-xh][-f <RunID>]"
    echo    "    : copy analyized data files to current working directory structure"
    echo    " "
    echo -e "   -f <RunID>                copy analyized file for run <RunID>"
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
#                                 DoCopy()                                  #
#############################################################################
DoCopy(){

    cp  $BASEDIR/$mode/$RunID.$mode  ./$mode/$RunID.$mode

}



#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  -f) shift ; RunID=$1 ;; 
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


mode=hbook ; DoCopy;
mode=log   ; DoCopy;
protect.pl -f $RunID -u

