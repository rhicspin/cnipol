<?php

//include("config.php");

//date_default_timezone_set('GMT');
setlocale(LC_TIME, 'en');
putenv("TZ=America/New_York");


if (isset($_GET['runid'])) {

   // check for correct and existing  runid

	$gRunId = $_GET['runid'];

   // Read information about this run
   include($gRunId."/runconfig.php"); 
   include("./config_plots.php"); 
   include_once("PlotHelper.php"); 

   $dir = "./$gRunId/images/";
   $gP = new PlotHelper($dir, $plots_set1);

	// Get all images in this folder
   $dir = opendir($gRunId."/images/");
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

      //echo "$file ".$rs['dirsize']."<br>\n";
      //include($file."/runconfig.php"); 
      $gRunId = $file;
      //$rs[$gRunId]['NumChannels'] = $rc['data']['NumChannels'];
      $rs[$gRunId]['dirsize'] = exec( "du -h -s $file" );
   }
}

closedir($dir);

include("runinfo_index.html");

?>
