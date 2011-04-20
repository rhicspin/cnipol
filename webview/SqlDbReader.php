<?php

include("DbEntry.php");

class SqlDbReader {

   var $entries  = array();
   var $result   = 0;
   var $sqlQuery = "";
   var $sqlWhere = "";
   var $nResults = 0;

   /** */
   function SqlDbReader()
   {
      mysql_connect("localhost", "cnipol2", "cnipol") or die(mysql_error());
      mysql_select_db("cnipol") or die(mysql_error());
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
                      . "ORDER BY `run_info`.`start_time` DESC\n"
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
   function HtmlPageIndex()
   {
      $this->CountEntries();

      $page = 1;
      $nRunsPerPage = 200;
      $nPages = ceil($nRuns/$nRunsPerPage);
      
      if ( isset($_GET['page']) ) $page = $_GET['page'];
      if ($page > $nPages || $page < 1) $page = 1;
      
      $html = "<p>\n";
      for ($i=1; $i<=$nPages; $i++) {
         if ($i == $page) 
            $html = "$i ";
         else
            $html = "<a href=./?page=$i>$i</a> ";
      }

      return $html;
   }


   /** */
   function PrintPageIndex()
   {
      print $this->HtmlPageIndex();
   }
}

?>
