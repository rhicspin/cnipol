
#include "AnaGlobResult.h"

#include <limits.h>
#include <sstream>
#include <fstream>

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
   fPathExternResults("./"),
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0),
   fDoCalcPolarNorm(kTRUE),
   fBeamEnergies(),
   fAnaFillResults(), fNormJetCarbon(), fNormJetCarbon2(),
   fNormJetCarbonByTarget2(), fNormProfPolar(), fNormProfPolar2(),
   fAvrgPCProfRUnWs(), fSystUvsDPolar(), fSystJvsCPolar(), fSystProfPolar()
{
   // Initialize averages with invalid values
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;

      ValErrPair valErrPair(1, -1);
      fNormJetCarbon2[polId] = valErrPair;
      fNormProfPolar2[polId] = valErrPair;
   }

   // ...same thing by target id
   TargetSetIter iTarget = gRunConfig.fTargets.begin();

   for ( ; iTarget != gRunConfig.fTargets.end(); ++iTarget)
   {
      TargetUId      targetUId = iTarget->fUId;
      EPolarimeterId polId     = targetUId.fPolId;
      ETargetOrient  tgtOrient = targetUId.fTargetOrient;
      //UShort_t       tgtId     = targetUId.fTargetId;

      ValErrPair valErrPair(0, -1);

      fNormJetCarbonByTarget2[targetUId] = valErrPair;
      fAvrgPCProfRUnWs[polId][tgtOrient] = valErrPair;
   }
}


/** */
AnaGlobResult::~AnaGlobResult() { }


/** */
void AnaGlobResult::Print(const Option_t* opt) const
{
   Info("Print", "Print members:");

   const char *l = strstr( opt, "all");

   if (l) {
      AnaFillResultMapConstIter iFill = fAnaFillResults.begin();
      for ( ; iFill!=fAnaFillResults.end(); ++iFill)
         iFill->second.Print();
   }

   printf("\nNorm. jet/carbon:\n");
   printf("    | avrg. of ratio                        |||||| ratio of avrg.                        |\n");

   PolId2ValErrMapConstIter iNormJC  = fNormJetCarbon.begin();
   PolId2ValErrMapConstIter iNormJC2 = fNormJetCarbon2.begin();

   for ( ; iNormJC2 != fNormJetCarbon2.end(); ++iNormJC2, ++iNormJC)
   {
      EPolarimeterId polId  = iNormJC2->first;
      string         sPolId = RunConfig::AsString(polId);

      printf("%s: ", sPolId.c_str());
      //printf(" %16.14f +/- %16.14f      ", iNormJC2->second.first, iNormJC2->second.second);
      //printf("(%16.14f +/- %16.14f)   \n", iNormJC->second.first,  iNormJC->second.second);
      printf(" %16.3f +/- %16.3f      ", iNormJC2->second.first, iNormJC2->second.second);
      printf("(%16.3f +/- %16.3f)   \n", iNormJC->second.first,  iNormJC->second.second);
   }

   printf("\nNorm. jet/carbon by target:\n");

   TargetSetIter iTarget = gRunConfig.fTargets.begin();

   for ( ; iTarget != gRunConfig.fTargets.end(); ++iTarget)
   {
      TargetUId  targetUId  = iTarget->fUId;
      ValErrPair valErrPair = fNormJetCarbonByTarget2.find(targetUId)->second;

      iTarget->Print();
      printf(": %16.14f +/- %16.14f\n", valErrPair.first, valErrPair.second);
   }

   printf("\nSyst. jet/carbon:\n");

   iNormJC2 = fNormJetCarbon2.begin();

   for ( ; iNormJC2 != fNormJetCarbon2.end(); ++iNormJC2)
   {
      EPolarimeterId polId  = iNormJC2->first;
      string         sPolId = RunConfig::AsString(polId);

      printf("%s: ", sPolId.c_str());
      printf("syst J vs C: %16.14f +/- %16.14f\n", fSystJvsCPolar.find(polId)->second.first, fSystJvsCPolar.find(polId)->second.second);
   }

   printf("\n");
   printf("Norm. prof polar carbon:\n");

   // Print polar vs prof polar normalization factors
   PolId2ValErrMapConstIter iNormPP  = fNormProfPolar.begin();
   PolId2ValErrMapConstIter iNormPP2 = fNormProfPolar2.begin();

   for ( ; iNormPP != fNormProfPolar.end(); ++iNormPP, ++iNormPP2) {
      EPolarimeterId polId  = iNormPP->first;
      string         sPolId = RunConfig::AsString(polId);

      printf("%s: ", sPolId.c_str());
      printf("%16.14f +/- %16.14f      ", iNormPP->second.first,  iNormPP->second.second);
      printf("%16.14f +/- %16.14f\n",     iNormPP2->second.first, iNormPP2->second.second);
   }

   printf("\n");
   printf("Syst. norm. prof polar carbon:\n");

   // Print polar vs prof polar normalization factors
   iNormPP  = fNormProfPolar.begin();

   for ( ; iNormPP != fNormProfPolar.end(); ++iNormPP) {
      EPolarimeterId polId  = iNormPP->first;
      string         sPolId = RunConfig::AsString(polId);

      printf("%s: ", sPolId.c_str());
      printf("syst prof: %16.14f +/- %16.14f\n", fSystProfPolar.find(polId)->second.first, fSystProfPolar.find(polId)->second.second);
   }

   printf("\nAvrg. polar. profile:\n");

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;
      string         sPolId = RunConfig::AsString(polId);

      TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

      for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
      {
         ETargetOrient tgtOrient    = *iTgtOrient;
         string        sTgtOrient   = RunConfig::AsString(tgtOrient);
         ValErrPair    avrgPolProfR = GetAvrgPCProfRUnW(polId, tgtOrient);

         printf("%s %s: %16.14f +/- %16.14f\n", sPolId.c_str(), sTgtOrient.c_str(), avrgPolProfR.first, avrgPolProfR.second);
      }

      //printf("%s syst. U vs D: %16.14f +/- %16.14f\n", sPolId.c_str(), fSystUvsDPolar.find(*iRingId)->second.first, fSystUvsDPolar.find(*iRingId)->second.second);
      printf("\n");
   }

   printf("\n");
}


/** */
void AnaGlobResult::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['fMinFill']      = %d;\n", fMinFill);
   fprintf(f, "$rc['fMaxFill']      = %d;\n", fMaxFill);
   fprintf(f, "$rc['fMinTime']      = %ld;\n", fMinTime);
   fprintf(f, "$rc['fMaxTime']      = %ld;\n", fMaxTime);
   fprintf(f, "$rc['fBeamEnergies'] = %s;\n\n", SetAsPhpArray<EBeamEnergy>(fBeamEnergies).c_str());

   PolId2ValErrMapConstIter iNormJC2 = fNormJetCarbon2.begin();

   for ( ; iNormJC2 != fNormJetCarbon2.end(); ++iNormJC2)
   {
      EPolarimeterId polId = iNormJC2->first;

      fprintf(f, "$rc['normHJPC']['%s']['nomi'] = new pair(%10.5f, %10.5f);\n",
              RunConfig::AsString(polId).c_str(), iNormJC2->second.first, iNormJC2->second.second);
   }

   TargetSetIter iTarget = gRunConfig.fTargets.begin();

   for ( ; iTarget != gRunConfig.fTargets.end(); ++iTarget)
   {
      TargetUId  targetUId  = iTarget->fUId;
      ValErrPair valErrPair = fNormJetCarbonByTarget2.find(targetUId)->second;

      if (valErrPair.second < 0) continue;

      fprintf(f, "$rc['normHJPC']['%s']['%s'][%d] = new pair(%10.5f, %10.5f);\n",
              RunConfig::AsString(targetUId.fPolId).c_str(), RunConfig::AsString(targetUId.fTargetOrient).c_str(),
              targetUId.fTargetId, valErrPair.first, valErrPair.second);
   }

   fprintf(f, "\n");
}


/** */
void AnaGlobResult::AddMeasResult(AnaMeasResult &result)
{
}


/** */
void AnaGlobResult::AddMeasResult(EventConfig &mm, DrawObjContainer *ocIn)
{
   UInt_t fillId = mm.fMeasInfo->GetFillId();

   UpdMinMaxFill(fillId);
   UpdMinMaxTime(mm.fMeasInfo->fStartTime);

   // Save the energies of all measurements
   BeamEnergySetIter iBEnergy = fBeamEnergies.find(mm.fMeasInfo->GetBeamEnergy());

   if (iBEnergy == fBeamEnergies.end()) {
      fBeamEnergies.insert(mm.fMeasInfo->GetBeamEnergy());
   }

   // Now consider individual fills
   AnaFillResult *anaFillResult = GetAnaFillResult(fillId);

   anaFillResult->AddMeasResult(mm);

   // Do something about the read draw obj container
   if (ocIn) {
      //oc->Print();
      anaFillResult->AddGraphMeasResult(mm, *ocIn);
   }
}


/** */
void AnaGlobResult::AddHJMeasResult()
{
   std::stringstream fullPath("");
   fullPath << fPathExternResults << "/hjet_pol";
   ifstream file(fullPath.str().c_str());

   Info("AddHJMeasResult", "Adding HJet polarization from file: %s", fullPath.str().c_str());

   if (file.good()) {
      UInt_t   fillId;
      Double_t bluPolar, bluPolarErr, bluAsym, bluAsymErr;
      Double_t yelPolar, yelPolarErr, yelAsym, yelAsymErr;
      Double_t energy;
      Int_t    iline = 0;
      string   dummy;

      // loop over the entries
      while ( file.good() )
      {
         iline++;

         file >> fillId >> yelAsym  >> dummy >> yelAsymErr
                        >> yelPolar >> dummy >> yelPolarErr
                        >> bluAsym  >> dummy >> bluAsymErr
                        >> bluPolar >> dummy >> bluPolarErr
                        >> energy;

         if (file.eof()) break;

         if (fillId < fMinFill || fillId > fMaxFill) continue;

         cout << iline << " " << fillId   << " " << fMinFill << " " << fMaxFill
                       << " " << yelAsym  << " " << yelAsymErr
                       << " " << yelPolar << " " << yelPolarErr
                       << " " << bluAsym  << " " << bluAsymErr
                       << " " << bluPolar << " " << bluPolarErr
                       << " " << energy   << endl;

         AnaFillResult *anaFillResult = GetAnaFillResult(fillId);

         if (bluPolarErr >= 0) {
            ValErrPair hjPolar_BLU(bluPolar, bluPolarErr);
            anaFillResult->SetHJPolar(kBLUE_RING, hjPolar_BLU);
         }

         if (bluAsymErr >= 0) {
            ValErrPair hjAsym_BLU(bluAsym, bluAsymErr);
            anaFillResult->SetHJAsym(kBLUE_RING, hjAsym_BLU);
         }

         if (yelPolarErr >= 0) {
            ValErrPair hjPolar_YEL(yelPolar, yelPolarErr);
            anaFillResult->SetHJPolar(kYELLOW_RING, hjPolar_YEL);
         }

         if (yelAsymErr >= 0) {
            ValErrPair hjAsym_YEL(yelAsym, yelAsymErr);
            anaFillResult->SetHJAsym(kYELLOW_RING, hjAsym_YEL);
         }
      }
   } else {
      Error("AddHJMeasResult", "Invalid file with hjet pol");
   }

   file.close();
}


/** */
void AnaGlobResult::Process(DrawObjContainer *ocOut)
{
   Info("Process", "Processing fills...");

   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill)
   {
      UInt_t         fillId  = iFill->first;
      AnaFillResult &anaFillResult = iFill->second;

      // First read the extern info if available...
      // Check whether a file with external info exists. Create corresponding
      // object if yes
      std::stringstream fullPath("");
      fullPath << fPathExternResults << "/cdev_info/cdev_" << fillId;
      ifstream file(fullPath.str().c_str());
      anaFillResult.AddExternInfo(file);

      // anaFillResult.fStartTime is properly determined at this stage
      // ... then do the rest

      // Process each fill
      anaFillResult.Process(ocOut);
   }

   //CalcMissingSlopes();

   if (fDoCalcPolarNorm) CalcPolarNorm();

   CalcAvrgPolProfR();
   CalcDependencies();
}


/** */
ValErrPair AnaGlobResult::GetBeamPolar(ERingId ringId, UInt_t fillId, Bool_t norm)
{
   ValErrPair result(0, -1);

   AnaFillResultMapIter iFill = fAnaFillResults.find(fillId);

   if ( iFill == fAnaFillResults.end() ) return result;

   AnaFillResult *fillRslt = &iFill->second;

   return fillRslt->GetBeamPolar(ringId);
}


/** */
AnaFillResult* AnaGlobResult::GetAnaFillResult(UInt_t fillId)
{
   AnaFillResultMapIter iFillRes = fAnaFillResults.find(fillId);

   if ( iFillRes == fAnaFillResults.end() ) {

      AnaFillResult *anaFillResult = new AnaFillResult(fillId);
      anaFillResult->SetAnaGlobResult(this);
      fAnaFillResults[fillId] = *anaFillResult;
      delete anaFillResult;

      return &fAnaFillResults[fillId];

   // otherwise, get a pointer to the existing one
   } else {
      // add AnaRunResult to existing AnaFillResult
      return &iFillRes->second;
   }
}


/** */
void AnaGlobResult::Configure(MAsymAnaInfo &mainfo)
{
   fPathExternResults = mainfo.GetExternInfoPath();
}


/** */
void AnaGlobResult::UpdMinMaxFill(UInt_t fillId)
{
   if (fillId < fMinFill ) fMinFill = fillId;
   if (fillId > fMaxFill ) fMaxFill = fillId;
}


/** */
void AnaGlobResult::UpdMinMaxTime(time_t time)
{
   if (time < fMinTime ) fMinTime = time;
   if (time > fMaxTime ) fMaxTime = time;
}


/** */
void AnaGlobResult::AdjustMinMaxFill()
{
   fMinFill -= 0.5;
   fMaxFill += 0.5;
}


/** */
void AnaGlobResult::CalcPolarNorm()
{
   Info("CalcPolarNorm()", "Called");

   // Create containers for polarization where both hjet and pcarbon results exist
   PolId2ValErrSet polarCrbSet;
   PolId2ValErrSet polarJetSet;

   // Containers for polar vs prof polar normalization
   PolId2ValErrSet polarCrbAllSet;
   PolId2ValErrSet polarCrbProfAllSet;


   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill)
   {
      UInt_t         fillId   =  iFill->first;
      AnaFillResult *fillRslt = &iFill->second;

      PolId2ValErrMapConstIter iPolar     = fillRslt->fPCPolars.begin();
      PolId2ValErrMapConstIter iProfPolar = fillRslt->fPCProfPolars.begin();

      for ( ; iPolar != fillRslt->fPCPolars.end(); ++iPolar, ++iProfPolar)
      {
         EPolarimeterId polId       = iPolar->first;
         ValErrPair     polarPC     = iPolar->second;
         ValErrPair     polarPCProf = iProfPolar->second;
         ERingId        ringId      = RunConfig::GetRingId(polId);
         string         sPolId      = RunConfig::AsString(polId);

         // Polarization profile scale factor
         if (polarPC.second >= 0 && polarPCProf.second >= 0) {
            polarCrbAllSet    [polId].insert(polarPC);
            polarCrbProfAllSet[polId].insert(polarPCProf);

            fNormProfPolar2[polId] = utils::CalcWeightedAvrgErr(fNormProfPolar2[polId], utils::CalcDivision(polarPCProf, polarPC, 1));
         }

         ValErrPair polarHJ = fillRslt->GetHJPolar(ringId);

         // For HJet normalization: Skip if there is no corresponding hjet result for this fill
         if (polarPC.second < 0 || polarHJ.second < 0) {
            Warning("CalcPolarNorm", "Fill %d, %s: p-Carbon or H-jet polarization not available", fillId, sPolId.c_str());
            continue;
         }

         TargetUId targetUId = fillRslt->GetPCTarget(polId);

         // The fills in which the carbon target is not well defined (multiple
         // targets used in a fill) are not used in the normalization
         // The idea behind this is to be consistent with scale factors by target
         if (!targetUId.IsValid()) continue;

         polarCrbSet[polId].insert(polarPC);
         polarJetSet[polId].insert(polarHJ);

         ValErrPair norm = utils::CalcDivision(polarHJ, polarPC);
         fNormJetCarbon2[polId] = utils::CalcWeightedAvrgErr(fNormJetCarbon2[polId], norm);

         // for debugging
         //cout << "jjjj " << fillId << " ";
         //cout << ": " << PairAsPhpArray(norm) << "      " << PairAsPhpArray(fNormJetCarbon2[polId]) << endl;
         //printf("%d: %s: %16.7f +/- %16.7f   %16.7f +/- %16.7f\n",
         //   fillId, sPolId.c_str(), polarPC.first, polarPC.second, polarHJ.first, polarHJ.second);

         fNormJetCarbonByTarget2[targetUId] = utils::CalcWeightedAvrgErr(fNormJetCarbonByTarget2[targetUId], norm);

         // for debugging
         //cout << "kkkk " << fillId << " ";
         //targetUId.Print();
         //cout << ": " << PairAsPhpArray(norm) << "      " << PairAsPhpArray(fNormJetCarbonByTarget2[targetUId]) << endl;
      }
   }

   // Now calculate the weighted average for each polarimeter
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      //Info("CalcPolarNorm", "Consider polarimeter %s", RunConfig::AsString(*iPolId).c_str() );

      if ( polarCrbSet.find(*iPolId) != polarCrbSet.end() ) {
         ValErrPair avrgCrb = utils::CalcWeightedAvrgErr(polarCrbSet[*iPolId]);
         ValErrPair avrgJet = utils::CalcWeightedAvrgErr(polarJetSet[*iPolId]);

         fNormJetCarbon[*iPolId] = utils::CalcDivision(avrgJet, avrgCrb);
      }

      if ( polarCrbAllSet.find(*iPolId) != polarCrbAllSet.end() ) {
         ValErrPair avrgPolar     = utils::CalcWeightedAvrgErr(polarCrbAllSet[*iPolId]);
         ValErrPair avrgProfPolar = utils::CalcWeightedAvrgErr(polarCrbProfAllSet[*iPolId]);

         fNormProfPolar[*iPolId]  = utils::CalcDivision(avrgProfPolar, avrgPolar);
      }
   }
}


/** */
void AnaGlobResult::CalcAvrgPolProfR()
{
   Info("CalcAvrgPolProfR()", "Called");

   AnaFillResultMapIter iFill = fAnaFillResults.begin();
   for ( ; iFill != fAnaFillResults.end(); ++iFill)
   {
      //UInt_t         fillId   =  iFill->first;
      AnaFillResult *fillRslt = &iFill->second;

      PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         EPolarimeterId polId  = *iPolId;

         TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

         for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
         {
            ETargetOrient tgtOrient   = *iTgtOrient;
            ValErrPair    fillPCProfR = fillRslt->GetPCProfR(polId, tgtOrient);
            ValErrPair    currAvrg    = GetAvrgPCProfRUnW(polId, tgtOrient);

            fAvrgPCProfRUnWs[polId][tgtOrient] = utils::CalcWeightedAvrgErr(currAvrg, fillPCProfR);
         }
      }

      //if (polarCrb.second < 0 || polarJet.second < 0) continue; // skip if not a valid result

      // for debugging
      //string sPolId = RunConfig::AsString(polId);
      //printf("%d: %s: %16.7f +/- %16.7f   %16.7f +/- %16.7f\n",
      //   fillId, sPolId.c_str(), polarCrb.first, polarCrb.second, polarJet.first, polarJet.second);
   }
}


/** */
void AnaGlobResult::CalcDependencies()
{
   Info("CalcDependencies()", "Called");

   // Create sets with valid syst ratio
   RingId2ValErrSet ratioU2DSet;
   PolId2ValErrSet  ratioHJ2PCSet;
   PolId2ValErrSet  ratioP2PPSet;

   AnaFillResultMapIter iFill = fAnaFillResults.begin();

   for ( ; iFill != fAnaFillResults.end(); ++iFill)
   {
      AnaFillResult *fillRslt = &iFill->second;

      // Using calculated normalization and pol. profiles calculate average beam
      // polarization and the same in collisions
      fillRslt->CalcBeamPolar(kTRUE);

      RingId2ValErrMap ratioU2D   = fillRslt->CalcPolarRatioU2D(fNormJetCarbon2);
      PolId2ValErrMap  ratioHJ2PC = fillRslt->CalcSystJvsCPolar(fNormJetCarbon2);
      PolId2ValErrMap  ratioP2PP  = fillRslt->CalcSystProfPolar(fNormProfPolar2);

      //
      PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         EPolarimeterId polId  = *iPolId;
         ERingId        ringId = RunConfig::GetRingId(polId);

         if (polId == kB1U && ratioU2D[ringId].second >= 0) ratioU2DSet[ringId].insert(ratioU2D[ringId]);
         if (polId == kY2U && ratioU2D[ringId].second >= 0) ratioU2DSet[ringId].insert(ratioU2D[ringId]);

         if (ratioHJ2PC[polId].second >= 0) ratioHJ2PCSet[polId].insert(ratioHJ2PC[polId]);
         if (ratioP2PP[polId].second  >= 0) ratioP2PPSet[polId].insert(ratioP2PP[polId]);
      }
   }

   // Calculate the contribution from the systematic effects
   utils::SystRatioFitFunctor srff;

   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();
   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;
      ERingId        ringId = RunConfig::GetRingId(polId);

      // Syst contribution for U vs D
      if (polId == kB1U || polId == kY2U) {
         srff.SetSet(ratioU2DSet[ringId]);
         TF1 systRatioFitFunc("systRatioFitFunc", srff, -10, 10, 0, "SystRatioFitFunctor");
         ROOT::Math::WrappedTF1 wf1(systRatioFitFunc);
         ROOT::Math::BrentRootFinder brf;
         brf.SetFunction(wf1, 0, 1);
         brf.Solve();
         fSystUvsDPolar[ringId].first = brf.Root();
      }

      // Syst contribution for H-Jet vs p-Carbon
      srff.SetSet(ratioHJ2PCSet[polId]);
      TF1 systRatioFitFunc_JvsC("systRatioFitFunc_JvsC", srff, -10, 10, 0, "SystRatioFitFunctor");
      ROOT::Math::WrappedTF1 wf1_JvsC(systRatioFitFunc_JvsC);
      ROOT::Math::BrentRootFinder brf_JvsC;
      brf_JvsC.SetFunction(wf1_JvsC, 0, 1);
      brf_JvsC.Solve();
      fSystJvsCPolar[polId].first = brf_JvsC.Root();

      // Syst contribution in p-Carbon polar vs p-Carbon prof polar
      srff.SetSet(ratioP2PPSet[polId]);
      TF1 systRatioFitFunc_Prof("systRatioFitFunc_Prof", srff, -10, 10, 0, "SystRatioFitFunctor");
      ROOT::Math::WrappedTF1 wf1_Prof(systRatioFitFunc_Prof);
      ROOT::Math::BrentRootFinder brf_Prof;
      brf_Prof.SetFunction(wf1_Prof, 0, 1);
      brf_Prof.Solve();
      fSystProfPolar[polId].first = brf_Prof.Root();
   }
}


/** */
void AnaGlobResult::UpdateInsertDb(AsymDbSql *asymDbSql)
{
   if (!asymDbSql) {
      Error("UpdateInsertDb", "Cannot connect to MySQL DB");
      return;
   }

   asymDbSql->OpenConnection();

   AnaFillResultMapConstIter iFillRes = fAnaFillResults.begin();

   for ( ; iFillRes != fAnaFillResults.end(); ++iFillRes)
   {
      const AnaFillResult &fillResult = iFillRes->second;
      UInt_t fillId = fillResult.GetFillId();

      cout << endl;
      Info("UpdateInsert", "fill %d", fillId);


      // test
      //Info("UpdateInsert", "Test Test");

      // Save results from the p-Carbon polarimeters
      PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         EPolarimeterId polId  = *iPolId;
         ERingId        ringId = RunConfig::GetRingId(polId);

         // Polarization
         MseFillPolarNewX* ofillP = asymDbSql->SelectFillPolar(fillId, polId, ringId);
         MseFillPolarNewX* nfillP = 0;

         if (ofillP) { // if fill found in database copy it to new one
            nfillP = new MseFillPolarNewX(*ofillP);
         } else { // if fill not found in database create it
            nfillP = new MseFillPolarNewX(fillId, polId, ringId);
         }

         nfillP->SetValuesPC(fillResult, polId);
         asymDbSql->UpdateInsert(ofillP, nfillP);

         //TargetUId targetUId = fillRslt->GetPCTarget(polId);
         //if (!targetUId.IsValid()) continue;

         // Profile
         TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

         for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
         {
            ETargetOrient tgtOrient = *iTgtOrient;

            // Skip measurements whit invalid profile values
            ValErrPair fillPCProfR = fillResult.GetPCProfR(polId, tgtOrient);
            if (fillPCProfR.second < 0) {
               Info("UpdateInsert", "Skip invalid profile measurement in polId %d, tgtOrient %d", polId, tgtOrient);
               continue;
            }


            MseFillProfileNewX* ofillProf = asymDbSql->SelectFillProfile(fillId, polId, tgtOrient);
            MseFillProfileNewX* nfillProf = 0;

            if (ofillProf) { // if fill found in database copy it to new one
               nfillProf = new MseFillProfileNewX(*ofillProf);
            } else { // if fill not found in database create it
               nfillProf = new MseFillProfileNewX(fillId, polId, tgtOrient);
            }

            nfillProf->SetValues(fillResult, polId, tgtOrient);
            asymDbSql->UpdateInsert(ofillProf, nfillProf);
         }
      }

      // Save results from the H-jet polarimeter
      RingIdSetIter iRingId = gRunConfig.fRings.begin();

      for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
      {
         EPolarimeterId polId = kHJET;
         ERingId ringId = *iRingId;

         MseFillPolarNewX* ofillP = asymDbSql->SelectFillPolar(fillId, polId, ringId);
         MseFillPolarNewX* nfillP = 0;

         if (ofillP) { // if fill found in database copy it to new one
            nfillP = new MseFillPolarNewX(*ofillP);
         } else { // if fill not found in database create it
            nfillP = new MseFillPolarNewX(fillId, polId, ringId);
         }

         nfillP->SetValuesHJ(fillResult, ringId);
         asymDbSql->UpdateInsert(ofillP, nfillP);
      }
      // test end
      // test end
      // test end
      // test end


      /*
      MseFillPolarX *ofill = asymDbSql->SelectFillPolar(fillId);
      MseFillPolarX *nfill = 0;

      if (ofill) { // if fill found in database copy it to new one
         nfill = new MseFillPolarX(*ofill);
      } else { // if fill not found in database create it
         nfill = new MseFillPolarX(fillId);
      }

      nfill->SetValues(fillResult);
      //nfill->Print();

      asymDbSql->UpdateInsert(ofill, nfill);


      // Update profile table
      MseFillProfileX *ofillProf = asymDbSql->SelectFillProfile(fillId);
      MseFillProfileX *nfillProf = 0;

      if (ofillProf) { // if fill found in database copy it to new one
         nfillProf = new MseFillProfileX(*ofillProf);
      } else { // if fill not found in database create it
         nfillProf = new MseFillProfileX(fillId);
      }

      nfillProf->SetValues(fillResult);
      //nfillProf->Print();

      asymDbSql->UpdateInsert(ofillProf, nfillProf);
      */
   }

   asymDbSql->CloseConnection();
}


/** */
ValErrPair AnaGlobResult::GetNormJetCarbon(EPolarimeterId polId)
{
   ValErrPair norm(1, -1); // default values

   if ( fNormJetCarbon2.find(polId) != fNormJetCarbon2.end() )
      norm = fNormJetCarbon2[polId];

   return norm;
}


/** */
ValErrPair AnaGlobResult::GetNormProfPolar(EPolarimeterId polId)
{
   ValErrPair norm(1, -1); // default values

   if ( fNormProfPolar2.find(polId) != fNormProfPolar2.end() )
      norm = fNormProfPolar2[polId];

   return norm;
}
