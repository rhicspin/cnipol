#! /usr/bin/perl

use Tk;
use English;
use Tk::FileSelect;

$| =1;

# $DEBUG=0 for running, =1 for testing
$DEBUG = 0;

# Specify "d" or "debug" on command line for debug mode...
$_ = substr($ARGV[0],0,1);
if ($_ eq "-") {        # in case someone uses "-d(D)"
  $_ = substr($ARGV[0],1,1);
}
s/d/D/i;
if ($_ eq "D") {
  $DEBUG = 1;
}

$INIDIR = "/usr/local/polarim/config";
$HBOOKDIR = "$INIDIR/hbook";
$DATADIR = "$INIDIR/data";
#$DATADIR = "$INIDIR/dataa";
$LOGDIR = "$INIDIR/log";

$ENV{RUN} = "99999.999";

# Initialize some defaults...
$runV = 1;      # enable run messages
$mlevel = 600;  # message level
$runtime = 10;  # run time
$wait4read = 4; # secs to wait for readout to finish
$runname = "test.100.data";
$runBY = "yellow";
$runtype = "calibr";
$attenuation = "x1/5";
$runP = 1;
$runG = 1;
$runC = 1;

#----------------------------------------
# Main Window 
#----------------------------------------
my $mw=MainWindow->new;
$mw->title("pC DAQ Testing Interface");

$frame1=$mw->Frame()
  ->pack(-side=>'top',-fill=>'x');
$frame2=$mw->Frame()
  ->pack(-side=>'top',-fill=>'x');
$frame3=$mw->Frame()
  ->pack(-side=>'top',-fill=>'x');
  $frame3L=$frame3->Frame()->pack(-side=>'left',-fill=>'x');
  $frame3R=$frame3->Frame()->pack(-side=>'left',-fill=>'x');
$frame4=$mw->Frame()
  ->pack(-side=>'top',-fill=>'x');
$frame5=$mw->Frame()                # -relief=>'sunken',-borderwidth=>1
  ->pack(-side=>'top',-fill=>'x');
  $frame5L=$frame5->Frame()->pack(-side=>'left',-fill=>'x');

#----------------------------------------
# Run information
#----------------------------------------
$frame1
  ->Label(-text=>"Run Name:")
  ->pack(-side=>'left',-padx=>2,-anchor=>'w');

$frame1
  ->Entry(-justify=>'right',-textvariable=>\$runname,-font=>'r16',
    -width=>20)
  ->pack(-side=>'left',-padx=>2,-anchor=>'w');

$frame1
  ->Label(-text=>" Run Time:")
  ->pack(-side=>'left',-padx=>1,-anchor=>'w');

$frame1
  ->Entry(-justify=>'right',-textvariable=>\$runtime,-font=>'r16',
    -width=>7)
  ->pack(-side=>'left',-padx=>1,-anchor=>'w');

# Time unit option menu...
$frame1 -> Optionmenu(
  -options => ["SEC", "MIN", "HRS"],
  -variable =>\$tvar,
  -command => \&tunit )
  ->pack(-side=>'left',-padx=>2,-anchor=>'w');

#----------------------------------------
# START BUTTON
#----------------------------------------
$start_b = $frame1 ->Button(-text => "Start Run", 
  -activebackground=>'gray60',
  -command=>\&p_start)
  ->pack(-side=>'left',-anchor=>'w');

$start_b->configure(-foreground=>'ForestGreen');

#----------------------------------------
# STOP BUTTON
#----------------------------------------
$stop_b = $frame1->Button(-text => "Stop Run", 
  -activebackground=>'gray60',
  -command=>\&p_stop)
  ->pack(-side=>'left',-anchor=>'w');

$stop_b->configure(-state=>"disabled");

#----------------------------------------
# EXIT BUTTON
#----------------------------------------
$frame1->Button(-text => "exit", 
  -activebackground=>'gray60',
  -command=>sub {exit})
  ->pack(-side=>'right',-anchor=>'w');

#----------------------------------------
# Comments entry field
#----------------------------------------
$frame2
  ->Label(-text=>"Comment:  ")
  ->pack(-side=>'left',-pady=>5,-anchor=>'w');

$frame2
  ->Entry(-textvariable=>\$comment,-font=>'r16',-width=>60)
  ->pack(-side=>'left',-anchor=>'w', -expand=>1);
  
#----------------------------------------
# Analyze Button
#----------------------------------------
$analyze_b = $frame2->Button(-text => "Analyze",
  -command=>\&analyze)
  ->pack(-side=>'left',-anchor=>'w');

#----------------------------------------
# Run setup options
#----------------------------------------
$frame3L
  ->Label(-text=>"   Run Type: ")
  ->pack(-side=>'left',-padx=>2);
$frame3L->Radiobutton(-variable=>\$runBY,-text=>'Blue pC  ',
  -value=>'blue',-command=>\&rselect)
  ->pack(-side=>'left',-anchor=>'w');
$frame3L->Radiobutton(-variable=>\$runBY,-text=>'Yellow pC    ',
  -value=>'yellow',-command=>\&rselect)
  ->pack(-side=>'left',-anchor=>'w');

$frame3R->Radiobutton(-variable=>\$runtype,-text=>'Calibration  ',
  -value=>'calibr',-command=>\&rselect)
  ->pack(-side=>'left',-anchor=>'w');
$frame3R->Radiobutton(-variable=>\$runtype,-text=>'Pulser (Test Mode)',
  -value=>'pulser',-command=>\&rselect)
  ->pack(-side=>'left',-anchor=>'w');

$frame4
  ->Label(-text=>"Attenuation: ")
  ->pack(-side=>'left',-padx=>2,-pady=>2);
$frame4->Radiobutton(-variable=>\$attenuation,-text=>'x1 ',
  -value=>'x1')
  ->pack(-side=>'left',-anchor=>'w');
$frame4->Radiobutton(-variable=>\$attenuation,-text=>'x1/3 ',
  -value=>'x1/3')
  ->pack(-side=>'left',-anchor=>'w');
$frame4->Radiobutton(-variable=>\$attenuation,-text=>'x1/5 ',
  -value=>'x1/5')
  ->pack(-side=>'left',-anchor=>'w');
$frame4->Radiobutton(-variable=>\$attenuation,-text=>'x1/10',
  -value=>'x1/10')
  ->pack(-side=>'left',-anchor=>'w');

#----------------------------------------
# Frame for run option switches
#----------------------------------------
$frame5L->Label(-text=>"Run options:")->grid
  ($frame5->Checkbutton(-variable=>\$runP,-text=>'Program (-P)'),
   $frame5->Checkbutton(-variable=>\$runV,-text=>'Error Messages (-v)'),
   $frame5->Entry(-textvariable=>\$mlevel,-justify=>'right',-font=>r16,-width=>4),
   -sticky=>'w');

$frame5L->Label(-text=>" ")->grid
  ($frame5->Checkbutton(-variable=>\$runC,-text=>'Internal Clock (-C)'),
   $frame5->Checkbutton(-variable=>\$runG,-text=>'No Target Data (-g)'),
   "x",-sticky=>'w');

#----------------------------------------
# Frame for run information text
#----------------------------------------
my $text = $mw->Scrolled("Text",-width=>80,-height=>20,
			 -font=>'r16')->pack(-fill=>'both', -expand=>1);

#----------------------------------------
# Frame for status information
#----------------------------------------
$mw->Label(-textvariable=>\$info, -relief=>'ridge')
	   ->pack(-side=>'bottom',-fill=>'x');

$comment = "Enter comments here";
$running = 0;
$info = "Not started.";

# Show warning if DEBUG mode is on...
if ($DEBUG != 0) {
  $text->insert('end', " **WARNING: DEBUG mode is on.**\n");
  $mw->idletasks;
}

MainLoop;

sub tunit {
  $timeunits = 1;
  if ($tvar eq "SEC") {$timeunits = 1;}
  if ($tvar eq "MIN") {$timeunits = 60;}
  if ($tvar eq "HRS") {$timeunits = 3600;}
}

# Start the run...
sub p_start {
  $info = "Starting run...";
# First, get the run parameters and switches...
  $thisruntime = $runtime * $timeunits;
  $t = "-t".$thisruntime;
  $P = "";
  $g = "";
  $C = "";
  $v = "";
  if ($runP) {$P = "-P";}
  if ($runC) {$C = "-C";}
  if ($runG) {$g = "-g";}
  if ($runV) {$v = "-v".$mlevel;}

  $outreg = 0x00;
  if ($runtype eq "pulser") {$outreg = 0x20;}
  if ($attenuation eq "x1/10") {$outreg += 0x00;}
  if ($attenuation eq "x1/5") {$outreg += 0x40;}
  if ($attenuation eq "x1/3") {$outreg += 0x80;}
  if ($attenuation eq "x1") {$outreg += 0xC0;}

# Need to make the ini file...
  if ($runBY eq "blue") {
    $ininame = ".blue_ini";
  }
  elsif ($runBY eq "yellow") {
    $ininame = ".yellow_ini";
  }
  else {die "There was a problem with the Blue or Yellow Ring selection.";}

  if (open(INITMPL,"$INIDIR/".$ininame."_template")) {
    if (open(INIF,">$INIDIR/$ininame")) {
      printf (INIF "OutReg=0x%02x\n",$outreg);
      while ($mline = <INITMPL>) {
        printf INIF $mline;
      }
    }
    else {die "Could not open $ininame for writing.";}
  }
  else {die "Could not open ".$ininame."_template.";}

  close(INITMPL);
  close(INIF);

# Get the MUX setting...
#
  if ($runBY eq "blue") {
   $mux = `./getCdevValue polarMux.RHIC.blu polNumM`;
  }
  elsif ($runBY eq "yellow") {
   $mux = `./getCdevValue polarMux.RHIC.yel polNumM`;
  }
  $text->insert('end',"MUX setting = $mux");
  $mw->idletasks;
  $ENV{MUX} = $mux;

# Now parse the file name...
#
  parse_file_name();

  if (-e $logfile) {system "rm -f $logfile";}

  $stop_b->configure(-state=>"normal");
  $stop_b->configure(-foreground=>"Red");
  $start_b->configure(-state=>"disabled");

  if ($DEBUG == 0) {
    $text->insert('end',"./rhicpol -e0 $P $C $g $v $t -l$logfile -i$INIDIR/$ininame -f$dataname.data -c\"$comment\"\n");
    $mw->idletasks;
    system "./rhicpol -e0 $P $C $g $v $t -i$INIDIR/$ininame -l$LOGDIR/$aname.log -f$dataname.data -c\"$comment\" &";
    sleep 5;
    $pid_rpol = substr(`ps ax | grep rhicpol | grep $dataname`,0,5);
    $pid_tail = open(MESS,"tail -n1000 -f -q $logfile |");
  }
  else {
    $text->insert('end',"**DEBUG: ./rhicpol -e0 $P $C $g $v $t -l$logfile -i$INIDIR/$ininame -f$dataname.data -c\"$comment\"\n");
    $mw->idletasks;
    system "./cnidebug.pl $t > .cnimessage &";
    $pid_tail = open(MESS,"tail -f -q .cnimessage |");
  }

# Set up message file callback...
  $mw->fileevent(MESS,'readable',[\&insert_text]);
  $mw->idletasks;

  $starttimeI = time();
  $starttime = localtime($starttimeI);
  $running = 1;

  TimeInfoMessage();
}

sub p_stop {
# If Stop button is clicked and we're still running...
  if ($running != 0) {
    if ($DEBUG == 0) {
      if (`ps -p $pid_rpol --noheading` != 0) {
        system "kill -s INT $pid_rpol";
      }
    }
    else {
      $dbgid = substr(`ps ax | grep cnidebug`,0,5);
      system "kill -9 $dbgid";
    }
  }
# But always do this...
# The order here is IMPORTANT, and necessary.
  $mw->fileevent(MESS,'readable',"");   # First, cancel the callback
  system "kill -s INT $pid_tail";       # Second, kill the tail process
  close(MESS);                          # Third, close the "MESS" file

  $tdiff = time() - $starttimeI - $wait4read;
  $info = "Stopped...  Run Time was ".$tdiff." sec.";
  $start_b->configure(-state=>"normal");
  $start_b->configure(-foreground=>'ForestGreen');
  $stop_b->configure(-state=>"disabled");
  $running = 0;
  
  $mw->idletasks;
}

sub analyze {
# Analyze button was clicked. Get the file name ($dataname) and send to 
# analysis program. Assumes this is a calibration or pulser run and will 
# analyze as if Am-241 sources was used.

  parse_file_name();

  system ("nice -19 asymrhic -n 1 -f $dataname.data -C -o $aname.calib.hbook | tee $logfile.calib ");
  system ("h2root $aname.calib.hbook ");
  system ("rm $aname.calib.hbook");
  system ("mv $aname.calib.root $HBOOKDIR");
  
  system("echo '.x exe_calib.C(\"$hbfile\")' > input.C");
  system("root -b -l < input.C | tee $logfile.fit");
  system("rm input.C");

  system("mv testfit.dat $hbfile.temp.dat");
  system("mv testfit.ps  $hbfile.fittemp.ps");
  if (-e testsummary.ps) {system("mv testsummary.ps $hbfile.summarytemp.ps");}
  system ("gv -landscape $hbfile.fittemp.ps &"); 
}

sub rselect {
  if ($runtype eq "calibr") {
    $attenuation = "x1/5";
    $runP = 1;
    $runG = 1;
    $runC = 1;
  }

  if ($runtype eq "pulser") {
    $attenuation = "x1";
    $runP = 1;
    $runG = 1;
    $runC = 1;
  }
}

sub TimeInfoMessage() {
  if ($running == 1) {
	$tdiff = time() - $starttimeI;
	$info = "Running .... data ".$runname."      Started : ".$starttime."   (".$tdiff." sec completed so far)";
    $mw->idletasks;

#    if ($DEBUG != 0) {
      if ($tdiff >= $thisruntime+$wait4read) {
        $text->insert('end',"Stopped due to timeout.");
        p_stop();
      }
#    }
    $mw->after(1000, \&TimeInfoMessage);
  }
}

sub insert_text {
  my $curline;
  if ($curline = <MESS>) {
	$text->insert('end',$curline);
	$text->yview('moveto',1.0);
    $mw->idletasks;
	if (substr($curline,0,4) eq ">>> ") {
	  $running = 0;
	  p_stop();
	}
  }
}

sub parse_file_name() {
# Parse the file name. Convert to lower case lc()...
# Strip off .data at the end, if necessary...
  $runname = lc($runname);
  $aname = $runname;
  $strl = length($aname);
  $endstr = substr($aname,$strl-5,5);
  if ($endstr eq ".data") {
    $aname = substr($aname,0,$strl-5);
  }

# Was a directory specified?
  @name = split(/\//,$aname);
  $n = 0;
  foreach $part (@name) {
    $n++;
  }

# No directory was specified, use default...  
  if ($n == 1) {
    $dataname = "$DATADIR/$aname";
  }
  else {
    $dataname = $aname;
  }
  $hbfile = "$HBOOKDIR/$aname.calib";
  $logfile = "$LOGDIR/$aname.log";
}
