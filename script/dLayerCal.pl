#! /usr/bin/perl
# defualts 
# HID:15000 (const.t) HID:15100: (banana)
$HID  = 15000; 
# Fit Energy Rnage Default
$EMIN=400;
$EMAX=900;

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
    $HID=15100;
}

# Get Run ID
my $Runn = $opt{f};
if (length ($Runn) == 0){
    print "Error: Specify <runID>.\n";
    help();
}


# Get Fit Energy Range
if ( $opt{E} ){
    @field = split(/:/,$opt{E});
    if (length($field[0]) != 0){ $EMIN=$field[0];}
    if (length($field[1]) != 0){ $EMAX=$field[1];}
}


sub help(){
    print "\n";
    print " Usage:\n  $0 -hb [ -f <runID>] [-E <Emin:Emax>]\n\n"; 
    print "    fit banana histograms and get deadlayer and t0.\n";
    print "    Execute dLayerGen.pl for creation.\n\n";
    print "\t -f <runID>     runID\n";
    print "\t -b             Fit on banana cut events. (def:const.t cut)\n";
    print "\t -E <Emin:Emax> Fit Energy Range in [keV] (def <$EMIN:$EMAX>) \n";
    print "\t -h             Show this help \n";
    print "\n";
    print "    ex.) dLayerCal.pl -E 350:950 -f 7279.005 -b \n";
    print "\n";
    exit(0);
}




#----------------------------------------------------------------------
#               Directory Path Setup
#----------------------------------------------------------------------
$BASEDIR      = $ENV{"ASYMDIR"};
$MACRODIR     = $ENV{"MACRODIR"};
$DLAYERDIR    = "$BASEDIR/dlayer";

unless (-d $DLAYERDIR) {
    mkdir $DLAYERDIR;
}



#----------------------------------------------------------------------
#               Get Experimental Condistion for the run
#----------------------------------------------------------------------
open(LOGFILE,"douts/$Runn.dl.log");
while (<LOGFILE>) {
    if (/MASSCUT/) {
	($F1,$F2,$F3) = split;
	$MASSCUT=$F3;
    } elsif (/Beam Energy/) {
	($F1,$F2,$F3,$F4) = split;
	$Bene=$F4;
    } elsif (/RHIC Beam/) {
	($F1,$F2,$F3,$F4) = split;
	$RHICBeam=$F4;
    } elsif (/Kinematic Const. E2T/) {
	($F1,$F2,$F3,$F4,$F5) = split;
	$E2T=$F5;
    };
};


printf("RUN NUMBER  : $Runn \n");
printf("Mass Cut    : $MASSCUT \n");
printf("Beam Energy : $Bene \n");
printf("RHICBeam    : $RHICBeam \n");
printf("E2T         : $E2T \n");
printf("Emin - Emax : $EMIN - $EMAX \n");
printf("HID         : $HID \n");


#----------------------------------------------------------------------
#               Main Routine
#----------------------------------------------------------------------

system("echo '.x $MACRODIR/ExeKinFit.C(\"$Runn\", $Bene, $RHICBeam, $E2T, $EMIN, $EMAX, $HID)' > input.C");
system("root -b < input.C | tee $DLAYERDIR/$Runn.fit.log");
    
system("mv testfit.dat $DLAYERDIR/$Runn.temp.dat");
system("mv testfit.ps $DLAYERDIR/$Runn.fittemp.ps");
system("mv testsummary.ps $DLAYERDIR/$Runn.summarytemp.ps");
system("rm input.C");




