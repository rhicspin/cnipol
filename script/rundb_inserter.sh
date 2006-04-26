#! /bin/bash
#rundb_insterter.sh
#April 20, 2006, A. Hoffman

INF=out.dat;
DB=testdb.txt;
LIST=runlist.txt;

#Find the run number, the next run number, and the reference line number

NLINE=`wc $DB | gawk '{print $1}'`
grep -n '^.' $LIST > tmplist.txt

RunN=`grep "^\[" $INF | sed -e "s/\[//" | sed "s/\]//" | sed -e "s/@//"`
echo -e "Run number is $RunN"

RefN=`grep -n "$RunN" $LIST | cut -f1 -d: | gawk '{print $1 +1}'`
echo -e "Reference number is $RefN"

NextN=`grep "^$RefN" tmplist.txt | sed "s/:/ /" | gawk '{print $2}'`
echo -e "Next number is $NextN"

tester=`grep "\[$NextN" $DB`
echo -e "$tester"

InsertTag()  {

    header=`grep -n "\[$NextN" $DB | sed -e "s/:/ /" | gawk '{print $1-1}'`
    echo "$header"
    head -n $header $DB > tmp1.txt

    grep '^.' $INF >> tmp1.txt
    echo -e "" >> tmp1.txt
    residual=$(( $NLINE - $header ));
    echo "$residual"
    tail -n $residual $DB >> tmp1.txt

    echo "insertion complete"
}

AppendTag() {

    head -n $NLINE $DB > tmp1.txt
    echo "" >> tmp1.txt
    grep '^.' $INF >> tmp1.txt
    echo "appending complete"

}

if [ $tester ]; then
    echo "insterting";
    InsertTag;

else
    echo "appending";
    AppendTag;

fi
