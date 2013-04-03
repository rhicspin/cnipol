<?php

include_once("config.php");
include_once("SqlDbReader.php");

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
   function FetchTargetUsage($startTime=null, $endTime=null)
   {
      global $POLARIMETER_ID, $TARGET_ID, $TARGET_ORIENT;

		$this->sqlWhere  = "`start_time` >= '$startTime'";
		$this->sqlWhere .= !empty($endTime) ? " AND `start_time` <= '$endTime'\n" : "";

      $this->sqlQuery = "SELECT polarimeter_id, target_id, target_orient, count(*) AS target_counts\n"
                      . "FROM run_info\n"
                      . "WHERE {$this->sqlWhere} \n"
                      . "AND `target_orient` != '-'\n"
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

         $this->targetUsage[$polId][$tgtId][$tgtOrient] = $row['target_counts'];
      }

      return $this->targetUsage;
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
		         $html .= "<td class='align_cm'>{$this->targetUsage[$polId][$tgtId][$tgtOrient]}\n";
			   }
			}
      }

		$html .= "</table>\n";

		print $html;
   }
}

?>
