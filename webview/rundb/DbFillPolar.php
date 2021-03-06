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
      mysql_connect("localhost", "cnipol", "(n!P0l") or die(mysql_error());
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
      $this->sqlQuery = "SELECT COUNT(distinct fill_polar.fill) FROM `fill_polar_new` AS fill_polar  WHERE {$this->sqlWhere}";
      //$this->sqlQuery = "SELECT COUNT(distinct fill) FROM `fill_polar_new` WHERE TRUE AND fill > '10000' AND fill < '11100'";

      //print $this->sqlQuery;

      $result = mysql_query($this->sqlQuery);
      $nrows  = mysql_fetch_array($result);
      $this->nResults = $nrows[0];
      return $this->nResults;
   }

   /** */
   function ReadEntries($start=0, $limit=100)
   {
      //$this->sqlQuery = "SELECT *\n"
      //                . "FROM `fill_polar_new` LEFT JOIN `fill_profile` ON fill_polar_new.fill = fill_profile.fill \n"
      //                . "WHERE {$this->sqlWhere} \n"
      //                . "ORDER BY `fill_polar_new`.`fill` ASC\n";

      //$this->sqlQuery = "SELECT *\n"
      //                . "FROM `fill_polar_new` \n"
      //              . "WHERE {$this->sqlWhere}\n"
      //                . "ORDER BY `fill_polar_new`.`fill` ASC\n";

      //$this->sqlQuery = "SELECT *\n"
      //                . "FROM `fill_polar_new` \n"
      //              . "WHERE TRUE AND fill > '10000' AND fill < '11100'\n"
      //                . "ORDER BY `fill_polar_new`.`fill` ASC\n";

      $this->sqlQuery = "SELECT fill_polar.*, target_orient, target_id, prof_r, prof_r_err, prof_r0, prof_r0_err, prof_rslope, prof_rslope_err "
                      . "FROM `fill_polar_new` AS fill_polar LEFT JOIN `fill_profile_new` ON "
                      . "fill_polar.fill = fill_profile_new.fill AND fill_polar.polarimeter_id = fill_profile_new.polarimeter_id "
                      //. "WHERE TRUE AND fill_polar.fill > '10000' AND fill_polar.fill < '11100' "
                      . "WHERE {$this->sqlWhere} \n"
                      . "ORDER BY `fill_polar`.`fill` ASC\n";

      //print $this->sqlQuery;

      $this->result = mysql_query($this->sqlQuery);
      return $this->result;
   }
}

?>
