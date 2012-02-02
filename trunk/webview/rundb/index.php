<?php

include("utils.php"); 
include("config.php");

//date_default_timezone_set('GMT');
setlocale(LC_TIME, 'en');
putenv("TZ=America/New_York");


if ( isset($_GET['sfx']) && !empty($_GET['sfx']) ) {
   $gSfx    = $_GET['sfx'];
   $gSuffix = "_{$_GET['sfx']}";
} else {
   $gSfx    = "";
   $gSuffix = "";
}


// Run details view
if (isset($_GET['runid']) && !isset($_GET['chanid']))
{
   $gRunId  = $_GET['runid'];
   $gRunDir = DATA_DIR."/$gRunId";

   // check for correct and existing  runid
   if (!is_dir($gRunDir) || !file_exists("$gRunDir/runconfig$gSuffix.php"))
   {
      print "No results for $gRunDir\n";
      exit;
   }

   // Read information about this run from the config file
   $rc = array();

   //include("$gRunDir/runconfig$gSuffix.php") or die;
   include("$gRunDir/runconfig$gSuffix.php");

   if (count($rc) == 0) {
      print "No config file found for $gRunDir\n";
      exit;
   }

   // Check for all runconfig files
   if ($handle = opendir($gRunDir)) {
       //echo "Directory handle: $handle\n";
       //echo "Files:\n";
   
       /* This is the correct way to loop over the directory. */
       while (false !== ($file = readdir($handle))) {
           //echo "$file\n";
           if (preg_match("/^runconfig(_?)(.*)\.php$/", $file, $matches)) $rc['suffixes'][] = $matches[2];
       }

       //print_r($rc['suffixes']);
   
       closedir($handle);
   } else {
      print "Could not find dir $gRunDir\n";
      exit;
   }

   //include("./config_plots.php");
   include_once("PlotHelper.php");

   // Set default directory for run images
   $dir = "../runs/$gRunId/images$gSuffix/";
   $gP = new PlotHelper($dir);

   if (ereg("[0-9]{3,}\.[0-9]{3}", $gRunId) && $rc['measurement_type'] != 1) {
      include("runinfo.html");
   } elseif ($rc['measurement_type'] == 1) {
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

   $dir = "../runs/$gRunId/images$gSuffix/";
   $gP = new PlotHelper($dir);

   if (ereg("[0-9]{3,}\.[0-9]{3}", $gRunId) && $rc['measurement_type'] != 1) {
      include("chaninfo.html");
   } elseif ($rc['measurement_type'] == 1) {
      include("chaninfo_calib.html");
   } else {
      print "Problem with $gRunId\n";
   }

   exit;
}


//Form default output
//$dir = opendir(DATA_DIR);

//if (!$dir) exit("ERROR\n");

//$rs = array();

//print "<pre>\n";

//while (false !== ($file = readdir($dir))) {
//	//print "$file\n";
//
//   if (is_dir(DATA_DIR."/$file") && file_exists(DATA_DIR."/$file/runconfig.php")) {
//      //if (ereg("[0-9]{3,}\.[0-9]{3}", $file) && is_dir($file))
//
//      //echo "$file ".$rs['dirsize']."<br>\n";
//      //include("$file/runconfig.php");
//      $rs[] = $file;
//	   //print "ok: $file\n";
//      //$rs[$gRunId]['NumChannels'] = $rc['data']['NumChannels'];
//      //$rs[$gRunId]['dirsize'] = exec( "du -h -s $file" );
//   }
//}

//print "</pre>\n";

//closedir($dir);

//sort($rs);

//print "<pre>\n";
//print_r($rs);
//print "</pre>\n";

include("runinfo_index.html");

?>
