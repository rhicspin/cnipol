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
    print " Usage:\n  $0 -h [ -f <filename>]\n\n"; 
    print "    Create energy calibration histograms and execute fit. \n";
    print "    CalibGen.pl & CalibCal.pl \n\n";
    print "\t -f <filename> calibration file name w/o .data \n";
    print "\t -h            Show this help";
    print "\n\n";
    print "    ex.) Calib.pl -f bluc_calib_0504\n";
    print "\n";
    exit(0);
}



#########################################################################
######                     Main Routine                           #######
#########################################################################

printf("CALIB DATA : $Runn \n");
system("echo 'Generating histograms...\n'");
system("CalibGen.pl -f $Runn \n");
system("echo 'Executing Fitting...\n'");
system("CalibCal.pl -f $Runn \n");


