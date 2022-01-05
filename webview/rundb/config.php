<?php

include_once("Pair.php");


define("DATA_DIR", "/gpfs/mnt/gpfs02/eic/eichome/cnipol/root");
//define("LOG_DIR",  "/home/cnipol/log");
//define("VIDEO_DIR",  "/home/cnipol/video");

// Approximate fill numbers
$RUN_PERIOD_BY_FILL = array (
   9  => array( "start" => 10018, "end" => 11100 ),
   11 => array( "start" => 14000, "end" => 15500 ),
   12 => array( "start" => 16000, "end" => 16800 ),
   13 => array( "start" => 17000, "end" => 18000 ),
   15 => array( "start" => 18000, "end" => 20000 ),
   17 => array( "start" => 20000, "end" => 22000),
   22 => array( "start" => 32000, "end" => 34000),
);

// Approximate dates
$RUN_PERIOD_BY_DATE = array (
   9  => array( "start" => "2009-01-01 00:00:00", "end" => "2009-07-10 00:00:00" ),
   11 => array( "start" => "2010-12-01 00:00:00", "end" => "2011-04-20 00:00:00" ),
   12 => array( "start" => "2011-11-01 00:00:00", "end" => "2012-06-01 00:00:00" ),
   13 => array( "start" => "2013-01-01 00:00:00", "end" => "2013-08-01 00:00:00" ),
   15 => array( "start" => "2015-01-01 00:00:00", "end" => "2015-08-01 00:00:00" ),
   17 => array( "start" => "2017-01-01 00:00:00", "end" => "2017-08-01 00:00:00" ),
   22 => array( "start" => "2021-11-01 00:00:00", "end" => "2022-05-01 00:00:00" ),
);

$RUN_PERIOD        = array(9 => "Run 9", 11 => "Run 11", 12 => "Run 12", 13 => "Run 13", 15 => "Run 15", 17 => "Run 17", 22 => "Run 22");
$POLARIMETER_ID    = array(0 => "B1U", 1 => "Y1D", 2 => "B2D", 3 => "Y2U");
$HJ_POLARIMETER_ID = 10;
$AGS_POLARIMETER_ID = 11;
$RHIC_RING         = array("1" => "Blue", "2" => "Yellow");
$RHIC_STREAM       = array("1" => "Upstream", "2" => "Downstream");
$MEASTYPE          = array(0x00 => "undef", 0x01 => "alpha", 0x02 => "sweep", 0x04 => "fixed", 0x08 => "ramp", 0x10 => "emit");
$FILLTYPE          = array(0x00 => "undef", 0x01 => "phys");
$BEAM_ENERGY       = array("24" => "24", "100" => "100", "104" => "104","250" => "250", "255" => "255");
$BEAM_ENERGY_INJ   = 24;
$BEAM_ENERGY_254   = 254;
$TARGET_ORIENT     = array("H" => "H", "V" => "V");
//$TARGET_ORIENT_ID  = array(0 => "H", "V" => "V");
$TARGET_ID         = array_combine(range(1, 6), range(1, 6));
$MODE              = array("1" => "user", "11" => "user (csv)", "2" => "expert");

$PLOT_IMAGES       = array (
   "profile/hIntensProfile"  => "IntensProfile",
   "profile/hPolarVsIntensProfile" => "PolarVsIntensProfile",
   "asym/hAsymVsBunchId_X90" => "AsymVsBunchId_X90",
   "raw/hTvsA"               => "TvsA",
   "raw_eb/hTvsA"            => "TvsA (empty bunch)"
);


// Normalization factors

$normJetCarbonByTarget[11][250]["B1U"] = 0.934;
$normJetCarbonByTarget[11][250]["Y1D"] = 0.909;
$normJetCarbonByTarget[11][250]["B2D"] = 1.058;
$normJetCarbonByTarget[11][250]["Y2U"] = 0.933;

$normJetCarbonByTarget[12][255]["B1U"] = 1.016;
$normJetCarbonByTarget[12][255]["Y1D"] = 1.073;
$normJetCarbonByTarget[12][255]["B2D"] = 1.197;
$normJetCarbonByTarget[12][255]["Y2U"] = 0.994;

// Run 9
/* $normHJPC[9][100]['B1U']['nomi'] = new pair(   1.00350,    0.01672); */
/* $normHJPC[9][100]['Y1D']['nomi'] = new pair(   1.00494,    0.01126); */
/* $normHJPC[9][100]['B2D']['nomi'] = new pair(   0.97621,    0.01097); */
/* $normHJPC[9][100]['Y2U']['nomi'] = new pair(   1.01649,    0.01183); */

$normHJPC[9][100]['B1U']['nomi'] = new pair(   1.001,  0.016);
$normHJPC[9][100]['Y1D']['nomi'] = new pair(   1.005,  0.011 );
$normHJPC[9][100]['B2D']['nomi'] = new pair(   0.9767, 0.0108);
$normHJPC[9][100]['Y2U']['nomi'] = new pair(   1.015,  0.012 );

/* $normHJPC[9][250]['B1U']['nomi'] = new pair(   0.89197,    0.02712); */
/* $normHJPC[9][250]['Y1D']['nomi'] = new pair(   0.91179,    0.02892); */
/* $normHJPC[9][250]['B2D']['nomi'] = new pair(   0.88177,    0.02703); */
/* $normHJPC[9][250]['Y2U']['nomi'] = new pair(   0.86630,    0.02722); */
$normHJPC[9][250]['B1U']['nomi'] = new pair( 0.8814  , 0.0258   );
$normHJPC[9][250]['Y1D']['nomi'] = new pair( 0.9097  , 0.0267   );
$normHJPC[9][250]['B2D']['nomi'] = new pair( 0.8625  , 0.0279   );
$normHJPC[9][250]['Y2U']['nomi'] = new pair( 0.8836  , 0.0258   );

// Run 11
/* $normHJPC[11][250]['B1U']['nomi'] = new pair(   0.95675,    0.01368); */
/* $normHJPC[11][250]['Y1D']['nomi'] = new pair(   0.96189,    0.01753); */
/* $normHJPC[11][250]['B2D']['nomi'] = new pair(   1.05085,    0.02548); */
/* $normHJPC[11][250]['B2D']['V'][4] = new pair(   1.11481,    0.04070); */
/* $normHJPC[11][250]['B2D']['V'][6] = new pair(   1.20940,    0.03638); */
/* $normHJPC[11][250]['Y2U']['nomi'] = new pair(   0.95513,    0.01278); */

$normHJPC[11][250]['B1U']['nomi'] = new pair(   0.9572,    0.0133);
$normHJPC[11][250]['Y1D']['nomi'] = new pair(   0.9604,    0.0174 );
$normHJPC[11][250]['B2D']['nomi'] = new pair(   1.102 ,    0.018 );
$normHJPC[11][250]['Y2U']['nomi'] = new pair(   0.9551,    0.0128 );

// Run 12
/* normHJPC[12][100]['B1U']['nomi'] = new pair(   1.04706,    0.01295); */
/* $normHJPC[12][100]['Y1D']['nomi'] = new pair(   1.02821,    0.01557); */
/* $normHJPC[12][100]['B2D']['nomi'] = new pair(   1.12185,    0.02087); */
/* $normHJPC[12][100]['Y2U']['nomi'] = new pair(   0.97165,    0.01640); */
$normHJPC[12][100]['B1U']['nomi'] = new pair(   1.040,    0.01);
$normHJPC[12][100]['Y1D']['nomi'] = new pair(   1.018,    0.015);
$normHJPC[12][100]['B2D']['nomi'] = new pair(   1.113,    0.020);
$normHJPC[12][100]['Y2U']['nomi'] = new pair(   0.9968,    0.0198);

/* $normHJPC[12][255]['B1U']['nomi'] = new pair(   0.99292,    0.01359); */
/* $normHJPC[12][255]['Y1D']['nomi'] = new pair(   1.08502,    0.01481); */
/* $normHJPC[12][255]['B2D']['nomi'] = new pair(   1.12329,    0.02078); */
/* $normHJPC[12][255]['Y2U']['nomi'] = new pair(   0.99830,    0.01658); */
$normHJPC[12][255]['B1U']['nomi'] = new pair(  0.971,  0.013);
$normHJPC[12][255]['Y1D']['nomi'] = new pair(  1.088,  0.015);
$normHJPC[12][255]['B2D']['nomi'] = new pair(  1.099,  0.019);
$normHJPC[12][255]['Y2U']['nomi'] = new pair(  0.9966, 0.0155);

// Run 13
//$normHJPC[13][24]['B1U']['nomi'] = new pair(   1.07559,    0.02920);
//$normHJPC[13][24]['Y1D']['nomi'] = new pair(   0.98324,    0.03519);
//$normHJPC[13][24]['B2D']['nomi'] = new pair(   1.01772,    0.09418);
//$normHJPC[13][24]['Y2U']['nomi'] = new pair(   1.03182,    0.04138);
//$normHJPC[13][255]['B1U']['nomi'] = new pair(   1.07559,    0.02920);
//$normHJPC[13][255]['Y1D']['nomi'] = new pair(   0.98324,    0.03519);
//$normHJPC[13][255]['B2D']['nomi'] = new pair(   1.01772,    0.09418);
//$normHJPC[13][255]['Y2U']['nomi'] = new pair(   1.03182,    0.04138);
$normHJPC[13][24]['B1U']['nomi'] = new pair(   1.06,    0.02);
$normHJPC[13][24]['Y1D']['nomi'] = new pair(   1.09,    0.02);
$normHJPC[13][24]['B2D']['nomi'] = new pair(   1.11,    0.03);
$normHJPC[13][24]['Y2U']['nomi'] = new pair(   1.08,    0.03);
/* $normHJPC[13][255]['B1U']['nomi'] = new pair(  1.06,    0.02); */
/* $normHJPC[13][255]['Y1D']['nomi'] = new pair(  1.09,    0.02); */
/* $normHJPC[13][255]['B2D']['nomi'] = new pair(  1.11,    0.03); */
/* $normHJPC[13][255]['Y2U']['nomi'] = new pair(  1.08,    0.03); */
$normHJPC[13][255]['B1U']['nomi'] = new pair(  0.9824,    0.0104);
$normHJPC[13][255]['Y1D']['nomi'] = new pair(  0.9552,    0.0094);
$normHJPC[13][255]['B2D']['nomi'] = new pair(  0.9671,    0.0090);
$normHJPC[13][255]['Y2U']['nomi'] = new pair(  0.9503,    0.0099);
/*run15 same as run13 needs update*/
$normHJPC[15][24]['B1U']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[15][24]['Y1D']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[15][24]['B2D']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[15][24]['Y2U']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[15][100]['B1U']['nomi'] = new pair(  0.957,    0.006);
$normHJPC[15][100]['Y1D']['nomi'] = new pair(  1.049,    0.007);
$normHJPC[15][100]['B2D']['nomi'] = new pair(  0.976,    0.006);
$normHJPC[15][100]['Y2U']['nomi'] = new pair(  1.026,    0.006);
$normHJPC[15][104]['B1U']['nomi'] = new pair(  0.997,    0.009);
$normHJPC[15][104]['Y1D']['nomi'] = new pair(  1.0,    -1.0);
$normHJPC[15][104]['B2D']['nomi'] = new pair(  0.980,    0.008);
$normHJPC[15][104]['Y2U']['nomi'] = new pair(  1.0,    -1.0);
/*run17 set to 1*/
$normHJPC[17][24]['B1U']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[17][24]['Y1D']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[17][24]['B2D']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[17][24]['Y2U']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[17][255]['B1U']['nomi'] = new pair(  0.960,    0.009);
$normHJPC[17][255]['Y1D']['nomi'] = new pair(  0.991,    0.009);
$normHJPC[17][255]['B2D']['nomi'] = new pair(  0.990,    0.010);
$normHJPC[17][255]['Y2U']['nomi'] = new pair(  1.022,    0.010);
/* updated with final hjet numbers 2018/06/21 */
$normHJPC[17][255]['B1U']['H'][2] = new pair( 1.00242, 0.07597 );
$normHJPC[17][255]['B1U']['H'][4] = new pair( 0.93785, 0.03126 );
$normHJPC[17][255]['B1U']['H'][6] = new pair( 0.96630, 0.04218 );
$normHJPC[17][255]['B1U']['V'][1] = new pair( 0.97667, 0.01591 );
$normHJPC[17][255]['B1U']['V'][3] = new pair( 0.97891, 0.01805 );
$normHJPC[17][255]['B1U']['V'][4] = new pair( 0.93936, 0.02402 );
$normHJPC[17][255]['B1U']['V'][5] = new pair( 0.95206, 0.05173 );
$normHJPC[17][255]['B1U']['V'][6] = new pair( 0.98427, 0.00827 );
$normHJPC[17][255]['Y1D']['H'][1] = new pair( 1.05235, 0.01519 );
$normHJPC[17][255]['Y1D']['H'][2] = new pair( 0.98226, 0.01426 );
$normHJPC[17][255]['Y1D']['H'][3] = new pair( 0.94749, 0.02984 );
$normHJPC[17][255]['Y1D']['H'][5] = new pair( 1.00086, 0.03996 );
$normHJPC[17][255]['Y1D']['V'][1] = new pair( 0.93822, 0.02946 );
$normHJPC[17][255]['Y1D']['V'][2] = new pair( 1.02758, 0.01128 );
$normHJPC[17][255]['Y1D']['V'][3] = new pair( 0.87299, 0.07159 );
$normHJPC[17][255]['Y1D']['V'][4] = new pair( 1.01064, 0.03569 );
$normHJPC[17][255]['Y1D']['V'][5] = new pair( 0.91551, 0.02209 );
$normHJPC[17][255]['Y1D']['V'][6] = new pair( 0.96267, 0.01687 );
$normHJPC[17][255]['B2D']['H'][1] = new pair( 0.98928, 0.01989 );
$normHJPC[17][255]['B2D']['H'][2] = new pair( 1.02879, 0.01254 );
$normHJPC[17][255]['B2D']['H'][4] = new pair( 1.05944, 0.01130 );
$normHJPC[17][255]['B2D']['V'][1] = new pair( 1.01651, 0.02131 );
$normHJPC[17][255]['Y2U']['H'][1] = new pair( 1.02950, 0.01150 );
$normHJPC[17][255]['Y2U']['H'][2] = new pair( 1.03245, 0.01997 );
$normHJPC[17][255]['Y2U']['H'][4] = new pair( 0.94707, 0.02233 );
$normHJPC[17][255]['Y2U']['V'][1] = new pair( 1.02009, 0.00902 );
//run-22 set to 1
$normHJPC[22][24]['B1U']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[22][24]['Y1D']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[22][24]['B2D']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[22][24]['Y2U']['nomi'] = new pair(   1.0,    -1.0);
$normHJPC[22][255]['B1U']['nomi'] = new pair(  0.93,    0.04);
$normHJPC[22][255]['Y1D']['nomi'] = new pair(  0.94,    0.03);
$normHJPC[22][255]['B2D']['nomi'] = new pair(  0.96,    0.04);
$normHJPC[22][255]['Y2U']['nomi'] = new pair(  1.00,    0.03);
$normHJPC[22][255]['B1U']['H'][2] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['H'][4] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['H'][6] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['V'][1] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['V'][3] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['V'][4] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['V'][5] = new pair( 1.0, -1 );
$normHJPC[22][255]['B1U']['V'][6] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['H'][1] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['H'][2] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['H'][3] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['H'][5] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['V'][1] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['V'][2] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['V'][3] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['V'][4] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['V'][5] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y1D']['V'][6] = new pair( 1.0, -1 );
$normHJPC[22][255]['B2D']['H'][1] = new pair( 1.0, -1 );
$normHJPC[22][255]['B2D']['H'][2] = new pair( 1.0, -1 );
$normHJPC[22][255]['B2D']['H'][4] = new pair( 1.0, -1 );
$normHJPC[22][255]['B2D']['V'][1] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y2U']['H'][1] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y2U']['H'][2] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y2U']['H'][4] = new pair( 1.0, -1 );
$normHJPC[22][255]['Y2U']['V'][1] = new pair( 1.0, -1 );

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
