<?php

class DbEntry {

   var $runId = 0;
   var $fieldNames = array(
      "RESET_ALL", "POLARIMETER_ID", "POLARIZATION", "MEASUREMENT_TYPE", "MASSCUT", "TSHIFT",
      "INJ_TSHIFT", "ENERGY_CALIB",
      "ALPHA_CALIB_RUN_NAME", "CONFIG", "DEFINE_SPIN_PATTERN",
      "DEFINE_FILL_PATTERN", "REFERENCE_RATE", "TARGET_COUNT_MM", "COMMENT",
      "DISABLED_BUNCHES", "EnableBunch", "DISABLED_CHANNELS", "EnableStrip", "DisabledBunches", "DisabledStrips",
      "RUN_STATUS", "START_TIME", "STOP_TIME", "NEVENTS_TOTAL",
      "NEVENTS_PROCESSED", "BEAM_ENERGY", "TARGET_ID", "ASYM_VERSION", "PROFILE_RATIO"
   );

   var $f = array();

   /** */
   function DbEntry($entry) {

      foreach($this->fieldNames as $fieldName) {
         //$this->$fieldName = "";
      }
      
      $this->Process($entry);
      //return $this;
   }

   /** */
   function Process($entry) {

      foreach($entry as $line) {

         if (preg_match("/\[([\w,\W]+)\]/", $line, $matches)) {
            $this->runId = $matches[1];
            continue;
         }

         foreach($this->fieldNames as $fieldName) {
            if (preg_match("/^\s*$fieldName\s*(\*?)=\s*(.*);.*$/", $line, $matches)) {
               $this->f[$fieldName] = $matches[2];
               $this->$fieldName = $matches[2];
            }
         }
      }
   }


   /** */
   function PrintThis()
   {
      print "RUN: {$this->runId}<br>\n";

      foreach($this->f as $fName => $fValue) {
         print "$fName = $fValue<br>\n";
      }
      print "---<br>\n";
   }
}

?>
