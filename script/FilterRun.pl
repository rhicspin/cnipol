#!/usr/bin/perl
# FilterRun.pl
# May 23, 2006
# I. Nakagawa (RIKEN)

$ASYMDIR=$ENV{"ASYMDIR"};
$SHAREDIR=$ENV{"SHAREDIR"};
$FILTER_DB="$SHAREDIR/db/PhenixPhysicsRun05.db";

#================================================================================#
#                                Command Line Options                            #
#================================================================================#

use Getopt::Long;

$DLAYER          = 0;
$UPDATE_LINK     = 0;
$PHENIX_DATABASE = 0;
$KEYWD           ="phenix";
$DEFAULT_LINK    = 0;

GetOptions(
	   'update-link' => \$UPDATE_LINK ,
	   'def' => \$DEFAULT_LINK ,
	   'phenix' => \$PHENIX_DATABASE ,
	   'dlayer' => \$DLAYER , 
	   'help' => \$HELP
);

if($HELP){
    help();
}


if($PHENIX_DATABASE) {$KEYWD="phenix"; $FILTER_DB="$SHAREDIR/db/PhenixPhysicsRun05.db";};
if($DEFAULT_LINK) {$KEYWD="all"; $UPDATE_LINK=1;};
if($DLAYER){DlayerMode();};


#================================================================================#
#                                   help()                                       #
#================================================================================#
sub help(){

    print "\n";
    print " Usage:\n  $0 -h [--dlayer][ -f <runlist>][--update-link][--def]\n\n";
    print "    Filter runs based on the runlist\n\n";
    print "\t -f <runlist>  run list file [def]:$FILTER_DB \n";
    print "\t --dlayer      Process deadlayer dabase\n";
    print "\t --phenix      Filter for PHENIX physics stores\n";
    print "\t --update-link Redirect link to new one\n";
    print "\t --def-link    Redirect link to default (all)\n";
    print "\t -h --help  Show this help";
    print "\n\n";
    print "    ex.1) $0 -f runlist.db --dlayer \n\n";
    print "    ex.2) Redirect link to default (all) files:\n";
    print "          $0 --dlayer --def \n\n";
    print "\n\n";
    exit(0);

}

#================================================================================#
#                              DlayerMode()                                      #
#================================================================================#
sub DlayerMode(){

    @BEAM=("Blue","Yellow");
    @MODE=("FTP","INJ");

    foreach $i (@BEAM) {
	foreach $j (@MODE) {
	    $SRC_DB="$ASYMDIR/summary/dLayer\_$i\_$j\_all.dat";
	    $DEST_DB="$ASYMDIR/summary/dLayer\_$i\_$j\_$KEYWD.dat";
	    if (!$DEFAULT_LINK){
		open(FILE, ">$DEST_DB") || die;
		Filter();
	    }
	    if($UPDATE_LINK){
		system("rm -f $ASYMDIR/summary/dLayer\_$i\_$j.dat");
		system("ln -s $DEST_DB $ASYMDIR/summary/dLayer\_$i\_$j.dat");

	    };

	}

    }

};



#================================================================================#
#                                  Filter()                                      #
#================================================================================#
sub Filter(){

    printf("$SRC_DB\n");
    open(db,"$SRC_DB") || die "Filter(): $!";
    while (<db>) {
	$line=$_;
	chop($_);
	@x=split(/\s+/,$_);
	$RunID = $x[0];
	$FILL  = sprintf("%d",$RunID);
	if (`grep $FILL $FILTER_DB`) { print FILE $line ; };
    }
    
}

