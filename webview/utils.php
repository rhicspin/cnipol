<?php

/**
 *    Get the ordinal suffix of an int (e.g. th, rd, st, etc.)
 *     @param  int   $n
 *     @return string   $n + $n's ordinal suffix
 */
function ordinal_suffix($n) {
     $n_last = $n % 100;
     if (($n_last > 10 && $n_last < 14) || $n == 0){
          return "{$n}th";
     }
     switch(substr($n, -1)) {
          case '1':    return "{$n}st";
          case '2':    return "{$n}nd";
          case '3':    return "{$n}rd";
          default:     return "{$n}th";
     }
}


function dateTimeDiff($startStamp, $endStamp)
{
   $d = $endStamp - $startStamp;

   if ($d < 0) return "";

   $secondsInMin  = 60;
   $secondsInHour = 3600;
   $secondsInDay  = 86400;

   $fullDays  = floor($d/$secondsInDay);
   $d = $d % $secondsInDay;
   $fullHours = floor($d/$secondsInHour);
   $d = $d % $secondsInHour;
   $fullMins  = floor($d/$secondsInMin);
   $d = $d % $secondsInMin;
   $fullSecs  = $d;

   $diff  = $fullDays ? "{$fullDays}d:" : "";
   $diff .= $fullHours ? "{$fullHours}h:" : "";
   $diff .= $fullMins ? "{$fullMins}m:" : "";
   $diff .= $fullSecs ? "{$fullSecs}s" : "";

   return $diff;
} 


/** Sort object arrays */
function hod(&$base, $path){
    $keys = explode("->", $path);
    $keys[0] = str_replace('$', '', $keys[0]);
    $expression = '$ret = ';
    $expression.= '$';
    foreach ($keys as $key){
        if (++$licz == 1){
            $expression.= 'base->';           
        } else {
            $expression.= $key.'->';
        }
    }
    $expression = substr($expression, 0, -2);
    $expression.= ';';
    eval($expression);
    return $ret;
}


function array_sort_func($a,$b=NULL) {
   static $keys;
   if($b===NULL) return $keys=$a;
   foreach($keys as $k) {
      if($k[0]=='!') {
         $k=substr($k,1);
         if(hod($a, '$a->'.$k)!==hod($b, '$b->'.$k)) {
            return strcmp(hod($b, '$b->'.$k),hod($a, '$a->'.$k));
         }
      }
      else if(hod($a, '$a->'.$k)!==hod($b, '$b->'.$k)) {
         return strcmp(hod($a, '$a->'.$k),hod($b, '$b->'.$k));
      }
   }
   return 0;
}


function array_sort(&$array) {
   if(!$array) return $keys;
   $keys=func_get_args();
   array_shift($keys);
   array_sort_func($keys);
   usort($array,"array_sort_func");      
}


/** */
function exportMysqlToCsv($table, $filename='rundb.csv')
{
   $csv_terminated = "\n";
   $csv_separator = ",";
   $csv_enclosed = '"';
   $csv_escaped = "\\";
   $sql_query = "select * from $table";

   // Gets the data from the database
   $result = mysql_query($sql_query);
   $fields_cnt = mysql_num_fields($result);


   $schema_insert = '';

   for ($i = 0; $i < $fields_cnt; $i++)
   {
       $l = $csv_enclosed . str_replace($csv_enclosed, $csv_escaped . $csv_enclosed,
           stripslashes(mysql_field_name($result, $i))) . $csv_enclosed;
       $schema_insert .= $l;
       $schema_insert .= $csv_separator;
   } // end for

   $out = trim(substr($schema_insert, 0, -1));
   $out .= $csv_terminated;

   // Format the data
   while ($row = mysql_fetch_array($result))
   {
       $schema_insert = '';
       for ($j = 0; $j < $fields_cnt; $j++)
       {
           if ($row[$j] == '0' || $row[$j] != '')
           {

               if ($csv_enclosed == '')
               {
                   $schema_insert .= $row[$j];
               } else
               {
                   $schema_insert .= $csv_enclosed . 
              str_replace($csv_enclosed, $csv_escaped . $csv_enclosed, $row[$j]) . $csv_enclosed;
               }
           } else
           {
               $schema_insert .= '';
           }

           if ($j < $fields_cnt - 1)
           {
               $schema_insert .= $csv_separator;
           }
       } // end for

       $out .= $schema_insert;
       $out .= $csv_terminated;
   } // end while

   header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
   header("Content-Length: " . strlen($out));
   // Output to browser with appropriate mime type, you choose ;)
   header("Content-type: text/x-csv");
   //header("Content-type: text/csv");
   //header("Content-type: application/csv");
   header("Content-Disposition: attachment; filename=$filename");
   echo $out;
   exit;
}




?>
