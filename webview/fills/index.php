<?php

include("../rundb/utils.php"); 
include("../rundb/config.php");

setlocale(LC_TIME, 'en');
date_default_timezone_set('America/New_York');


// Run details view
if ( isset($_GET['fillid']) && isset($_GET['rp']) )
{
   $gFillId    = $_GET['fillid'];
   $gFillId    = strip_tags($gFillId);
   $gAnaName   = "run".sprintf("%02d", $_GET['rp']);
   $gAnaName   = strip_tags($gAnaName);
   $gAnaResDir = DATA_DIR."/$gAnaName/";

   // Check for correct and existing fillid
   if (!is_dir($gAnaResDir) || !file_exists("$gAnaResDir/anainfo.php"))
   {
      print "No results found for $gAnaName\n";
      exit;
   }

   if ( ereg("[0-9]{3,}", $gFillId) ) {
      include("fillinfo.html");
   } else {
      print "No results found for $gFillId\n";
   }

   exit;
}

$gMode = isset($_GET['mode']) ? $_GET['mode'] : 1;

include("fills_index.html");

?>
