#! /usr/bin/perl
# defualts
# defualts 
# HID:15000 (const.t) HID:15100: (banana)
$HID  = 15000; 
# Fit Energy Rnage Default
$EMIN=400;
$EMAX=900;
$OPT  = " ";

#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('E:f:bh', \%opt);

if ( $opt{h} ) {
    help();
}

if ( $opt{b} ){
    $OPT="-b $OPT";
}

# Get Run ID
my $Runn = $opt{f};
if (length ($Runn) == 0){
    print "Error: Specify <runID>.\n";
    help();
}

# Get Fit Energy Range
if ( $opt{E} ){
    $OPT="-E $opt{E} $OPT";
}


sub help(){
    print "\n";
    print " Usage:\n  $0 -hb [-f <runID>] [-E <Emin:Emax>]\n\n"; 
    print "    Generate histograms for deadlayer fit and execute fit.\n";
    print "    Executs dLayerGen.pl and dLayerCal.pl \n\n";
    print "\t -f <runID> runID\n";
    print "\t -b         fit on banana cut events. (def:const.t cut)\n";
    print "\t -E <Emin:Emax> Fit Energy Range in [keV] (def <$EMIN:$EMAX>) \n";
    print "\t -h         Show this help \n";
    print "\n";
    print "    ex.) dLayer.pl -f 7279.005 -b -E 350:950 \n";
    print "\n";
    exit(0);
}


#########################################################################
######                     Main Routine                           #######
#########################################################################

printf("Dlayer DATA : $Runn \n");
system("echo 'Generating histograms...\n'");
system("dLayerGen.pl -f $Runn \n");
system("echo 'Executing Fitting...\n'");
system("dLayerCal.pl $OPT -f $Runn \n");
