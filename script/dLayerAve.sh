#!/bin/bash
#dLayerAve.sh
#Makes deadlayer database
#Feb.24,2006
#I. Nakagawa

BASEDIR=$ASYMDIR
DLAYERDIR=$ASYMDIR/dlayer
RUNLIST=$ASYMDIR/.runlist
DUMMY=0;

Execute(){

NLINE=`wc $RUNLIST | gawk '{print $1}'`

for (( i=$Start; i<=$NLINE ; i++ )) ;
  do
    RUNID=`line.sh $i $RUNLIST  | gawk '{print $1}'`
    FILL=`echo $RUNID | gawk '{printf("%4d",$RUNID)}'`;
    Test=`echo $RUNID $FILL | gawk '{RunN=($1-$2)*10; printf("%1d",RunN)}'`
    if [ $Test -eq 1 ] ; then
	Beam="Yellow"
    else
	Beam="Blue"
    fi

    DlayerFile=$DLAYERDIR/$RUNID.temp.dat
    if [ -f $DlayerFile ] ; then

	    EVENT_RATES=`grep 'Event Rate'  $BASEDIR/douts/$RUNID.dl.log | gawk '{print $4}'`
	    READ_RATES=`grep 'Read Rate' $BASEDIR/douts/$RUNID.dl.log | gawk '{print $5}'`
	    AVE_Dl=`grep "dlave =" $DLAYERDIR/$RUNID.fit.log | gawk '{print $3}'`
	    AVE_Dl_ERROR=`grep "Deviation/strip=" $DLAYERDIR/$RUNID.fit.log | gawk '{print $2}'`
	    AVE_WCM=`grep 'WCM Average  ' $BASEDIR/douts/$RUNID.dl.log | gawk '{print $4}'`
	    FILL_BUNCH=`grep '# of Filled Bunch' $BASEDIR/douts/$RUNID.dl.log | gawk '{print $6}'`
	    SPECIFIC_LUMI=`grep 'Specific Luminosity' $BASEDIR/douts/$RUNID.dl.log | gawk '{print $6}'`
	    TGT_POS=`grep @ $BASEDIR/douts/$RUNID.dl.log | gawk '{print $4}'`

	    echo -e -n "$RUNID $AVE_Dl $AVE_Dl_ERROR $READ_RATES " 
	    echo -e -n "$AVE_WCM $SPECIFIC_LUMI $FILL_BUNCH "
	    echo -e -n "$DUMMY $DUMMY $DUMMY $DUMMY $DUMMY "
	    echo -e -n "$DUMMY $DUMMY $DUMMY $DUMMY $DUMMY "
	    echo -e -n "\n";

    fi

  done


}

################################################################
############                  Select Mode        ###############
################################################################
#Mode Blue+Flattop
#Mode=1 ; 
#Mode Blue+Injection
Mode=2 ;
#Mode Yellow+Flattop
#Mode=3 ; 
#Mode Yellow+Injection
#Mode=4 ; 
#Profile 7133 (Yellow)
#Mode=5 ; 
#Profile 7151 (Yellow)
#Mode=6 ; 
#Profile 7151 (Blue)
#Mode=7 ; 
################################################################




Profile=0;
Start=1;
if [ $Mode -eq 1 ] ; then 
    Beam=Blue
    Mode=FTP
elif [ $Mode -eq 2 ] ; then
    Beam=Blue
    Mode=INJ
elif [ $Mode -eq 3 ] ; then
    Beam=Yellow
    Mode=FTP
elif [ $Mode -eq 4 ] ; then
    Beam=Yellow 
    Mode=INJ
elif [ $Mode -eq 5 ] ; then
    Beam=Yellow;
    Profile=1;
    Fill=7133;
    Start=2;
elif [ $Mode -eq 6 ] ; then
    Beam=Yellow;
    Profile=1;
    Fill=7151;
    Start=2;
elif [ $Mode -eq 7 ] ; then
    Beam=Blue;
    Profile=1;
    Fill=7151;
    Start=2;
fi


Execute;

