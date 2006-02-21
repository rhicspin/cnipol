#! /usr/bin/perl
# J. Wood, Feb. 18, 2006
# script to scale alpha calibration to adjust for attuation fluctuations

#----------------------------------------------------------------------
#               Command Line Options
#----------------------------------------------------------------------
use Getopt::Std;
my %opt;
getopts('c:a:h', \%opt);

if ( $opt{h} ) {
    help();
}
 
# Get calibration Run ID
my $Runn = $opt{c};
if (length ($Runn) == 0){
    print "Error: Specify calibration <filename>.\n";
    help();
}

# Get attenuation factor file
my $atten_name = $opt{a};
if (length ($atten_name) == 0){
    print "Error: Specify attenuation factor <filename>.\n";
    help();
}

sub help(){
    print "\n";
    print " Usage:\n  $0 -h [ -c <calibfile> -a <attenfile>]\n\n"; 
    print "    Adjust alpha calibrations for attuation fluctuations \n";
    print "\t -c <calibfile> calibration fit result file number \n";
    print "\t -a <attenfile> attenuation factor file name (specify full path)\n";
    print "\t -h            Show this help";
    print "\n\n";
    print "    ex.) ScaleCalibStbySt.pl -c 60217.001\n";
    print "\n";
    exit(0);
}

#########################################################################
######                     Main Routine                           #######
#########################################################################
$BASEDIR=$ENV{"ASYMDIR"};

$calib_name = $BASEDIR."/calib/".$Runn.".temp.dat";
$out_name = ">".$BASEDIR."/calib/".$Runn.".scale.dat";

open(CALIBFILE,$calib_name) or die "Can't open file ",$calib_name;
$i = 0;
while ($cline = <CALIBFILE>){
    chop($cline);
    @cwords = split(/\s+/,$cline);
    $stc[$i] = $cwords[0];
    $calpha[$i] = $cwords[1];
    $calpherr[$i] = $cwords[2];
    $peak[$i] = $cwords[3];
    $peakerr[$i] = $cwords[4];
    $chisq[$i] = $cwords[5];
    $ndf[$i] = $cwords[6];
    $stat[$i] = $cwords[7];
    $i++;
}
close(CALIBFILE);

open(ATTENFILE,$atten_name) or die "Can't open file ",$atten_name;
$i = 0;
while ($aline = <ATTENFILE>){
    chop($aline);
    @awords = split(/\s+/,$aline);
    $sta[$i] = $awords[1];
    $scatten[$i] = $awords[2];
    $scatterr[$i] = $awords[3];
#    printf " %d %4.3f %4.3f \n",$sta[$i],$scatten[$i],$scatterr[$i];
    $i++;
}
close(ATTENFILE);

open(OUTFILE,$out_name);
printf "Output written to %s\n", $out_name;

$nchan = 72;
for ($i=0;$i<$nchan;$i++) {

    $newCalpha[$i] = $scatten[$i] * $calpha[$i];
    printf OUTFILE "%d %4.3f %6.6f %4.1f %5.5f %3.2f %2d %s\n",
       $stc[$i],$newCalpha[$i],$calpherr[$i],$peak[$i],$peakerr[$i],
    $chisq[$i],$ndf[$i],$stat[$i];
    
}

close(OUTFILE);
