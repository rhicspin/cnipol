#! /usr/bin/perl

$Runn = $ARGV[0];
$ConfigFileName = $ARGV[1];

if (length ($ConfigFileName) != 0) {
    $ConfigFile = "$ConfigFileName.dat";
    $OPT_CONFIG = " -F $ConfigFile ";
}

$DATADIR      = $ENV{"DATADIR"}; 
$BASEDIR      = $ENV{"ASYMDIR"};

$CHECKDATA= "$DATADIR/$Runn.data";
$DATAFILE = "$Runn.data";
$LOGFILE  = "$BASEDIR/douts/$Runn.dl.log";
$COMMAND  = "Asym";

if (-e $CHECKDATA) {
    
	# Number of events to be skipped 1: all the events
	$NEVOPT = " -n 1 ";    
	$opt_bwidth="3";     # banana mass cut <sigma> 
	$options = "-D -A";     # TOF vs. EDEP without Tzero subtraction
	
	printf "nice -19 $COMMAND $NEVOPT -f $DATAFILE -m $opt_bwidth $options  -o $Runn.hbook $tshiftopt $OPT_CONFIG | tee $LOGFILE\n "; 
	
	##### START #####
	system ("nice -19 $COMMAND $NEVOPT -f $DATAFILE -m $opt_bwidth $options  -o $Runn.hbook $tshiftopt $OPT_CONFIG | tee $LOGFILE\n ");
	system ("h2root $Runn.hbook ");
	system ("mv $Runn.root ./douts");
	system ("rm $Runn.hbook");
	
    
} else {
    
    printf ("*********************************************\n");
    printf ("**** DATA FILE IS NOT EXISTING **************\n");
    printf ("*********************************************\n");
    printf (" check $DATAFILE \n");
    
} 
