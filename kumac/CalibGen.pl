#! /usr/bin/perl

$Runn = $ARGV[0];

$basedir = $basedir = "/home/itaru/pol2004-offl";

## need to be changed
##----------------------------------
##$datafile = "/data/2005/calibr/$Runn.data";
#$datafile = "/data/2005/data/$Runn.data";
$datafile = "/home/itaru/data/$Runn.data";
##----------------------------------
$logfile = "$basedir/Macro/Douts/$Runn.calib.log";
$command = "Asym";


if (-e $datafile) {
    
    $datsize = (stat($datafile))[7];

    if ($datsize < 5000){
	# do not proceed just give warning

	printf ("*********************************************\n");
	printf ("**** THIS IS A SCALER DATA  *****************\n");
	printf ("*********************************************\n");

    } else {
	
	# Number of events to be skipped 1: all the events
	$NEVOPT = " -n 1 ";    
	$options = " -C ";     # TOF vs. EDEP without Tzero subtraction
	
	
	printf "nice -19 $command $NEVOPT -f $datafile $options  -o $Runn.calib.hbook $tshiftopt| tee $logfile\n "; 

	##### START #####
	system ("nice -19 $command $NEVOPT -f $datafile $options -o $Runn.calib.hbook $tshiftopt| tee $logfile "); 
	
	system ("h2root $Runn.calib.hbook ");
	system ("mv $Runn.calib.root ./Douts");
	system ("rm $Runn.calib.hbook");

    }
    
} else {
    
    printf ("*********************************************\n");
    printf ("**** DATA FILE IS NOT EXISTING **************\n");
    printf ("*********************************************\n");
    printf (" check $datafile \n");
    
} 



