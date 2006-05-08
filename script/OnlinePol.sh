#!/bin/bash


#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`;
    echo    " "
    echo    " $COMMAND [-xh][-f <RunID>]"
    echo    "    : Calculate Online Polarization "
    echo    " "
    echo -e "   -f <RunID>     Calculate Online Polarization of <RunID>";
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




GetOnlinePolarization(){

 TMPDIR=/tmp/cnipol;
 if [ ! -d $TMPDIR ]; then
     mkdir $TMPDIR;
 fi

 $MACRODIR/pvector.pl $RunID
 paw -b $MACRODIR/pvect_simple.kumac &> /dev/null;
 echo -e -n "$RunID" > $TMPDIR/tmp.log
 cat $TMPDIR/tmp.dat | gawk '{printf("%6.1f%5.1f\n",$1,$2)}' 
 rm -f $TMPDIR/tmp.dat
 rm -f $TMPDIR/pvect.dat

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

if [ ! $RunID ] ; then
    echo -e -n "Erorr: RunID is not defined";
    help;
else
    export RUN=$RunID
fi

GetOnlinePolarization;


