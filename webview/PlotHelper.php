<?php

class PlotHelper {

   var $dir;
   var $ps;

   function PlotHelper($dir, $ps=null) {
      $this->dir = $dir;
      $this->ps = $ps;
   }

   function img($pName, $w=300, $href=null)
   {
	   $pi = pathinfo($pName);

	   $imgName = "{$this->dir}/{$pi['dirname']}/c_{$pi['basename']}.png";

      if (empty($href)) {
         $href = $imgName;
      }

      $html = "<a href=\"$href\"><img width=$w src=\"$imgName\"></a>";
		return $html;
   }
}

?>
