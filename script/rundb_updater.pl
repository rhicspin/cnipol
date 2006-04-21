#! /usr/bin/perl
# rundb_updater.pl
# Apr 18, 2006 A. Hoffman


$BASEDIR   = $ENV{"ASYMDIR"};
$INF       = "$BASEDIR/out.dat";
$OUTF      = "$BASEDIR/test.dat";


#print "hello world \n";
#print "$INF \n";
#print "$OUTF \n";

open (MYFILE,"$INF");
open (OTHERFILE, ">>$OUTF");

while (<MYFILE>) {
#    if (/7\d\d\d.\d\d\d/) {
#	$_ s/(]@)/ /;
#	print;
 #   }
    chomp;
    print OTHERFILE "$_\n";
}

close (MYFILE);
close (OTHERFILE);

