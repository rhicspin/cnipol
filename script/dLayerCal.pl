#! /usr/bin/perl

$Runn = $ARGV[0];

$BASEDIR      = $ENV{"ASYMDIR"};
$MACRODIR     = $ENV{"MACRODIR"};
$DLAYERDIR    = "$BASEDIR/dlayer";

unless (-d $DLAYERDIR) {
    mkdir $DLAYERDIR;
}


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
    };
};


printf("RUN NUMBER  : $Runn \n");
printf("Mass Cut    : $MASSCUT \n");
printf("Beam Energy : $Bene \n");
printf("RHICBeam    : $RHICBeam \n");
#Main Routine

    system("echo '.x $MACRODIR/ExeKinFit.C(\"$Runn\", $Bene, $RHICBeam)' > input.C");
    system("root -b < input.C | tee $DLAYERDIR/$Runn.fit.log");
    
    system("mv testfit.dat $DLAYERDIR/$Runn.temp.dat");
    system("mv testfit.ps $DLAYERDIR/$Runn.fittemp.ps");
    system("mv testsummary.ps $DLAYERDIR/$Runn.summarytemp.ps");
    system("rm input.C");


