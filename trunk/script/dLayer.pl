#! /usr/bin/perl
# defualts
# defualts 
# HID:15000 (const.t) HID:15100: (banana)
$HID  = 15000; 
# Fit Energy Rnage Default
$EMIN=400;
$EMAX=900;
$OPT_CAL  = " ";
$OPT_GEN  = " ";

#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('d:F:E:f:ibgh', \%opt);
if ( $opt{h} ) {    help();  }
if ( $opt{b} ){    $OPT_CAL="-b $OPT_CAL"; }
if ( $opt{i} ) {   $OPT_CAL="-i $OPT_CAL"; }
if ( $opt{g} ){    $OPT_CAL="-g $OPT_CAL"; }
if ( $opt{d} ){    $OPT_CAL="$OPT_CAL -d $opt{d}"; }


# specify config file
if ( $opt{F} ) {
    $OPT_GEN="$OPT_GEN -F $opt{F}";
    if (length ($opt{F}) == 0){
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

# Get Fit Energy Range
if ( $opt{E} ){
    $OPT_CAL="$OPT_CAL -E $opt{E} ";
}


sub help(){
    print "\n";
    print " Usage:\n  $0 -hgb [-f <runID>][-E <Emin:Emax>][-F <file>][-d <dir>]\n\n"; 
    print "    Generate histograms for deadlayer fit and execute fit.\n";
    print "    Executes dLayerGen.pl and dLayerCal.pl \n\n";
    print "\t -f <runID>     runID\n";
    print "\t -F <file>      Load configulation from <file> \n";
    print "\t -b             Fit on banana cut events. (def:const.t cut)\n";
    print "\t -d <dir>       Temporary output directory for KinFit.C\n";
    print "\t -g             Launch ghostviewer after fit.\n";
    print "\t -i             Interplet <Emin:Emax> as kinetic energy. See dLayerCal.pl -h for detail\n";
    print "\t -E <Emin:Emax> Fit Energy Range in [keV] (def <$EMIN:$EMAX>) \n";
    print "\t -h             Show this help \n";
    print "\n";
    print "    ex.) dLayer.pl -f 7279.005 -b -E 350:950 \n\n";
    print "    ex.2) Load configulation from ./config/7586.014.config.dat :\n\n";
    print "          dLayer.pl -f 7602.004 -F ./config/7586.014.config.dat \n\n";
    print "\n";
    exit(0);
}


#########################################################################
######                     Main Routine                           #######
#########################################################################

printf("Dlayer DATA : $Runn \n");
system("echo 'Generating histograms...\n'");
system("dLayerGen.pl -f $Runn $OPT_GEN \n");
system("echo 'Executing Fitting...\n'");
system("dLayerCal.pl $OPT_CAL -f $Runn \n");
