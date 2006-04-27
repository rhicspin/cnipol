#! /usr/bin/perl
$cfile="";
$OPT_CONFIG=0;

#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('F:f:h', \%opt);

if ( $opt{h} ) {
    help();
}

if ( $opt{F} ) {
    $OPT_CONFIG=1;
    $cfile = $opt{F};
    if (length ($cfile) == 0){
	print "Error: Specify Config <file>.\n";
	help();
    } 
}


# Get Run ID
my $Runn = $opt{f};
if (length ($Runn) == 0){
    print "Error: Specify <runID>.\n";
    help();
}

sub help(){
    print "\n";
    print " Usage:\n  $0 -h [ -f <runID>][-F <file>]\n\n"; 
    print "    create banana histograms for fit. Execute dLayerCal.pl next.\n\n";
    print "\t -f <runID> runID\n";
    print "\t -F <file>  Load configulation from <file> \n";
    print "\t -h         Show this help";
    print "\n\n";
    print "    ex.) dLayerGen.pl -f 7279.005\n\n";
    print "    ex.2) Load configulation from ./config/7586.014.config.dat :\n\n";
    print "          dLayerCal.pl -f 7602.004 -F ./config/7586.014.config.dat \n\n";
    print "\n\n";
    exit(0);
}

$DATADIR      = $ENV{"DATADIR"}; 
$BASEDIR      = $ENV{"ASYMDIR"};

$CHECKDATA= "$DATADIR/$Runn.data";
$DATAFILE = "$Runn.data";
$LOGFILE  = "$BASEDIR/douts/$Runn.dl.log";
$COMMAND  = "Asym";


if (-e $CHECKDATA) {
    
	# Number of events to be skipped 1: all the events
	$NEVOPT = " -n 1";    
	$options = " -D -A";     # TOF vs. EDEP without Tzero subtraction
	if ($OPT_CONFIG) {
	    $options = " $options -F $cfile";
	}

	printf "nice +19 $COMMAND $NEVOPT -f $DATAFILE $options  -o $Runn.hbook | tee $LOGFILE\n "; 

	##### START #####
	system ("nice +19 $COMMAND $NEVOPT -f $DATAFILE  $options  -o $Runn.hbook | tee $LOGFILE\n ");
	system ("h2root $Runn.hbook ");
	system ("mv $Runn.root ./douts");
	system ("rm $Runn.hbook");
	
    
} else {
    
    printf ("*********************************************\n");
    printf ("**** DATA FILE IS NOT EXISTING **************\n");
    printf ("*********************************************\n");
    printf (" check $DATAFILE \n");
    
} 
