<?php

include_once("config.php");
include_once("utils.php");

class FillResult
{
   public $fGlobResult;
   public $fRunPeriod;
   public $fFillId;
   public $fStartTime;
   public $fStopTime;
   public $fType;
   public $fBeamEnergy;

   public $fNorm             = array();

   public $fPCPolars         = array();
   public $fPCPolarP0s       = array();
   public $fPCPolarSlopes    = array();
   public $fPCProfRs         = array();
   public $fPCProfR0s        = array();
   public $fPCProfRSlopes    = array();
   public $fHJPolars         = array();
   public $fBeamPolars       = array();
   public $fBeamPolarP0s     = array();
   public $fBeamPolarSlopes  = array();
   public $fBeamProfRs       = array();
   public $fBeamProfR0s       = array();
   public $fBeamProfRSlopes       = array();
   public $fTargetOrients    = array();
   public $fTargetIds        = array();

   public $fCollPolarScale      = array();
   public $fCollBeamPolars      = array();
   public $fCollBeamPolarP0s    = array();
   public $fCollBeamPolarSlopes = array();
   public $fCollBeamPolarP0s_patched    = array();
   public $fCollBeamPolarSlopes_patched = array();


   /** */
   function FillResult($fillId, &$gR)
   { //{{{
      $this->fGlobResult = $gR;
      $this->fRunPeriod  = $this->fGlobResult->fRunPeriod;
      $this->fFillId     = $fillId;

      foreach (range(0, 3) as $polId)
      {
         $this->fPCPolars[$polId]      = new pair(0, -1);
         $this->fPCPolarP0s[$polId]    = new pair(0, -1);
         $this->fPCPolarSlopes[$polId] = new pair(0, -1);
         $this->fTargetOrients[$polId] = -1;
         $this->fTargetIds[$polId]      = -1;

         foreach (range(0, 1) as $tgtOrient)
         {
            $this->fPCProfRs[$ringId][$tgtOrient]      = new pair(0, -1);
            $this->fPCProfR0s[$ringId][$tgtOrient]     = new pair(0, -1);
            $this->fPCProfRSlopes[$ringId][$tgtOrient] = new pair(0, -1);
         }
      }

      foreach (range(1, 2) as $ringId)
      {
         $this->fHJPolars[$ringId]        = new pair(0, -1);
         $this->fCollPolarScale[$ringId]  = new pair(0, -1);
         $this->fBeamPolars[$ringId]      = new pair(0, -1);
         $this->fBeamPolarP0s[$ringId]    = new pair(0, -1);
         $this->fBeamPolarSlopes[$ringId] = new pair(0, -1);

         foreach (range(0, 1) as $tgtOrient)
         {
            $this->fBeamProfRs[$ringId][$tgtOrient] = new pair(0, -1);
            $this->fBeamProfR0s[$ringId][$tgtOrient] = new pair(0, -1);
            $this->fBeamProfRSlopes[$ringId][$tgtOrient] = new pair(0, -1);
         }
      }

   } //}}}


   /** */
   function AddMeas($row)
   { //{{{
      $this->fStartTime    = strtotime($row['start_time']);
      $this->fStopTime    = strtotime($row['stop_time']);
      $this->fType         = $row['type'];
      $this->fBeamEnergy   = $row['beam_energy'];

      global $POLARIMETER_ID, $HJ_POLARIMETER_ID;

      $polId     = $row['polarimeter_id'];
      $ringId    = $row['ring_id'];

      $this->fTargetOrients[$polId] = is_null($row['target_orient']) ? -1 : $row['target_orient'];
      $this->fTargetIds[$polId]     = is_null($row['target_id']) ? -1 : $row['target_id'];

      if ( array_key_exists($polId, $POLARIMETER_ID) )
      {
         //if ($row['polar_err'] >= 0) //!isset($this->fPCPolars[$polId]) ) // assign values once (ignore two target orientations)
         //{
         $this->fPCPolars[$polId]   = new pair($row['polar'], $row['polar_err']);
         $this->fPCPolarP0s[$polId] = new pair($row['polar_p0'], $row['polar_p0_err']);

         // invalidate polar slope if it does not exist
         if ($row['polar_slope'] == 0 && $row['polar_slope_err'] == 0)
            $this->fPCPolarSlopes[$polId] = new pair(0, -1);
         else
            $this->fPCPolarSlopes[$polId] = new pair($row['polar_slope'], $row['polar_slope_err']);

         //$this->fPCProfRs[$polId]      = new pair($row['prof_r'], $row['prof_r_err']);
         //print "<br>{$this->fFillId}<br>$polId: ".print_r($this->fPCPolars[$polId]);
         //}

         $tgtOrient = $this->fTargetOrients[$polId];

         if ($tgtOrient >= 0) {
            $this->fPCProfRs[$polId][$tgtOrient]      = new pair($row['prof_r'],      $row['prof_r_err']);
            $this->fPCProfR0s[$polId][$tgtOrient]     = new pair($row['prof_r0'],     $row['prof_r0_err']);
            $this->fPCProfRSlopes[$polId][$tgtOrient] = new pair($row['prof_rslope'], $row['prof_rslope_err']);

            $this->fBeamProfRs[$ringId][$tgtOrient] =
               calcWeigtedAvrgErrPairs($this->fBeamProfRs[$ringId][$tgtOrient], $this->fPCProfRs[$polId][$tgtOrient]);
            $this->fBeamProfR0s[$ringId][$tgtOrient] =
               calcWeigtedAvrgErrPairs($this->fBeamProfR0s[$ringId][$tgtOrient], $this->fPCProfR0s[$polId][$tgtOrient]);
            $this->fBeamProfRSlopes[$ringId][$tgtOrient] =
               calcWeigtedAvrgErrPairs($this->fBeamProfRSlopes[$ringId][$tgtOrient], $this->fPCProfRSlopes[$polId][$tgtOrient]);
         }

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
      global $POLARIMETER_ID;

      foreach (range(0, 3) as $polId)
      {
         $ringId = GetRingId($polId);

         //$this->fNorm = $doNorm ? $this->fGlobResult->fNormHJ2PCPolars[$polId]->first : 1.;
         //$this->fNorm[$polId] = $doNorm ? getHJPCNorm($this->fRunPeriod, $this->fBeamEnergy, $POLARIMETER_ID[$polId]) : new pair(1, -1);
         $this->fNorm[$polId] = $doNorm ?
            getHJPCNorm($this->fRunPeriod, $this->fBeamEnergy, $POLARIMETER_ID[$polId], $this->fTargetOrients[$polId], $this->fTargetIds[$polId]) : new pair(1, -1);
         //$this->fNorm = new pair(1, -1);

         $pcPolar = $this->fPCPolars[$polId]->ClonePair();

         if ($pcPolar->second >= 0) {
            $pcPolar->first  *= $this->fNorm[$polId]->first;
            $pcPolar->second *= $this->fNorm[$polId]->first;

            $this->fBeamPolars[$ringId] = calcWeigtedAvrgErrPairs($this->fBeamPolars[$ringId], $pcPolar);
         }

         $pcPolarP0 = $this->fPCPolarP0s[$polId]->ClonePair();

         if ($pcPolarP0->second >= 0) {
            $pcPolarP0->first  *= $this->fNorm[$polId]->first;
            $pcPolarP0->second *= $this->fNorm[$polId]->first;

            $this->fBeamPolarP0s[$ringId] = calcWeigtedAvrgErrPairs($this->fBeamPolarP0s[$ringId], $pcPolarP0);
         }

         $pcPolarSlope = $this->fPCPolarSlopes[$polId]->ClonePair();

         if ($pcPolarSlope->second >= 0)
         {
            $pcPolarSlope->first  *= $this->fNorm[$polId]->first;
            $pcPolarSlope->second *= $this->fNorm[$polId]->first;

            $this->fBeamPolarSlopes[$ringId] = calcWeigtedAvrgErrPairs($this->fBeamPolarSlopes[$ringId], $pcPolarSlope);
         }

         //print "<pre>\n";
         //print "fNorm: ".$this->fNorm;
         //print_r($this->fBeamPolars[$ringId]);
         //print "</pre>\n";
      }
   } //}}}


   /** */
   function CalcPolarCollScale()
   { //{{{
      // Calculate average between B and Y
      //$avrgProfRs = array();
      // initialize the array
      //$avrgProfRs[$tgtOrient] = new pair(0, -1);

      foreach (range(1, 2) as $ringId)
      {
         $beamProfR = array();

         foreach (range(0, 1) as $tgtOrient)
         {
            $beamProfR[$tgtOrient]  = $this->fBeamProfRs[$ringId][$tgtOrient];
            $beamProfR0[$tgtOrient] = $this->fBeamProfR0s[$ringId][$tgtOrient];

            if ( $beamProfR[$tgtOrient]->second < 0 )
            {
               $beamProfR[$tgtOrient]                       = $this->fGlobResult->fMissingBeamProfRs[$ringId][$tgtOrient][$this->fBeamEnergy];
            }

            if ( $beamProfR0[$tgtOrient]->second < 0 ||
                ($beamProfR0[$tgtOrient]->first < -1 && $beamProfR0[$tgtOrient]->first > -2) )
            {
               $this->fBeamProfR0s[$ringId][$tgtOrient]     = $this->fGlobResult->fMissingBeamProfR0s[$ringId][$tgtOrient][$this->fBeamEnergy];
               $this->fBeamProfRSlopes[$ringId][$tgtOrient] = $this->fGlobResult->fMissingBeamProfRSlopes[$ringId][$tgtOrient][$this->fBeamEnergy];
            }

            //$avrgProfRs[$tgtOrient] = calcWeigtedAvrgErrPairs($avrgProfRs[$tgtOrient], $beamProfR);
         }

         $this->fCollPolarScale[$ringId] = calcPolarCollisionScale($beamProfR[0], $beamProfR[1]);
      }

      //$profRHorz = $avrgProfRs[0];
      //$profRVert = $avrgProfRs[1];
      //$this->fCollPolarScale = calcPolarCollisionScale($profRHorz, $profRVert);

      foreach (range(1, 2) as $ringId)
      {
         if ($this->fBeamPolars[$ringId]->second > 0) {
            $this->fCollBeamPolars[$ringId]      = calcPolarCollision($this->fBeamPolars[$ringId], $this->fCollPolarScale[$ringId]);
         } else {
            $this->fCollBeamPolars[$ringId]      = new pair(0, -1);
         }

         if ($this->fBeamPolarP0s[$ringId]->second > 0) {
            $this->fCollBeamPolarP0s[$ringId]          = calcPolarCollision($this->fBeamPolarP0s[$ringId], $this->fCollPolarScale[$ringId]);
            $this->fCollBeamPolarP0s_patched[$ringId]  = calcPolarP0Collision($this->fBeamPolarP0s[$ringId], $this->fBeamProfR0s[$ringId][0], $this->fBeamProfR0s[$ringId][1]);
            $this->fCollBeamPolarP0s_patched[$ringId]->second = $this->fCollBeamPolarP0s[$ringId]->second;
         } else {
            $this->fCollBeamPolarP0s[$ringId]    = new pair(0, -1);
            $this->fCollBeamPolarP0s_patched[$ringId]    = new pair(0, -1);
         }

         if ($this->fBeamPolarSlopes[$ringId]->second > 0) {
            $this->fCollBeamPolarSlopes[$ringId]          = calcPolarCollision($this->fBeamPolarSlopes[$ringId], $this->fCollPolarScale[$ringId]);
            $this->fCollBeamPolarSlopes_patched[$ringId]  =
               calcPolarSlopeCollision($this->fBeamPolarP0s[$ringId], $this->fBeamPolarSlopes[$ringId], $this->fBeamProfR0s[$ringId], $this->fBeamProfSlopes[$ringId]);
            $this->fCollBeamPolarSlopes_patched[$ringId]->second = $this->fCollBeamPolarSlopes[$ringId]->second;
         } else {
            $this->fCollBeamPolarSlopes[$ringId] = new pair(0, -1);
            $this->fCollBeamPolarSlopes_patched[$ringId] = new pair(0, -1);
         }
      }
   } //}}}


   /** */
   function FormatAsCSV()
   { //{{{

      $collBeamPolarSlopeBlu = $this->fCollBeamPolarSlopes[1]->second >= 0 ?
                               $this->fCollBeamPolarSlopes_patched[1] : $this->fGlobResult->fMissingBeamPolarSlopes[1][$this->fBeamEnergy];

      $collBeamPolarSlopeYel = $this->fCollBeamPolarSlopes[2]->second >= 0 ?
                               $this->fCollBeamPolarSlopes_patched[2] : $this->fGlobResult->fMissingBeamPolarSlopes[2][$this->fBeamEnergy];

      return sprintf("%10d %10d %12d %16s %16s %16s %16s %16s %16s\n",
             $this->fFillId, $this->fBeamEnergy, $this->fStartTime,
             polarPairToString($this->fCollBeamPolars[1], "", "%5.2f", false), polarPairToString($this->fCollBeamPolarP0s_patched[1], "", "%5.2f", false), polarPairToString($collBeamPolarSlopeBlu, "", "%5.2f", false),
             polarPairToString($this->fCollBeamPolars[2], "", "%5.2f", false), polarPairToString($this->fCollBeamPolarP0s_patched[2], "", "%5.2f", false), polarPairToString($collBeamPolarSlopeYel, "", "%5.2f", false) );
   } //}}}
}

?>
