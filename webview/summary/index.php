<?php

include("../rundb/utils.php"); 
include("../rundb/config.php");

//date_default_timezone_set('GMT');
setlocale(LC_TIME, 'en');
putenv("TZ=America/New_York");


if ( isset($_GET['ana']) )
{
   $gAnaId  = $_GET['ana'];
   $gAnaDir = DATA_DIR."/$gAnaId";

   // Check for correct and existing runid
   if (!is_dir($gAnaDir) || !file_exists("$gAnaDir/anainfo.php"))
   {
      print "No results found for $gAnaId<br>\n";
      exit;
   }


   // Read information about this run from the config file
   $ai = array();
   include("$gAnaDir/anainfo.php");
   $ai = $rc;

   if (count($ai) == 0) {
      print "No config file found for $gAnaId\n";
      exit;
   }

   include("summary.html");
} elseif ( isset($_GET['alpha_list']) ) {
   include("summary_malpha.html");
} else {
   include("summary_index.html");
}

?>
