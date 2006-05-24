#!/bin/bash
OPT="";

#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    COMMAND=`basename $0`;
    echo    " "
    echo    " $COMMAND [-xh][-f <psfile>][--rotate <angle>]"
    echo    "    : converts postscript file to gif format"
    echo    " "
    echo -e "   -f <psfile>      postscript file to be converted";
    echo -e "   --rotate <angle> rotation agnle [deg.]";
    echo -e "   -h | --help      Show this help"
    echo -e "   -x               Show example"
    echo    " "
    exit;
}


#############################################################################
#                                    Main                                   #
#############################################################################

while test $# -ne 0; do
  case "$1" in
  -f) shift ; psfile=$1 ;;
  --rotate) shift ; angle=$1 ; OPT="$OPT -rotate $angle";;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done

file=`basename $psfile | sed -e '{s/.ps//}'`;

ps2epsi $psfile $file.eps
convert $OPT $file.eps $file.gif
rm -f $file.eps

echo "conveted to $file.gif"

