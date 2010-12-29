<?php

include("utils.php"); 
include("config.php");

//date_default_timezone_set('GMT');
setlocale(LC_TIME, 'en');
putenv("TZ=America/New_York");

// Run details view
if (isset($_GET['runid']) && !isset($_GET['chanid'])) {

   $gRunId = $_GET['runid'];

   // check for correct and existing  runid
   if (!is_dir(DATA_DIR."/$gRunId") || !file_exists(DATA_DIR."/$gRunId/runconfig.php")) {
      print "No results for ".DATA_DIR."/$gRunId\n";
      exit;
   }

   // Read information about this run
   $rc = array();
   include(DATA_DIR."/$gRunId/runconfig.php");
   if (count($rc) == 0) exit;

   //include("./config_plots.php");
   include_once("PlotHelper.php");

   $dir = "/runs/$gRunId/images/";
   $gP = new PlotHelper($dir);

   if (ereg("[0-9]{3,}\.[0-9]{3}", $gRunId) && $rc['CMODE'] == 0) {
      include("runinfo.html");
   } elseif ($rc['CMODE'] == 1) {
      include("runinfo_calib.html");
   } else {
      print "Problem with $gRunId\n";
   }

   exit;
}


// Channel details view
if (isset($_GET['runid']) && isset($_GET['chanid'])) {

   $gRunId  = $_GET['runid'];
   $gChanId = $_GET['chanid'];

   // check for correct and existing  runid
   if (!is_dir(DATA_DIR."/$gRunId") || !file_exists(DATA_DIR."/$gRunId/runconfig.php")) {
      print "No results for $gRunId\n";
      exit;
   }

   // Read information about this run
   $rc = array();
   include(DATA_DIR."/$gRunId/runconfig.php");
   if (count($rc) == 0) exit;

   //include("./config_plots.php");
   include_once("PlotHelper.php");

   $dir = "/runs/$gRunId/images/";
   $gP = new PlotHelper($dir);

   if (ereg("[0-9]{3,}\.[0-9]{3}", $gRunId) && $rc['CMODE'] == 0) {
      include("chaninfo.html");
   } elseif ($rc['CMODE'] == 1) {
      include("chaninfo_calib.html");
   } else {
      print "Problem with $gRunId\n";
   }

   exit;
}


//Form default output
$dir = opendir(DATA_DIR);

if (!$dir) exit("ERROR\n");

$rs = array();

print "<pre>\n";

while (false !== ($file = readdir($dir))) {
	//print "$file\n";

   if (is_dir(DATA_DIR."/$file") && file_exists(DATA_DIR."/$file/runconfig.php")) {
      //if (ereg("[0-9]{3,}\.[0-9]{3}", $file) && is_dir($file))

      //echo "$file ".$rs['dirsize']."<br>\n";
      //include("$file/runconfig.php");
      $rs[] = $file;
	   //print "ok: $file\n";
      //$rs[$gRunId]['NumChannels'] = $rc['data']['NumChannels'];
      //$rs[$gRunId]['dirsize'] = exec( "du -h -s $file" );
   }
}

print "</pre>\n";

closedir($dir);

sort($rs);

//print "<pre>\n";
//print_r($rs);
//print "</pre>\n";

include("runinfo_index.html");

?>
