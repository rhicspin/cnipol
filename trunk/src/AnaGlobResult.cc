
#include "AnaGlobResult.h"

#include <limits.h>

#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"
#include "TGraphErrors.h"

#include "AsymGlobals.h"
#include "AsymDbSql.h"
#include "MseFillPolar.h"

#include "utils/utils.h"

ClassImp(AnaGlobResult)

using namespace std;


/** */
AnaGlobResult::AnaGlobResult() : TObject(),
   fPathHjetResults("./"),
   fFileNameYelHjet("hjet_pol_yel.txt"),
   fFileNameBluHjet("hjet_pol_blu.txt"),
   fMinFill(UINT_MAX), fMaxFill(0),
   fAnaFillResults(), fNormJetCarbon(), fNormJetCarbon2(),
   fNormJetCarbonByTarget2(), fNormProfPolar(), fNormProfPolar2(),
   fAvrgPolProfRs(), fSystUvsDPolar(), fSystJvsCPolar(), fSystProfPolar()
{
}


/** */
AnaGlobResult::~AnaGlobResult() { }


/** */
void AnaGlobResult::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Print members:");
   //PrintAsPhp();

   char *l = strstr( opt, "all");

   if (l) {

      AnaFillResultMapConstIter iFill = fAnaFillResults.begin();

      for ( ; iFill!=fAnaFillResults.end(); ++iFill) {

         printf("\nFill %d:\n", iFill->first);
         iFill->second.Print();
      }
   }

   printf("\n");
   printf("Norm jet/carbon:\n");

   PolId2ValErrMapConstIter iNormJC  = fNormJetCarbon.begin();
   PolId2ValErrMapConstIter iNormJC2 = fNormJetCarbon2.begin();

   for ( ; iNormJC != fNormJetCarbon.end(); ++iNormJC, ++iNormJC2)
   {
      EPolarimeterId  polId = iNormJC->first;
      string         sPolId = RunConfig::AsString(polId);

      printf("%s: ", sPolId.c_str());
      printf("%16.14f +/- %16.14f  ", iNormJC->second.first,  iNormJC->second.second);
      printf("%16.14f +/- %16.14f\n", iNormJC2->second.first, iNormJC2->second.second);

      printf("%s syst J vs C: %16.14f +/- %16.14f\n", sPolId.c_str(), fSystJvsCPolar.find(polId)->second.first, fSystJvsCPolar.find(polId)->second.second);
   }

   printf("\n");
   printf("Norm prof polar carbon:\n");

   // Print polar vs prof polar normalization factors
   PolId2ValErrMapConstIter iNormPP  = fNormProfPolar.begin();
   PolId2ValErrMapConstIter iNormPP2 = fNormProfPolar2.begin();

   for ( ; iNormPP != fNormProfPolar.end(); ++iNormPP, ++iNormPP2) {
      EPolarimeterId  polId = iNormPP->first;
      string         sPolId = RunConfig::AsString(polId);

      printf("%s: %16.14f +/- %16.14f", sPolId.c_str(), iNormPP->second.first, iNormPP->second.second);
      printf("      %16.14f +/- %16.14f\n", iNormPP2->second.first, iNormPP2->second.second);

      printf("%s syst prof: %16.14f +/- %16.14f\n", sPolId.c_str(), fSystProfPolar.find(polId)->second.first, fSystProfPolar.find(polId)->second.second);
   }

   printf("\n");

   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      string sRingId = RunConfig::AsString(*iRingId);

      TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

      for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
      {
         string sTgtOrient        = RunConfig::AsString(*iTgtOrient);
         //const ValErrPair& avrgPolProfR = fAvrgPolProfRs[*iRingId][*iTgtOrient];
         const ValErrPair &avrgPolProfR = fAvrgPolProfRs.find(*iRingId)->second.find(*iTgtOrient)->second;

         printf("%s %s: %16.14f +/- %16.14f\n", sRingId.c_str(),
            sTgtOrient.c_str(), avrgPolProfR.first, avrgPolProfR.second);
      }


      printf("%s syst U vs D: %16.14f +/- %16.14f\n", sRingId.c_str(), fSystUvsDPolar.find(*iRingId)->second.first, fSystUvsDPolar.find(*iRingId)->second.second);
      printf("\n");
   }

   printf("\n");

} //}}}


///** */
//void AnaGlobResult::PrintAsPhp(FILE *f) const
//{ //{{{
//} //}}}


/** */
void AnaGlobResult::AddMeasResult(AnaMeasResult &result)
{ //{{{
} //}}}


/** */
void AnaGlobResult::AddMeasResult(EventConfig &mm)
{ //{{{
   UInt_t fillId = mm.fMeasInfo->GetFillId();

   if (fillId > fMaxFill) fMaxFill = fillId;
   if (fillId < fMinFill) fMinFill = fillId;

   AnaFillResultMapIter iFill = fAnaFillResults.find(fillId);

   AnaFillResult *fillRes = 0;

   // if not found result for this fill create and add a new one
   if (iFill == fAnaFillResults.end()) {

      // create new AnaFillResult
      fillRes = new AnaFillResult();
      fillRes->AddMeasResult(mm, this);
      fAnaFillResults[fillId] = *fillRes;

   // otherwise, get a pointer to the existing one
   } else {
      // add AnaRunResult to existing AnaFillResult
      AnaFillResult *fillRes = &iFill->second;
      fillRes->AddMeasResult(mm, this);
   }

} //}}}


/** */
void AnaGlobResult::Process()
{ //{{{
   // Read jet results from text files and save them to fill result containers
   TGraphErrors* grYel = new TGraphErrors((fPathHjetResults + fFileNameYelHjet).c_str());
   TGraphErrors* grBlu = new TGraphErrors((fPathHjetResults + fFileNameBluHjet).c_str());

   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill) {

      UInt_t         fillId  =  iFill->first;
      AnaFillResult *fillRes = &iFill->second;

      // Process each fill
      fillRes->Process();

      // set hjet values
      ValErrPair *hjetYel = utils::FindValErrY(grYel, fillId);
      ValErrPair *hjetBlu = utils::FindValErrY(grBlu, fillId);

      if (hjetYel) {
         fillRes->AddHjetPolar(kYELLOW_RING, *hjetYel);
         delete hjetYel;
      }

      if (hjetBlu) {
         fillRes->AddHjetPolar(kBLUE_RING,   *hjetBlu);
         delete hjetBlu;
      }
   }

   CalcPolarNorm();
   CalcAvrgPolProfR();
   CalcPolarDependants();

   // Using calculated normalization and pol. profiles calculate average beam
   // polarization and the same in collisions
   //iFill = fAnaFillResults.begin();

   //for ( ; iFill != fAnaFillResults.end(); ++iFill) {

   //   UInt_t         fillId  =  iFill->first;
   //   AnaFillResult *fillRes = &iFill->second;

   //   //fillRes
   //}

} //}}}


/** */
ValErrPair AnaGlobResult::GetPolarBeam(ERingId ringId, UInt_t fillId, Bool_t norm)
{ //{{{
   ValErrPair result(0, -1);

   AnaFillResultMapIter iFill = fAnaFillResults.find(fillId);

   if ( iFill == fAnaFillResults.end() ) return result;

   AnaFillResult *fillRslt = &iFill->second;

   return fillRslt->GetPolarBeam(ringId);
} //}}}


/** */
void AnaGlobResult::AdjustMinMaxFill()
{ //{{{
   fMinFill -= 0.5;
   fMaxFill += 0.5;
} //}}}


/** */
void AnaGlobResult::CalcPolarNorm()
{ //{{{
   // Create containers for polarization where both hjet and pcarbon results exist
   PolId2ValErrSet polarCrbSet;
   PolId2ValErrSet polarJetSet;

   PolId2ValErrSet polarNormSet;

   // Containers for polar vs prof polar normalization
   PolId2ValErrSet polarCrbAllSet;
   PolId2ValErrSet polarCrbProfAllSet;

   PolId2ValErrSet polarNormAllSet;


   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill)
   {
      UInt_t         fillId   =  iFill->first;
      AnaFillResult *fillRslt = &iFill->second;

      PolId2ValErrMapConstIter iPolar     = fillRslt->fPolars.begin();
      PolId2ValErrMapConstIter iProfPolar = fillRslt->fProfPolars.begin();

      for ( ; iPolar != fillRslt->fPolars.end(); ++iPolar, ++iProfPolar)
      {
         EPolarimeterId polId        = iPolar->first;
         ValErrPair     polarCrb     = iPolar->second;
         ValErrPair     polarCrbProf = iProfPolar->second;
         ERingId        ringId       = RunConfig::GetRingId(polId);

         // Polarization profile scale factor
         if (polarCrb.second >= 0 && polarCrbProf.second >= 0) {
            polarCrbAllSet    [polId].insert(polarCrb);
            polarCrbProfAllSet[polId].insert(polarCrbProf);

            polarNormAllSet[polId].insert(CalcDivision(polarCrbProf, polarCrb, 1));
         }

         // For HJet normalization: Skip if there is no corresponding hjet result for this fill
         if (fillRslt->fHjetPolars.find(ringId) == fillRslt->fHjetPolars.end()) {
            Warning("CalcPolarNorm", "Fill %d, pol. %d: H-jet polarization not available", fillId, polId);
            continue;
         }

         ValErrPair polarJet = fillRslt->fHjetPolars[ringId];

         if (polarCrb.second < 0 || polarJet.second < 0) continue; // skip if not a valid result

         polarCrbSet[polId].insert(polarCrb);
         polarJetSet[polId].insert(polarJet);

         polarNormSet[polId].insert(CalcDivision(polarJet, polarCrb));

         // for debugging
         //string sPolId = RunConfig::AsString(polId);
         //printf("%d: %s: %16.7f +/- %16.7f   %16.7f +/- %16.7f\n",
         //   fillId, sPolId.c_str(), polarCrb.first, polarCrb.second, polarJet.first, polarJet.second);
      }


      // Loop over polarization values for different targets
      TargetUId2ValErrMapConstIter iPolarByTgt = fillRslt->fPolarsByTargets.begin();

      for ( ; iPolarByTgt != fillRslt->fPolarsByTargets.end(); ++iPolarByTgt)
      {
         TargetUId      targetUId    = iPolarByTgt->first;
         ValErrPair     polarCrb     = iPolarByTgt->second;

         EPolarimeterId polId        = targetUId.fPolId;
         ETargetOrient  targetOrient = targetUId.fTargetOrient;
         UShort_t       targetId     = targetUId.fTargetId;

         ERingId        ringId       = RunConfig::GetRingId(polId);
         ValErrPair     polarJet     = fillRslt->fHjetPolars[ringId];

         ValErrPair norm = CalcDivision(polarJet, polarCrb, 0);
         
         if (norm.second >=0)
         {
            if (fNormJetCarbonByTarget2.find(targetUId) == fNormJetCarbonByTarget2.end())
            {
               fNormJetCarbonByTarget2[targetUId] = norm;
            } else {
               fNormJetCarbonByTarget2[targetUId] = CalcWeightedAvrgErr(fNormJetCarbonByTarget2[targetUId], norm);
            }
         }
      }
   }

   // Now calculate the weighted average for each polarimeter
   PolarimeterIdIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      if ( polarCrbSet.find(*iPolId) != polarCrbSet.end() ) {
         ValErrPair avrgCrb = CalcWeightedAvrgErr(polarCrbSet[*iPolId]);
         ValErrPair avrgJet = CalcWeightedAvrgErr(polarJetSet[*iPolId]);

         fNormJetCarbon[*iPolId] = CalcDivision(avrgJet, avrgCrb);
      }

      if ( polarNormSet.find(*iPolId) != polarNormSet.end() ) {
         fNormJetCarbon2[*iPolId] = CalcWeightedAvrgErr(polarNormSet[*iPolId]);
      }

      if ( polarCrbAllSet.find(*iPolId) != polarCrbAllSet.end() ) {
         ValErrPair avrgPolar     = CalcWeightedAvrgErr(polarCrbAllSet[*iPolId]);
         ValErrPair avrgProfPolar = CalcWeightedAvrgErr(polarCrbProfAllSet[*iPolId]);

         fNormProfPolar[*iPolId]  = CalcDivision(avrgProfPolar, avrgPolar);
      }

      if ( polarNormAllSet.find(*iPolId) != polarNormAllSet.end() ) {
         fNormProfPolar2[*iPolId] = CalcWeightedAvrgErr(polarNormAllSet[*iPolId]);
      }
   }
} //}}}


/** */
void AnaGlobResult::CalcAvrgPolProfR()
{ //{{{
   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill) {

      UInt_t         fillId   =  iFill->first;
      AnaFillResult *fillRslt = &iFill->second;

      RingIdSetIter iRingId = gRunConfig.fRings.begin();

      for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
      {
         TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

         for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
         {
            ValErrPair fillPolProfR = fillRslt->GetPolProfR(*iRingId, *iTgtOrient);

            ValErrPair& currAvrg = fAvrgPolProfRs[*iRingId][*iTgtOrient];
            currAvrg = CalcWeightedAvrgErr(currAvrg, fillPolProfR);
         }
      }

      //if (polarCrb.second < 0 || polarJet.second < 0) continue; // skip if not a valid result

      // for debugging
      //string sPolId = RunConfig::AsString(polId);
      //printf("%d: %s: %16.7f +/- %16.7f   %16.7f +/- %16.7f\n",
      //   fillId, sPolId.c_str(), polarCrb.first, polarCrb.second, polarJet.first, polarJet.second);
   }
} //}}}


/** */
void AnaGlobResult::CalcPolarDependants()
{ //{{{
   // Create sets with valid syst ratio
   RingId2ValErrSet systUvsDSet;
   PolId2ValErrSet  systJvsCSet;
   PolId2ValErrSet  systProfSet;

   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill)
   {
      UInt_t         fillId   =  iFill->first;
      AnaFillResult *fillRslt = &iFill->second;

      //fillRslt->CalcBeamPolar(fNormJetCarbon);
      fillRslt->CalcBeamPolar(fNormJetCarbon2);

      //RingId2ValErrMap systUvsD = fillRslt->CalcSystUvsDPolar(fNormJetCarbon);
      //PolId2ValErrMap  systJvsC = fillRslt->CalcSystJvsCPolar(fNormJetCarbon);
      //PolId2ValErrMap  systProf = fillRslt->CalcSystProfPolar(fNormProfPolar);
      RingId2ValErrMap systUvsD = fillRslt->CalcSystUvsDPolar(fNormJetCarbon2);
      PolId2ValErrMap  systJvsC = fillRslt->CalcSystJvsCPolar(fNormJetCarbon2);
      PolId2ValErrMap  systProf = fillRslt->CalcSystProfPolar(fNormProfPolar2);

      //
      PolarimeterIdConstIter iPolId = gRunConfig.fPolarimeters.begin();

      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         EPolarimeterId polId  = *iPolId;
         ERingId        ringId = RunConfig::GetRingId(polId);

         if (polId == kB1U && systUvsD[ringId].second >= 0) systUvsDSet[ringId].insert(systUvsD[ringId]);
         if (polId == kY2U && systUvsD[ringId].second >= 0) systUvsDSet[ringId].insert(systUvsD[ringId]);

         if (systJvsC[polId].second >= 0) systJvsCSet[polId].insert(systJvsC[polId]);
         if (systProf[polId].second >= 0) systProfSet[polId].insert(systProf[polId]);
      }
   }

   //
   utils::SystRatioFitFunctor srff;

   //srff.SetSet(systUvsDSet[kBLUE_BEAM]);

   //TF1 systRatioFitFunc("systRatioFitFunc", srff, -10, 10, 0, "SystRatioFitFunctor");

   //ROOT::Math::WrappedTF1 wf1(systRatioFitFunc);
   //ROOT::Math::BrentRootFinder brf;

   //brf.SetFunction(wf1, 0, 1);
   //brf.Solve();
   //fSystUvsDPolar[kBLUE_BEAM].first = brf.Root();

   //return;

   PolarimeterIdConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;
      ERingId        ringId = RunConfig::GetRingId(polId);

      if (polId == kB1U || polId == kY2U) {
         srff.SetSet(systUvsDSet[ringId]);
         TF1 systRatioFitFunc("systRatioFitFunc", srff, -10, 10, 0, "SystRatioFitFunctor");
         ROOT::Math::WrappedTF1 wf1(systRatioFitFunc);
         ROOT::Math::BrentRootFinder brf;
         brf.SetFunction(wf1, 0, 1);
         brf.Solve();
         fSystUvsDPolar[ringId].first = brf.Root();
      }

      srff.SetSet(systJvsCSet[polId]);
      TF1 systRatioFitFunc_JvsC("systRatioFitFunc_JvsC", srff, -10, 10, 0, "SystRatioFitFunctor");
      ROOT::Math::WrappedTF1 wf1_JvsC(systRatioFitFunc_JvsC);
      ROOT::Math::BrentRootFinder brf_JvsC;
      brf_JvsC.SetFunction(wf1_JvsC, 0, 1);
      brf_JvsC.Solve();
      fSystJvsCPolar[polId].first = brf_JvsC.Root();

      srff.SetSet(systProfSet[polId]);
      TF1 systRatioFitFunc_Prof("systRatioFitFunc_Prof", srff, -10, 10, 0, "SystRatioFitFunctor");
      ROOT::Math::WrappedTF1 wf1_Prof(systRatioFitFunc_Prof);
      ROOT::Math::BrentRootFinder brf_Prof;
      brf_Prof.SetFunction(wf1_Prof, 0, 1);
      brf_Prof.Solve();
      fSystProfPolar[polId].first = brf_Prof.Root();
   }
} //}}}


/** */
void AnaGlobResult::UpdateInsertDb()
{ //{{{

   if (!gAsymDb) {
      Error("UpdateInsertDb", "Cannot connect to MySQL DB");
      return;
   }

   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill) {

      cout << endl;
      Info("UpdateInsert", "fill %d", iFill->first);

      MseFillPolarX *ofill = gAsymDb->SelectFillPolar(iFill->first);
      MseFillPolarX *nfill = 0;

      if (ofill) { // if fill found in database copy it to new one
         nfill = new MseFillPolarX(*ofill);
      } else { // if fill not found in database create it
         nfill = new MseFillPolarX(iFill->first);
      }

      nfill->SetValues(iFill->second);
      //nfill->Print();

      gAsymDb->UpdateInsert(ofill, nfill);


      // Update profile table
      MseFillProfileX *ofillProf = gAsymDb->SelectFillProfile(iFill->first);
      MseFillProfileX *nfillProf = 0;

      if (ofillProf) { // if fill found in database copy it to new one
         nfillProf = new MseFillProfileX(*ofillProf);
      } else { // if fill not found in database create it
         nfillProf = new MseFillProfileX(iFill->first);
      }

      nfillProf->SetValues(iFill->second);
      //nfillProf->Print();

      gAsymDb->UpdateInsert(ofillProf, nfillProf);
   }
} //}}}


/** */
ValErrPair AnaGlobResult::GetNormJetCarbon(EPolarimeterId polId)
{ //{{{
   ValErrPair norm(1, -1); // default values
   
   if ( fNormJetCarbon2.find(polId) != fNormJetCarbon2.end() )
      norm = fNormJetCarbon2[polId];

   return norm;
} //}}}


/** */
ValErrPair AnaGlobResult::GetNormProfPolar(EPolarimeterId polId)
{ //{{{
   ValErrPair norm(1, -1); // default values
   
   if ( fNormProfPolar2.find(polId) != fNormProfPolar2.end() )
      norm = fNormProfPolar2[polId];

   return norm;
} //}}}
