<?php

include("../rundb/utils.php"); 
include("../rundb/config.php");

setlocale(LC_TIME, 'en');
date_default_timezone_set('America/New_York');


if ( isset($_GET['ana']) )
{
   $gAnaId  = $_GET['ana'];
   $gAnaId  = strip_tags($gAnaId);
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
}elseif( isset($_GET['anar22']) ){
   $gAnaId  = $_GET['anar22'];
   $gAnaId  = strip_tags($gAnaId);
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

   include("summary_r22.html");
}elseif( isset($_GET['anar22b']) ){
   $gAnaId  = $_GET['anar22b'];
   $gAnaId  = strip_tags($gAnaId);
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

   include("summary_r22_254.html");
}elseif ( isset($_GET['alpha_list']) ) {
   include("summary_malpha.html");
} elseif ( isset($_GET['bes_list']) ) {
   include("summary_bes.html");
} else {
   include("summary_index.html");
}

?>
