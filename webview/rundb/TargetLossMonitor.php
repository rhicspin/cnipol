<?php

include_once("config.php");
include_once("utils.php");

date_default_timezone_set('America/New_York');

class TargetLossMonitor
{
   var $targetLosses  = array();


   /** */
   function TargetLossMonitor()
   {
      global $POLARIMETER_ID, $TARGET_ID, $TARGET_ORIENT;

      $fileNameFull = realpath(dirname(__FILE__)) . "/target_losses.txt";
      $file = fopen($fileNameFull, "r");

      if ($file === FALSE) {
         print("No file found\n");
         return;
      }

      while (($data = fgetcsv($file, 3000, ",")) !== FALSE)
      {
         $num = count($data);

         $data = array_map("trim", $data);

         $polId     = array_search($data[0], $POLARIMETER_ID);
         $tgtOrient = substr($data[1], 0, 1);
         $tgtId     = substr($data[1], 1, 1);

         if ($polId === FALSE) continue;

         //if ( !array_key_exists($polId, $this->targetLosses) )
         //   $this->targetLosses[$polId] = array();

         //if ( !array_key_exists($tgtOrient, $this->targetLosses[$polId]) )
         //   $this->targetLosses[$polId][$tgtOrient] = array();

         //if ( !array_key_exists($tgtId, $this->targetLosses[$polId][$tgtOrient]) )
         //   $this->targetLosses[$polId][$tgtOrient][$tgtId] = array();

         //$this->targetLosses[$polId][$tgtOrient][$tgtId][] = DateTime::createFromFormat("M d, Y H:i:s D", $data[2]);
         $polTgtOrientId = implode(',', array($polId, $tgtOrient, $tgtId) );

         //if ( !array_key_exists($polTgtOrientId, $this->targetLosses) )
         $this->targetLosses[$polTgtOrientId][] = DateTime::createFromFormat("M d, Y H:i:s D", $data[2]);

         #echo "<p> $num fields in line $row: <br /></p>\n";
         #$row++;
         #for ($c=0; $c < $num; $c++) {
         #    echo $data[$c] . "<br />\n";
         #}
      }

      fclose($file);
   }
   
   function flattenTargetLosses()
   {
      return makeNonNested($this->targetLosses);
   }

}

?>
