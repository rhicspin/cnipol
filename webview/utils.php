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

?>
