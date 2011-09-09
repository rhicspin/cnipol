<?php

//include_once("DbEntry.php");
include_once("RunSelector.php");

class SqlDbReader {

   var $entries      = array();
   var $result       = 0;
   var $urlQuery     = "";
   var $sqlQuery     = "";
   var $sqlWhere     = "";
   var $sqlOrderBy   = "";
   var $nResults     = 0;
   var $nRunsPerPage = 200;
   var $runSelector  = null;

   /** */
   function SqlDbReader($runSelector=null)
   {
      mysql_connect("localhost", "cnipol2", "cnipol") or die(mysql_error());
      mysql_select_db("cnipol") or die(mysql_error());

      if (!empty($runSelector)) {
         $this->runSelector = $runSelector;
         $this->urlQuery    = $runSelector->urlQuery;
         $this->sqlWhere    = $runSelector->sqlWhere;
         $this->sqlOrderBy  = $runSelector->sqlOrderBy;
      }
   }

   /** */
   function SetSqlWhere($where="")
   {
      $this->sqlWhere = $where;
   }

   /** */
   function CreateSqlWhere($runSelector=0)
   {
      if (!$runSelector) return;
   }

   /** */
   function CountEntries()
   {
      $this->sqlQuery = "SELECT COUNT(run_name) FROM `run_info` WHERE {$this->sqlWhere}";
      
      $result = mysql_query($this->sqlQuery);
      $nrows  = mysql_fetch_array($result);
      $this->nResults = $nrows[0];
      return $this->nResults;
   }

   /** */
   function ReadEntries($start=0, $limit=100)
   {
      $this->sqlQuery = "SELECT *\n"
                      . "FROM `run_info` WHERE {$this->sqlWhere}\n"
                      . "ORDER BY {$this->sqlOrderBy} `run_info`.`start_time` DESC\n"
                      . "LIMIT $start, $limit\n";
      
      $this->result = mysql_query($this->sqlQuery);
      return $this->result;
   }


   /** */
   function SortByTime() {
   }


   /**
    * Create page links
    */
   function HtmlPageIndex($nPages=1, $curPage=1)
   {
      $this->CountEntries();

      $html = "<div align=center class=s120>\n";

      for ($i=1; $i<=$nPages; $i++) {
         if ($i == $curPage) 
            $html .= "$i ";
         else
            $html .= "<a href=\"?page=$i&{$this->urlQuery}\">$i</a> ";
      }

      $html .= "</div>\n";

      return $html;
   }


   /** */
   function PrintPageIndex($nPages=1, $curPage=1)
   {
      print $this->HtmlPageIndex($nPages, $curPage);
   }
}

?>
