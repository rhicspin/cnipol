#! /usr/bin/perl

# ==================================================================
# Online GUI software for RHIC CNI polarimeter monitor
# Created Run-03
# Modified in Run-04
# ==================================================================
#  final modification is needed for the part #@@@@@

#######################################################################
#  Contents
#    Define default variables
#    Geometry of the pads
#      Main Window
#      Fill list button
#      Fill number
#      Run list button
#      Runnumber
#      Start Process button
#      show result button
#      menu bar for option
#      exit
#      Showing the message with Scroll Bar
#      message bar
#      Scaler Banana button
#      Scaler Left-Right button
#      Polarization vector button
#      See the daq log
#      See the analysis log
#      menu bar for timing shift
#    Sub routines
#      start the analysis 
#      option for beam energy
#      show the result with PAW
#      change the option for plots  
#      show the result with PAW  (scaler banana)
#      show the result with PAW (scaler LR)
#      show the trigger min value
#      show the warning when the file does not exist
#      show the warning and say the file is scaler mode data  
#      show confirmation on the file conversion
#      show the daq log
#      show the analysis log
#      file list box
#      run list box 
#######################################################################

use Tk;
use English;
##$| =1;

# ******************************************
# Define default variables
# ******************************************

$fillnumber="7279";              # default fill number
$runnumber="005";                # default run number 
$opt_bwidth="-15:15";            # banana cut width in ns
$info = "not started";
$ploption = "all";
$feedbackinfo = "Off";
$ENV{"PLOT"} = "all";
$tshift = "0.0";
$energyinfo = "flattop";
$nskip = "1";
$emin = "400";
$emax = "900";
$conffile = "none";

$ONLINEDIR    = $ENV{"ONLINEDIR"}; 
$DATADIR      = $ENV{"DATADIR"}; 
$BASEDIR      = $ENV{"ASYMDIR"};
$MACRODIR     = $ENV{"MACRODIR"}; 
$CONFDIR      = $ENV{"CONFDIR"};

printf "\n";
printf " ONLINEDIR       =   $ONLINEDIR\n";
printf " DATADIR         =   $DATADIR\n";
printf " BASEDIR         =   $BASEDIR\n";
printf " MACRODIR        =   $MACRODIR\n";
printf " CONFDIR         =   $CONFDIR\n";
printf "\n";


#******************************************
# Geometry of the pads
#******************************************

# -------------------
#  Main Window
# -------------------

my $mw=MainWindow->new;
$mw->title("RHIC CNI ANALYSIS ");
$frame=$mw->Frame->pack(-side=>'top',-fill=>'x');
$frame2=$mw->Frame->pack(-side=>'top',-fill=>'x');
$frame3=$mw->Frame->pack(-side=>'top',-fill=>'x');
$frame4=$mw->Frame->pack(-side=>'top',-fill=>'x');

# -------------------
# Fill list button 
# -------------------

$frame->Button(-text => "Fill", -command=>\&filelist)
    ->pack(-side=>'left',-anchor=>'w');

# -------------------
# Fill number
# -------------------

$frame->Label(-text=>"Fill:")
    ->pack(-side=>'left',-anchor=>'w');
$frame->Entry(-justify=>'right',-textvariable=>\$fillnumber,-font=>'r16',
	      -background=>'White',-width=>6)
    ->pack(-side=>'left',-anchor=>'w');

# -------------------
# Run list button
# -------------------

$frame->Button(-text => "Run", -command=>\&runlist)
    ->pack(-side=>'left',-anchor=>'w');

# -------------------
# Runnumber
# -------------------

$frame->Label(-text=>"Run:")
    ->pack(-side=>'left',-anchor=>'w');
$frame->Entry(-justify=>'right',-textvariable=>\$runnumber,-font=>'r16',
	      -background=>'White',-width=>6)
    ->pack(-side=>'left',-anchor=>'w');

# ---------------------
# Start Process button
# ---------------------

$frame->Button(-text => "Start Process", -command=>\&popup_start)
    ->pack(-side=>'left',-anchor=>'w');

# ---------------------
# show result button
# ---------------------

$frame->Button(-text => "Show Result", -command=>\&show_result)
    ->pack(-side=>'left',-anchor=>'w');


# -------------------
# menu bar for option
# -------------------

$menub=$frame->Menubutton(-textvariable=>\$ploption,
                          -indicatoron=>1,
                          -relief=>'raised')
    ->pack(-side=>'left',-anchor=>'w');

foreach (qw/all 
	 plot_banana 
	 mass
	 strips 
	 bunch_dist 
	 energy 
	 accumulation 
	 asymmetries 
	 st_by_st_pol
	 R-L 
	 ratio 
	 timing 
	 different_t 
	 -t_dependence 
	 phi_fit
	 bunchselect
	 first_tdc_bin
	 profile/){
    
    $menub->radiobutton(-label=> $_,
                        -value=> $_,
                        -variable=> \$ploption,
                        -command=>\&pl_optchange);
}


# -------------------
# exit
# -------------------

$frame->Button(-text => "exit", -command=>sub {exit})
    ->pack(-side=>'right',-anchor=>'w');


# --------------------------------------
# Showing the message with Scroll Bar 
# --------------------------------------

my $text = $mw->Scrolled("Text",-background=>'Gray80',
			 -foreground=>'Black',-font=>'r16')
    ->pack(-fill=>'both', -expand=>1);

# -------------------
# message bar
# -------------------

$mw->Label(-textvariable=>\$info, -relief=>'ridge',-font=>'r16')
    ->pack(-side=>'bottom',-fill=>'both');

# ---------------------
# Scaler Banana button
# ---------------------

$mw->Button(-text => "Scaler banana", -command=>\&scaler_banana)
    ->pack(-side=>'left',-anchor=>'w');

# ---------------------------
# Scaler Left-Right button
# ---------------------------

$mw->Button(-text => "Scaler L-R", -command=>\&scaler_lr)
    ->pack(-side=>'left',-anchor=>'w');

# -----------------------------
# Polarization vector button
# -----------------------------

$mw->Button(-text => "P Vector", -command=>\&pvector)
    ->pack(-side=>'left',-anchor=>'w');

# -------------------
# See the online log
# -------------------

$frame2->Button(-text => "Log(online)", -command=>\&onlinelog)
    ->pack(-side=>'left',-anchor=>'w');

# ----------------------
# See the analysis log
# ----------------------
$frame2->Button(-text => "Log(analysis)", -command=>\&analog)
    ->pack(-side=>'left',-anchor=>'w');

# ----------------------------
# menu bar for feedback mode
# ----------------------------
$frame2->Label(-text => "Feedback")
    ->pack(-side=>'left',-anchor=>'w');;

$tmenub=$frame2->Menubutton(-textvariable=>\$feedbackinfo,
                          -indicatoron=>1,
                          -relief=>'raised')
    ->pack(-side=>'left',-anchor=>'w');

foreach (qw/Off On/){

    $tmenub->radiobutton(-label=> $_,
			 -value=> $_,
			 -variable=> \$feedbackinfo,
			 -command=>\&feedbackopt);
}


$frame2->Label(-text=>" TSHIFT: ")
    ->pack(-side=>'left',-anchor=>'w');
$frame2->Entry(-justify=>'right',-textvariable=>\$tshift,-font=>'r16',
	      -background=>'White',-width=>6)
    ->pack(-side=>'left',-anchor=>'w');
$frame2->Label(-text=>" (nsec) ")
    ->pack(-side=>'left',-anchor=>'w');


#-----------------------------------
## N Skip
#-----------------------------------
$frame2->Label(-text=>" Nskip : ")
    ->pack(-side=>'left',-anchor=>'w');
$frame2->Entry(-justify=>'right',-textvariable=>\$nskip,-font=>'r16',
	      -background=>'White',-width=>6)
    ->pack(-side=>'left',-anchor=>'w');
$frame2->Label(-text=>" (default=1)")
    ->pack(-side=>'left',-anchor=>'w');


#$option_b = $mw->Button(-text => "option", -command=>\&option)
#    ->pack(-side=>'left',-anchor=>'w');



#-----------------------------------
# Energy threshold
#-----------------------------------
$frame3->Label(-text=>"Threshold Emin : ")
    ->pack(-side=>'left',-anchor=>'w');
$frame3->Entry(-justify=>'right',-textvariable=>\$emin,-font=>'r16',
	      -background=>'White',-width=>4)
    ->pack(-side=>'left',-anchor=>'w');
$frame3->Label(-text=>" Emax : ")
    ->pack(-side=>'left',-anchor=>'w');
$frame3->Entry(-justify=>'right',-textvariable=>\$emax,-font=>'r16',
	      -background=>'White',-width=>4)
    ->pack(-side=>'left',-anchor=>'w');

#------------------------------------
# Configuration file (overwrite)
#------------------------------------
$frame3-> Label(-text=>" config file: ")
    ->pack(-side=>'left',-anchor=>'w');
$frame3->Entry(-justify=>'right',-textvariable=>\$conffile,-font=>'r16',
	      -background=>'White',-width=>25)
    ->pack(-side=>'left',-anchor=>'w');
$frame3->Button(-text => "Browse", -command=>\&conflist)
    ->pack(-side=>'left',-anchor=>'w');

#-----------------------------------
# see the history plot
#-----------------------------------
$frame4->Button(-text=>"time plot", -command=>\&timeplot)
    ->pack(-side=>'left',-anchor=>'w');

$nt_low = 0;
$nt_up = 0;
$sp_low = 0;
$sp_up = 500;

MainLoop;

#******************************************
# Sub routines
#******************************************


# -------------------------
# start the analysis
# -------------------------

sub startana {
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    $datafile = "$DATADIR/$Runn.data";
    $DATAFILE="$Runn.data";
    $logfile = "$BASEDIR/log/$Runn.log";
    $command = "Asym";

    # Number of events to be skipped 1: all the events
    $NEVOPT = " -n $nskip";

    if (-e $datafile) {
	
	$datsize = (stat($datafile))[7];
	if ($datsize < 650000){
	    # do not proceed just give warning
	    scaler_file();
	} else {
	
	    # decide which ring 
	    #@@@@@@@@

	    if ($runnumber>100) {
		$ring = "y";
	    } else {
		$ring = "b";
	    }


	    if ($conffile eq "none") {
		$option = "";
	    } else { 
		$conffile =`basename $conffile`;
		printf "$confile";
		$option = "-F $conffile"; 
	    }
	    
	    # start program
	    
	    $info = "Analyzing .... data ".$fillnumber.".".$runnumber;
	    
	    system ("xterm -e tcsh -c \"nice +19 $command $NEVOPT -f $DATAFILE $option -e $emin:$emax  -t $tshift $fopt -o $Runn.hbook | tee $logfile\" "); 

	    system ("mv $Runn.hbook hbook/");
	    $info = "Finished Analysis";
	    
	    # SHOW the log file 
	    $text->delete("1.0",'end');
	    open(LOG,$logfile);
	    while (<LOG>){
		$text->insert("end",$_);
	    }
	    close(LOG);
	    $text->yviewMoveto(1.0);
	  }
	
    } else {
	warning_file();
    } 
}

# -------------------------
# option for feedback
# -------------------------

sub feedbackopt {

if ($feedbackinfo eq "On") {
    $fopt=" -b";
}

else {
    $fopt="";
}

}
# -------------------------
# option for beam energy
# -------------------------

sub optenergy {
# Run2004
    $TS_YI = "7.0";
    $TS_YF = "0.0";
    $TS_BI = "7.0";
    $TS_BF = "0.0";
# Run2005
#    $TS_YI = "2.0";
#    $TS_YF = "-5.0";
#    $TS_BI = "3.0";
#    $TS_BF = "-3.0";
# 
    $TS_YI = "0.0";
    $TS_YF = "0.0";
    $TS_BI = "0.0";
    $TS_BF = "0.0";

#    if ($energyinfo eq "injection") {
#	if ($runnumber > 100) {
#	    $tshift = " $TS_YI ";
#	} else {
#	    $tshift = " $TS_BI ";
#	}
#    } elsif ($energyinfo eq "flattop") {
#	if ($runnumber > 100) {
#	    $tshift = " $TS_YF ";
#	} else {
#	    $tshift = " $TS_BF ";
#	}
#    } elsif ($energyinfo eq "other"){
#	$tshift = "-3 ";
#    } else {
#	$tshift = "";
#    }	
    #printf ("$energyinfo $runnumber tshift is = $tshiftopt\n");
}

# -------------------------
# show the result with PAW 
# -------------------------

sub show_result {
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    $ENV{"RUN"} = $Runn;
    system("xterm -e tcsh -c \"paw -w 1 -b $MACRODIR/asymplot.kumac \" &");
    
}


# -----------------------------
# change the option for plots
# -----------------------------

sub pl_optchange {
    if ($ploption eq "all"){
	$ENV{"PLOT"} = "all";
    } elsif ($ploption eq "plot_banana") {
	$ENV{"PLOT"} = "plot_banana";
    } elsif ($ploption eq "mass") {
	$ENV{"PLOT"} = "mass";
    } elsif ($ploption eq "strips") {
	$ENV{"PLOT"} = "strip";
    } elsif ($ploption eq "bunch_dist") {
	$ENV{"PLOT"} = "splumi";
    } elsif ($ploption eq "energy") {
	$ENV{"PLOT"} = "energy";
    } elsif ($ploption eq "accumulation") {
	$ENV{"PLOT"} = "physasym";
    } elsif ($ploption eq "asymmetries") {
	$ENV{"PLOT"} = "asymother";
    } elsif ($ploption eq "st_by_st_pol") {
	$ENV{"PLOT"} = "pol_each";
    } elsif ($ploption eq "R-L") {
	$ENV{"PLOT"} = "RL";
    } elsif ($ploption eq "ratio") {
	$ENV{"PLOT"} = "ratio";
    } elsif ($ploption eq "timing") {
	$ENV{"PLOT"} = "timing";
    } elsif ($ploption eq "different_t") {
	$ENV{"PLOT"} = "diff_t";
    } elsif ($ploption eq "-t_dependence") {
	$ENV{"PLOT"} = "t_dep";
    } elsif ($ploption eq "phi_fit") {
	$ENV{"PLOT"} = "chisq";
    } elsif ($ploption eq "bunchselect") {
	$ENV{"PLOT"} = "bunchselect";
    } elsif ($ploption eq "first_tdc_bin") {
	$ENV{"PLOT"} = "first_tdc_bin";
    } elsif ($ploption eq "profile") {
	$ENV{"PLOT"} = "profile";
    }
}

# ---------------------------------------------
# show the result with PAW  (scaler banana)
# ---------------------------------------------

sub scaler_banana {
    trigemin();
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    $ENV{"RUN"} = $Runn;
    $ENV{"EMIN"} = $TRIGEMIN;
    #printf "Trig Emin = %f keV\n",$TRIGEMIN;
    system("xterm  -e tcsh -c \"nice +20 paw -w 1 -b $MACRODIR/cutq  \"& ");
}

# -----------------------------------------
# show the result with PAW (scaler LR)
# -----------------------------------------

sub scaler_lr {
    $Runn = sprintf



("%04d.%03d",$fillnumber,$runnumber);
    $ENV{"RUN"} = $Runn;
    system("$MACRODIR/lr_spinpat.pl $Runn");
    system("xterm  -e tcsh -c \"nice +20 paw -w 1 -b $MACRODIR/lr_bunch  \" &");
}

# -----------------------------------------
# show the result with PAW (scaler LR)
# -----------------------------------------
sub pvector {
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    $ENV{"RUN"} = $Runn;
    system("$MACRODIR/pvector.pl $Runn");
    system("xterm  -e tcsh -c \"nice +20 paw -w 1 -b $MACRODIR/pvector.kumac  \" &");
}


# -----------------------------------------
#  show the trigger min value
# -----------------------------------------




sub trigemin() {
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    open(CUR,"$ONLINEDIR/log/$Runn.log");
    while ($line = <CUR>){
	chop($line);
	@words = split(/\s+/,$line);
	if ($words[0] eq "Trigger") {
#	    printf "0 $words[0] 1 $words[1] 2 $words[2] 3 $words[3] 4 $words[4] 5 $words[5] 6 $words[6]\n";
	    $TRIGEMIN = $words[5];
	}
    }
    close(CUR);
} 


# -----------------------------------------------------
#  show the warning when the file does not exist
# -----------------------------------------------------

sub warning_file(){
    $w1=$mw->Toplevel();
    $w1->title("WARNING");
    $w1->Label(-text=>"FILE DOES NOT EXISTS")->pack(-side=>'top');
    $w1->Button(-text=>"Close",-command=>sub{$w1->withdraw})->pack;
}

# ---------------------------------------------------------
#  show the warning and say the file is scaler mode data 
# ---------------------------------------------------------

sub scaler_file(){
    $ws=$mw->Toplevel();
    $ws->title("WARNING");
    $ws->Label(-text=>"FILE IS SCLAER MODE DATA")->pack(-side=>'top');
    $ws->Button(-text=>"Close",-command=>sub{$ws->withdraw})->pack;
}

# -------------------------------------------
# show confirmation on the file conversion
# -------------------------------------------

sub popup_start(){
    $wt=$mw->Toplevel();
    $wt->title("WARNING");
    $wt->Label(-text=>"Are you sure? (this will take some time)")
	->pack(-side=>'top');
    $wt->Button(-text=>"Yes",-command=>sub{startana();$wt->withdraw})
	->pack(-side=>'left');
    $wt->Button(-text=>"No",-command=>sub{$wt->withdraw})
	->pack(-side=>'left');
}

# -------------------------------
# show the online log
# -------------------------------

sub onlinelog(){
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    $w3=$mw->Toplevel();
    $w3->title("Online Log");
    $t3=$w3->Scrolled("Text")->pack(-fill=>'both',-expand=>1);
    $w3->Button(-text=>"Close",-command=>sub{$w3->withdraw})->pack();
    open(CUR,"$ONLINEDIR/log/$Runn.log");
    while (<CUR>){
        $t3->insert("end",$_);
    }
    $t3->yview('moveto',1.0);
    close(CUR);
}

# -------------------------------
# show the analysis log
# -------------------------------

sub analog(){
    $Runn = sprintf("%04d.%03d",$fillnumber,$runnumber);
    $w4=$mw->Toplevel();
    $w4->title("Analysis Log");
    $t4=$w4->Scrolled("Text")->pack(-fill=>'both',-expand=>1);
    $w4->Button(-text=>"Close",-command=>sub{$w4->withdraw})->pack();
    open(CUR,"$ONLINEDIR/log/an$Runn.log");
    while (<CUR>){
        $t4->insert("end",$_);
    }
    $t4->yview('moveto',1.0);
    close(CUR);
}


# ------------------------------
## file list box
# ------------------------------
sub filelist(){
    $wlist=$mw->Toplevel();
    $wlist->title("selection of fill");
    $lb = $wlist->Scrolled("Listbox",-scrollbars=>"e",-selectmode=>"single")
	->pack();
    
    $DIR="$DATADIR";
    @flist = <$DIR/[0-9]*.data>;

    $pfill = 0;
    $i=0;
    foreach $fname (@flist) {
# time
# 9: last i-node modification time
#10: last access time 
	$time = (stat($fname))[9];
	($min,$hour,$day,$month)=(localtime($time))[1,2,3,4];
	$btime = sprintf("%d/%d/04 %02d:%02d",$month+1,$day,$hour,$min);
	
#file name 
	$fname =~ s#.+/##;   # take off the directory name
	$_ = $fname;
	s/.data//;
	($fillname,$runname) = split(/\.+/,$_);
	
	if ($fillname ne $pfill){
	    $fill[$i] = sprintf("%d",$fillname);
	    $fillstring[$i] = sprintf(" %d [ %s ]",$fillname,$btime);
	    $starttime{$fillname} = 
		sprintf("%d/%d/04 %02dh%02dm",$month+1,$day,$hour,$min);
	    $i++;
	}
	$pfill = $fillname;
	$etime = $btime;
    }
    
    $lb->insert('end',@fillstring);
    $lb->see('end');
    $wlist->Button(-text=>"Close",-command=>sub{$wlist->withdraw})
	->pack(-side=>'left');
    $wlist->Button(-text=>"Send",
		   -command=>sub{
		       @selection = $lb->curselection();
		       $fillnumber = $fill[$selection[0]]})
	->pack(-side=>'left');
}

# ------------------------------
## run list box
# ------------------------------
sub runlist(){
    $rlist=$mw->Toplevel();
    $rlist->title("selection of run");
    $rlb = $rlist->Scrolled("Listbox",-scrollbars=>"e",-width=>"30",
			    -selectmode=>"single")
	->pack();

    $DIR="$DATADIR";
    #@flist = <$DIR/[0-9]*.data>;
    $cfill = $fillnumber;
    @flist = <$DIR/$cfill*.data>;

    $i=0;
    @run = ();
    @runstring = ();
    foreach $fname (@flist) {
# time
	$time = (stat($fname))[10];
	($min,$hour,$day,$month)=(localtime($time))[1,2,3,4];
	$btime = sprintf("%d/%d/04 %02d:%02d",$month+1,$day,$hour,$min);
	
# data size
	$fsize = (stat($fname))[7];
	if ($fsize < 650000){
	    $dmode = "S";
	} else {
	    $dmode = "E";
	}

#file name 
	$fname =~ s#.+/##;   # take off the directory name
	$_ = $fname;
	s/.data//;
	($fillname,$runname) = split(/\.+/,$_);
	
# Beam Energy
	$Runn = sprintf("%04d.%03d",$fillname,$runname);
	open(DLOG,"$ONLINEDIR/log/$Runn.log");
	while ($line = <DLOG>){
	    chop($line); @words = split(/\s+/,$line);
	    if ($words[6] eq "GeV;") {
		$bene = $words[5];
	    }
	    if ($words[5] eq "GeV;") {
		$_ = $words[4];  s/E=//;
		$bene = $_;
	    }
	}
	
	if ($bene == 0.) {
	    $enestatus = "?";
	} elsif ($bene > 50.) {
	    $enestatus = "Flattop";
	} elsif ($bene < 50.) {
	    $enestatus = "Injection";
	}	    
	close(DLOG);

	if ($fillname eq $cfill){
	    $run[$i] = sprintf("%03d",$runname);
	    $runstring[$i] = sprintf(" %d [ %s ] $dmode $enestatus",
				     $runname,$btime);
	    $i++;
	}
    }
    
    $rlb->insert('end',@runstring);
    $rlist->Button(-text=>"Close",-command=>sub{$rlist->withdraw})
	->pack(-side=>'left');
    $rlist->Button(-text=>"Send",
		   -command=>sub{
		       @selection = $rlb-> curselection();
		       $runnumber=$run[$selection[0]];})
	->pack(-side=>'left');
}

# ------------------------------
## Configuration file list 
# ------------------------------
sub conflist(){
    $clist=$mw->Toplevel();
    $clist->title("configuration");
    $clb = $clist->Scrolled("Listbox",-scrollbars=>"e",-width=>"60",-selectmode=>"single")
	->pack(-fill=>'both', -expand=>1) ;
    
    @cflist = <$CONFDIR/*.dat>;

    $clb->insert('end',@cflist);
    $clb->see('end');
    $clist->Button(-text=>"Close",-command=>sub{$clist->withdraw})
	->pack(-side=>'left');
    $clist->Button(-text=>"Send",
		   -command=>sub{
		       @selection = $clb->curselection();
		       $conffile = $cflist[$selection[0]]})
	->pack(-side=>'left');
}

sub onlineconfig(){

	$Runn = sprintf("%04d.%03d",$fillname,$runname);
	open(DLOG,"$ONLINEDIR/log/$Runn.log");
	while ($line = <DLOG>){
	    chop($line); @words = split(/\s+/,$line);
	    if ($words[3] eq "calibration") {
		$conffile = $words[7];
	    }
	}
      
}




sub timeplot{
    $ENV{"STIME"} = $starttime{$fillnumber};
    system("cd /home/itaru/pol2004-offl/History; ./extract.pl $fillnumber");
    system("cd /home/itaru/pol2004-offl/History; xterm -e tcsh -c \"nice +20 paw -w -1 -b /home/itaru/pol2004-offl/History/history\"& ");

}














