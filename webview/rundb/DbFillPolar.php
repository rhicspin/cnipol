<?php

include_once("SqlDbReader.php");

class DbFillPolar extends SqlDbReader
{

   /** */
   function DbFillPolar($runSelector=null)
   {
      mysql_connect("localhost", "cnipol2", "cnipol") or die(mysql_error());
      mysql_select_db("cnipol") or die(mysql_error());
   }


   /** */
   function CountEntries()
   {
      $this->sqlQuery = "SELECT COUNT(fill) FROM `fill_polar` ";
      
      $result = mysql_query($this->sqlQuery);
      $nrows  = mysql_fetch_array($result);
      $this->nResults = $nrows[0];
      return $this->nResults;
   }

   /** */
   function ReadEntries($start=0, $limit=100)
   {
      $this->sqlQuery = "SELECT *\n"
                      //. "FROM `fill_polar` \n"
                      . "FROM `fill_polar` LEFT JOIN `fill_profile` ON fill_polar.fill = fill_profile.fill \n"
                      . "ORDER BY `fill_polar`.`fill` ASC\n";
      //$this->sqlQuery = "SELECT *\n"
      //                . "FROM `run_info` WHERE {$this->sqlWhere}\n"
      //                . "ORDER BY {$this->sqlOrderBy} `run_info`.`start_time` DESC\n"
      //                . "LIMIT $start, $limit\n";
      
      $this->result = mysql_query($this->sqlQuery);
      return $this->result;
   }
}

?>
