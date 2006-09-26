#! /bin/bash
#rundb_insterter.sh
#April 20, 2006, A. Hoffman

# original database to be updated
DB=run.db;
# input file to be inserted to run.db
INF=$TMPOUTDIR/rundb.log;
# tempolary dababase file during this operation. It will be renamed as run.db at the end
OFILE=$TMPOUTDIR/rundb_inserter.db;
# log file for this script
LOGFILE=$TMPOUTDIR/rundb_inserter.log;
Test=0;

#Find the run number, the next run number, and the reference line number

# Total Lines in run.db database
NLINE=`wc $DB | gawk '{print $1}'`


# Get Total Lines in temporary RunID list 
grep "\[" $DB | sed -e 's/\[//' | sed -e 's/\]//' | sed -e 's/@//' > $TMPOUTDIR/tmplist.txt
DbN=`wc $TMPOUTDIR/tmplist.txt | gawk '{ print $1 }'`


# RunID to be inserted 
RunN=`grep "^\[" $INF | sed -e "s/\[//" | sed "s/\]@//" | gawk '{print $1}'`
echo -e "Run number to be inserted : $RunN"


#############################################################################
#                                InsertTag                                  #
#############################################################################


InsertTag()  {

    echo  -e "Get line number for Reference ID $RefN";
    grep -n $RefN $DB;

    header=`grep -n "^\[$RefN" $DB | sed -e "s/:/ /" | gawk '{print $1-1}'`
    echo -e "header # = $header";
    head -n $header $DB > $OFILE;

    grep '^.' $INF >> $OFILE;
    echo -e "" >> $OFILE;
    residual=$(( $NLINE - $header ));
    echo -e "$residual";
    tail -n $residual $DB >> $OFILE;

    echo "insertion complete"
}

#############################################################################
#                                  AppendTag                                #
#############################################################################

AppendTag() {

    head -n $NLINE $DB > $OFILE
    echo "" >> $OFILE
    grep '^.' $INF >> $OFILE
    echo "appending complete"

}



#############################################################################
#                                    Main                                   #
#############################################################################


for (( i=1; i<=$DbN; i++ )) do
    RefN=`line.sh "$i" $TMPOUTDIR/tmplist.txt | gawk '{printf("%8.3f",$1)}'`;
    Test=`CompareRunNmbr $RefN $RunN`;
    if [[ $Test == 1 ]]; then
 	break;
    fi
done
 
echo -e "Test = $Test";

if [[ $Test == 1 ]]; then
    echo "Run# to be inserted: $RunN, Reference Number in rundb: $RefN";
    InsertTag;
    echo "$header";

else
    echo "appending";
    AppendTag;

fi

NLINE_OFILE=`wc $OFILE | gawk '{print $1}'`;
if [ $NLINE_OFILE -ge $NLINE ] ; then
    mv $OFILE $DB
else 
    echo "=====================" >> $LOGFILE;
    echo "RunID=$RunN" >> $LOGFILE;
    echo "-->contents of run.db log" >> $LOGFILE;
    cat $INF >> $LOGFILE;
    echo "-->contents of rundb_inserter.db" >> $LOGFILE;
    cat $OFILE >> $LOGFILE;
    echo " " ;
fi
