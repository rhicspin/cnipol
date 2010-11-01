<?php

class PlotHelper {

   var $dir;
   var $ps;

   function PlotHelper($dir, $ps) {
      $this->dir = $dir;
      $this->ps = $ps;
   }

   function img($pName) {
	   $imgName = $this->dir."/c_$pName.png";
      $html = "<a href=$imgName><img width=400 src=$imgName></a>";
		return $html;
   }

}

?>
