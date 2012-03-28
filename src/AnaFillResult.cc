
#include <string>

#include "AnaFillResult.h"
#include "AnaGlobResult.h"
#include "Target.h"

ClassImp(AnaFillResult)

using namespace std;


/** */
AnaFillResult::AnaFillResult() : TObject(), fAnaGlobResult(0),
   fStartTime((time_t)-1), fAnaMeasResults(), fPolars(), fPolarsByTargets(),
   fProfPolars(), fHjetPolars(), fBeamPolars(), fBeamCollPolars(),
   fSystProfPolar(), fSystJvsCPolar(), fSystUvsDPolar(),
   fMeasTgtOrients(), fMeasTgtIds(), fMeasRingIds(), fPolProfRs(),
   fPolProfPMaxs(), fPolProfPs()
{
}


/** */
AnaFillResult::~AnaFillResult() { }


/** */
time_t AnaFillResult::GetStartTime() { return fStartTime; }


/** */
void AnaFillResult::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Print members:");
   //PrintAsPhp();

   AnaMeasResultMapConstIter iMeasRslt = fAnaMeasResults.begin();

   printf("Runs: ");
   for ( ; iMeasRslt!=fAnaMeasResults.end(); ++iMeasRslt) {
      printf("%s \n", iMeasRslt->first.c_str());
      //iMeasRslt->second.Print();
   }
   printf("\n");


   PolarimeterIdConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      ValErrPair polar = fPolars.find(*iPolId)->second;

      cout << sPolId << ": " << PairAsPhpArray(polar) << "      ";

      polar.first  *= fAnaGlobResult->GetNormJetCarbon(*iPolId).first;
      polar.second *= fAnaGlobResult->GetNormJetCarbon(*iPolId).first;
      cout << PairAsPhpArray(polar) << endl;

      ValErrPair profPolar = fProfPolars.find(*iPolId)->second;

      cout << "     " << PairAsPhpArray(profPolar) << "      ";
      profPolar.first  *= fAnaGlobResult->GetNormProfPolar(*iPolId).first;
      profPolar.second *= fAnaGlobResult->GetNormProfPolar(*iPolId).first;
      cout << PairAsPhpArray(profPolar) << endl;

      ValErrPair systPP = fSystProfPolar.find(*iPolId)->second;

      cout << "     " << "      ";
      cout << PairAsPhpArray(systPP) << endl;
   }

   printf("\n");

   RingIdConstIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId) {
      string sRingId = RunConfig::AsString(*iRingId);
      ValErrPair polar = fBeamPolars.find(*iRingId)->second;
      cout << sRingId << ": " << PairAsPhpArray(polar) << "      ";
      ValErrPair systUvsD = fSystUvsDPolar.find(*iRingId)->second;
      cout << PairAsPhpArray(systUvsD) << endl;
   }

   printf("\n");
   printf("Print normalization factors by target\n");

   TargetSetIter iTarget = gRunConfig.fTargets.begin();

   for ( ; iTarget != gRunConfig.fTargets.end(); ++iTarget)
   {
      iTarget->Print();
      TargetUId targetUId = iTarget->fUId;

      if (fPolarsByTargets.find(targetUId) != fPolarsByTargets.end()) {
         //fPolarsByTargets[targetUId].Print();
         cout << PairAsPhpArray(fPolarsByTargets.find(targetUId)->second) << endl;
      }
   }


   printf("\n");

   //cout << "Polarimeter polar:" << endl;
   //cout << MapAsPhpArray<EPolarimeterId, ValErrPair>(fPolars) << endl;
   //cout << "Beam polar:" << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fBeamPolars) << endl;
   //cout << "Syst U vs D:" << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fSystUvsDPolar) << endl;
   //cout << "Profile polar:" << endl;
   //cout << MapAsPhpArray<EPolarimeterId, ValErrPair>(fProfPolars) << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fHjetPolars) << endl;
   //cout << MapAsPhpArray<ERingId, TgtOrient2ValErrMap>(fPolProfPs) << endl;
} //}}}


///** */
//void AnaFillResult::PrintAsPhp(FILE *f) const
//{ //{{{
//} //}}}


/** */
void AnaFillResult::AddMeasResult(AnaMeasResult &result)
{ //{{{
} //}}}


/** */
void AnaFillResult::AddMeasResult(EventConfig &mm, AnaGlobResult *globRes)
{ //{{{
   string runName = mm.fMeasInfo->GetRunName();

   if (mm.fMeasInfo->fStartTime < fStartTime) fStartTime = mm.fMeasInfo->fStartTime;

   // add or overwrite new AnaFillResult
   fAnaMeasResults[runName] = *mm.fAnaMeasResult;
   fMeasTgtOrients[runName] =  mm.fMeasInfo->GetTargetOrient();
   fMeasTgtIds[runName]     =  mm.fMeasInfo->GetTargetId();
   fMeasRingIds[runName]    =  mm.fMeasInfo->GetRingId();

   fAnaGlobResult = globRes;

} //}}}


/** */
void AnaFillResult::Process()
{ //{{{
   //Info("Process", "check");

   // Calc polars by target
   AnaMeasResultMapConstIter iMeasRslt  = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient = fMeasTgtOrients.begin();
   String2TargetIdMapIter    iTgtId     = fMeasTgtIds.begin();

   for ( ; iMeasRslt!=fAnaMeasResults.end(); ++iMeasRslt, ++iTgtOrient, ++iTgtId)
   {
      const AnaMeasResult &measResult = iMeasRslt->second;
      EPolarimeterId       polId      = MeasInfo::ExtractPolarimeterId(iMeasRslt->first);
      ETargetOrient        tgtOrient  = iTgtOrient->second;
      UShort_t             tgtId      = iTgtId->second;

      TargetUId targetUId(polId, tgtOrient, tgtId);

      if (fPolarsByTargets.find(targetUId) == fPolarsByTargets.end())
      {
         fPolarsByTargets[targetUId] = measResult.GetPolar();
      } else {
         fPolarsByTargets[targetUId] = CalcWeightedAvrgErr(fPolarsByTargets[targetUId], measResult.GetPolar());
      }
   }


   // Calculate average polarization and profiles disregarding the target
   PolarimeterIdIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      // Calculate average polarizations
      fPolars[*iPolId]     = CalcAvrgPolar(*iPolId);
      fProfPolars[*iPolId] = CalcAvrgPolProfPolar(*iPolId);

      // Now calculate average profiles: R and P_max
      ERingId ringId = RunConfig::GetRingId(*iPolId);

      // Get values only once for each beam (note we loop over polarimeters)
      if (fPolProfRs.find(ringId) == fPolProfRs.end()) {

         TgtOrient2ValErrMap tgtPolProfRs;
         TgtOrient2ValErrMap tgtPolProfPMaxs;
         TgtOrient2ValErrMap tgtPolProfPs;

         TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

         for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
         {
            ValErrPair valerrR           = CalcAvrgPolProfR(ringId, *iTgtOrient);
            tgtPolProfRs[*iTgtOrient]    = valerrR;

            ValErrPair valerrPMax        = CalcAvrgPolProfPMax(ringId, *iTgtOrient);
            tgtPolProfPMaxs[*iTgtOrient] = valerrPMax;

            tgtPolProfPs[*iTgtOrient]    = CalcPolProfP(valerrR, valerrPMax);
         }

         fPolProfRs[ringId]    = tgtPolProfRs;
         fPolProfPMaxs[ringId] = tgtPolProfPMaxs;
         fPolProfPs[ringId]    = tgtPolProfPs;
      }
   }
} //}}}


/** */
ValErrPair AnaFillResult::GetPolarHJ(EPolarimeterId polId)
{ //{{{
   ERingId ringId = RunConfig::GetRingId(polId);
   return GetPolarHJ(ringId);
} //}}}


/** */
ValErrPair AnaFillResult::GetPolarHJ(ERingId ringId)
{ //{{{
   return fHjetPolars.find(ringId) == fHjetPolars.end() ? ValErrPair(0, -1) : fHjetPolars[ringId];
} //}}}


/** */
ValErrPair AnaFillResult::GetPolarPC(EPolarimeterId polId, PolId2ValErrMap *normJC)
{ //{{{
   Double_t norm = 1;

   if (normJC)
      norm = normJC->find(polId) == normJC->end() ? 1 : (*normJC)[polId].first;

   ValErrPair result(0, -1);

   if (fPolars.find(polId) != fPolars.end() )
   {
      result.first  = norm * fPolars[polId].first;
      result.second = norm * fPolars[polId].second;
   }

   return result;
} //}}}


/** */
ValErrPair AnaFillResult::GetPolarBeam(ERingId ringId)
{ //{{{
   return fBeamPolars.find(ringId) == fBeamPolars.end() ? ValErrPair(0, -1) : fBeamPolars[ringId];
} //}}}


/** */
ValErrPair AnaFillResult::GetSystUvsDPolar(ERingId ringId)
{ //{{{
   return fSystUvsDPolar.find(ringId) == fSystUvsDPolar.end() ? ValErrPair(0, -1) : fSystUvsDPolar[ringId];
} //}}}


/** */
ValErrPair AnaFillResult::GetSystJvsCPolar(EPolarimeterId polId)
{ //{{{
   return fSystJvsCPolar.find(polId) == fSystJvsCPolar.end() ? ValErrPair(0, -1) : fSystJvsCPolar[polId];
} //}}}


/** */
ValErrPair AnaFillResult::GetSystProfPolar(EPolarimeterId polId)
{ //{{{
   return fSystProfPolar.find(polId) == fSystProfPolar.end() ? ValErrPair(0, -1) : fSystProfPolar[polId];
} //}}}


/** */
void AnaFillResult::CalcBeamPolar(PolId2ValErrMap &normJC)
{ //{{{
   ValErrPair result(0, -1);

   // Create set of measurements for requested ringId
   ValErrSet polars;

   PolId2ValErrMapConstIter iPolar = fPolars.begin();

   for ( ; iPolar != fPolars.end(); ++iPolar)
   {
      EPolarimeterId polId    = iPolar->first;
      ValErrPair     polarCrb = iPolar->second;
      ERingId        ringId   = RunConfig::GetRingId(polId);

      Double_t norm = normJC.find(polId) == normJC.end() ? 1 : normJC[polId].first;

      polarCrb.first  *= norm;
      polarCrb.second *= norm;

      if ( fBeamPolars.find(ringId) == fBeamPolars.end() ) {
         fBeamPolars[ringId] = polarCrb;
      } else {
         fBeamPolars[ringId] = CalcWeightedAvrgErr(fBeamPolars[ringId], polarCrb);
      }

      //polars.insert(polarCrb);
   }
   //fBeamPolars[ringId] = CalcWeightedAvrgErr(polars);
   //return fBeamPolars[ringId];
} //}}}


/** */
RingId2ValErrMap AnaFillResult::CalcSystUvsDPolar(PolId2ValErrMap &normJC)
{ //{{{
   RingId2ValErrMapConstIter iBeamPolar = fBeamPolars.begin();

   for ( ; iBeamPolar != fBeamPolars.end(); ++iBeamPolar)
   {
      ERingId    ringId    = iBeamPolar->first;
      ValErrPair polarU(-1, -1);
      ValErrPair polarD(-1, -1);

      if (ringId == kBLUE_RING) {
         polarU = GetPolarPC(kB1U, &normJC);
         polarD = GetPolarPC(kB2D, &normJC);
      }

      if (ringId == kYELLOW_RING) {
         polarU = GetPolarPC(kY2U, &normJC);
         polarD = GetPolarPC(kY1D, &normJC);
      }

      if (polarU.second < 0 || polarD.second < 0) {
         fSystUvsDPolar[ringId] = ValErrPair(-1, -1);
         continue; // skip invalid result
      }

      // 100% correlation between U and D is a more conservative assumption
      ValErrPair ratio = CalcDivision(polarU, polarD, 0);
      //ValErrPair ratio = CalcDivision(polarU, polarD, 1);

      fSystUvsDPolar[ringId] = ratio;

   }

   return fSystUvsDPolar;
} //}}}


/**
 * Calculates and returns the ratio of the HJ to PC polarization. The latter is
 * expected to be normalized.
 */
PolId2ValErrMap AnaFillResult::CalcSystJvsCPolar(PolId2ValErrMap &normJC)
{ //{{{
   PolId2ValErrMapConstIter iPolar = fPolars.begin();

   for ( ; iPolar != fPolars.end(); ++iPolar)
   {
      EPolarimeterId polId   = iPolar->first;
      ERingId        ringId  = RunConfig::GetRingId(polId);
      ValErrPair     polarPC = GetPolarPC(polId, &normJC);  // Get normalized p-Carbon polarization
      ValErrPair     polarHJ = GetPolarHJ(ringId);

      // Skip invalid measurements
      if (polarPC.second < 0 || polarHJ.second < 0) {
         fSystJvsCPolar[polId] = ValErrPair(0, -1);
         continue;
      }

      // Uncorrelated ratio: Assume 0% correlation
      ValErrPair ratio = CalcDivision(polarHJ, polarPC, 0);

      fSystJvsCPolar[polId] = ratio;
   }

   return fSystJvsCPolar;
} //}}}


/** */
PolId2ValErrMap AnaFillResult::CalcSystProfPolar(PolId2ValErrMap &normPP)
{ //{{{
   // Find the maximum difference between the average beam polarization and
   // different polarimeters
   PolId2ValErrMapConstIter iPolar     = fPolars.begin();
   PolId2ValErrMapConstIter iProfPolar = fProfPolars.begin();

   for ( ; iPolar != fPolars.end(); ++iPolar, ++iProfPolar)
   {
      EPolarimeterId polId   = iPolar->first;
      ValErrPair     polarPC = iPolar->second;
      ValErrPair     polarPP = iProfPolar->second;

      if (polarPC.second < 0 || polarPP.second < 0) {
         fSystProfPolar[polId] = ValErrPair(-1, -1);
         continue; // skip invalid result
      }

      Double_t norm = normPP.find(polId) == normPP.end() ? 1 : normPP[polId].first;

      polarPC.first  *= norm;
      polarPC.second *= norm;

      ValErrPair ratio = CalcDivision(polarPP, polarPC, 1);

      fSystProfPolar[polId] = ratio;

      //Double_t re_polarPC = polarPC.second/polarPC.first;
      //Double_t re_polarPP = polarPP.second/polarPP.first;
      //Double_t re_polar_syst = (ratio.first - 1)*(ratio.first - 1)/ratio.first/ratio.first -
      //                         re_polarPC*re_polarPC - re_polarPP*re_polarPP + 2*re_polarPC*re_polarPP; // assume 100% correlation
      //Double_t syst = 0;
      //if (re_polar_syst >= 0) syst = sqrt(re_polar_syst);// * polarPC.first;
      //fSystProfPolar[polId] = ValErrPair(syst, syst); // error does not really mean anything
   }

   return fSystProfPolar;
} //}}}


/** */
ValErrPair AnaFillResult::GetPolProfR(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrPair result(0, -1);

   if (fPolProfRs.find(ringId) == fPolProfRs.end()) return result;

   TgtOrient2ValErrMap tgtPolProfRs = fPolProfRs[ringId];

   if (tgtPolProfRs.find(tgtOrient) == tgtPolProfRs.end()) return result;

   return tgtPolProfRs[tgtOrient];

} //}}}


/** */
ValErrPair AnaFillResult::GetPolProfPMax(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrPair result(0, -1);

   if (fPolProfPMaxs.find(ringId) == fPolProfPMaxs.end()) return result;

   TgtOrient2ValErrMap tgtPolProfPMaxs = fPolProfPMaxs[ringId];

   if (tgtPolProfPMaxs.find(tgtOrient) == tgtPolProfPMaxs.end()) return result;

   return tgtPolProfPMaxs[tgtOrient];

} //}}}


/** */
ValErrPair AnaFillResult::GetPolProfP(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrPair result(0, -1);

   if (fPolProfPs.find(ringId) == fPolProfPs.end()) return result;

   TgtOrient2ValErrMap tgtPolProfPs = fPolProfPs[ringId];

   if (tgtPolProfPs.find(tgtOrient) == tgtPolProfPs.end()) return result;

   return tgtPolProfPs[tgtOrient];

} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolar(EPolarimeterId polId)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iMeasRslt  = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient = fMeasTgtOrients.begin();
   String2TargetIdMapIter    iTgtId     = fMeasTgtIds.begin();

   for ( ; iMeasRslt!=fAnaMeasResults.end(); ++iMeasRslt, ++iTgtOrient, ++iTgtId) {

      // Consider only results for polId
      if ( MeasInfo::ExtractPolarimeterId(iMeasRslt->first) != polId) continue;

      ValErrPair    val_err(0, -1);
      TFitResultPtr fitres = iMeasRslt->second.fFitResPolarPhi;

      if (fitres.Get()) {

         val_err.first  = fitres->Value(0);
         val_err.second = fitres->FitResult::Error(0);

      } else {

         Error("CalcAvrgPolar", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(val_err);
   }

   return CalcWeightedAvrgErr(allRunResults);
} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfPolar(EPolarimeterId polId)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iMeasRslt = fAnaMeasResults.begin();

   for ( ; iMeasRslt!=fAnaMeasResults.end(); ++iMeasRslt) {

      // Consider only results for polId
      if ( MeasInfo::ExtractPolarimeterId(iMeasRslt->first) != polId) continue;

      ValErrPair    vePolProfP(0, -1), veR(0, -1), vePMax(0, -1);
      TFitResultPtr fitres = iMeasRslt->second.fFitResProfilePvsI;

      if (fitres.Get()) {

         vePMax.first  = fitres->Value(0);
         vePMax.second = fitres->FitResult::Error(0);
         veR.first     = fitres->Value(1);
         veR.second    = fitres->FitResult::Error(1);

         vePolProfP    = CalcPolProfP(veR, vePMax);
      } else {
         Error("CalcAvrgPolProfPolar", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(vePolProfP);
   }

   return CalcWeightedAvrgErr(allRunResults);

} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfR(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iMeasRslt  = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient = fMeasTgtOrients.begin();
   String2RingIdMapIter      iRingId    = fMeasRingIds.begin();

   for ( ; iMeasRslt!=fAnaMeasResults.end(); ++iMeasRslt, ++iTgtOrient, ++iRingId) {

      // Consider only results for ringId and tgtOrient
      if ( iRingId->second    != ringId)    continue;
      if ( iTgtOrient->second != tgtOrient) continue;

      ValErrPair    val_err(0, -1);
      TFitResultPtr fitres = iMeasRslt->second.fFitResProfilePvsI;

      if (fitres.Get()) {

         val_err.first  = fitres->Value(1);
         val_err.second = fitres->FitResult::Error(1);

      } else {

         Error("CalcAvrgPolProfR", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(val_err);
   }

   return CalcWeightedAvrgErr(allRunResults);

} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfPMax(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iMeasRslt  = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient = fMeasTgtOrients.begin();
   String2RingIdMapIter      iRingId    = fMeasRingIds.begin();

   for ( ; iMeasRslt!=fAnaMeasResults.end(); ++iMeasRslt, ++iTgtOrient, ++iRingId)
   {
      // Consider only results for ringId and tgtOrient
      if ( iRingId->second    != ringId)    continue;
      if ( iTgtOrient->second != tgtOrient) continue;

      ValErrPair    val_err(0, -1);
      TFitResultPtr fitres = iMeasRslt->second.fFitResProfilePvsI;

      if (fitres.Get()) {

         val_err.first  = fitres->Value(0);
         val_err.second = fitres->FitResult::Error(0);

      } else {

         Error("CalcAvrgPolProfPMax", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(val_err);
   }

   return CalcWeightedAvrgErr(allRunResults);

} //}}}


/** */
ValErrPair AnaFillResult::CalcPolProfP(ValErrPair R, ValErrPair Pmax)
{ //{{{
   ValErrPair polProfP(0, -1);

   if (Pmax.second < 0 || R.second < 0 || Pmax.first <= -1) return polProfP;

   polProfP.first = Pmax.first / sqrt(1 + R.first);

   //relDelta2 = Pmax.second * Pmax.second / Pmax.first / Pmax.first + R.second * R.second / R.first / R.first;
   //polProfP.second = polProfP.first * sqrt(relDelta2);

   Double_t delta2 = Pmax.first * Pmax.first * R.second *
                     R.second/4./(1+R.first)/(1+R.first)/(1+R.first) +
                     Pmax.second * Pmax.second / (1 + R.first);

   polProfP.second = sqrt(delta2);

   return polProfP;
} //}}}


/** */
void AnaFillResult::AddHjetPolar(ERingId ringId, ValErrPair ve)
{ //{{{
   //RingId2ValErrMapIter iBeamRes = fHjetPolars.find(ringId);
   //if (iBeamRes == fHjetPolars.end())
   //else

   fHjetPolars[ringId] = ve;
} //}}}
