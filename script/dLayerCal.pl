#! /usr/bin/perl
#
# Nov 2, 2010 Dmitri Smirnov
#    - Formatted for better reading experience
#

# defualts 
$GHOSTVIEW=0;
# HID:15000 (const.t) HID:15100: (banana)
$HID  = 15000; 
# Fit Energy Rnage Default
$EMIN=400;
$EMAX=900;
$cfile="config.dat";
$ONLINE_CONFIG="";
$OUTPUTDIR=".";
$ONE_PAR_FIT_OPTION=1;

#Default Dlayer Width
$DLAYER_WIDTH_DEFAULT=50;
$DLAYER_WIDTH=$DLAYER_WIDTH_DEFAULT;
#Default Dlayer Width Maximum. Beyond this, force reset tobe $DLAYER_WIDTH_DEFAULT;
$DLAYER_WIDTH_MAX=150;
$INVERT_KINEMA=0;


#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('d:E:f:O:ibgh', \%opt);

if ( $opt{h} ) { help(); }
if ( $opt{i} ) { $INVERT_KINEMA = 1; }
if ( $opt{g} ) { $GHOSTVIEW = 1; }
if ( $opt{b} ) { $HID = 15100; }
if ( $opt{d} ) { $OUTPUTDIR=$opt{d}; }

if ( $opt{O} ){
    $ONE_PAR_FIT_OPTION=$opt{O};
    if (($ONE_PAR_FIT_OPTION != 1)&&($ONE_PAR_FIT_OPTION!=11)) {
	print "Error : Invarid One Par Fit Option <$ONE_PAR_FIT_OPTION> \n";
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
    @field = split(/:/,$opt{E});
    if (length($field[0]) != 0){ $EMIN=$field[0];}
    if (length($field[1]) != 0){ $EMAX=$field[1];}
}

sub help(){
    print "\n";
    print " Usage:\n  $0 [-hgbi][ -f <runID>][-E <Emin:Emax>][-d <dir>][-O <fit mode>]\n\n"; 
    print "    fit banana histograms and get deadlayer and t0.\n";
    print "    Execute dLayerGen.pl for histogram creation.\n\n";
    print "\t -f <runID>     runID\n";
    print "\t -b             Fit on banana cut events. (def:const.t cut)\n";
    print "\t -d <dir>       Temporary output directory.\n";
    print "\t -g             Launch ghostviewer after fit.\n";
    print "\t -E <Emin:Emax> Fit Energy Range in [keV] (def <$EMIN:$EMAX>) \n";
    print "\t -h             Show this help \n";
    print "\t ----------------- expert options ------------------\n";
    print "\t -O <fit mode>  One parameter fitting option. [Def]:$ONE_PAR_FIT_OPTION\n";
    print "\t                  1: fix deadlayer   11: fix t0\n";
    print "\t -i             Interplet -E <EMIN:EMAX> as Kinetic Energy. Calculate relevant energy depsit\n";
    print "\t                for average <dwidth>. Average <dwidth> is taken from dlayer/<runID>.fit.log.\n";
    print "\t                Take default value <$DLAYER_WIDTH> if the file doesn't exist\n";
    print "\n";
    print "\n";
    print "    ex.1) dLayerCal.pl -E 350:950 -f 7279.005 -b \n\n";
    print "    ex.2) Fixed t0 one parameter fit.\n";
    print "          dLayerCal.pl -O 11 -f 7300.002 -b \n\n";
    print "    ex.3) Convert <EMIN:EMAX> from kinetic energy to energy deposit.\n";
    print "          dLayerCal.pl -f 7279.001 -E 400:900 -i \n\n";
    print "\n";
    exit(0);
}

sub GhostView(){
    system("gv dlayer/$Runn.fittemp.ps");
    system("gv dlayer/$Runn.residual.ps");
    system("gv dlayer/$Runn.summarytemp.ps");
}


#----------------------------------------------------------------------
#               Directory Path Setup
#----------------------------------------------------------------------
$BASEDIR      = $ENV{"ASYMDIR"};
$MACRODIR     = $ENV{"MACRODIR"};
$DLAYERDIR    = "$BASEDIR/dlayer";
$CONFDIR      = $ENV{"CONFDIR"};

unless (-d $DLAYERDIR) {
    mkdir $DLAYERDIR;
}


#----------------------------------------------------------------------
#         Get Online Configulation File Name from Online Log
#----------------------------------------------------------------------
sub GetOnlineConfig() {

   $ONLINEDIR = $ENV{"ONLINEDIR"};
   $ONLINELOG = "$ONLINEDIR/log/$Runn.log";
   
   unless (open(ONLINE_LOG_FILE,"$ONLINELOG")) {
      print "Warning: $ONLINELOG doesn't exist. Deadlayer for online won't be plotted.\n";
   }
   
   while (<ONLINE_LOG_FILE>) {
      if (/Reading calibration parameters from file/) {
         ($F1,$F2,$F3,$F4,$F5,$F6,$F7,$F8) = split;
         $ONLINE_CONFIG="$CONFDIR/$F8";
      };
   };
}


#----------------------------------------------------------------------
#           Convert energy deposit to kinetic energy
#----------------------------------------------------------------------
sub ConvertEdep2Ekin(){

    #Get Average deadlayer width from fit log file
    $FITLOGFILE="dlayer/$Runn.fit.log";
    if (-f "$FITLOGFILE"){
	open(fitlogfile,"$FITLOGFILE") || die "ConvertEdep2Ekin(): $!";
	while (<fitlogfile>) {
	    if (/dlave =/){
		($F1,$F2,$F3) = split;
		$DLAYER_WIDTH = "$F3";
	    }
	}
	if ($DLAYER_WIDTH>$DLAYER_WIDTH_MAX) {
	    printf "DeadLayer $DLAYER_WIDTH exceeds limit $DLAYER_WIDTH_MAX. ";
	    printf "Reset default $DLAYER_WIDTH_DEFAULT\n";
	    $DLAYER_WIDTH=$DLAYER_WIDTH_DEFAULT;
	}
    } else {
	print "$FITLOGFILE doesn't exist. Use default <dwidth>=$DLAYER_WIDTH\n";
    }

    $E_KIN_MIN=$EMIN;
    $E_KIN_MAX=$EMAX;
    #Convert <EMIN:EMAX> to Energy Deposits
    my @tmp = split(" ", `KinFunc -w $DLAYER_WIDTH -e $EMIN -i`);
    $EMIN = $tmp[1];
    my @tmp = split(" ", `KinFunc -w $DLAYER_WIDTH -e $EMAX -i`);
    $EMAX = $tmp[1];
}


#----------------------------------------------------------------------
#           Get Run Condistions from Offline Log
#----------------------------------------------------------------------
sub GetLog(){

   unless (open(LOGFILE,"douts/$Runn.dl.log")) {
       die "Error: douts/$Runn.dl.log doesn't exist. Run dLayerGen.pl first.\n";
   }
   
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
      } elsif (/CONFIG/) {
         ($F1,$F2,$F3) = split;
         $cfile=$F3;
      };
   };
}


#----------------------------------------------------------------------
#               Print Run Condition
#----------------------------------------------------------------------

sub PrintRunCondition(){

    printf("RUN NUMBER           : $Runn \n");
    printf("Mass Cut             : $MASSCUT \n");
    printf("Beam Energy          : $Bene \n");
    printf("RHICBeam             : $RHICBeam \n");
    printf("E2T                  : $E2T \n");
    printf("Emin - Emax          : $EMIN - $EMAX \n");
    if ($INVERT_KINEMA) {printf("Average DLAYER_WIDTH : $DLAYER_WIDTH \n"); 
			 printf("Corresp.Kin.EMIN-EMAX: $E_KIN_MIN - $E_KIN_MAX \n");   };
    printf("HID                  : $HID \n");
    printf("Config File          : $cfile \n");
    printf("Online Config        : $ONLINE_CONFIG \n");
    printf("One Par Fit option   : $ONE_PAR_FIT_OPTION \n");
    printf("Temporary output Dir : $OUTPUTDIR \n");
}


#----------------------------------------------------------------------
#                             Main Routine
#----------------------------------------------------------------------

# Get Run Conditions from Offline Log file
GetLog();

# Get Online Configulation file name for online monitoring.
GetOnlineConfig();

# Calculate EMIN:EMAX range from kinetic energies for given dlayer thickness
if ( $INVERT_KINEMA ) { ConvertEdep2Ekin(); };

# Print Run Conditions
PrintRunCondition();

# Make input macro for fitting.
system("echo '.x $MACRODIR/ExeKinFit.C(\"$Runn\", $Bene, $RHICBeam, $E2T, $EMIN, $EMAX, $HID,\"$cfile\",\"$ONLINE_CONFIG\",\"$OUTPUTDIR\", $ONE_PAR_FIT_OPTION)' > $OUTPUTDIR/input.C");

# Execute deadlayer fitting on root
system("root -b < $OUTPUTDIR/input.C | tee $DLAYERDIR/$Runn.fit.log");
    
if (-f "$OUTPUTDIR/testfit.dat")    {system("mv $OUTPUTDIR/testfit.dat $DLAYERDIR/$Runn.temp.dat");}
if (-f "$OUTPUTDIR/testfit.ps")     {system("mv $OUTPUTDIR/testfit.ps $DLAYERDIR/$Runn.fittemp.ps");}
if (-f "$OUTPUTDIR/testsummary.ps") {system("mv $OUTPUTDIR/testsummary.ps $DLAYERDIR/$Runn.summarytemp.ps");}
if (-f "$OUTPUTDIR/residual.ps")    {system("mv $OUTPUTDIR/residual.ps $DLAYERDIR/$Runn.residual.ps");}
if (-f "$OUTPUTDIR/input.C")        {system("rm $OUTPUTDIR/input.C");}

if ($GHOSTVIEW) {GhostView();};
