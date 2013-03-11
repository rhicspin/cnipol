<?php

/** */
class pair {
   var $first;
   var $second;
   var $val;
   var $err;

   function __construct($f=0, $s=0) {
      $this->first  = $f;
      $this->second = $s;

      $this->val =& $this->first;
      $this->err =& $this->second;
   }

   function ClonePair()
   {
      return unserialize(serialize($this));
      //$this->val =& unserialize(serialize($this))->first;
      //$this->err =& unserialize(serialize($this))->second;
   }


   function ToString($classes="", $format="%3.2f", $isHtml=true)
   {
      if ($this == null || $this->second < 0) return $isHtml ? "&nbsp;" : "";
   
      $str = "";
   
      if ($isHtml)
         $str = sprintf("$format&nbsp;&plusmn;&nbsp;$format", $this->first, $this->second);
      else
         $str = sprintf("$format +- $format", $this->first, $this->second);
     
      if (!empty($classes) && $isHtml)
        $str = "<span class='$classes'>$str</span>";
      
      return $str;
   }
}

?>
