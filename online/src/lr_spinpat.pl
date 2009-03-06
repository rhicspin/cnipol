#! /usr/bin/perl

$run = $ARGV[0];

# $ONLINEDIR=$ENV{"ONLINEDIR"};

# $OUTFILE = ">spinpat.dat";
$OUTFILE = ">/usr/local/polarim/config/log/spinpat.dat";
# $LOGFILE = "$ONLINEDIR/log/an$run.log";
$LOGFILE = "/usr/local/polarim/config/log/an$run.log";

#printf "Processing run $run \n";

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
    
    if ($i == 40) {printf OUTFILE "\n";}
    if ($i == 80) {printf OUTFILE "\n";}

    if ($cspin eq "+") {
	printf OUTFILE "%d ", $i+1;
    } else {
	printf OUTFILE "-10 ";
    }

}
printf OUTFILE "\n";

for ($i=0;$i<120;$i++) {
    $cspin = substr($spintxt, $i, 1); 
    
    if ($i == 40) {printf OUTFILE "\n";}
    if ($i == 80) {printf OUTFILE "\n";}

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

