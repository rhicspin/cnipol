<?php

include_once("FillResult.php");

class GlobResult
{
   var $fFillResults     = array();
   var $fAvrgPCPolars    = array();
   var $fAvrgPCnHJPolars = array();
   var $fAvrgHJPolars    = array();
   var $fAvrgHJnPCPolars = array();
   var $fNormHJ2PCPolars = array();
   var $fAvrgBeamProfRs  = array();

   var $fMissedBeamProfRs = array();


   /** */
   function GlobResult()
   { //{{{
      foreach (range(0, 3) as $polId)
      {
         $this->fAvrgPCPolars[$polId]    = new pair(0, -1);
         $this->fAvrgPCnHJPolars[$polId] = new pair(0, -1);
         $this->fAvrgHJnPCPolars[$polId] = new pair(0, -1);
         $this->fNormHJ2PCPolars[$polId] = new pair(0, -1);
      }

      foreach (range(1, 2) as $ringId)
      {
         $this->fAvrgHJPolars[$ringId] = new pair(0, -1);

         foreach (range(0, 1) as $tgtOrient)
         {
            $this->fAvrgBeamProfRs[$ringId][$tgtOrient] = new pair(0, -1);
            $this->fMissedBeamProfRs[$ringId][$tgtOrient] = new pair(0, -1);
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
               $beamProfR = $fillResult->fBeamProfRs[$ringId][$tgtOrient];

               if ($beamProfR->second >= 0) 
                  $beamProfRSet[$ringId][$tgtOrient][] = $beamProfR;
  
               $this->fAvrgBeamProfRs[$ringId][$tgtOrient] = calcWeigtedAvrgErrPairs($this->fAvrgBeamProfRs[$ringId][$tgtOrient], $beamProfR);
            }
         }
      }

      //print "<pre>\n";
      //print_r($beamProfRSet);
      //print "</pre>\n";

      // calculate profile R value for missed measurements
      foreach (range(1, 2) as $ringId)
      {
         foreach (range(0, 1) as $tgtOrient)
         {
            //$ve = new pair(calcWeigtedAvrgErr($beamProfRSet[$ringId][$tgtOrient]), calcWeigtedStdDev( $beamProfRSet[$ringId][$tgtOrient]));
            //$this->fMissedBeamProfRs[$ringId][$tgtOrient] = $ve;
            $this->fMissedBeamProfRs[$ringId][$tgtOrient]->first  = calcWeigtedAvrgErr($beamProfRSet[$ringId][$tgtOrient])->first;
            $this->fMissedBeamProfRs[$ringId][$tgtOrient]->second = calcWeigtedStdDev( $beamProfRSet[$ringId][$tgtOrient])->first;

            //print "<pre>\n";
            //print_r($this->fMissedBeamProfRs[$ringId][$tgtOrient]);
            //print "</pre>\n";
         }
      }
   } //}}}


   /** */
   function CalcDependencies()
   { //{{{
      foreach ( $this->fFillResults as $fillResult)
      {
         $fillResult->CalcBeamPolar(false);
         $fillResult->CalcPolarCollScale();
      }
   } //}}}

}

?>
