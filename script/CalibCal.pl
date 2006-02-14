#! /usr/bin/perl
##############  defulats ##############
$MACRODIR     = $ENV{"MACRODIR"};
$GHOSTVIEW=0;
#######################################



#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('f:gh', \%opt);

if ( $opt{h} ) {
    help();
}

if ( $opt{g} ) {
    $GHOSTVIEW=1;
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
    print "\t -g         Execute ghostscript to view  fitted results.\n";
    print "\t -h         Show this help";
    print "\n\n";
    print "    ex.) CalibCal.pl -f bluc_calib_0504\n";
    print "\n";
    exit(0);
}


sub GhostView(){
    system("gv calib/$Runn.fittemp.ps");
    system("gv calib/$Runn.summarytemp.ps");
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

if ($GHOSTVIEW) {GhostView();};



