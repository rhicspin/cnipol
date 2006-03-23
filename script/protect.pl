#! /usr/bin/perl
# protect.pl
# Alan Hoffman (MIT)
# March 23, 2006

# defaults
$BASEDIR=$ENV{"ASYMDIR"};
$HBOOKDIR="$BASEDIR/hbook";
$LOGDIR="$BASEDIR/log";
$RUNLIST="analyzed_run.list";
$EXE_SCAN=0;


#############################################################################
#                         Option Hundling and help                          #
#############################################################################
use Getopt::Std;
my %opt;
getopts('hl:f:u:', \%opt);

if ($opt{h}) {
    help();
}

if ($opt{l}) {
    $RUNLIST=$opt{l};
    unless (open(instream,$RUNLIST)) {
	die "Error: $RUNLIST doesn't exist. \n";
    }
    $EXE_SCAN=1;
}


if ($opt{f}) {
    $Runn = $opt{f};
    if (length ($Runn) == 0)  {
	print "Error: Specify <runID>.\n";
	help();
    }
    protect();
}

if ($opt{u}) {
    $Runn = $opt{u};
    if (length ($Runn) == 0)  {
	print "Error: Specify <runID>.\n";
	help();
    }
    unprotect();
}

sub help() {

    print "\n";
    print "Usage:\n";
    print "             \n\n";
    print "\t -f <runid>   protect hbook and log files for run <runid> \n";
    print "\t -l <runlist> prtects all runs in list \n";
    print "\t -h    show this menu\n";
    print "\t -u <runid>   unprotect file \n"; 
    print "\n";
    exit(0);
}

#############################################################################
#                                protect()                                  #
#############################################################################
sub protect() {

    printf("Protecting hbook & log : $Runn \n");
    system("chmod 444 $HBOOKDIR/$Runn.hbook");
    system("chmod 444 $LOGDIR/$Runn.log");

}

#############################################################################
#                               unprotect()                                 #
#############################################################################
sub unprotect() {
    printf("Unprotecting hbook & log: $Runn \n");
    system("chmod 644 $HBOOKDIR/$Runn.hbook");
    system("chmod 664 $LOGDIR/$Runn.log");
}

#############################################################################
#                                  scan()                                   #
#############################################################################
sub scan(){
                                                                           
    while (defined($_=<instream>)) {  # readin line to $_ from instream
	chomp($_);                    # strip off charrige return from $_
	$Runn=$_;        
	protect();
    }

}


#############################################################################
#                                  main                                     #
#############################################################################

if ( $EXE_SCAN ) {
    scan();
};
