<?php

include_once("config.php");
include_once("SqlDbReader.php");
include_once("TargetLossMonitor.php");

date_default_timezone_set('America/New_York');


class DbRunInfo extends SqlDbReader
{
   var $urlQuery     = "";
   var $sqlQuery     = "";
   var $sqlWhere     = "";
   var $sqlOrderBy   = "";
   var $nResults     = 0;

   var $targetUsage  = array();
   var $targetLosses = array();


   /** */
   function DbRunInfo(&$tgtLossMon)
   {
      mysql_connect("localhost", "cnipol", "(n!P0l") or die(mysql_error());
      mysql_select_db("cnipol") or die(mysql_error());

      $this->targetLosses = $tgtLossMon->targetLosses;
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
            array("counts" => $row['target_counts'], "interval1" => $fl_use['interval1'],
                  "interval2" => $fl_use['interval2']);
      }

      // check if loss date is within the interval
      $startDateTime = new DateTime($startTime);
      $endDateTime   = new DateTime($endTime);

      $tgtLos = &$this->targetLosses;

      foreach ($tgtLos as $polTgtOrientId => $lossDateTimes)
      {
         list($polId, $tgtOrient, $tgtId) = explode(",", $polTgtOrientId);

         foreach ($lossDateTimes as $lossDateTime)
         {
            if ($lossDateTime > $startDateTime && $lossDateTime < $endDateTime )
            {
               if ( !isset($this->targetUsage[$polId][$tgtId][$tgtOrient]) )
               {
                  $this->targetUsage[$polId][$tgtId][$tgtOrient] =
                     array("counts" => 0, "interval1" => null, "interval2" => null );
               }

               $this->targetUsage[$polId][$tgtId][$tgtOrient]["lossDateTime"] = $lossDateTime;
               break;
            }
         }
          
         //if ( isset($this->targetUsage[$polId][$tgtId][$tgtOrient]) )
         //   $this->targetUsage[$polId][$tgtId][$tgtOrient][] = 
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

      $startDateTime = new DateTime($startTime);
      $endDateTime   = new DateTime($endTime);
      $datetime2 = new DateTime($row['first_use']);
      $datetime3 = new DateTime($row['last_use']);
      $interval1 = $startDateTime->diff($datetime2);
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

      foreach ($TARGET_ORIENT as $tgtOrientId => $tgtOrient)
      {
         $row1 .= "<th colspan=6>$tgtOrient\n";

         $row2evn = $row2odd = "";

         foreach ($TARGET_ID as $tgtId => $tgtUId)
         {
            if ($tgtId % 2) $row2odd .= "<th>$tgtId\n";
            else            $row2evn .= "<th>$tgtId\n";
         }

         $row2 .= $row2odd.$row2evn;
      }

      $html .= $row1;
      $html .= $row2;

      foreach ($POLARIMETER_ID as $polId => $polName)
      {
         $html .= "<tr class=my_tr_highlight><th>$polName\n";

         foreach ($TARGET_ORIENT as $tgtOrientId => $tgtOrient)
         {  
            $row2evn = $row2odd = "";

            foreach ($TARGET_ID as $tgtId => $tgtUId)
            {
               $tgtInfo = $this->targetUsage[$polId][$tgtId][$tgtOrient];

               $cell = "";
	            $cssHighlight = !empty( $tgtInfo["lossDateTime"] ) ? "feature" : "";

               if (empty($tgtInfo)) 
                  $cell .= "<td class='align_cm $cssHighlight'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n ";
                  //$cell .= "<td class='align_cm'>&nbsp; [$polId, $tgtId, $tgtOrient]\n ";
               else {
                  $cell .= "<td class='align_cm $cssHighlight'>{$tgtInfo['counts']} ";
                  $cell .= "(<span class=green>".( $tgtInfo["interval1"] ? $tgtInfo["interval1"]->format("%a") : "0")."</span>";
                  $cell .= "<span class=red>+".  ( $tgtInfo["interval2"] ? $tgtInfo["interval2"]->format("%a") : "0")."</span>)\n";

                  //$cell .= "<br>[$polId, $tgtId, $tgtOrient]";
               }

               if ($tgtId % 2) $row2odd .= $cell;
               else            $row2evn .= $cell;
            }

            $html .= $row2odd.$row2evn;
         }
         //$html .= $row;
      }

      $html .= "</table>\n";

      print $html;
   }
}

?>
