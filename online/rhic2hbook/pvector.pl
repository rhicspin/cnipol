#! /usr/bin/perl

#
# Jan 6, 2012 - Dmitri Smirnov
#    - Now can save online polarization values to a file
#
# Feb 9, 2012 - Dmitri Smirnov
#    - Moved saving polarization to a separate script online_polar.pl
#

use Env;

$run = $ARGV[0];

# $ONLINEDIR=$ENV{"ONLINEDIR"};

$LOGFILE       = $LOGDIR."/an$run.log";
$OUTFILE       = ">".$LOGDIR."/pvect.dat";

#printf "Processing run $run ($LOGFILE)\n";

open(LOGFILE,       $LOGFILE);
open(OUTFILE,       $OUTFILE);

$SQ2 = 1.4142;

while ($line = <LOGFILE>) {
    chop($line);
    @words = split(/\s+/,$line);

    # X90
    if ($words[1] eq "X90") {
        $X90P = $words[3];
        $X90L = $words[4];
        $X90A = $words[5];
        ($x90p,$x90pe) = split(/\+-+/,$X90P);
        ($x90l,$x90le) = split(/\+-+/,$X90L);
        ($x90a,$x90ae) = split(/\+-+/,$X90A);
        if ($x90p eq "nan") {
            $x90p  = 0.;
            $x90pe = 0.;
        }
    }

    # X45
    if ($words[1] eq "X45") {
        $X45P = $words[3];
        $X45L = $words[4];
        $X45A = $words[5];

        ($x45p,$x45pe) = split(/\+-+/,$X45P);
        ($x45l,$x45le) = split(/\+-+/,$X45L);
        ($x45a,$x45ae) = split(/\+-+/,$X45A);

        $x45p *= $SQ2; $x45pe *= $SQ2;
        $x45l *= $SQ2; $x45le *= $SQ2;
        $x45a *= $SQ2; $x45ae *= $SQ2;
    }

    # Y45
    if ($words[1] eq "Y45") {
        $Y45P = $words[3];
        $Y45L = $words[4];
        $Y45A = $words[5];
        ($y45p,$y45pe) = split(/\+-+/,$Y45P);
        ($y45l,$y45le) = split(/\+-+/,$Y45L);
        ($y45a,$y45ae) = split(/\+-+/,$Y45A);

        $y45p *= $SQ2; $y45pe *= $SQ2;
        $y45l *= $SQ2; $y45le *= $SQ2;
        $y45a *= $SQ2; $y45ae *= $SQ2;
    }

    # CR45
    if ($words[1] eq "CR45") {
        $C45P = $words[3];
        $C45L = $words[4];
        $C45A = $words[5];
        ($c45p,$c45pe) = split(/\+-+/,$C45P);
        ($c45l,$c45le) = split(/\+-+/,$C45L);
        ($c45a,$c45ae) = split(/\+-+/,$C45A);

        $c45p *= $SQ2; $c45pe *= $SQ2;
        $c45l *= $SQ2; $c45le *= $SQ2;
        $c45a *= $SQ2; $c45ae *= $SQ2;

    }

    # 1-4
    if ($words[1] eq "1-4") {
        $C14P = $words[3];
        $C14L = $words[4];
        $C14A = $words[5];
        ($c14p,$c14pe) = split(/\+-+/,$C14P);
        ($c14l,$c14le) = split(/\+-+/,$C14L);
        ($c14a,$c14ae) = split(/\+-+/,$C14A);

        $c14p *= $SQ2; $c14pe *= $SQ2;
        $c14l *= $SQ2; $c14le *= $SQ2;
        $c14a *= $SQ2; $c14ae *= $SQ2;
    }

    # 3-6
    if ($words[1] eq "3-6") {
        $C36P = $words[3];
        $C36L = $words[4];
        $C36A = $words[5];
        ($c36p,$c36pe) = split(/\+-+/,$C36P);
        ($c36l,$c36le) = split(/\+-+/,$C36L);
        ($c36a,$c36ae) = split(/\+-+/,$C36A);

        $c36p *= $SQ2; $c36pe *= $SQ2;
        $c36l *= $SQ2; $c36le *= $SQ2;
        $c36a *= $SQ2; $c36ae *= $SQ2;
    }

    # X least fit
    if (($words[1] eq "X") && ($words[2] eq ":")) {
        $XCHP = $words[3];
        ($xchp,$xchpe) = split(/\+-+/,$XCHP);
    }

    # Y least fit
    if (($words[1] eq "Y")&&($words[2] eq ":")) {
        $YCHP = $words[3];
        ($ychp,$ychpe) = split(/\+-+/,$YCHP);
    }

    # CHISQ OF FIT
    if (($words[1] eq "FCN=")){
        $CHISQ = $words[2];
    }

    # counts
    if (($words[0] eq "Cnt")&&($words[1] eq "+")){
        for ($si=0;$si<6;$si++){
            $PCNT[$si] = $words[$si+2];
        }
    }

    if (($words[0] eq "Cnt")&&($words[1] eq "-")){
        for ($si=0;$si<6;$si++){
            $MCNT[$si] = $words[$si+2];
        }
    }

    # Analyzing power
    if (($words[1] eq "Average")&&($words[2] eq "analyzing")){
        $A_N = $words[7];
        if ($A_N eq "NaN") {
           $A_N = 0.;
        }
    }
}

printf OUTFILE "$x90p $x90pe\n";
printf OUTFILE "$x45p $x45pe\n";
printf OUTFILE "$y45p $y45pe\n";
printf OUTFILE "$c45p $c45pe\n";
printf OUTFILE "$c14p $c14pe\n";
printf OUTFILE "$c36p $c36pe\n";
printf OUTFILE "$xchp $xchpe\n";
printf OUTFILE "$ychp $ychpe\n";
printf OUTFILE "$A_N\n";
printf OUTFILE "$PCNT[0] $PCNT[1] $PCNT[2] $PCNT[3] $PCNT[4] $PCNT[5]\n";
printf OUTFILE "$MCNT[0] $MCNT[1] $MCNT[2] $MCNT[3] $MCNT[4] $MCNT[5]\n";

close(LOGFILE);
close(OUTFILE);

