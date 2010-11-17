<?php

include("utils.php"); 
//include("config.php");

//date_default_timezone_set('GMT');
setlocale(LC_TIME, 'en');
putenv("TZ=America/New_York");

// Run details view
if (isset($_GET['runid']) && !isset($_GET['chanid'])) {

   $gRunId = $_GET['runid'];

   // check for correct and existing  runid
   if (!is_dir($gRunId) || !file_exists("$gRunId/runconfig.php")) {
      print "No results for $gRunId\n";
      exit;
   }

   // Read information about this run
   $rc = array();
   include("$gRunId/runconfig.php");
   if (count($rc) == 0) exit;

   //include("./config_plots.php");
   include_once("PlotHelper.php");

   $dir = "./$gRunId/images/";
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
   if (!is_dir($gRunId) || !file_exists("$gRunId/runconfig.php")) {
      print "No results for $gRunId\n";
      exit;
   }

   // Read information about this run
   $rc = array();
   include("$gRunId/runconfig.php");
   if (count($rc) == 0) exit;

   //include("./config_plots.php");
   include_once("PlotHelper.php");

   $dir = "./$gRunId/images/";
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
$dir = opendir('.');

if (!$dir) exit("ERROR\n");

$rs = array();

while (false !== ($file = readdir($dir))) {

   if (is_dir($file) && file_exists("$file/runconfig.php")) {
      //if (ereg("[0-9]{3,}\.[0-9]{3}", $file) && is_dir($file))

      //echo "$file ".$rs['dirsize']."<br>\n";
      //include("$file/runconfig.php");
      $rs[] = $file;
      //$rs[$gRunId]['NumChannels'] = $rc['data']['NumChannels'];
      //$rs[$gRunId]['dirsize'] = exec( "du -h -s $file" );
   }
}

closedir($dir);

sort($rs);

include("runinfo_index.html");

?>
