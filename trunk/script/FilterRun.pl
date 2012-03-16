#!/usr/bin/perl
# FilterRun.pl
# May 23, 2006
# I. Nakagawa (RIKEN)

$ASYMDIR=$ENV{"ASYMDIR"};
$SHAREDIR=$ENV{"SHAREDIR"};
$FILTER_DB="$SHAREDIR/db/PhysicsFill_Run05_100GeV.db";
$DEF_FILTERD_DB="./Filtered.list";

#================================================================================#
#                                Command Line Options                            #
#================================================================================#

use Getopt::Long;

$OFFLINEPOL       = 0;
$DLAYER           = 0;
$UPDATE_LINK      = 0;
$PHYSICS_DATABASE = 0;
$PHENIX_DATABASE  = 0;
$PROFILE_RUN      = 0;
$FILTER_RUN       = 0;
$KEYWD            ="phenix";
$DEFAULT_LINK     = 0;

GetOptions(
	   'update-link' => \$UPDATE_LINK ,
	   'def-link' => \$DEFAULT_LINK ,
	   'phys' => \$PHYSICS_DATABASE ,
	   'phenix' => \$PHENIX_DATABASE ,
	   'profile' => \$PROFILE_RUN ,
	   'dlayer' => \$DLAYER , 
	   'OfflinePol' => \$OFFLINEPOL ,
	   'f:s' => \$FILTER_DB ,
	   'src-list:s' => \$SRC_DB ,
	   'help' => \$HELP
);

if($HELP){
    help();
}


if($PHYSICS_DATABASE) {$KEYWD="phys"; $FILTER_DB="$SHAREDIR/db/PhysicsRun05_100GeV.db";};
if($PHENIX_DATABASE) {$KEYWD="phenix"; $FILTER_DB="$SHAREDIR/db/PhenixPhysicsRun05.db";};
if($PROFILE_RUN) {$KEYWD="profile"; $FILTER_DB="$SHAREDIR/db/ProfileRun05.db";};
if($DEFAULT_LINK) {$KEYWD="all"; $UPDATE_LINK=1;};
if($SRC_DB) { 
    open(FILE,">$DEF_FILTERD_DB") || die ; 
    Filter();
    print "Output: $DEF_FILTERD_DB\n";
}
if($DLAYER){DlayerMode();};
if($OFFLINEPOL){AsymMode();};


#================================================================================#
#                                   help()                                       #
#================================================================================#
sub help(){

    print "\n";
    print " Usage:\n  $0 [-h][--dlayer][ -f <runlist>][--update-link][--def-link]\n";
    print "              [--src-list <srclist>]\n\n"; 
    print "    Filter runs based on the runlist.\n";
    print "    dlayer SRC datafile = summary/dLayer_Blue(Yellow)_FTP(INJ)_all.dat.\n\n";
    print "\t -f <runlist>   base run list file [def]:$FILTER_DB \n";
    print "\t --src-list <srclist>  src run list file to be filtered \n";
    print "\t --dlayer       Process deadlayer dabase\n";
    print "\t --OfflinePol   Process OfflinePol database\n";
    print "\t --phys         Filter for PHYSICS stores \n";
#    print "\t --phenix       Filter for PHENIX physics stores\n";
    print "\t --profile      Filter for profile runs\n";
    print "\t --update-link  Redirect link to new one\n";
    print "\t --def-link     Redirect link to default (all)\n";
    print "\t -h --help  Show this help";
    print "\n\n";
    print "    ex.1) $0 -f runlist.db --dlayer \n\n";
    print "    ex.2) Redirect link to default (all) files:\n";
    print "          $0 --dlayer --def-link \n\n";
    print "    ex.3) Filter <srclist> based on <runlist>. Default output $DEF_FILTERD_DB\n";
    print "          $0 --src-list <srclist> -f <runlist>";
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
#                              AsymMode()                                      #
#================================================================================#
sub AsymMode(){

    @BEAM=("Blue","Yellow");

    foreach $i (@BEAM) {
	    $SRC_DB="$ASYMDIR/summary/OfflinePol\_$i\_all.dat";
	    $DEST_DB="$ASYMDIR/summary/OfflinePol\_$i\_$KEYWD.dat";
	    if (!$DEFAULT_LINK){
		open(FILE, ">$DEST_DB") || die;
		Filter();
	    }
	    if($UPDATE_LINK){
		system("rm -f $ASYMDIR/summary/OfflinePol\_$i.dat");
		system("ln -s $DEST_DB $ASYMDIR/summary/OfflinePol\_$i.dat");

	    };

	};

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

