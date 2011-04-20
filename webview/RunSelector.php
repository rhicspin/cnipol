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
       // Create a table with the necessary header informations
       echo "<form action='".$_SERVER['PHP_SELF']."' method='get' name='formRunSelector'>\n";
       echo '<div align=center>
             <table border=0>';

       echo "  <tr>
               <td colspan=4 class=padding2><b>Run:</b>
               <input type=text name=rn>

               <tr>
               <td class=\"align_cm padding2\"><b>Polarimeter:</b>
	               <select name='pi'>
						<option value=''></option>
						<option value='0'>B1U</option>
						<option value='1'>Y1D</option>
						<option value='2'>B2D</option>
						<option value='3'>Y2U</option>
						</select>

               <td class=\"align_cm padding2\"><b>Type:</b>
	               <select name='mt'>
						<option value=''></option>
						<option value='0'>undef</option>
						<option value='1'>alpha</option>
						<option value='2'>sweep</option>
						<option value='4'>fixed</option>
						<option value='8'>ramp</option>
						<option value='16'>emit</option>
						</select>

               <td class=\"align_cm padding2\"><b>Beam Energy:</b>
	               <select name='be'>
						<option value=''></option>
						<option value='24'>24</option>
						<option value='100'>100</option>
						<option value='250'>250</option>
						</select>

               <td class=\"align_cm padding2\"><b>Target:</b>
	               <select name='to'>
						<option value=''></option>
						<option value='H'>H</option>
						<option value='V'>V</option>
						</select>
	               <select name='ti'>
						<option value=''></option>
						<option value='1'>1</option>
						<option value='2'>2</option>
						<option value='3'>3</option>
						<option value='4'>4</option>
						<option value='5'>5</option>
						<option value='6'>6</option>
						</select>
            ";

       echo "<tr>\n";
       echo "   <td colspan=5 class=\"align_cm padding2\">\n";
       echo "   <p><input type='submit' name=sb value='Select'>\n";
       echo "</table>\n";
       echo "</div>\n";
       echo "</form>\n";
   }
}
