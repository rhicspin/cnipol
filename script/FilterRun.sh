#!/bin/bash
#find anomalous runs
#A. Hoffman
#April 6, 2006

INF="$ASYMDIR/testlist.txt";
B=5;
DUMP=0;
M=0;
P=0;

#############################################################################
#                                     Help                                  #
#############################################################################

helpmenu() {

    echo    " "
    echo    " anomfinder.sh [-dhm][-n <#>]";
    echo    "    : list runs with large difference in onlineP - offlineP ";
    echo    " "
    echo -e "   -h                        Show this help file";
    echo -e "   -d                        dump list into anom.list"
    echo -e "   -n <#>                    finds all lists with a difference greater than <#>";
    echo -e "   -m                        updates the mkDB.txt database first (this will take some time)";
    echo    " ";
    echo -e "   default minimum difference is 5%";
    echo    " ";
    exit;
}


#############################################################################
#                        AwkDeltaPRuns()                                    #
#############################################################################

AwkDeltaPRuns() {

    if test $DUMP -eq 1
    then
	awk "\$21 > $B || \$21 < -$B"  $INF > anom.list;
	echo "found all runs with a polarization difference of $B % or greater and dumped them into anom.list";
	
    else
	awk "\$21 > $B || \$21 < -$B" $INF;
	echo "found all runs with a polarization difference of $B % or greater";
    fi
}


#############################################################################
#                        AwkPhiRuns()                                       #
#############################################################################

AwkPhiRuns() {

    if test $DUMP -eq 1
    then
	awk "\$13 > $P || \$13 < -$P"  $INF > anom.list;
	echo "found all runs with a phi difference of $P % or greater and dumped them into anom.list";
	
    else
	awk "\$13 > $P || \$13 < -$P" $INF;
	echo "found all runs with a phi difference of $P % or greater";
    fi
}


#############################################################################
#                                    Main                                   #
#############################################################################


while getopts n:p:dhm opt
do
  case "$opt" in
      n) B="$OPTARG" ;;
      d) DUMP=1 ;;
      h) helpmenu ;;
      m) M=1;;
      p) P="$OPTARG" ;;
  esac
done


if test $M -eq 1
then
    echo "updating testlist.txt...";
    mkDB.sh --exclusive | grep -v Junk | grep ^7 | grep -v /home/ > $INF;
fi

if test $P -ne 0
then
    AwkPhiRuns;

else
    AwkDeltaPRuns;
fi
