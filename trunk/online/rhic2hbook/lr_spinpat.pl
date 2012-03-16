#! /usr/bin/perl

use Env;

$run = $ARGV[0];

# $ONLINEDIR=$ENV{"ONLINEDIR"};

$OUTFILE = ">".$LOGDIR."/spinpat.dat";
$LOGFILE = $LOGDIR."/an$run.log";

#printf "Processing run $run \n";
#printf "Output goes to $OUTFILE \n";

open(OUTFILE,$OUTFILE);
open(LOGFILE,$LOGFILE);

$SQ2 = 1.4142;
while ($line = <LOGFILE>) {
    chop($line);
    @words = split(/\s+/,$line);
# Polarization pattern
    if ($words[0] eq "Pol." && $words[1] eq "pattern:") {

        $spintxt = $words[2];
    }

}

for ($i=0;$i<120;$i++) {
    $cspin = substr($spintxt, $i, 1);
#       SvirLex: to patch our pawX11 ...
    if ($i == 0)  {printf OUTFILE   "vec/cre BIDP1(40) R ";}
    if ($i == 40) {printf OUTFILE "\nvec/cre BIDP2(40) R ";}
    if ($i == 80) {printf OUTFILE "\nvec/cre BIDP3(40) R ";}

    if ($cspin eq "+") {
        printf OUTFILE "%d ", $i+1;
    } else {
        printf OUTFILE "-10 ";
    }

}
printf OUTFILE "\n";

for ($i=0;$i<120;$i++) {
    $cspin = substr($spintxt, $i, 1);
#       SvirLex: to patch our pawX11 ...
    if ($i == 0)  {printf OUTFILE   "vec/cre BIDM1(40) R ";}
    if ($i == 40) {printf OUTFILE "\nvec/cre BIDM2(40) R ";}
    if ($i == 80) {printf OUTFILE "\nvec/cre BIDM3(40) R ";}

    if ($cspin eq "-") {
        printf OUTFILE "%d ", $i+1;
    } else {
        printf OUTFILE "-10 ";
    }

}
printf OUTFILE "\n";
#printf "spin pattern : $spintxt\n";


close(OUTFILE);
close(LOGFILE);

