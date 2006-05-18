#/bin/bash
# Data file handler for OfflinePol.C
# Created May 18, 2006
# I. Nakagawa

DIR="$ASYMDIR/summary";
BEAM="Blue";
FILE0=$DIR/OfflinePol_$BEAM.dat;
MODE="_all";
DEF_FILE=$DIR/OfflinePol_$BEAM\_all.dat;
FILE=$DIR/OfflinePol_$BEAM$MODE.dat;
FILTER=0;

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`
    echo    " "
    echo    " $COMMAND [-xh][--all][-n <number>]"
    echo    "    : Make specific database or/and switch link betwen data files."
    echo    "      Default link is $FILE";
    echo    " "
    echo -e "   --all         Switch to $FILE";
    echo -e "   --7579-7714   ";
    echo -e "   --7715-7783   ";
    echo -e "   --7784-7842   ";
    echo -e "   --7842after   ";
    echo -e "   -n <number>   Select only the last <number> runs";
    echo -e "   -h | --help   Show this help"
    echo -e "   -x            Show example"
    echo    " "
    exit;
}



ShowExample(){

    echo    " "
    echo    "1. To make runs for the last 100 runs.";
    echo    " "
    echo    "    OfflinePol.sh -n 100"
    echo    " "
    exit;

}


#############################################################################
#                                 ExeFileSwitch                             #
#############################################################################
ExeFileSwitch(){

    FILE0=$DIR/OfflinePol_$BEAM.dat;
    if [ -f $FILE0 ] ; then
	rm -f $FILE0;
    fi

    SOURCE=$DIR/OfflinePol_$BEAM$MODE.dat;
    if [ $FILTER -eq 1 ] ; then
	tail -n $N $DEF_FILE | tee $SOURCE
    fi

    ln -s $SOURCE $FILE0;

}

#############################################################################
#                                 FileSwitcher                              #
#############################################################################
FileSwitcher(){

	
    BEAM="Blue";
    ExeFileSwitch;
    BEAM="Yellow";
    ExeFileSwitch;

}

#############################################################################
#                                 FileSwitcher                              #
#############################################################################
BackDefault(){

    MODE="_all";
    FileSwitcher;

}

#############################################################################
#                                    Main                                   #
#############################################################################

while test $# -ne 0; do
  case "$1" in
  --all) MODE="_all" ;;
  --7579-7714) MODE="_7579-7714";;
  --7715-7783) MODE="_7715-7783";;
  --7784-7842) MODE="_7784-7842";;
  --7842after) MODE="_7842after";;
  -n) shift ; N=$1; MODE="_L$N" ; FILTER=1;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1";
      help;;
  esac
  shift
done



FileSwitcher;
root -b -q $MACRODIR/OfflinePol.C
cp ps/OfflinePol.ps ps/OfflinePol$MODE.ps
echo -e "ps file is ps/OfflinePol$MODE.ps";
BackDefault > /dev/null ;
