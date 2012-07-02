<?php

//include("config.php");


class Selector {

   var $urlQuery;
   var $sqlWhere;
   var $sqlOrderBy = "";

   static $queryVarNames  = array();
   static $shortSortNames = array();


   function Selector()
   { //{{{
      global $RUN_PERIOD_BY_DATE, $RUN_PERIOD, $POLARIMETER_ID, $RHIC_STREAM, $MEASTYPE, $TARGET_ORIENT, $TARGET_ID;

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

      if (isset($_GET['srtn']) && array_key_exists($_GET['srtn'], self::$shortSortNames))
      {
         $this->sqlOrderBy  =  " ".self::$shortSortNames[$_GET['srtn']]." ";
         $this->sqlOrderBy .= ( isset($_GET['srtd']) && is_numeric($_GET['srtd']) && $_GET['srtd'] < 0 ) ? "DESC" : "ASC";
         $this->sqlOrderBy .= ", ";
      }
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
