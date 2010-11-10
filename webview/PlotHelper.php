<?php

class PlotHelper {

   var $dir;
   var $ps;

   function PlotHelper($dir, $ps=null) {
      $this->dir = $dir;
      $this->ps = $ps;
   }

   function img($pName) {
	   $pi = pathinfo($pName);
	   $imgName = "{$this->dir}/{$pi['dirname']}/c_{$pi['basename']}.png";
      $html = "<a href=$imgName><img width=300 src=$imgName></a>";
		return $html;
   }

}

?>
