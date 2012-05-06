
#include <string>
#include <climits>

#include "TF1.h"
#include "TF2.h"
#include "TROOT.h"

#include "AnaFillResult.h"
#include "AnaGlobResult.h"
#include "Target.h"
#include "CnipolAsymHists.h"
#include "MAsymFillHists.h"
#include "MAsymSingleFillHists.h"

#include "utils/utils.h"

ClassImp(AnaFillResult)

using namespace std;


/** */
AnaFillResult::AnaFillResult(UInt_t fillId) : TObject(), fFillId(fillId),
   fAnaGlobResult(0), fStartTime(LONG_MAX), fAnaFillExternResult(0),
   fPCPolarGraphs(), fPCPolarFitRes(), fPolProfRGraphs(), fPolProfRFitRes(),
   //fAsymVsBunchId_X(0),
   fFlattopEnergy(0),
   fAnaMeasResults(), fMeasInfos(), fPCPolars(), fPCPolarUnWs(), fPCPolarsByTargets(),
   fPCProfPolars(), fHJPolars(), fBeamPolars(), fBeamCollPolars(),
   fSystProfPolar(), fSystJvsCPolar(), fSystUvsDPolar(), fMeasTgtOrients(),
   fMeasTgtIds(), fMeasRingIds(), fPolProfRs(), fPolProfPMaxs(), fPolProfPs()
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

   // debugging
   if (fAnaFillExternResult)
      Info("Print", "extern result is available");
   else
      Error("Print", "extern result is NOT available");

   AnaMeasResultMapConstIter iAnaMeasResult = fAnaMeasResults.begin();

   printf("Runs: ");
   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult) {
      printf("%s \n", iAnaMeasResult->first.c_str());
      //iAnaMeasResult->second.Print();
   }
   printf("\n");


   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      ValErrPair polar = fPCPolars.find(*iPolId)->second;

      cout << sPolId << ": " << PairAsPhpArray(polar) << "      ";

      polar.first  *= fAnaGlobResult->GetNormJetCarbon(*iPolId).first;
      polar.second *= fAnaGlobResult->GetNormJetCarbon(*iPolId).first;
      cout << PairAsPhpArray(polar) << endl;

      ValErrPair profPolar = fPCProfPolars.find(*iPolId)->second;

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

      if (fPCPolarsByTargets.find(targetUId) != fPCPolarsByTargets.end()) {
         //fPCPolarsByTargets[targetUId].Print();
         cout << PairAsPhpArray(fPCPolarsByTargets.find(targetUId)->second) << endl;
      }
   }


   printf("\n");

   //cout << "Polarimeter polar:" << endl;
   //cout << MapAsPhpArray<EPolarimeterId, ValErrPair>(fPCPolars) << endl;
   //cout << "Beam polar:" << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fBeamPolars) << endl;
   //cout << "Syst U vs D:" << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fSystUvsDPolar) << endl;
   //cout << "Profile polar:" << endl;
   //cout << MapAsPhpArray<EPolarimeterId, ValErrPair>(fPCProfPolars) << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fHJPolars) << endl;
   //cout << MapAsPhpArray<ERingId, TgtOrient2ValErrMap>(fPolProfPs) << endl;
} //}}}


TGraphErrors* AnaFillResult::GetGrBluIntens() const { return fAnaFillExternResult ? fAnaFillExternResult->GetGrBluIntens() : 0; }
TGraphErrors* AnaFillResult::GetGrYelIntens() const { return fAnaFillExternResult ? fAnaFillExternResult->GetGrYelIntens() : 0; }


/** */
TGraphErrors* AnaFillResult::GetIntensGraph(ERingId ringId) const
{ //{{{
   TGraphErrors *gr;

   if (ringId == kBLUE_RING)        gr = GetGrBluIntens();
   else if (ringId == kYELLOW_RING) gr = GetGrYelIntens();

   return gr;
} //}}}


/** */
TGraphErrors* AnaFillResult::GetRotCurStarGraph(ERingId ringId) const
{ //{{{
   TGraphErrors *gr = 0;

   if (!fAnaFillExternResult) return gr;

   if (ringId == kBLUE_RING && fAnaFillExternResult->fBluRotCurStarGraph)
      gr = fAnaFillExternResult->fBluRotCurStarGraph;
   else if (ringId == kYELLOW_RING && fAnaFillExternResult->fYelRotCurStarGraph)
      gr = fAnaFillExternResult->fYelRotCurStarGraph;

   return gr;
} //}}}


/** */
TGraphErrors* AnaFillResult::GetRotCurPhenixGraph(ERingId ringId) const
{ //{{{
   TGraphErrors *gr = 0;

   if (!fAnaFillExternResult) return gr;

   if (ringId == kBLUE_RING && fAnaFillExternResult->fBluRotCurPhenixGraph)
      gr = fAnaFillExternResult->fBluRotCurPhenixGraph;
   else if (ringId == kYELLOW_RING && fAnaFillExternResult->fYelRotCurPhenixGraph)
      gr = fAnaFillExternResult->fYelRotCurPhenixGraph;

   return gr;
} //}}}


/** */
TGraphErrors* AnaFillResult::GetSnakeCurGraph(ERingId ringId) const
{ //{{{
   TGraphErrors *gr = 0;

   if (!fAnaFillExternResult) return gr;

   if (ringId == kBLUE_RING && fAnaFillExternResult->fBluSnakeCurGraph)
      gr = fAnaFillExternResult->fBluSnakeCurGraph;
   else if (ringId == kYELLOW_RING && fAnaFillExternResult->fYelSnakeCurGraph)
      gr = fAnaFillExternResult->fYelSnakeCurGraph;

   return gr;
} //}}}


AnaFillExternResult* AnaFillResult::GetAnaFillExternResult() const { return fAnaFillExternResult; }


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
   // this should go away when fanameasresult and measinfo are merged
   mm.fAnaMeasResult->fStartTime = mm.fMeasInfo->fStartTime;

   string runName = mm.fMeasInfo->GetRunName();

   if (mm.fMeasInfo->fStartTime < fStartTime) fStartTime = mm.fMeasInfo->fStartTime;

   //Info("AddMeasResult", "fStartTime %d (meas)", mm.fMeasInfo->fStartTime);
   //Info("AddMeasResult", "fStartTime %d", fStartTime);

   // add or overwrite new AnaFillResult
   fAnaMeasResults[runName] = *mm.fAnaMeasResult;
   fMeasInfos[runName]      = *mm.fMeasInfo;

   fMeasTgtOrients[runName] =  mm.fMeasInfo->GetTargetOrient();
   fMeasTgtIds[runName]     =  mm.fMeasInfo->GetTargetId();
   fMeasRingIds[runName]    =  mm.fMeasInfo->GetRingId();

   fAnaGlobResult = globRes;

} //}}}


/**
 * This func can be (should be?) merged with AddMeasResult
 */
void AnaFillResult::AddGraphMeasResult(EventConfig &mm, DrawObjContainer &ocIn)
{ //{{{
   // Assume it is asym container
   CnipolAsymHists *asymHists;

   if (ocIn.d.find("asym") == ocIn.d.end() ) {
      Error("AddGraphMeasResult", "No input container 'asym' found");
      return;
   } else {
      asymHists = (CnipolAsymHists*) ocIn.d.find("asym")->second;
   }

   //asymHists->Print();

   TH1* hAsymVsBunchId_X90 = (TH1*) asymHists->o["hAsymVsBunchId_X90"];
   TH1* hAsymVsBunchId_X45 = (TH1*) asymHists->o["hAsymVsBunchId_X45"];
   TH1* hAsymVsBunchId_Y45 = (TH1*) asymHists->o["hAsymVsBunchId_Y45"];

   string runName = mm.fMeasInfo->GetRunName();

   //Info("AddGraphMeasResult", "current dir");
   // Switch to a new default directory since the input one is deleted when the
   // input file is closed
   gROOT->cd();
   //gDirectory->Print();
   fAnaMeasResults[runName].fhAsymVsBunchId_X90 = (TH1*) hAsymVsBunchId_X90->Clone();
   fAnaMeasResults[runName].fhAsymVsBunchId_X45 = (TH1*) hAsymVsBunchId_X45->Clone();
   fAnaMeasResults[runName].fhAsymVsBunchId_Y45 = (TH1*) hAsymVsBunchId_Y45->Clone();

   //fAnaMeasResults[runName].fhAsymVsBunchId_X45->Print();
   //fAnaMeasResults[runName].fhAsymVsBunchId_X45->GetListOfFunctions()->Print();
   //Info("AddGraphMeasResult", "yyy %p", (void*) fAnaMeasResults[runName].fhAsymVsBunchId_X45 );
} //}}}


/** */
void AnaFillResult::AddExternInfo(std::ifstream &file)
{ //{{{
   if (file.good()) {

      long begin = file.tellg();
      file.seekg(0, ios::end);
      long end = file.tellg();

      if (end - begin <= 0) {
         Error("AddExternInfo", "Invalid/Empty file provided with external info");
         file.close();
         return;
      }

      file.seekg(0, ios::beg);

      Info("AddExternInfo", "Found external info");
      fAnaFillExternResult = new AnaFillExternResult(file);
      //fAnaFillExternResult->GetGrBluIntens()->Print();
      //fAnaFillExternResult->GetGrYelIntens()->Print();
   } else {
      Error("AddExternInfo", "Invalid file provided with external info");
   }

   file.close();
} //}}}


/** */
void AnaFillResult::Process(DrawObjContainer *ocOut)
{ //{{{
   //Info("Process", "check");

   // Calc polars by target
   AnaMeasResultMapConstIter iAnaMeasResult = fAnaMeasResults.begin();
   MeasInfoMapConstIter      iMeasInfo      = fMeasInfos.begin();
   String2TgtOrientMapIter   iTgtOrient     = fMeasTgtOrients.begin();
   String2TargetIdMapIter    iTgtId         = fMeasTgtIds.begin();

   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult, ++iMeasInfo, ++iTgtOrient, ++iTgtId)
   {
      const AnaMeasResult &anaMeasResult = iAnaMeasResult->second;
      const MeasInfo      &measInfo      = iMeasInfo->second;
      EPolarimeterId       polId         = MeasInfo::ExtractPolarimeterId(iAnaMeasResult->first);
      ETargetOrient        tgtOrient     = iTgtOrient->second;
      UShort_t             tgtId         = iTgtId->second;

      // Get polar meas result
      ValErrPair polarValErr = anaMeasResult.GetPCPolar();
      // Skip invalid results
      if (polarValErr.second < 0) continue;

      // Put result in the right container
      TargetUId targetUId(polId, tgtOrient, tgtId);

      if (fPCPolarsByTargets.find(targetUId) == fPCPolarsByTargets.end())
      {
         fPCPolarsByTargets[targetUId] = polarValErr;
      } else {
         // Update the average for existing targets
         fPCPolarsByTargets[targetUId] = utils::CalcWeightedAvrgErr(fPCPolarsByTargets[targetUId], polarValErr);
      }

      // Pick the graph for this measurement and corresponding polarimeter
      // Create the graph with polarization measurements as a func of time
      TGraphErrors *grPolarPC;

      if (fPCPolarGraphs.find(polId) == fPCPolarGraphs.end())
      { // create a new graph
         grPolarPC = new TGraphErrors(0);
         fPCPolarGraphs[polId] = grPolarPC;
      } else
         grPolarPC = (TGraphErrors*) fPCPolarGraphs[polId];

      UInt_t iPoint = grPolarPC->GetN();
      grPolarPC->SetPoint(iPoint, anaMeasResult.fStartTime - fStartTime, polarValErr.first);
      grPolarPC->SetPointError(iPoint, 0, polarValErr.second);

      //Info("Process", "xxx1");
      // Proceed only with flattop measurements
      if ( !IsValidFlattopMeas(measInfo) ) continue;

      //Info("Process", "zzz %p", (void*) iAnaMeasResult->second.fhAsymVsBunchId_X45);
      //iAnaMeasResult->second.fhAsymVsBunchId_X45->Print();
      if (fAnaFillExternResult && fAnaFillExternResult->GetGrBluIntens()) {
         fFlattopEnergy = measInfo.GetBeamEnergy();
      }

      if (ocOut)
         CalcAvrgPolarByBunch(anaMeasResult, measInfo, *ocOut);
   }

   // do whatever with the combined fill results
   UpdateExternGraphRange();
   FitExternGraphs();
   FitPolarGraphs();


   // Calculate average polarization and profiles disregarding the target
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      // Calculate average polarizations
      fPCPolars[*iPolId]     = CalcAvrgPolar(*iPolId);
      fPCPolarUnWs[*iPolId]  = CalcAvrgPolarUnweighted(*iPolId);
      fPCProfPolars[*iPolId] = CalcAvrgPolProfPolar(*iPolId);

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
Bool_t AnaFillResult::IsValidFlattopMeas(const MeasInfo &measInfo)
{ //{{{
   if (measInfo.GetBeamEnergy() == kINJECTION) return kFALSE;

   //if (!fAnaFillExternResult) return kFALSE;

   //time_t lumion  = fAnaFillExternResult->fTimeEventLumiOn  - fStartTime;
   //time_t lumioff = fAnaFillExternResult->fTimeEventLumiOff - fStartTime;

   //if (measInfo.fStartTime-fStartTime < lumion || measInfo.fStartTime-fStartTime > lumioff) return kFALSE;

   return kTRUE;
} //}}}


/** Function can return empty graph. */
TGraphErrors* AnaFillResult::GetPCPolarGraph(EPolarimeterId polId)
{ //{{{
   TGraphErrors *grPolarPC;

   if (fPCPolarGraphs.find(polId) == fPCPolarGraphs.end())
      grPolarPC = new TGraphErrors(0);
   else
      grPolarPC = (TGraphErrors*) fPCPolarGraphs[polId];

   return grPolarPC;
} //}}}


/** */
ValErrPair AnaFillResult::GetPCPolarDecay(EPolarimeterId polId)
{ //{{{
   ValErrPair decay(0, -1);

   TGraphErrors *gr = GetPCPolarGraph(polId);
   if (!gr) return decay;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return decay;

   decay.first  = func->GetParameter(1);
   decay.second = func->GetParError(1);

   return decay;
} //}}}


/** */
ValErrPair AnaFillResult::GetIntensDecay(ERingId ringId)
{ //{{{
   ValErrPair decay(0, -1);

   TGraphErrors *gr = GetIntensGraph(ringId);
   if (!gr) return decay;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return decay;

   decay.first  = func->GetParameter(1);
   decay.second = func->GetParError(1);

   return decay;
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
   return fHJPolars.find(ringId) == fHJPolars.end() ? ValErrPair(0, -1) : fHJPolars[ringId];
} //}}}


/** */
ValErrPair AnaFillResult::GetPolarPC(EPolarimeterId polId, PolId2ValErrMap *normJC)
{ //{{{
   Double_t norm = 1;

   if (normJC)
      norm = normJC->find(polId) == normJC->end() ? 1 : (*normJC)[polId].first;

   ValErrPair result(0, -1);

   if (fPCPolars.find(polId) != fPCPolars.end() )
   {
      result.first  = norm * fPCPolars[polId].first;
      result.second = norm * fPCPolars[polId].second;
   }

   return result;
} //}}}


/** */
ValErrPair AnaFillResult::GetPolarPCUnW(EPolarimeterId polId, PolId2ValErrMap *normJC)
{ //{{{
   Double_t norm = 1;

   if (normJC)
      norm = normJC->find(polId) == normJC->end() ? 1 : (*normJC)[polId].first;

   ValErrPair result(0, -1);

   if (fPCPolarUnWs.find(polId) != fPCPolarUnWs.end() )
   {
      result.first  = norm * fPCPolarUnWs[polId].first;
      result.second = norm * fPCPolarUnWs[polId].second;
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

   PolId2ValErrMapConstIter iPolar = fPCPolars.begin();

   for ( ; iPolar != fPCPolars.end(); ++iPolar)
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
         fBeamPolars[ringId] = utils::CalcWeightedAvrgErr(fBeamPolars[ringId], polarCrb);
      }

      //polars.insert(polarCrb);
   }
   //fBeamPolars[ringId] = utils::CalcWeightedAvrgErr(polars);
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
      ValErrPair ratio = utils::CalcDivision(polarU, polarD, 0);
      //ValErrPair ratio = utils::CalcDivision(polarU, polarD, 1);

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
   PolId2ValErrMapConstIter iPolar = fPCPolars.begin();

   for ( ; iPolar != fPCPolars.end(); ++iPolar)
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
      ValErrPair ratio = utils::CalcDivision(polarHJ, polarPC, 0);

      fSystJvsCPolar[polId] = ratio;
   }

   return fSystJvsCPolar;
} //}}}


/** */
PolId2ValErrMap AnaFillResult::CalcSystProfPolar(PolId2ValErrMap &normPP)
{ //{{{
   // Find the maximum difference between the average beam polarization and
   // different polarimeters
   PolId2ValErrMapConstIter iPolar     = fPCPolars.begin();
   PolId2ValErrMapConstIter iProfPolar = fPCProfPolars.begin();

   for ( ; iPolar != fPCPolars.end(); ++iPolar, ++iProfPolar)
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

      ValErrPair ratio = utils::CalcDivision(polarPP, polarPC, 1);

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
   ValErrPair avrgPol(0, -1);

   // Check that we have graph
   TGraphErrors *grPCPolar = GetPCPolarGraph(polId);
   if (!grPCPolar) return avrgPol;

   // Check that graph has fit func
   TF1* funcPCPolar = grPCPolar->GetFunction("fitFunc");
   if (!funcPCPolar) return avrgPol;

   // Get intens graph
   ERingId ringId = RunConfig::GetRingId(polId);
   TGraphErrors *grIntens = GetIntensGraph(ringId);
   if (!grIntens) return avrgPol;

   Double_t time, intens, polar;
   Double_t numer=0, denom=0;

   for (UInt_t iPoint=0; iPoint<grIntens->GetN(); iPoint++) {
      grIntens->GetPoint(iPoint, time, intens);
      polar = funcPCPolar->Eval(time);

      numer += polar*intens;
      denom += intens;
   }

   // All polarization values are between 0 and 1 internaly except graphs
   avrgPol.first  = numer/denom/100.;
   avrgPol.second = funcPCPolar->GetParError(1)/100.; // error on the slope

   return avrgPol;
} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolarUnweighted(EPolarimeterId polId)
{ //{{{
   // Create a container to keep all measurements for polarimeter polId
   ValErrSet  allRunResults;
   ValErrPair avrgPol(0, -1);

   TGraphErrors *grPCPolar = GetPCPolarGraph(polId);
   if (!grPCPolar) return avrgPol;

   if (!fAnaFillExternResult) {
      Error("CalcAvrgPolarUnweighted", "External results not available");
      return avrgPol;
   }

   time_t lumion  = fAnaFillExternResult->fTimeEventLumiOn  - fStartTime;
   time_t lumioff = fAnaFillExternResult->fTimeEventLumiOff - fStartTime;

   //grPCPolar = (TGraphErrors*) utils::SubGraph(grPCPolar, lumion, lumioff);

   Double_t time, polar, polarErr;

   for (UInt_t iPoint=0; iPoint<grPCPolar->GetN(); iPoint++) {
      grPCPolar->GetPoint(iPoint, time, polar);
      if ( time < lumion || time > lumioff ) continue;
      polarErr = grPCPolar->GetErrorY(iPoint);

      ValErrPair val_err(polar, polarErr);

      allRunResults.insert(val_err);
   }

   return utils::CalcWeightedAvrgErr(allRunResults);
} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfPolar(EPolarimeterId polId)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iAnaMeasResult = fAnaMeasResults.begin();

   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult) {

      // Consider only results for polId
      if ( MeasInfo::ExtractPolarimeterId(iAnaMeasResult->first) != polId) continue;

      ValErrPair    vePolProfP(0, -1), veR(0, -1), vePMax(0, -1);
      TFitResultPtr fitres = iAnaMeasResult->second.fFitResProfilePvsI;

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

   return utils::CalcWeightedAvrgErr(allRunResults);

} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfR(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iAnaMeasResult  = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient = fMeasTgtOrients.begin();
   String2RingIdMapIter      iRingId    = fMeasRingIds.begin();

   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult, ++iTgtOrient, ++iRingId) {

      // Consider only results for ringId and tgtOrient
      if ( iRingId->second    != ringId)    continue;
      if ( iTgtOrient->second != tgtOrient) continue;

      ValErrPair    val_err(0, -1);
      TFitResultPtr fitres = iAnaMeasResult->second.fFitResProfilePvsI;

      if (fitres.Get()) {

         val_err.first  = fitres->Value(1);
         val_err.second = fitres->FitResult::Error(1);

      } else {

         Error("CalcAvrgPolProfR", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(val_err);
   }

   return utils::CalcWeightedAvrgErr(allRunResults);

} //}}}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfPMax(ERingId ringId, ETargetOrient tgtOrient)
{ //{{{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iAnaMeasResult  = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient = fMeasTgtOrients.begin();
   String2RingIdMapIter      iRingId    = fMeasRingIds.begin();

   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult, ++iTgtOrient, ++iRingId)
   {
      // Consider only results for ringId and tgtOrient
      if ( iRingId->second    != ringId)    continue;
      if ( iTgtOrient->second != tgtOrient) continue;

      ValErrPair    val_err(0, -1);
      TFitResultPtr fitres = iAnaMeasResult->second.fFitResProfilePvsI;

      if (fitres.Get()) {

         val_err.first  = fitres->Value(0);
         val_err.second = fitres->FitResult::Error(0);

      } else {

         Error("CalcAvrgPolProfPMax", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(val_err);
   }

   return utils::CalcWeightedAvrgErr(allRunResults);

} //}}}


/** */
ValErrPair AnaFillResult::CalcPolProfP(ValErrPair R, ValErrPair Pmax)
{ //{{{
   ValErrPair polProfP(0, -1);

   if (Pmax.second < 0 || R.second < 0 || R.first <= -1) return polProfP;

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
void AnaFillResult::CalcAvrgPolarByBunch(const AnaMeasResult &amr, const MeasInfo &mi, DrawObjContainer &ocOut) const
{ //{{{
   MAsymFillHists *fillHists;

   if (ocOut.d.find("fills") == ocOut.d.end() ) {
      Error("CalcAvrgPolarByBunch", "No output container 'fills' found");
      return;
   } else {
      fillHists = (MAsymFillHists*) ocOut.d.find("fills")->second;
   }

   ERingId ringId = mi.GetRingId();
   string sRingId = RunConfig::AsString(ringId);

   stringstream ssFillId("");
   ssFillId << fFillId;
   string sFillId = ssFillId.str();

   MAsymSingleFillHists *singleFillHists = (MAsymSingleFillHists*) fillHists->GetSingleFillHists(fFillId);
   TH1* hAsymVsBunchId_X_       = (TH1*) singleFillHists->o["hAsymVsBunchId_X_" + sFillId + "_" + sRingId];
   TH1* hAsymVsBunchId_X90      = amr.fhAsymVsBunchId_X90;
   TH1* hAsymVsBunchId_X45      = amr.fhAsymVsBunchId_X45;

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      string grName = "grAsymVsBunchId_X_" + sSS;
      TGraphErrors* grAsymVsBunchId_X_fill   = (TGraphErrors*) hAsymVsBunchId_X_->GetListOfFunctions()->FindObject(grName.c_str());


      grName = "grAsymVsBunchId_X45_" + sSS;
      TGraphErrors* grAsymVsBunchId_X45      = (TGraphErrors*) hAsymVsBunchId_X45->GetListOfFunctions()->FindObject(grName.c_str());

      utils::MergeGraphs(grAsymVsBunchId_X_fill, grAsymVsBunchId_X45);


      grName = "grAsymVsBunchId_X90_" + sSS;
      TGraphErrors* grAsymVsBunchId_X90      = (TGraphErrors*) hAsymVsBunchId_X90->GetListOfFunctions()->FindObject(grName.c_str());

      utils::MergeGraphs(grAsymVsBunchId_X_fill, grAsymVsBunchId_X90);
   }

   //TGraphErrors *grAsymVsBunchId_X90 = new TGraphErrors(0);
   //utils::MergeGraphs(grAsymVsBunchId_X90, hAsymVsBunchId_X90->GetListOfFunctions());
   //utils::BinGraph(grAsymVsBunchId_X90, hAsymVsBunchId_X90_fill);
} //}}}


/** */
void AnaFillResult::UpdateExternGraphRange()
{ //{{{
   if (!fAnaFillExternResult) {
      Error("FitGraphs", "External results not available");
      return;
   }

   // Fit the intensity graphs
   // Loop over rings
   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   // Update the X range of the graph
   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId = *iRingId;

      Double_t x, y;
      TGraphErrors *gr;

      gr = GetIntensGraph(ringId);
      if (gr)
         for (Int_t i=0; i<gr->GetN(); ++i) { gr->GetPoint(i, x, y); gr->SetPoint(i, x - fStartTime, y); }

      gr = GetRotCurStarGraph(ringId);
      if (gr)
         for (Int_t i=0; i<gr->GetN(); ++i) { gr->GetPoint(i, x, y); gr->SetPoint(i, x - fStartTime + 300*(gRandom->Rndm()-0.5), y); }

      gr = GetRotCurPhenixGraph(ringId);
      if (gr)
         for (Int_t i=0; i<gr->GetN(); ++i) { gr->GetPoint(i, x, y); gr->SetPoint(i, x - fStartTime + 300*(gRandom->Rndm()-0.5), y); }

      gr = GetSnakeCurGraph(ringId);
      if (gr)
         for (Int_t i=0; i<gr->GetN(); ++i) { gr->GetPoint(i, x, y); gr->SetPoint(i, x - fStartTime + 300*(gRandom->Rndm()-0.5), y); }
   }
} //}}}


/** */
void AnaFillResult::FitExternGraphs()
{ //{{{
   if (!fAnaFillExternResult) {
      Error("FitExternGraphs", "External results not available");
      return;
   }

   time_t lumion  = fAnaFillExternResult->fTimeEventLumiOn  - fStartTime;
   time_t lumioff = fAnaFillExternResult->fTimeEventLumiOff - fStartTime;

   if (lumion >= lumioff) {
      Error("FitExternGraphs", "Lumi-on and lumi-off markers are invalid");
      return;
   }

   Info("FitExternGraphs", "Using range %d - %d", lumion, lumioff);

   TF1 *fitFunc;

   // Fit the intensity graphs
   // Loop over rings
   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId = *iRingId;

      TGraphErrors *grIntens = GetIntensGraph(ringId);

      if (!grIntens) continue;

      //Info("FitExternGraphs", "Duration %d", fAnaFillExternResult->fTimeEventLumiOff - fAnaFillExternResult->fTimeEventLumiOn);

      if ( fabs(lumioff - lumion) < 3600 ) continue;

      stringstream ssFormula("");
      //ssFormula << "[0] * 1./exp((x - " << lumion << ")/3600./[1])";
      ssFormula << "[0] * (1.- (x - " << lumion << ")/3600./[1])";

      fitFunc = new TF1("fitFunc", ssFormula.str().c_str());
      fitFunc->SetParNames("I_{0}", "Lifetime, h");
      fitFunc->SetParLimits(0, 1, 200);
      fitFunc->SetParLimits(1, 1, 200);
      //fitFunc->SetParLimits(1, 1e-5, 10);

      grIntens->Fit(fitFunc, "M E", "", lumion, lumioff);

      delete fitFunc;
   }
} //}}}


/** */
void AnaFillResult::FitPolarGraphs()
{ //{{{
   // Fit the polarization decay graphs
   // Create a function to multiply graphs by 100% for esthetic reasons
   TF2 *dummyScale = new TF2("dummyScale", "100.*y");

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {

      EPolarimeterId polId = *iPolId;
      TGraphErrors *grPolarPC;

      if (fPCPolarGraphs.find(polId) == fPCPolarGraphs.end())
         continue;
      else
         grPolarPC = (TGraphErrors*) fPCPolarGraphs[polId];

      grPolarPC->Apply(dummyScale);

      if (!fAnaFillExternResult) {
         Error("FitPolarGraphs", "External results not available");
         continue;
      }

      time_t lumion  = fAnaFillExternResult->fTimeEventLumiOn  - fStartTime;
      time_t lumioff = fAnaFillExternResult->fTimeEventLumiOff - fStartTime;

      if (lumion >= lumioff) {
         Error("FitPolarGraphs", "Lumi-on and lumi-off markers are invalid");
         continue;
      }

      Info("FitPolarGraphs", "Using range %d - %d", lumion, lumioff);

      if (utils::SubGraph(grPolarPC, lumion, lumioff)->GetN() <= 0) continue;

      stringstream ssFormula("");
      ssFormula << "[0] + [1]*(x - " << lumion << ")/3600.";

      TF1 *fitFunc = new TF1("fitFunc", ssFormula.str().c_str());
      fitFunc->SetParNames("P_{0}, %", "Decay, %/h");

      if (utils::SubGraph(grPolarPC, lumion, lumioff)->GetN() == 1) {
         //fitFunc->SetParameter(1, 0);
         fitFunc->FixParameter(1, 0);
      }

      grPolarPC->Fit(fitFunc, "", "", lumion, lumioff);

      delete fitFunc;
   }

   delete dummyScale;
} //}}}


/** */
void AnaFillResult::AddHjetPolar(ERingId ringId, ValErrPair ve)
{ //{{{
   //RingId2ValErrMapIter iBeamRes = fHJPolars.find(ringId);
   //if (iBeamRes == fHJPolars.end())
   //else

   fHJPolars[ringId] = ve;
} //}}}
