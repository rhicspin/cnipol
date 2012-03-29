<?php

include("config.php");
include("utils.php"); 

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

   // Check for correct and existing runid
   if (!is_dir($gRunDir) || !file_exists("$gRunDir/runconfig$gSuffix.php"))
   {
      print "No results found for $gRunId<br>\n";
      //print "Please wait for the data to be analyzed\n";
      exit;
   }

   // Read information about this run from the config file
   $rc = array();
   include("$gRunDir/runconfig$gSuffix.php");
   if (count($rc) == 0) {
      print "No config file found for $gRunId\n";
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

   include_once("PlotHelper.php");

   // Set default directory for run images
   $dir = "../runs/$gRunId/images$gSuffix/";
   $gP = new PlotHelper($dir);

   if (ereg("[0-9]{3,}\.[0-9]{3}", $gRunId) && $rc['measurement_type'] != 1) {

      // Check if raw data display requested      
      if (isset($_GET['raw'])) {
         include("runinfo_raw.html");
      } else
         include("runinfo.html");

   } elseif ($rc['measurement_type'] == 1) {
      include("runinfo_calib.html");
   } else {
      //print "Problem displaying info for $gRunId\n";
      // XXX this is a temporary fix for notgt data files
      //include("runinfo.html"); 
      if (isset($_GET['raw'])) {
         include("runinfo_raw.html");
      } else
         include("runinfo.html");
   }

   exit;
}


// Channel details view
if (isset($_GET['runid']) && isset($_GET['chanid'])) {

   $gRunId  = $_GET['runid'];
   $gChanId = $_GET['chanid'];
   $gRunDir = DATA_DIR."/$gRunId";

   // Check for correct and existing runid
   if (!is_dir($gRunDir) || !file_exists("$gRunDir/runconfig$gSuffix.php"))
   {
      print "No results found for $gRunId\n";
      exit;
   }

   // Check for correct and existing  runid
   if (!is_dir(DATA_DIR."/$gRunId") || !file_exists(DATA_DIR."/$gRunId/runconfig.php")) {
      print "No results found for $gRunId\n";
      exit;
   }

   // Read information about this run
   $rc = array();
   include("$gRunDir/runconfig$gSuffix.php");
   if (count($rc) == 0) {
      print "No config file found for $gRunDir\n";
      exit;
   }

   include_once("PlotHelper.php");

   $dir = "../runs/$gRunId/images$gSuffix/";
   $gP = new PlotHelper($dir);

   //if (ereg("[0-9]{3,}\.[0-9]{3}", $gRunId) && $rc['measurement_type'] != 1)
   if ($rc['measurement_type'] != 1)
   {
      include("chaninfo.html");
   } elseif ($rc['measurement_type'] == 1) {
      include("chaninfo_calib.html");
   } else {
      print "Problem with $gRunId\n";
   }

   exit;
}

include("runinfo_index.html");

?>
