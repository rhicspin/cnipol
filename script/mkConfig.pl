#! /usr/bin/perl
# mkConfig.pl
# Feb.10

$OPT  = " ";


#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('f:Dhb', \%opt);

if ( $opt{h} ) {
    help();
}

if ( $opt{D} ){
    $DlayerFit=1;
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
    print " Usage:\n  $0 -hDb [ -f <runID>]\n\n"; 
    print "\t -f <runID> runID\n";
    print "\t -D         Execute deadlayer fit\n";
    print "\t -b         Banana cut event selection on deadlayer fit.\n";
    print "\t            (used with -D option)\n";
    print "\t -h         Show this help\n";
    print "\n";
    print "    ex.) To perform deadlayer fit and make new configulation:\n\n";
    print "            % mkConfig.pl -f 7279.005 -D -b \n\n";
    print "         To make configulation file with existing deadlayer data\n\n";
    print "            % mkConfig.pl -f 7279.005 \n";
    print "\n";
    exit(0);

}



#----------------------------------------------------------------------
#               Directory Path Setup
#----------------------------------------------------------------------
$BASEDIR      = $ENV{"ASYMDIR"};
$MACRODIR     = $ENV{"MACRODIR"};
$CONFIGDIR    = "$BASEDIR/config";

unless (-d $CONFIGDIR) {
    mkdir $CONFIGDIR;
}



#----------------------------------------------------------------------
#               Execute DeadLayer Fit
#----------------------------------------------------------------------

if ($DlayerFit){
    printf("Dlayer DATA : $Runn \n");
    system("echo 'Generating histograms...\n'");
    system("dLayerGen.pl -f $Runn \n");
    system("echo 'Executing Fitting...\n'");
    system("dLayerCal.pl $OPT -f $Runn \n");
    system("echo 'Executing Integral Fitting...\n'");
    system("IntegCal.pl -f $Runn \n");
}


#----------------------------------------------------------------------
#               Command line argument hundling routine
#----------------------------------------------------------------------
$Calb=`RunDBReader -f $Runn | grep CALIB | gawk -f $MACRODIR/mkConfig.awk`;
chop $Calb;
if (length ($Calb) == 0) {die "Problem in getting calibration filename in run.db\n";}


#----------------------------------------------------------------------
#               Filename Allocation
#----------------------------------------------------------------------
$DlayerFile = "dlayer/$Runn.temp.dat";
$CalibFile  = "$Calb.temp.dat";
$IntegFile  = "integ/$Runn.temp.dat";


#----------------------------------------------------------------------
# The rest is just as it was in ParamConvine.pl, except that a copy
# command was added to get the desired file name.
#----------------------------------------------------------------------

# Initialization 
# old valus for acoef was 7.0

for ($st=0;$st<72;$st++) {

    $tzero[$st] = 5.0;
    $ecoef[$st] = 1.1; $edead[$st] = 100.;
    $a0[$st] =10. ; $a1[$st] = 100.; $acoef[$st] = 8.5;
    $dwidth[$st] = 50.; $pede[$st] = 0.;
    $pcoef0[$st] = 0.; $pcoef1[$st] = 0.; 
    $pcoef2[$st] = 0.; $pcoef3[$st] = 0.; 
    $pcoef4[$st] =0.;
    
}

#========================================
# READ DEAD LAYER and TZERO
#========================================
if ($Runn != "-i") {    # don't do if initializing...
open(Dlayer,$DlayerFile) || die "cannot open $DlayerFile";
printf "Reading $DlayerFile ... \n";
while ($dline = <Dlayer>) {
    chop($dline);
    @dlines = split(/\s+/,$dline);
    if ($dlines[0] ne "#") {

	$Strip = $dlines[0];
	$dwidth[$Strip] = $dlines[1];
	$tzero[$Strip] = $dlines[2];
	$edead[$Strip] = $dlines[11];
	$ecoef[$Strip] = $dlines[12];
	$pcoef0[$Strip] = $dlines[13];
	$pcoef1[$Strip] = $dlines[14];
	$pcoef2[$Strip] = $dlines[15];
	$pcoef3[$Strip] = $dlines[16];
	$pcoef4[$Strip] = $dlines[17];

    }
}
close(Dlayer);
}  # skipped if intiializing
#=======================================
# READ Am Calibration
#=======================================

open(ESCALE,$CalibFile) || die "cannot open $CalibFile";
printf "Reading $CalibFile ... \n";
while ($escale = <ESCALE>) {
    chop($escale);
    @escales = split(/\s+/,$escale);
    if ($escales[0] ne "#") {
	$Strip = $escales[0];
	$acoef[$Strip] = $escales[1];

#	printf "St: $Strip Am: $acoef[$Strip] \n";
    }
}
close(ESCALE);

#=========================================
# READ Integral-Amplitude Correlation
#=========================================
if ($Runn != "-i") {   # skip if initializing...
open(IA,$IntegFile) || die "cannot open $IntegFile";
printf "Reading $IntegFile ... \n";
while ($ia = <IA>) {
    chop($ia);
    @ias = split(/\s+/,$ia);

    if ($iavar[0] ne "#") {
	$Strip = $ias[0];
	$a0[$Strip] = $ias[1];
	$a1[$Strip] = $ias[2];
    }
}
close(IA);
}  # skipped if initializing

#==============================================
# Initializing if $Runn == "-i"
# READ default configuration file "config.dat"
#==============================================
if ($Runn == "-i") {
 open (CF,"config.dat") || die "There MUST be a config.dat file!";
 printf "Reading config.dat...\n";
 $Strip = 0;
 while ($cf = <CF>) {
  chop($cf);
  @cfs = split(/\s+/,$cf);

  if ($cfs[0] ne "*" && $cfs[0] ne "#") {
   $cfs[1] = substr($cfs[0],10);

   if ($cfs[5] > 0.) {$ecoef[$Strip] = $cfs[1]/$cfs[5];}

   $tzero[$Strip] = $cfs[1];
   $edead[$Strip] = $cfs[2];
   $dwidth[$Strip] = $cfs[6];
   $pcoef0[$Strip] = $cfs[8];
   $pcoef1[$Strip] = $cfs[9];
   $pcoef2[$Strip] = $cfs[10];
   $pcoef3[$Strip] = $cfs[11];
   $pcoef4[$Strip] = $cfs[12];

   $Strip++;
  }
 }
 close(CF);
}  # initialized

#=====================================================================
#  OutPut to File
#=====================================================================

$paraout = ">config.dat";

open(PARA,$paraout);
printf PARA "* Strip t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4\n";
printf PARA "* for the dead layer and T0  : $DlayerFile\n";
printf PARA "* for the Am calibration     : $CalibFile\n";
printf PARA "* for the Integral/Amplitude : $IntegFile\n";
printf PARA "* \n";
for ($st=0;$st<72;$st++) {
    printf PARA "Channel%02d=%5.3f %5.3f %7.1f %4.1f %5.2f %5.3f %4.1f %4.1f %4.3G %4.3G %4.3G %4.3G %4.3G\n",
    $st+1,$tzero[$st],$ecoef[$st]*$acoef[$st],$edead[$st],
    $a0[$st],$a1[$st],$acoef[$st],$dwidth[$st],$pede[$st],
    $pcoef0[$st],$pcoef1[$st],$pcoef2[$st],$pcoef3[$st],$pcoef4[$st];
    
}

close(PARA);


system("cp config.dat $CONFIGDIR/$Runn.config.dat");
printf "New configulation file: $CONFIGDIR/$Runn.config.dat\n\n";
