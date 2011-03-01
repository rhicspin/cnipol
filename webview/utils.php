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

?>
