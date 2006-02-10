#! /usr/bin/perl

#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('f:bh', \%opt);

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
    print " Usage:\n  $0 -hb [ -f <runID>]\n\n"; 
    print "    Apply linear fit on the peak and integral value of WFD adcs. \n\n";
    print "\t -f <runID> runID\n";
    print "\t -h         Show this help \n";
    print "\n";
    print "    ex.) IntegCal.pl -f 7279.005 -b \n";
    print "\n";
    exit(0);
}


#----------------------------------------------------------------------
#               Directory Path Setup
#----------------------------------------------------------------------
$BASEDIR      = $ENV{"ASYMDIR"};
$MACRODIR     = $ENV{"MACRODIR"};
$INTEGDIR    = "$BASEDIR/integ";

unless (-d $INTEGDIR) {
    mkdir $INTEGDIR;
}

printf("INTEG-AMP DATA : $Runn \n");
    
#----------------------------------------------------------------------
#               Main Routine
#----------------------------------------------------------------------

system("echo '.x $MACRODIR/ExeInteg.C(\"$Runn\")' > input.C");
system("root -b < input.C | tee integ/$Runn.int.log");
    
system("mv testfit.dat integ/$Runn.temp.dat");
system("mv testfit.ps  integ/$Runn.fittemp.ps");
system("mv testsummary.ps integ/$Runn.summarytemp.ps");
system("rm input.C");


