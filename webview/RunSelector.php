<?php

//include("config.php");


class RunSelector {

   var $urlQuery;
   var $sqlWhere;
   static $queryVarNames = array("rn", "pi", "mt", "be", "to", "ti");

   function RunSelector()
   {
      global $POLARIMETER_ID, $RHIC_BEAM, $RHIC_STREAM, $MEASTYPE, $TARGET_ORIENT, $TARGET_ID;

      $url = parse_url($_SERVER['REQUEST_URI']);
      //$urlQuery = $url['query'];
      parse_str($url['query'], $urlQuery);

      // Copy only valid variables
      $urlQueryNew = array();

      foreach (self::$queryVarNames as $varName)
      {
         if (isset($urlQuery[$varName]))
            $urlQueryNew[$varName] = $urlQuery[$varName];
         else
            $urlQueryNew[$varName] = "";
      }

      //$this->urlQuery = urlencode($url['query']);
      //$this->urlQuery = $url['query'];
      $this->urlQuery = http_build_query($urlQueryNew);

      $this->sqlWhere = "TRUE";

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
   }


   function PrintForm()
   {
      global $POLARIMETER_ID, $RHIC_BEAM, $RHIC_STREAM, $MEASTYPE, $BEAM_ENERGY, $TARGET_ORIENT, $TARGET_ID;

      // Create a table with the necessary header informations
      echo "<form action='".$_SERVER['PHP_SELF']."' method='get' name='formRunSelector'>\n";
      echo '<div align=center>
            <table border=0>';

      echo "  <tr>
              <td colspan=4 class=padding2><b>Run:</b>
              <input type=text name=rn>
              &nbsp;&nbsp;&nbsp;
              Use \"%\" to match any number of characters, use \"_\" to match any single character in run name

              <tr>
              <td class=\"align_cm padding2\"><b>Polarimeter:</b>\n";

      $this->HtmlSelectField($POLARIMETER_ID, "pi");

      echo "  <td class=\"align_cm padding2\"><b>Type:</b>\n";

      $this->HtmlSelectField($MEASTYPE, "mt");

      echo "  <td class=\"align_cm padding2\"><b>Beam Energy:</b>\n";

      $this->HtmlSelectField($BEAM_ENERGY, "be");

      echo "  <td class=\"align_cm padding2\"><b>Target:</b>\n";

      $this->HtmlSelectField($TARGET_ORIENT, "to");
      $this->HtmlSelectField($TARGET_ID, "ti");

      echo "<tr>\n";
      echo "   <td colspan=5 class=\"align_cm padding2\">\n";
      echo "   <p><input type='submit' name=sb value='Select'>\n";
      echo "</table>\n";
      echo "</div>\n";
      echo "</form>\n";
   }


   function HtmlSelectField($options, $v="")
   {
      //$key = key($vpair);

      $html  = "<select name='$v'>\n";
      $html .= "<option value=''></option>\n";

      foreach($options as $ovalue => $oname) {

         if (isset($_GET[$v]) && array_key_exists($_GET[$v], $options) && ($_GET[$v] == $ovalue) ) $select = "selected";
         else $select = "";

         $html .= "<option value='$ovalue' $select>$oname</option>\n";
      }

      $html .= "</select>\n";

      print $html;
   }
}
