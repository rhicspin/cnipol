#! /usr/bin/perl



#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('f:h', \%opt);
if ( $opt{h} ) {
    help();
}

# Get Run ID
my $Runn = $opt{f};
if (length ($Runn) == 0){
    print "Error: Specify calibration <filename>.\n";
    help();
}

sub help(){
    print "\n";
    print " Usage:\n  $0 -h [ -f <runID>]\n\n"; 
    print "    create calibration energy histograms for fit. Execute CalibCal.pl next.\n\n";
    print "\t -f <filename> energy calibration filename.\n";
    print "\t -h           Show this help\n";
    print "\n\n";
    print "    ex.) CalibCal.pl -f blue_calib_0504\n";
    print "\n";
    exit(0);
}


$DATADIR      = $ENV{"DATADIR"}; 
$BASEDIR      = $ENV{"ASYMDIR"};

$CHECKDATA= "$DATADIR/$Runn.data";
$DATAFILE = "$Runn.data";
$LOGFILE  = "$BASEDIR/douts/$Runn.calib.log";
$COMMAND  = "Asym";

if (-e $CHECKDATA) {
    
    $datsize = (stat($CHECKDATA))[7];

    if ($datsize < 5000){
	# do not proceed just give warning

	printf ("*********************************************\n");
	printf ("**** THIS IS A SCALER DATA  *****************\n");
	printf ("*********************************************\n");

    } else {
	
	# Number of events to be skipped 1: all the events
	$NEVOPT = " -n 1 ";    
	$options = " -C ";     # TOF vs. EDEP without Tzero subtraction
	
	
	printf "nice -19 $COMMAND $NEVOPT -f $DATAFILE $options  -o $Runn.calib.hbook $tshiftopt| tee $LOGFILE\n "; 

	##### START #####
	system ("nice -19 $COMMAND $NEVOPT -f $DATAFILE $options -o $Runn.calib.hbook $tshiftopt| tee $LOGFILE "); 
	
	system ("h2root $Runn.calib.hbook ");
	system ("mv $Runn.calib.root ./douts");
	system ("rm $Runn.calib.hbook");

    }
    
} else {
    
    printf ("*********************************************\n");
    printf ("**** DATA FILE IS NOT EXISTING **************\n");
    printf ("*********************************************\n");
    printf (" check $datafile \n");
    
} 



