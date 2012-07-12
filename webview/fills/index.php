<?php

include("../rundb/utils.php"); 
include("../rundb/config.php");

//date_default_timezone_set('GMT');
setlocale(LC_TIME, 'en');
putenv("TZ=America/New_York");


// Run details view
if ( isset($_GET['fillid']) && isset($_GET['rp']) )
{
   $gFillId    = $_GET['fillid'];
   $gAnaName   = "run".sprintf("%02d", $_GET['rp']);
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

if ( isset($_GET['test']) )
{
include("fills_index_test.html");
} else {
include("fills_index.html");
}

?>
