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
$EXE_UN=0;
$MOD_HBOOK=444;
$MOD_LOG=$MOD_HBOOK;


#############################################################################
#                         Option Hundling and help                          #
#############################################################################
use Getopt::Std;
my %opt;
getopts('uhl:f:', \%opt);

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
}

if ($opt{u}) {
    $MOD_HBOOK=644;
    $MOD_LOG=664;
}

sub help() {

    print "\n";
    print "Usage:\n    $0 -hu [-f <runID>][-l <runlist>]\n\n"; 
    print "    Switches the write protection on/off of hbook and log files\n";
    print "    in order to protect them to be overwritten accidentally.\n\n";
    print "\t -f <runid>   protect hbook and log files for run <runid> \n";
    print "\t -l <runlist> prtects all runs in list \n";
    print "\t -u <runid>   unprotect file \n"; 
    print "\t -h           show this menu\n";
    print "\n";
    print "    ex.1) change 7559.106 hbook & log files to be protected mode\n\n";
    print "           protect.pl -f 7669.106\n\n";
    print "    ex.2) change list of runs in <run.list> to be unprotected mode\n\n";
    print "           protect.pl -l run.list -u \n\n";
    print "\n";
    exit(0);
}

#############################################################################
#                                protect()                                  #
#############################################################################
sub protect() {

#    printf("Changing Mode $Runn.hbook -> $MOD_HBOOK, $Runn.log -> $MOD_LOG \n");
    system("chmod $MOD_HBOOK $HBOOKDIR/$Runn.hbook");
    system("ls -lh $HBOOKDIR/$Runn.hbook");
    system("chmod $MOD_LOG   $LOGDIR/$Runn.log");
    system("ls -lh $LOGDIR/$Runn.log");

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
} else {
    protect();
};

