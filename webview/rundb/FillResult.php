<?php

include_once("config.php");

class FillResult
{
   var $fGlobResult;
   var $fFillId;
   var $fType;
   var $fBeamEnergy;

   var $fPCPolars        = array();
   var $fPCPolarP0s      = array();
   var $fPCPolarSlopes   = array();
   var $fPCProfRs        = array();
   var $fHJPolars        = array();
   var $fBeamPolars      = array();
   var $fBeamPolarP0s    = array();
   var $fBeamPolarSlopes = array();
   var $fBeamProfRs      = array();

   var $fCollPolarScale;
   var $fCollBeamPolars  = array();


   /** */
   function FillResult($fillId, &$gR)
   { //{{{
      $this->fGlobResult = $gR;
      $this->fFillId     = $fillId;

      foreach (range(0, 3) as $polId)
      {
         $this->fPCPolars[$polId]      = new pair(0, -1);
         $this->fPCPolarP0s[$polId]    = new pair(0, -1);
         $this->fPCPolarSlopes[$polId] = new pair(0, -1);

         foreach (range(0, 1) as $tgtOrient)
         {
            $this->fPCProfRs[$ringId][$tgtOrient] = new pair(0, -1);
         }
      }

      foreach (range(1, 2) as $ringId)
      {
         $this->fHJPolars[$ringId]        = new pair(0, -1);
         $this->fBeamPolars[$ringId]      = new pair(0, -1);
         $this->fBeamPolarP0s[$ringId]    = new pair(0, -1);
         $this->fBeamPolarSlopes[$ringId] = new pair(0, -1);

         foreach (range(0, 1) as $tgtOrient)
         {
            $this->fBeamProfRs[$ringId][$tgtOrient] = new pair(0, -1);
         }
      }

      $this->fCollPolarScale = new pair(0, -1);
   } //}}}


   /** */
   function AddMeas($row)
   { //{{{
      $this->fType       = $row['type'];
      $this->fBeamEnergy = $row['beam_energy'];

      global $POLARIMETER_ID, $HJ_POLARIMETER_ID;

      $polId     = $row['polarimeter_id'];
      $ringId    = $row['ring_id'];
      $tgtOrient = $row['target_orient'];

      if ( array_key_exists($polId, $POLARIMETER_ID) )
      {
         if ($row['polar_err'] > 0) //!isset($this->fPCPolars[$polId]) ) // assign values once (ignore two target orientations)
         {
            $this->fPCPolars[$polId]      = new pair($row['polar'], $row['polar_err']);
            $this->fPCPolarP0s[$polId]    = new pair($row['polar_p0'], $row['polar_p0_err']);
            $this->fPCPolarSlopes[$polId] = new pair($row['polar_slope'], $row['polar_slope_err']);
            //$this->fPCProfRs[$polId]      = new pair($row['prof_r'], $row['prof_r_err']);
            //print "<br>$polId: ".print_r($this->fPCPolars[$polId]);
         }

         $this->fPCProfRs[$polId][$tgtOrient] = new pair($row['prof_r'], $row['prof_r_err']);
         $this->fBeamProfRs[$ringId][$tgtOrient] = calcWeigtedAvrgErrPairs($this->fBeamProfRs[$ringId][$tgtOrient], $this->fPCProfRs[$polId][$tgtOrient]);

      } else if ($polId == $HJ_POLARIMETER_ID) {
         $this->fHJPolars[$ringId] = new pair($row['polar'], $row['polar_err']);
      }

      //print "<br>".count($this->fPCPolars);
      //print " ".count($this->fHJPolars);
   } //}}}


   /** */
   function GetBeamPolar($ringId)
   { //{{{
   } //}}}


   /** */
   function CalcBeamPolar($doNorm=false)
   { //{{{
      foreach (range(0, 3) as $polId)
      {
         $ringId = GetRingId($polId);

         $norm = $doNorm ? $this->fGlobResult->fNormHJ2PCPolars[$polId]->first : 1.;

         $pcPolar = $this->fPCPolars[$polId];

         if ($pcPolar->second >= 0) {
            $pcPolar->first  *= $norm;
            $pcPolar->second *= $norm;

            $this->fBeamPolars[$ringId] = calcWeigtedAvrgErrPairs($this->fBeamPolars[$ringId], $pcPolar);
         }

         $pcPolarP0 = $this->fPCPolarP0s[$polId];

         if ($pcPolarP0->second >= 0) {
            $pcPolarP0->first  *= $norm;
            $pcPolarP0->second *= $norm;

            $this->fBeamPolarP0s[$ringId] = calcWeigtedAvrgErrPairs($this->fBeamPolarP0s[$ringId], $pcPolarP0);
         }

         $pcPolarSlope = $this->fPCPolarSlopes[$polId];

         if ($pcPolarSlope->second >= 0) {

            $this->fBeamPolarSlopes[$ringId] = calcWeigtedAvrgErrPairs($this->fBeamPolarSlopes[$ringId], $pcPolarSlope);
         }

         //print "<pre>\n";
         //print "norm: ".$norm;
         //print_r($this->fBeamPolars[$ringId]);
         //print "</pre>\n";
      }
   } //}}}


   /** */
   function CalcPolarCollScale()
   { //{{{
      // Calculate average between B and Y
      $avrgProfRs = array();

      foreach (range(0, 1) as $tgtOrient)
      {
         // initialize the array
         $avrgProfRs[$tgtOrient] = new pair(0, -1);

         foreach (range(1, 2) as $ringId)
         {
            $beamProfR = $this->fBeamProfRs[$ringId][$tgtOrient];
            $beamProfR = $beamProfR->second < 0 ? $this->fGlobResult->fMissedBeamProfRs[$ringId][$tgtOrient] : $beamProfR;
            $avrgProfRs[$tgtOrient] = calcWeigtedAvrgErrPairs($avrgProfRs[$tgtOrient], $beamProfR);
         }
      }

      $profRHorz = $avrgProfRs[0];
      $profRVert = $avrgProfRs[1];

      $this->fCollPolarScale = calcPolarCollisionScale($profRHorz, $profRVert);

      foreach (range(1, 2) as $ringId)
      {
         $this->fCollBeamPolars[$ringId] = calcPolarCollision($this->fBeamPolars[$ringId], $this->fCollPolarScale);
      }
   } //}}}

}

?>
