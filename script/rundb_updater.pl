#! /usr/bin/perl
# rundb_updater.pl
# Apr 18, 2006 A. Hoffman


$BASEDIR   = $ENV{"ASYMDIR"};
$TMPDIR    = $ENV{"TMPOUTDIR"};
$INF       = "$TMPDIR/dLayerChecker.dat";
$OUTF      = "$BASEDIR/run.db";

open (MYFILE,"$INF");
open (OTHERFILE, ">>$OUTF");

while (<MYFILE>) {
    chomp;
    print OTHERFILE "$_\n";
}

close (MYFILE);
close (OTHERFILE);

