<?php

include_once("SqlDbReader.php");

class DbFillPolar extends SqlDbReader
{
   var $selector  = null;
   var $urlQuery     = "";
   var $sqlQuery     = "";
   var $sqlWhere     = "";
   var $sqlOrderBy   = "";
   var $nResults     = 0;

   /** */
   function DbFillPolar($fillSelector=null)
   {
      mysql_connect("localhost", "cnipol2", "cnipol") or die(mysql_error());
      mysql_select_db("cnipol") or die(mysql_error());

      if (!empty($fillSelector)) {
         $this->selector     = $fillSelector;
         $this->urlQuery     = $fillSelector->urlQuery;
         $this->sqlWhere     = $fillSelector->sqlWhere;
         $this->sqlOrderBy   = $fillSelector->sqlOrderBy;
      } else{
         $this->sqlWhere     = "TRUE";
      }
   }


   /** */
   function CountEntries()
   {
      //$this->sqlQuery = "SELECT COUNT(fill) FROM `fill_polar` ";
      $this->sqlQuery = "SELECT COUNT(fill) FROM `fill_polar` WHERE {$this->sqlWhere}";
      
      $result = mysql_query($this->sqlQuery);
      $nrows  = mysql_fetch_array($result);
      $this->nResults = $nrows[0];
      return $this->nResults;
   }

   /** */
   function ReadEntries($start=0, $limit=100)
   {
      $this->sqlQuery = "SELECT *\n"
                      . "FROM `fill_polar` LEFT JOIN `fill_profile` ON fill_polar.fill = fill_profile.fill \n"
                      . "WHERE {$this->sqlWhere} \n"
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
