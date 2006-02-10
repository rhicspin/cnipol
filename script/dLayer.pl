#! /usr/bin/perl
# defualts
$HID  = 15000;
$OPT  = " ";

#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('f:bh', \%opt);

if ( $opt{h} ) {
    help();
}

if ( $opt{b} ){
    $OPT="-b";
}

# Get Run ID
my $Runn = $opt{f};
if (length ($Runn) == 0){
    print "Error: Specify <runID>.\n";
    help();
}

sub help(){
    print "\n";
    print " Usage:\n  $0 -hb [ -f <runID>]\n\n"; 
    print "    Generate histograms for deadlayer fit and execute fit.\n";
    print "    Executs dLayerGen.pl and dLayerCal.pl \n\n";
    print "\t -f <runID> runID\n";
    print "\t -b         fit on banana cut events. (def:const.t cut)\n";
    print "\t -h         Show this help \n";
    print "\n";
    print "    ex.) dLayer.pl -f 7279.005 -b \n";
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
system("dLayerCal.pl -f $Runn \n");

