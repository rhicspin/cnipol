#! /usr/bin/perl

$Runn = $ARGV[0];
$ConfigFileName = $ARGV[1];

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
    print "Error: Specify <runID>.\n";
    help();
}

sub help(){
    print "\n";
    print " Usage:\n  $0 -h [ -f <runID>]\n\n"; 
    print "    create banana histograms for fit. Execute dLayerCal.pl next.\n\n";
    print "\t -f <runID> runID\n";
    print "\t -h         Show this help\n";
    print "\n";
    exit(0);
}



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
