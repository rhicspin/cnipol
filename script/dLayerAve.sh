#!/bin/bash
#dLayerAve.sh
#Makes deadlayer database
#Feb.24,2006
#I. Nakagawa

BASEDIR=$ASYMDIR
DLAYERDIR=$ASYMDIR/dlayer


Execute(){

NLINE=`wc $RUNLIST | gawk '{print $1}'`

for (( i=$Start; i<=$NLINE ; i++ )) ;
  do
    run=`line.sh $i $RUNLIST  | gawk '{print $1}'`
    date=`line.sh $i $RUNLIST  | gawk '{print $3}'`
    DlayerFile=$DLAYERDIR/$run.temp.dat
    if [ -f $DlayerFile ] ; then
	target=`grep @ $BASEDIR/douts/$run.dl.log | gawk '{print $5}' | head -n 1`

	# process only vertical target measurements
	if [ $target -lt 10 ] ; then 
	    rates=`grep 'Event Rate'  $BASEDIR/douts/$run.dl.log | gawk '{print $4}'`
	    chi2=`grep "Deviation/strip=" $DLAYERDIR/$run.fit.log | gawk '{print $2}'`
	    rdrates=`grep 'Read Rate' $BASEDIR/douts/$run.dl.log | gawk '{print $4}'`
	    wcmave=`grep 'WCM Average  ' $BASEDIR/douts/$run.dl.log | gawk '{print $4}'`
	    fillb=`grep '# of Filled Bunch' $BASEDIR/douts/$run.dl.log | gawk '{print $6}'`
	    SpeLumi=`grep 'Specific Luminosity' $BASEDIR/douts/$run.dl.log | gawk '{print $6}'`
	    tgtpos=`grep @ $BASEDIR/douts/$run.dl.log | gawk '{print $4}'`
#	    echo -e -n "$run   $date  $chi2  $rdrates $wcmave $fillb " 
	    echo -e -n "$run   $date  $chi2  $rdrates $tgtpos $fillb  " 
	    DlayerAverage -f $run.temp.dat -D $DisableList -d $DLAYERDIR
	fi

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

DisableList=1
if [ $Beam = "Yellow" ] ;then
    DisableList=0
fi

if [ $Profile -eq 0 ] ; then
    RUNLIST=$HOME/2005/db/Sampled-$Beam\_$Mode.list
else 
    RUNLIST=$HOME/2005/offline/DlayerHistory/dat/profile_$Fill.$Beam.ext.dat
fi

Execute;

