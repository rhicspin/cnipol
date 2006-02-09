#! /usr/bin/perl
$MACRODIR     = $ENV{"MACRODIR"};


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
    print "    Execute fit on calibration histograms. \n";
    print "    Run CalibGen.pl to create calibration histogram data file.\n\n";
    print "\t -f <filename> calibration file name w/o .data \n";
    print "\t -h         Show this help";
    print "\n\n";
    print "    ex.) CalibCal.pl -f bluc_calib_0504\n";
    print "\n";
    exit(0);
}



#########################################################################
######                     Main Routine                           #######
#########################################################################

printf("CALIB DATA : $Runn \n");
system("echo '.x $MACRODIR/ExeCalib.C(\"$Runn\")' > input.C");
system("root -b < input.C | tee calib/$Runn.fit.log");
    
system("mv testfit.dat calib/$Runn.temp.dat");
system("mv testfit.ps  calib/$Runn.fittemp.ps");
system("mv testsummary.ps calib/$Runn.summarytemp.ps");
system("rm input.C");





