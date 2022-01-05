<?php

class RunSelector
{
   var $urlQuery;
   var $sqlWhere;
   var $sqlOrderBy = "";
   var $defaultRun = 22;

   static $queryVarNames  = array("rp", "rn", "pi", "mt", "be", "to", "ti", "himg",   "srtn", "srtd");

   static $shortSortNames = array("rn" => "run_name",
                                  "pi" => "polarimeter_id",
                                  "po" => "polarization",
                                  "mt" => "measurement_type",
                                  "be" => "beam_energy",
                                  "te" => "nevents_total",
                                  "pr" => "profile_ratio",
                                  "at" => "ana_start_time",
                                  "av" => "asym_version",
                                  "tgt",
                                  "ph" => "phase",
);

   function RunSelector()
   {
      global $RUN_PERIOD_BY_DATE, $RUN_PERIOD, $POLARIMETER_ID, $RHIC_STREAM, $MEASTYPE, $TARGET_ORIENT, $TARGET_ID;

      $url = parse_url($_SERVER['REQUEST_URI']);

      if (isset($url['query']))
         parse_str($url['query'], $urlQuery);

      // Copy only valid variables
      $urlQueryNew = array();

      foreach (self::$queryVarNames as $varName)
      {
         if (isset($urlQuery[$varName]))
            $urlQueryNew[$varName] = $urlQuery[$varName];
      }

      $this->urlQuery = http_build_query($urlQueryNew);

      $this->sqlWhere = "TRUE";

      if ( isset($_GET['rp']) && array_key_exists($_GET['rp'], $RUN_PERIOD)) {
         $rp = $_GET['rp'];
         $this->sqlWhere .= " AND start_time > '{$RUN_PERIOD_BY_DATE[$rp]['start']}' AND start_time < '{$RUN_PERIOD_BY_DATE[$rp]['end']}'";
      } else if ( isset($_GET['rp']) && empty($_GET['rp'])) {
      } else {
         $rp = $this->defaultRun;
         $_GET['rp'] = $rp;
         $this->sqlWhere .= " AND start_time > '{$RUN_PERIOD_BY_DATE[$rp]['start']}' AND start_time < '{$RUN_PERIOD_BY_DATE[$rp]['end']}'";
      }

      if (!empty($_GET['rn']))
         $this->sqlWhere .= " AND run_name LIKE '{$_GET['rn']}'";

      if (isset($_GET['pi']) && array_key_exists($_GET['pi'], $POLARIMETER_ID))
         $this->sqlWhere .= " AND polarimeter_id = {$_GET['pi']}";

      if (isset($_GET['mt']) && array_key_exists($_GET['mt'], $MEASTYPE))
         $this->sqlWhere .= " AND measurement_type = {$_GET['mt']}";

      if (!empty($_GET['be']))
         $this->sqlWhere .= " AND ROUND(beam_energy) = {$_GET['be']}";

      if (isset($_GET['to']) && array_key_exists($_GET['to'], $TARGET_ORIENT))
         $this->sqlWhere .= " AND target_orient LIKE '{$_GET['to']}'";

      if (isset($_GET['ti']) && array_key_exists($_GET['ti'], $TARGET_ID))
         $this->sqlWhere .= " AND target_id = {$_GET['ti']}";


      if (isset($_GET['srtn']) && array_key_exists($_GET['srtn'], self::$shortSortNames))
      {
         $this->sqlOrderBy  =  " ".self::$shortSortNames[$_GET['srtn']]." ";
         $this->sqlOrderBy .= ( isset($_GET['srtd']) && is_numeric($_GET['srtd']) && $_GET['srtd'] < 0 ) ? "DESC" : "ASC";
         $this->sqlOrderBy .= ", ";
      }
   }


   function HtmlSortLinks($key=null)
   {
      // first check for a page variable in the query
      $url = parse_url($_SERVER['REQUEST_URI']);

      $tmpQuery = null;

      if (isset($url['query']))
         parse_str($url['query'], $tmpQuery);

      $page = (isset($tmpQuery['page']) && is_numeric($tmpQuery['page'])) ? "page={$tmpQuery['page']}&" : "" ;

      // then remove unwanted sorting variables
      parse_str($this->urlQuery, $urlVars);

      unset($urlVars['srtn']);
      unset($urlVars['srtd']);

      $this->urlQuery = http_build_query($urlVars);

      $html = "<a href=\"?$page{$this->urlQuery}&srtn=$key&srtd=-1\">&#9660;</a> ".
              "<a href=\"?$page{$this->urlQuery}&srtn=$key&srtd=+1\">&#9650;</a>";

      return $html;
   }


   function PrintForm()
   {
      global $RUN_PERIOD, $POLARIMETER_ID, $RHIC_STREAM, $MEASTYPE,
             $BEAM_ENERGY, $TARGET_ORIENT, $TARGET_ID, $PLOT_IMAGES;

      // Create a table with the necessary header informations
      echo "<form action='' method='get' name='formRunSelector'>\n";
      echo '<div align=center>
            <table border=0>';

      echo "
            <tr>
               <td colspan=4 class=padding2><b>Run period:</b>\n";

      $this->HtmlSelectField($RUN_PERIOD, "rp", $this->defaultRun);

      $runName = isset($_GET['rn']) ? $_GET['rn'] : "";

      echo "<tr>
               <td colspan=4 class=padding2><b>Run:</b>
               <input type=text name=rn value='{$runName}'>
               &nbsp;&nbsp;&nbsp;
               Use \"%\" to match any number of characters, use \"_\" to match any single character in run id\n";

      echo "<tr> ";
      echo "<td colspan=4 class=padding2><b>Graph:</b>\n";

      $this->HtmlSelectField($PLOT_IMAGES, "himg");

      echo "<tr>
               <td class=\"padding2\"><b>Polarimeter:</b>\n";

      $this->HtmlSelectField($POLARIMETER_ID, "pi");

      echo "   <td class=\"padding2\"><b>Type:</b>\n";

      $this->HtmlSelectField($MEASTYPE, "mt");

      echo "   <td class=\"padding2\"><b>Beam energy:</b>\n";

      $this->HtmlSelectField($BEAM_ENERGY, "be");

      echo "   <td class=\"padding2\"><b>Target:</b>\n";

      $this->HtmlSelectField($TARGET_ORIENT, "to");
      $this->HtmlSelectField($TARGET_ID, "ti");

      echo "<tr>
               <td colspan=4 class=\"align_cm padding2\">
               <p><input type='submit' name=sb value='Select'>
            </table>
            </div>
            </form>\n";
   }


   function HtmlSelectField($options, $v="", $default=null)
   {
      $html  = "<select name='$v'>\n";
      $html .= "<option value=''></option>\n";

      foreach($options as $ovalue => $oname)
      {
         if ( isset($_GET[$v]) && array_key_exists($_GET[$v], $options) && ($_GET[$v] == $ovalue) )
            $html .= "<option value='$ovalue' selected>$oname</option>\n";
         else if ( !isset($_GET[$v]) && !empty($default) && $default == $ovalue )
            $html .= "<option value='$ovalue' selected>$oname</option>\n";
         else
            $html .= "<option value='$ovalue'>$oname</option>\n";
      }

      $html .= "</select>\n";

      print $html;
   }
}
