#! /usr/bin/perl
# J.Wood, 23 May 2006
# Plot the ratio of fixed target polarization to target scan polarization
# target scan polarization is averaged over all runs in a fill

# defaults
$OFLDIR = $ENV{"ASYMDIR"};
$MACRODIR = $ENV{"MACRODIR"};
$SUMDIR = $OFLDIR."/ps/";
$OPT_GV=0;
$ONOFF=0;
$PLOTNAME = $SUMDIR."fixed-scan_ratio.ps";
$ONOFFNAME = $SUMDIR."onl-offl_ratio.ps";

# get arguments
use Getopt::Std;
my %opt;
getopts('hog', \%opt);

if ( $opt{h} ) {
    help();
}
if ( $opt{o} ) {
    $ONOFF=1;
}
if ( $opt{g} ) {
    $OPT_GV=1;
}

sub help(){
    print "\n";
    print " Usage:\n  $0 -[options]\n\n"; 
    print "    Search the offline summary files for fills with a fixed \n";
    print "    target measurement (only Vertical targets at 100 GeV). \n";
    print "    Plot the ratio of fixed target polarization to target scan \n";
    print "    polarization.  The target scan polarization is averaged \n";
    print "    over all scan measurements in a particular fill \n\n";
    print "\t -o            Plot ratio of online and offline polarizations\n";
    print "\t -g            Execute ghostscript to view plot results.\n";
    print "\t -h            Show this help";
    print "\n\n";
    print "    ex.) fixedTargRatio.pl -g\n";
    print "\n";
    exit(0);
}

# display plots with gv
sub Display() {
    system("gv $PLOTNAME &");
    if ($ONOFF) {
	system("gv $ONOFFNAME &");
    }
}

# scan offline summary files, write output
print "BLUE: \n";
system("readFixedFill.pl b \n");
print " \n";
print "YELLOW: \n";
system("readFixedFill.pl y \n");
print " \n";

# exec kumac, make plot
print "Creating image file ... $PLOTNAME \n\n";
$ENV{"FIXEDPLOT"} = $PLOTNAME;
system("nice -n+19 paw -w 0 -b $MACRODIR/fixed-scan.kumac >$SUMDIR.paw.output");
if ($ONOFF) {
    print "Comparing Online/Offline ratio ...\n $ONOFFNAME \n";
    $ENV{"RATIOPLOT"} = $ONOFFNAME;
    system("nice -n+19 paw -w 0 -b $MACRODIR/onl-offl.kumac >>$SUMDIR.paw.output");    
}

# show plot
if ($OPT_GV) {Display()};
