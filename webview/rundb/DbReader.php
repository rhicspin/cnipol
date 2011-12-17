<?php

include("DbEntry.php");

class DbReader {

   var $dbFile;
   var $dbFileName;
   var $entries = array();

   function DbReader($dbFileName="run.db")
   {
      $this->dbFileName = $dbFileName;
      $this->dbFile = @fopen($this->dbFileName, "r");
   }


   /** */
   function ReadEntries()
   {
      $entry = array();

      while (($line = fgets($this->dbFile, 4096)) !== false) {

          //print "- $line<br>\n";

          //if (TPRegexp("^(\\s*)//(.*)$").MatchB(sline)) { continue; }
          //if (TPRegexp("\\[([\\w,\\W]+)\\]").MatchB(sline) || nb < 0)

          $line = trim($line);

          if (strlen($line) <= 0) continue;

          // Skip comment lines
          if (preg_match("/^(\s*)\/\/(.*)$/", $line)) continue;
             //print("found comment<br>\n");

          if (preg_match("/\[([\w,\W]+)\]/", $line, $matches)) {
             //print("found run {$matches[1]}<br>\n");

             if (count($entry) > 1) {
                $this->entries[] = new DbEntry($entry); //$dbEntry->Process($entry);
                //end($this->entries)->PrintThis();
             }

             $entry = array();
          }

          $entry[] = $line;
      }

      if (!feof($this->dbFile)) {
          echo "Error: unexpected fgets() fail\n";
      }

      // save the last entry
      $this->entries[] = new DbEntry($entry); //$dbEntry->Process($entry);

      fclose($this->dbFile);

      return $this->entries;
   }

   /** */
   function SortByTime() {

   }
}

?>
