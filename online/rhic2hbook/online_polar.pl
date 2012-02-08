#! /usr/bin/perl

# Jan 6, 2012 - Dmitri Smirnov
#    - Now can save online polarization values to a file
#

use Env;

$run = $ARGV[0];

# $ONLINEDIR=$ENV{"ONLINEDIR"};

$LOGFILE       = $LOGDIR."/an$run.log";
$OUTFILE_POLAR = ">>".$LOGDIR."/online_polar.dat";

#printf "Processing run $run ($LOGFILE)\n";

open(LOGFILE,       $LOGFILE);
open(OUTFILE_POLAR, $OUTFILE_POLAR);


while ($line = <LOGFILE>) {
    chop($line);
    @words = split(/\s+/,$line);

    ## Run 
    #if (($words[1] eq "Run") && ($words[2] eq "Number")){
    #    $sRunId = $words[3];

    #    if ($sPolar eq "") {
    #       $sRunId = 0.;
    #    }
    #}

    # Polarization as calculated online
    if (($words[1] eq "Polarization") && ($words[2] eq ":")){
        $sPolar = $words[3];

        ($polar,$polarErr) = split(/\+-+/,$sPolar);

        if ($sPolar eq "NaN") {
           $polar    = 0.;
           $polarErr = 0.;
        }
    }
}

#printf OUTFILE_POLAR "$sRunId, $polar, $polarErr\n";
printf OUTFILE_POLAR "$run, $polar, $polarErr\n";

close(LOGFILE);
close(OUTFILE_POLAR);
