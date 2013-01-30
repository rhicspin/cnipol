<?php

include_once("FillResult.php");

class GlobResult
{
   public $fRunPeriod;
   var $fFillResults     = array();
   var $fBeamEnergies    = array();
   var $fAvrgPCPolars    = array();
   var $fAvrgPCnHJPolars = array(); // average of PC measurements given HJ measurement is available
   var $fAvrgHJPolars    = array();
   var $fAvrgHJnPCPolars = array();
   var $fNormHJ2PCPolars = array();
   var $fAvrgBeamProfRs  = array();

   var $fMissingBeamProfRs      = array();
   var $fMissingBeamProfR0s      = array();
   var $fMissingBeamProfRSlopes      = array();
   var $fMissingBeamPolarSlopes = array();


   /** */
   function GlobResult($rp)
   { //{{{
      $this->fRunPeriod = $rp;

      foreach (range(0, 3) as $polId)
      {
         $this->fAvrgPCPolars[$polId]    = new pair(0, -1);
         $this->fAvrgPCnHJPolars[$polId] = new pair(0, -1);
         $this->fAvrgHJnPCPolars[$polId] = new pair(0, -1);
         $this->fNormHJ2PCPolars[$polId] = new pair(1, -1);
      }

      foreach (range(1, 2) as $ringId)
      {
         $this->fAvrgHJPolars[$ringId] = new pair(0, -1);

         foreach (range(0, 1) as $tgtOrient)
         {
            $this->fAvrgBeamProfRs[$ringId][$tgtOrient]   = new pair(0, -1);
            //$this->fMissingBeamProfRs[$ringId][$tgtOrient] = new pair(0, -1);
         }
      }
   } //}}}


   /** */
   function AddMeas($row)
   { //{{{
      $fillId = $row['fill'];

      if ( array_key_exists($fillId, $this->fFillResults) )
      {
         $this->fFillResults[$fillId]->AddMeas($row);
      } else {
         $this->fFillResults[$fillId] = new FillResult($fillId, $this);
         $this->fFillResults[$fillId]->AddMeas($row);
      }
   } //}}}


   /** */
   function Process()
   { //{{{
      $beamProfRSet = array();
      $beamProfR0Set = array();
      $beamProfRSlopeSet = array();

      foreach ( $this->fFillResults as $fillResult)
      {
         // Process results by polarimeter
         foreach (range(0, 3) as $polId)
         {
            $ringId = GetRingId($polId);

            $pcPolar = $fillResult->fPCPolars[$polId];
            //if ($pcPolar->second < 0) continue;
            $this->fAvrgPCPolars[$polId] = calcWeigtedAvrgErrPairs($this->fAvrgPCPolars[$polId], $pcPolar);

            $hjPolar = $fillResult->fHJPolars[$ringId];
            //if ($hjPolar->second < 0) continue;
            $this->fAvrgPCnHJPolars[$polId] = calcWeigtedAvrgErrPairs($this->fAvrgPCnHJPolars[$polId], $pcPolar);
            $this->fAvrgHJnPCPolars[$polId] = calcWeigtedAvrgErrPairs($this->fAvrgHJnPCPolars[$polId], $hjPolar);

            // Now we can calculate the ratio of HJ to PC polar
            $norm = calcDivision($hjPolar, $pcPolar);
            $this->fNormHJ2PCPolars[$polId] = calcWeigtedAvrgErrPairs($this->fNormHJ2PCPolars[$polId], $norm);
         }

         // Process results by ring
         foreach (range(1, 2) as $ringId)
         {
            $hjPolar = $fillResult->fHJPolars[$ringId];
            //if ($hjPolar->second < 0) continue;
            $this->fAvrgHJPolars[$ringId] = calcWeigtedAvrgErrPairs($this->fAvrgHJPolars[$ringId], $hjPolar);

            foreach (range(0, 1) as $tgtOrient)
            {
               $beamProfR  = $fillResult->fBeamProfRs[$ringId][$tgtOrient];
               $beamEnergy = $fillResult->fBeamEnergy;

               if ($beamProfR->second >= 0) {
                  $beamProfRSet[$ringId][$tgtOrient][$beamEnergy][] = $beamProfR;
                  $beamProfR0Set[$ringId][$tgtOrient][$beamEnergy][] = $fillResult->fBeamProfR0s[$ringId][$tgtOrient];
                  $beamProfRSlopeSet[$ringId][$tgtOrient][$beamEnergy][] = $fillResult->fBeamProfRSlopes[$ringId][$tgtOrient];
               }
  
               $this->fAvrgBeamProfRs[$ringId][$tgtOrient] = calcWeigtedAvrgErrPairs($this->fAvrgBeamProfRs[$ringId][$tgtOrient], $beamProfR);
            }
         }
      }

      //print "<pre>\n";
      //print_r($beamProfRSet);
      //print "</pre>\n";

      // calculate profile R value for missing measurements
      foreach (range(1, 2) as $ringId)
      {
         foreach (range(0, 1) as $tgtOrient)
         {
            //$ve = new pair(calcWeigtedAvrgErr($beamProfRSet[$ringId][$tgtOrient]), calcWeigtedStdDev( $beamProfRSet[$ringId][$tgtOrient]));
            //$this->fMissingBeamProfRs[$ringId][$tgtOrient] = $ve;

            $beamProfRSetByRingByTgt = $beamProfRSet[$ringId][$tgtOrient];

            foreach ($beamProfRSetByRingByTgt as $beamEnergy => $beamProfRSetByBeamEnergy)
            {
               $v = calcWeigtedAvrgErr($beamProfRSetByBeamEnergy)->first;
               $e = calcWeigtedStdDev( $beamProfRSetByBeamEnergy)->first;

               $this->fMissingBeamProfRs[$ringId][$tgtOrient][$beamEnergy] = new pair($v, $e);

               $v = calcWeigtedAvrgErr($beamProfR0Set[$ringId][$tgtOrient][$beamEnergy])->first;
               $e = calcWeigtedStdDev( $beamProfR0Set[$ringId][$tgtOrient][$beamEnergy])->first;

               $this->fMissingBeamProfR0s[$ringId][$tgtOrient][$beamEnergy] = new pair($v, $e);

               $v = calcWeigtedAvrgErr($beamProfRSlopeSet[$ringId][$tgtOrient][$beamEnergy])->first;
               $e = calcWeigtedStdDev( $beamProfRSlopeSet[$ringId][$tgtOrient][$beamEnergy])->first;

               $this->fMissingBeamProfRSlopes[$ringId][$tgtOrient][$beamEnergy] = new pair($v, $e);

               //print "<pre>\n";
               //print_r($this->fMissingBeamProfRs[$ringId][$tgtOrient]);
               //print "</pre>\n";
            }
         }
      }
   } //}}}


   /**
    * Another loop over the fills. This time the normalization is available and
    * we can use it.
    */
   function CalcDependencies()
   { //{{{

      $beamPolarSlopeSet = array(); //[ringId][beamEnergy]

      foreach ( $this->fFillResults as $fillResult)
      {
         $fillResult->CalcBeamPolar(true);      // normalize Pavrg, P0, Pslope and calculate the averate Pavrg, P0, Pslope per beam
         //$fillResult->CalcBeamPolar(false);
         $fillResult->CalcPolarCollScale();

         $beamEnergy = $fillResult->fBeamEnergy;

         // Save all available measurements of Polar slope to a set
         foreach (range(1, 2) as $ringId)
         {
            if ($fillResult->fCollBeamPolarSlopes[$ringId]->second > 0) 
               $beamPolarSlopeSet[$ringId][$beamEnergy][] = $fillResult->fCollBeamPolarSlopes[$ringId];
         }
      }

      //print "<pre>\n";
      //print_r($beamPolarSlopeSet);
      //print "</pre>\n";

      // Calculate the mean and the standard deviation from the available polar slopes
      // Assign and same the missing polar slopes
      foreach (range(1, 2) as $ringId)
      {
         $beamPolarSlopeSetByRing = $beamPolarSlopeSet[$ringId];

         foreach ($beamPolarSlopeSetByRing as $beamEnergy => $beamPolarSlopeSetByBeamEnergy)
         {
            $v = calcWeigtedAvrgErr($beamPolarSlopeSetByBeamEnergy)->first;
            $e = calcWeigtedStdDev( $beamPolarSlopeSetByBeamEnergy)->first;

            $this->fMissingBeamPolarSlopes[$ringId][$beamEnergy] = new pair($v, $e);
         }
      }
   } //}}}

}

?>
