<?php

include_once("config.php");
include_once("SqlDbReader.php");

date_default_timezone_set('America/New_York');

class DbRunInfo extends SqlDbReader
{
   var $urlQuery     = "";
   var $sqlQuery     = "";
   var $sqlWhere     = "";
   var $sqlOrderBy   = "";
   var $nResults     = 0;

   var $targetUsage  = array();


   /** */
   function DbRunInfo()
   {
      mysql_connect("localhost", "cnipol", "(n!P0l") or die(mysql_error());
      mysql_select_db("cnipol") or die(mysql_error());
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
      $this->sqlQuery = "SELECT fill_polar.*, target_orient, target_id, prof_r, prof_r_err, prof_r0, prof_r0_err, prof_rslope, prof_rslope_err "
                      . "FROM `fill_polar_new` AS fill_polar LEFT JOIN `fill_profile_new` ON "
                      . "fill_polar.fill = fill_profile_new.fill AND fill_polar.polarimeter_id = fill_profile_new.polarimeter_id "
                      . "WHERE {$this->sqlWhere} \n"
                      . "ORDER BY `fill_polar`.`fill` ASC\n";

      //print $this->sqlQuery;

      $this->result = mysql_query($this->sqlQuery);
      return $this->result;
   }

   /** */
   function FetchTargetUsage($beamEnergy=255, $startTime=null, $endTime=null)
   {
      $this->sqlWhere  = "`start_time` >= '$startTime'"
                       . (!empty($endTime) ? " AND `start_time` <= '$endTime'\n" : "\n")
                       . "AND `target_orient` != '-'\n";
                       //. " AND ROUND(beam_energy) = $beamEnergy";

      $this->sqlQuery = "SELECT polarimeter_id, target_id, target_orient, count(*) AS target_counts\n"
                      . "FROM `run_info`\n"
                      . "WHERE {$this->sqlWhere} \n"
                      . "GROUP BY polarimeter_id, target_id, target_orient\n";

      //print $this->sqlQuery;

      $this->result = mysql_query($this->sqlQuery);
      $this->targetUsage  = array();

      while ($row = mysql_fetch_assoc($this->result))
      {
         $polId = $row['polarimeter_id'];
         $tgtId = $row['target_id'];
         //$tgtOrient = array_search($row['target_orient'], $TARGET_ORIENT);
         $tgtOrient = $row['target_orient'];

         $fl_use = $this->FetchTargetFirstLastTime($polId, $tgtId, $tgtOrient, $startTime, $endTime);

         $this->targetUsage[$polId][$tgtId][$tgtOrient] =
            array("counts" => $row['target_counts'], "interval1" => $fl_use['interval1'], "interval2" => $fl_use['interval2']);
      }

      //print "<pre>";
      //print_r($this->targetUsage);
      //print "</pre>\n";

      return $this->targetUsage;
   }


   function FetchTargetFirstLastTime($polId, $tgtId, $tgtOrient, $startTime=null, $endTime=null)
   {
      $this->sqlWhere  = "`start_time` >= '$startTime'"
                       . (!empty($endTime) ? " AND `start_time` <= '$endTime'\n" : "\n")
                       . "AND polarimeter_id = $polId\n"
                       . "AND target_id = $tgtId\n"
                       . "AND target_orient LIKE '$tgtOrient'\n";

      $this->sqlQuery = "SELECT MIN(start_time) as first_use, MAX(start_time) as last_use\n"
                      . "FROM `run_info`\n"
                      . "WHERE {$this->sqlWhere}"
                      . "LIMIT 1\n";

      //print "<pre>{$this->sqlQuery}</pre>";

      $result = mysql_query($this->sqlQuery);
      $row = mysql_fetch_assoc($result);

      $datetime1 = new DateTime($startTime);
      $datetime2 = new DateTime($row['first_use']);
      $datetime3 = new DateTime($row['last_use']);
      $interval1 = $datetime1->diff($datetime2);
      $interval2 = $datetime2->diff($datetime3);

      //print "<pre>";
      //print $interval1->format("%R%a, ");
      //print $interval2->format("%R%a days ");
      //print_r($row);
      //print "</pre>\n";

      return array("interval1" => $interval1, "interval2" => $interval2);
   }


   /** */
   function PrintTargetUsage()
   {
      global $POLARIMETER_ID, $TARGET_ID, $TARGET_ORIENT;

      $html  = "<table class='wikitable cntr' cellspacing=15 align=center>\n";

      $row1  = "<tr><th>&nbsp;\n";
      $row2  = "<tr><th>&nbsp;\n";

      foreach ($TARGET_ID as $tgtId => $tgtUId)
      {
         $row1 .= "<th colspan=2>$tgtId\n";

         foreach ($TARGET_ORIENT as $tgtOrientId => $tgtOrient)
         {
            $row2 .= "<th>$tgtOrient\n";
         }
      }

      $html .= $row1;
      $html .= $row2;

      foreach ($POLARIMETER_ID as $polId => $polName)
      {
         $html .= "<tr class=my_tr_highlight><th>$polName\n";

         foreach ($TARGET_ID as $tgtId => $tgtUId)
         {
            foreach ($TARGET_ORIENT as $tgtOrientId => $tgtOrient)
            {  
               $tgtInfo = $this->targetUsage[$polId][$tgtId][$tgtOrient];

               if (empty($tgtInfo)) 
                  $html .= "<td class='align_cm'>\n ";
               else {
                  $html .= "<td class='align_cm'>{$tgtInfo['counts']} ";
                  $html .= "(<span class=green>".$tgtInfo["interval1"]->format("%a")."</span>";
                  $html .= "<span class=red>+".$tgtInfo["interval2"]->format("%a")."</span>)\n";
               }
            }
         }
      }

      $html .= "</table>\n";

      print $html;
   }
}

?>
