#! /usr/bin/perl

$Runn = $ARGV[0];

printf("CALIB DATA : $Runn \n");
#printf("Are these correct? (if yes type Y/y, if no CTRL-C )\n");

#$ANSWER = <STDIN>; chop($ANSWER);

#if ($ANSWER eq 'Y' || $ANSWER eq 'y') {
    
    system("echo '.x exe_calib.C(\"$Runn\")' > input.C");
    system("root -b < input.C | tee Calib/$Runn.fit.log");
    
    system("mv testfit.dat Calib/$Runn.temp.dat");
    system("mv testfit.ps  Calib/$Runn.fittemp.ps");
    system("mv testsummary.ps Calib/$Runn.summarytemp.ps");
    system("rm input.C");
#}

#printf ("$ANSWER\n");
