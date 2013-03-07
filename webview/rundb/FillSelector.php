<?php

//include("config.php");


class FillSelector {

   var $urlQuery;
   var $sqlWhere;
   var $sqlOrderBy = "";
	var $defaultRun = 13;

   static $queryVarNames  = array("rp", "fn", "ft", "be", "mode",   "srtn", "srtd");


   function FillSelector()
   { //{{{
      global $RUN_PERIOD_BY_FILL, $RUN_PERIOD_BY_DATE, $RUN_PERIOD, $FILLTYPE;
      global $BEAM_ENERGY_INJ;

      $url = parse_url($_SERVER['REQUEST_URI']);
      //$urlQuery = $url['query'];
      parse_str($url['query'], $urlQuery);

      // Copy only valid variables
      $urlQueryNew = array();

      foreach (self::$queryVarNames as $varName)
      {
         if (isset($urlQuery[$varName]))
            $urlQueryNew[$varName] = $urlQuery[$varName];
         //else
         //   $urlQueryNew[$varName] = "";
      }

      //$this->urlQuery = urlencode($url['query']);
      //$this->urlQuery = $url['query'];
      $this->urlQuery = http_build_query($urlQueryNew);

      $this->sqlWhere = "TRUE";

      if ( isset($_GET['rp']) && array_key_exists($_GET['rp'], $RUN_PERIOD)) {
         $rp = $_GET['rp'];
         $this->sqlWhere .= " AND fill_polar.fill > '{$RUN_PERIOD_BY_FILL[$rp]['start']}' AND fill_polar.fill < '{$RUN_PERIOD_BY_FILL[$rp]['end']}'";
      } else if ( isset($_GET['rp']) && empty($_GET['rp'])) {

      } else {
         $rp = $this->defaultRun;
         $_GET['rp'] = $rp;
         $this->sqlWhere .= " AND fill_polar.fill > '{$RUN_PERIOD_BY_FILL[$rp]['start']}' AND fill_polar.fill < '{$RUN_PERIOD_BY_FILL[$rp]['end']}'";
      }

      if (!empty($_GET['fn']))
         $this->sqlWhere .= " AND fill_polar.fill LIKE '{$_GET['fn']}'";

      if (isset($_GET['ft']) && array_key_exists($_GET['ft'], $FILLTYPE)) {
         $this->sqlWhere .= " AND type = {$_GET['ft']}";
      } else if ( isset($_GET['ft']) && empty($_GET['ft'])) {

      } else {
         //$ft = 0x01; // phys type is default
         $ft = 0x00; // phys type is default
         $this->sqlWhere .= " AND type = $ft";
      }

      if (!empty($_GET['be']))
         $this->sqlWhere .= " AND ROUND(beam_energy) = {$_GET['be']}";

      //$this->sqlWhere .= " AND beam_energy >= {$BEAM_ENERGY_INJ}";
   } //}}}


   function HtmlSortLinks($key=null)
   { //{{{
      // first check for a page variable in the query
      $url = parse_url($_SERVER['REQUEST_URI']);
      parse_str($url['query'], $tmpQuery);

      $page = is_numeric($tmpQuery['page']) ? "page={$tmpQuery['page']}&" : "" ;

      // then remove unwanted sorting variables
      parse_str($this->urlQuery, $urlVars);

      unset($urlVars['srtn']);
      unset($urlVars['srtd']);

      $this->urlQuery = http_build_query($urlVars);

      $html = "<a href=\"?$page{$this->urlQuery}&srtn=$key&srtd=-1\">&#9660;</a> ". 
              "<a href=\"?$page{$this->urlQuery}&srtn=$key&srtd=+1\">&#9650;</a>";

      return $html;
   } //}}}


   function PrintForm()
   { //{{{
      global $RUN_PERIOD, $FILLTYPE, $BEAM_ENERGY, $MODE;

      // Create a table with the necessary header informations
      //echo "<form action='".$_SERVER['PHP_SELF']."?uri=5&' method='get' name='formFillSelector'>\n";
      echo "<form action='' method='get' name='formFillSelector'>\n";
      echo "<div align=center>
            <table border=0>";

      echo "

            <tr>
              <td colspan=3 class=padding2><b>Run period:</b>\n";

      $this->HtmlSelectField($RUN_PERIOD, "rp", $this->defaultRun);

      echo "<tr>
              <td colspan=3 class=padding2><b>Fill:</b>
              <input type=text name=fn value='{$_GET['fn']}'>
              &nbsp;&nbsp;&nbsp;
              Use \"%\" to match any number of characters, use \"_\" to match any single character in fill id

            <tr>\n";

      echo "  <td class=\"padding2\"><b>Type:</b>\n";

      //$this->HtmlSelectField($FILLTYPE, "ft", 1); // default type is "phys"
      $this->HtmlSelectField($FILLTYPE, "ft", 0); // default type is "undef"

      echo "  <td class=\"padding2\"><b>Beam energy:</b>\n";

      $this->HtmlSelectField($BEAM_ENERGY, "be");

      echo "  <td class=\"padding2\"><b>Table format:</b>\n";

      $this->HtmlSelectField($MODE, "mode", 1);

      echo "<tr>\n";
      echo "   <td colspan=3 class=\"align_cm padding2\">\n";
      echo "   <p><input type='submit' name=sb value='Select'>\n";
      echo "</table>\n";
      echo "</div>\n";
      echo "</form>\n";
   } //}}}


   function HtmlSelectField($options, $v="", $default=null)
   { //{{{
      $selected      = "";

      $html  = "<select name='$v'>\n";
      $html .= "<option value=''></option>\n";

      foreach($options as $ovalue => $oname) {

         if ( empty($selected) ) {

            if ( isset($_GET[$v]) && array_key_exists($_GET[$v], $options) && ($_GET[$v] == $ovalue) )
               $html .= "<option value='$ovalue' selected>$oname</option>\n";
            else if ( !isset($_GET[$v]) && !empty($default) && $default == $ovalue )
               $html .= "<option value='$ovalue' selected>$oname</option>\n";
            else
               $html .= "<option value='$ovalue'>$oname</option>\n";

         } else {
            $html .= "<option value='$ovalue'>$oname</option>\n";
         }
      }

      $html .= "</select>\n";

      print $html;
   } //}}}
}
