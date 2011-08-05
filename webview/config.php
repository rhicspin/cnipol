<?php

define("DATA_DIR", "/usr/local/polarim/root");

// Approximate fill numbers
$RUN_PERIOD_BY_FILL = array (
   9  => array( "start" => 10000, "end" => 11100 ),
   11 => array( "start" => 14000, "end" => 15500 )
);

// Approximate dates
$RUN_PERIOD_BY_DATE = array (
   9  => array( "start" => "2009-03-01 00:00:00", "end" => "2009-07-10 00:00:00" ),
   11 => array( "start" => "2010-12-01 00:00:00", "end" => "2011-04-20 00:00:00" )
);

$RUN_PERIOD     = array(9 => "Run 9", 11 => "Run 11");
$POLARIMETER_ID = array(0 => "B1U", 1 => "Y1D", 2 => "B2D", 3 => "Y2U");
$RHIC_BEAM      = array("1" => "Yellow", "2" => "Blue");
$RHIC_STREAM    = array("1" => "Upstream", "2" => "Downstream");
$MEASTYPE       = array(0x00 => "undef", 0x01 => "alpha", 0x02 => "sweep", 0x04 => "fixed", 0x08 => "ramp", 0x10 => "emit");
$BEAM_ENERGY    = array("24" => "24", "100" => "100", "250" => "250");
$TARGET_ORIENT  = array("H" => "H", "V" => "V");
$TARGET_ID      = array_combine(range(1, 6), range(1, 6));

?>
