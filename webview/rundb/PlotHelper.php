<?php

class PlotHelper {

   var $dir;
   var $ps;

   function PlotHelper($dir, $ps=null) {
      $this->dir = $dir;
      $this->ps  = $ps;
   }

   function img($pName, $thumb=false, $w=null, $href=null)
   {
      $strThumb = $thumb ? "_thumb" : "" ;
      $strWidth = (empty($w) && !$thumb) ? "width=300" : ( !empty($w) ? "width=$w" : "" ) ;

	   $pi = pathinfo($pName);

	   $imgSrc  = "{$this->dir}/{$pi['dirname']}/c_{$pi['basename']}$strThumb.png";
	   $imgHref = "{$this->dir}/{$pi['dirname']}/c_{$pi['basename']}.png";

      if (!empty($href)) {
         $imgHref = $href;
      }

      $html = "<a href=\"$imgHref\"><img $strWidth src=\"$imgSrc\" class='thumbimage'></a>";
		return $html;
   }
}

?>
