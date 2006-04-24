#! /bin/bash
#rundb_insterter.sh
#April 20, 2006, A. Hoffman

INF=out.dat;
DB=test.dat;
LIST=runlist.txt;

NLINE=`wc $DB | gawk '{print $1}'`

InsertTag()  {

    grep -n '^.' $LIST > tmplist.txt

    RunN=`grep "^\[" $INF | sed -e "s/\[//" | sed "s/\]//" | sed -e "s/@//"`
    echo -e "$RunN"

    RefN=`grep -n "$RunN" $LIST | cut -f1 -d: | gawk '{print $1 +1}'`
    echo -e "$RefN"

    NextN=`grep "^$RefN" tmplist.txt | sed "s/:/ /" | gawk '{print $2}'`
    echo -e "$NextN"

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

InsertTag
