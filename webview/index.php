<?php

//include("config.php");

$sss="test";


if (isset($_GET['runid'])) {

   // check for correct and existing  runid

	$runId = $_GET['runid'];

   // Read information about this run
   include($runId."/runconfig.php"); 

	// Get all images in this folder
   $dir = opendir($runId);
	$imgs = array();
   
   while (false !== ($file = readdir($dir))) {
      if ( preg_match("/^.+\.png$/", $file) ) {
         //echo "$file<br>\n";
			$imgs[] = $file;
      }
   }

   include("runinfo.html");

	exit;
}

//Form default output

$dir = opendir('.');

if (!$dir) exit("ERROR\n");

while (false !== ($file = readdir($dir))) {

   if (ereg("[0-9]{3,}\.[0-9]{3}", $file) && is_dir($file)) {

      //echo "$file<br>\n";
      include($file."/runconfig.php"); 
      $runId = $file;
      $rs[$runId]['NumChannels'] = $rc['data']['NumChannels'];
   }
}

closedir($dir);

include("runinfo_index.html");

?>
