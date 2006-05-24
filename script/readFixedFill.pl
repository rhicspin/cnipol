#! /usr/bin/perl
# J.Wood, May 22, 2006
# script to extract data from offline summary (~e950/offline/summary/Offline*)
# then calculate ration of P for fixed target to scan measurements

$ring = $ARGV[0];

$BASEDIR = $ENV{"ASYMDIR"};

if ($ring eq "y") {
    $infile = $BASEDIR."/summary/OfflinePol_Yellow.dat";
    $outfile = ">".$BASEDIR."/summary/fixed-scan_ratio_yel.dat";
}
elsif ($ring eq "b") {
    $infile = $BASEDIR."/summary/OfflinePol_Blue.dat";
    $outfile = ">".$BASEDIR."/summary/fixed-scan_ratio_blu.dat";
}
else {
    print "Must specify \'y\' for Yellow or \'b\' for Blue\n";
    exit(0);
}
print "Reading ... $infile\n";
print "Output written ... $outfile\n";

open(SUMFILE,$infile);
open(OUTFILE,$outfile);
printf OUTFILE "# fill, ratio-err(onl), ratio-err(offl), energy, # scans, P-err(onl), P-err(offl), fixed run, P-err(onl), P-err(offl), rate\n";

$nline = 0;
$nfix = 0;
while ($line = <SUMFILE>) {
    chop($line);
    @words = split(/\s+/,$line);
    if ($words[3] eq "Good" && $words[17] eq "fixd" && $nline>0 
	&& $words[16] eq "V" && $words[5] eq "100") {
	$run[$nfix] = $words[0];
	($ffill,$frun) = split(/\./,$run[$nfix]);
#	print "Run $run, fill $fill\n";
	$p_onl[$nfix] = $words[1];
	$err_onl[$nfix] = $words[2];
	$ene[$nfix] = $words[5];
	$p_offl[$nfix] = $words[10];
	$err_offl[$nfix] = $words[11];
	$targ[$nfix] = $words[16];
	$rate[$nfix] = $words[18];
# array of fixed target fill numbers
	$goodfix[$nfix] = $ffill;
	$nfix++;
    }
    $nline++;
}
close(SUMFILE);

# find scan runs from same fills as fixed runs
for ($i=0; $i<$nfix; $i++) {
    $nscan = 0;
    $avp_onl = 0.;
    $averr_onl = 0.;
    $avp_offl = 0.;
    $averr_offl = 0.;
    ($fill,$nrun) = split(/\./,$run[$i]);
#    print "Found fixd, fill: $fill, targ: $targ[$i], ene: $ene[$i]\n";
    open(SUMFILE,$infile);
    while ($sline = <SUMFILE>) {
	chop($sline);
	@swords = split(/\s+/,$sline);
	$srun = $swords[0];
	($sfill,$snrun) = split(/\./,$srun);
#	printf "$swords[3], $swords[16], $sfill, $swords[5], $swords[17]\n";
	if ($swords[3] eq "Good" && $swords[16] eq $targ[$i] 
	    && $sfill eq $fill && $swords[5] eq $ene[$i]
	    && $swords[17] eq "scan"){
	    $sp_onl[$nscan] = $swords[1];
	    $serr_onl[$nscan] = $swords[2];
	    $sp_offl[$nscan] = $swords[10];
	    $serr_offl[$nscan] = $swords[11];
	    $nscan++;
	}
    }
    close(SUMFILE);
#    print "\# scan $nscan\n";
    if ($nscan > 1) {
	$sum_nonl = 0.;
	$sum_donl = 0.;
	$sum_noffl = 0.;
	$sum_doffl = 0.;
	for ($j=0; $j<$nscan; $j++) {
	    if ($serr_onl[$j]>0. && $serr_offl[$j]>0.) {
		$sum_nonl += $sp_onl[$j]/$serr_onl[$j]**2;
		$sum_donl += 1./$serr_onl[$j]**2;
		$sum_noffl += $sp_offl[$j]/$serr_offl[$j]**2;
		$sum_doffl += 1./$serr_offl[$j]**2;
	    }
	}
	$avp_onl = $sum_nonl / $sum_donl;
	$averr_onl = 1./sqrt($sum_donl);
	$avp_offl = $sum_noffl / $sum_doffl;
	$averr_offl = 1./sqrt($sum_doffl);
    } elsif ($nscan == 1) {
	$avp_onl = $sp_onl[0];
	$averr_onl = $serr_onl[0];
	$avp_offl = $sp_offl[0];
	$averr_offl = $serr_offl[0];
    }
    if ($nscan >= 1) {
#  calculate ratios
	$rat_onl = $p_onl[$i] / $avp_onl;
	$erat_onl = $rat_onl*sqrt(($err_onl[$i]/$p_onl[$i])**2 + ($averr_onl/$avp_onl)**2);
	$rat_offl = $p_offl[$i] / $avp_offl;
	$erat_offl = $rat_offl*sqrt(($err_offl[$i]/$p_offl[$i])**2 + ($averr_offl/$avp_offl)**2);
# warn if out of range
	if ($rat_onl<0.5 || $rat_offl<0.5) {
	    print "** Warning:  Fill $fill, Fixed target/Scan ratio < 0.5\n";
	}elsif ($rat_onl>1.5 || $rat_offl>1.5) {
	    print "** Warning:  Fill $fill, Fixed target/Scan ratio > 1.5\n";
	}
# write output
	printf OUTFILE "%4d %2.2f %2.2f %2.2f %2.2f %3d %3d %2.1f %2.1f %2.1f %2.1f %7.3f %2.1f %2.1f %2.1f %2.1f %2.2f\n", $fill, $rat_onl, $erat_onl, $rat_offl, $erat_offl, $ene[$i], $nscan, $avp_onl, $averr_onl, $avp_offl, $averr_offl, $run[$i], $p_onl[$i], $err_onl[$i], $p_offl[$i], $err_offl[$i], $rate[$i]; 
    }
}

close(OUTFILE);
