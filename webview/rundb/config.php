<?php

define("DATA_DIR", "/usr/local/polarim/root");
define("LOG_DIR",  "/usr/local/polarim/log");

// Approximate fill numbers
$RUN_PERIOD_BY_FILL = array (
   9  => array( "start" => 10000, "end" => 11100 ),
   11 => array( "start" => 14000, "end" => 15500 ),
   12 => array( "start" => 16000, "end" => 16800 )
);

// Approximate dates
$RUN_PERIOD_BY_DATE = array (
   9  => array( "start" => "2009-03-01 00:00:00", "end" => "2009-07-10 00:00:00" ),
   11 => array( "start" => "2010-12-01 00:00:00", "end" => "2011-04-20 00:00:00" ),
   12 => array( "start" => "2011-11-01 00:00:00", "end" => "2012-06-01 00:00:00" )
);

$RUN_PERIOD      = array(9 => "Run 9", 11 => "Run 11", 12 => "Run 12");
$POLARIMETER_ID  = array(0 => "B1U", 1 => "Y1D", 2 => "B2D", 3 => "Y2U");
$RHIC_BEAM       = array("1" => "Yellow", "2" => "Blue");
$RHIC_STREAM     = array("1" => "Upstream", "2" => "Downstream");
$MEASTYPE        = array(0x00 => "undef", 0x01 => "alpha", 0x02 => "sweep", 0x04 => "fixed", 0x08 => "ramp", 0x10 => "emit");
$BEAM_ENERGY     = array("24" => "24", "100" => "100", "250" => "250", "255" => "255");
$BEAM_ENERGY_INJ = 24;
$TARGET_ORIENT   = array("H" => "H", "V" => "V");
$TARGET_ID       = array_combine(range(1, 6), range(1, 6));


// Normalization factors
//$normJetCarbonByTarget[12][255] = 0.9585;       // the average of all polarimeters in run 11 
//$normJetCarbonByTarget[12][100] = 1;
//$normJetCarbonByTarget[12][255] = 0.9585;       // the average of all polarimeters in run 11 
//$normJetCarbonByTarget[12][100] = 1;

$normJetCarbonByTarget[12][255]["B1U"] = 1.016;
$normJetCarbonByTarget[12][255]["Y1D"] = 1.073;
$normJetCarbonByTarget[12][255]["B2D"] = 1.197;
$normJetCarbonByTarget[12][255]["Y2U"] = 0.994;

$normJetCarbonByTarget[11][250]["B1U"] = 0.934;
$normJetCarbonByTarget[11][250]["Y1D"] = 0.909;
$normJetCarbonByTarget[11][250]["B2D"] = 1.058;
$normJetCarbonByTarget[11][250]["Y2U"] = 0.933;

//$normJetCarbonByTarget[11][250]2U"] = 0.933;

//$normJetCarbonByTarget[11][250]["B1U"]["H"][1] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["H"][2] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["H"][3] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["H"][4] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["H"][5] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["H"][6] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["V"][1] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["V"][2] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["V"][3] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["V"][4] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["V"][5] = 0.934;
//$normJetCarbonByTarget[11][250]["B1U"]["V"][6] = 0.934;
//$normJetCarbonByTarget[11][250]["Y1D"]["H"][1] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["H"][2] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["H"][3] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["H"][4] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["H"][5] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["H"][6] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["V"][1] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["V"][2] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["V"][3] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["V"][4] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["V"][5] = 0.909;
//$normJetCarbonByTarget[11][250]["Y1D"]["V"][6] = 0.909;
//$normJetCarbonByTarget[11][250]["B2D"]["H"][1] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["H"][2] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["H"][3] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["H"][4] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["H"][5] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["H"][6] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["V"][1] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["V"][2] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["V"][3] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["V"][4] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["V"][5] = 1.058;
//$normJetCarbonByTarget[11][250]["B2D"]["V"][6] = 1.058;
//$normJetCarbonByTarget[11][250]["Y2U"]["H"][1] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["H"][2] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["H"][3] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["H"][4] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["H"][5] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["H"][6] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["V"][1] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["V"][2] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["V"][3] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["V"][4] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["V"][5] = 0.933;
//$normJetCarbonByTarget[11][250]["Y2U"]["V"][6] = 0.933;

// sigma = fill-to-fill uncorrelated errors
// delta = fill-to-fill correlated errors
$relSystErr["B1U"]["sigma"]["norm"]    = 0.070;
$relSystErr["B1U"]["delta"]["norm"]    = 0.011;
$relSystErr["B1U"]["delta"]["jet_bkg"] = 0.020;
$relSystErr["Y1D"]["sigma"]["norm"]    = 0.063;
$relSystErr["Y1D"]["delta"]["norm"]    = 0.015;
$relSystErr["Y1D"]["delta"]["jet_bkg"] = 0.020;
$relSystErr["B2D"]["sigma"]["norm"]    = 0.101;
$relSystErr["B2D"]["delta"]["norm"]    = 0.012;
$relSystErr["B2D"]["delta"]["jet_bkg"] = 0.020;
$relSystErr["Y2U"]["sigma"]["norm"]    = 0.054;
$relSystErr["Y2U"]["delta"]["norm"]    = 0.011;
$relSystErr["Y2U"]["delta"]["jet_bkg"] = 0.020;

?>
