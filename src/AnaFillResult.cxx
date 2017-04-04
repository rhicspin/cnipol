
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
   fAnaGlobResult(0), fStartTime(LONG_MAX), fEndTime(0), fAnaFillExternResult(fillId),
   fPCPolarGraphs(), fPCPolarInjGraphs(), fPCProfRGraphs(), fPCProfRInjGraphs(),
   fPCTargets(),
   fPCPolarFitRes(), fPolProfRFitRes(),
   fAgsPolFitRes(),
   fFlattopEnergy(0),
   fFillType(kFILLTYPE_UNKNOWN),
   fAnaMeasResults(), fMeasInfos(), fMeasInfosByPolId(),
   fPCPolars(), fPCPolarUnWs(),
   fPCProfRs(),
   fPCPolarsByTargets(),
   fPCProfPolars(), fHJPolars(), fHJAsyms(),
   fBeamPolars(), fBeamPolarP0s(), fBeamPolarSlopes(), fBeamCollPolars(),
   fSystProfPolar(), fSystJvsCPolar(), fPolarRatioU2D(), fRotatorPCPolarRatio(), fRampPCPolarRatio(),
   fMeasTgtOrients(), fMeasTgtIds(), fMeasRingIds(), fPolProfRs(), fPolProfPMaxs(), fPolProfPs()
{
   // Initialize averages with invalid values
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;

      TargetUId unknownTarget(kUNKNOWN_POLID, kUNKNOWN_ORIENT, 0);
      fPCTargets[polId] = unknownTarget;
   }

   // Initialize averages with invalid values
   TargetSetIter iTarget = gRunConfig.fTargets.begin();

   for ( ; iTarget != gRunConfig.fTargets.end(); ++iTarget)
   {
      TargetUId targetUId = iTarget->fUId;

      ValErrPair valErrPair(0, -1);
      fPCPolarsByTargets[targetUId] = valErrPair;
   }
}


/** */
AnaFillResult::~AnaFillResult() { }


/** */
time_t AnaFillResult::GetStartTime()      const { return fStartTime; }
time_t AnaFillResult::GetEndTime()        const { return fEndTime; }
time_t AnaFillResult::GetLumiOnRelTime()  const { return fAnaFillExternResult.fTimeEventLumiOn  - fStartTime; }
time_t AnaFillResult::GetLumiOffRelTime() const { return fAnaFillExternResult.fTimeEventLumiOff - fStartTime; }
time_t AnaFillResult::GetLumiOnTime()     const { return fAnaFillExternResult.fTimeEventLumiOn; }
time_t AnaFillResult::GetLumiOffTime()    const { return fAnaFillExternResult.fTimeEventLumiOff; }


/** */
void AnaFillResult::Print(const Option_t* opt) const
{
   Info("Print", "Print members:");
   printf("Fill %d:\n", fFillId);
   //PrintAsPhp();

   AnaMeasResultMapConstIter iAnaMeasResult = fAnaMeasResults.begin();

   printf("\nPC measurements:\n");

   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult) {
      printf("%s \n", iAnaMeasResult->first.c_str());
      //iAnaMeasResult->second.Print();
   }

   printf("\nPrint polar:\n");

   PolarimeterIdSetConstIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;
      string         sPolId = RunConfig::AsString(polId);

      ValErrPair polarPC = GetPCPolar(polId);
      //ValErrPair polarPC  = GetPCPolarUnW(polId);
      TargetUId targetPC = GetPCTarget(polId);

      cout << sPolId << " [" << targetPC << "]: " << PairAsPhpArray(polarPC) << "      ";

      polarPC = GetPCPolar(polId, &fAnaGlobResult->fNormJetCarbon2);
      //polarPC = GetPCPolarUnW(polId, &fAnaGlobResult->fNormJetCarbon2);

      cout << "   scaled: " << PairAsPhpArray(polarPC) << endl;

      //ValErrPair profPolar = fPCProfPolars.find(polId)->second;

      //cout << "     " << PairAsPhpArray(profPolar) << "      ";
      //profPolar.first  *= fAnaGlobResult->GetNormProfPolar(polId).first;
      //profPolar.second *= fAnaGlobResult->GetNormProfPolar(polId).first;
      //cout << PairAsPhpArray(profPolar) << endl;

      //ValErrPair systPP = fSystProfPolar.find(polId)->second;

      //cout << "     " << "      ";
      //cout << PairAsPhpArray(systPP) << endl;
   }

   //printf("\n");

   //RingIdConstIter iRingId = gRunConfig.fRings.begin();

   //for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId) {
   //   string sRingId = RunConfig::AsString(*iRingId);
   //   ValErrPair polar = fBeamPolars.find(*iRingId)->second;
   //   cout << sRingId << ": " << PairAsPhpArray(polar) << "      ";
   //   ValErrPair systUvsD = fPolarRatioU2D.find(*iRingId)->second;
   //   cout << PairAsPhpArray(systUvsD) << endl;
   //}

   printf("\nPrint polar by target:\n");

   TargetSetIter iTarget = gRunConfig.fTargets.begin();

   for ( ; iTarget != gRunConfig.fTargets.end(); ++iTarget)
   {
      iTarget->Print();
      TargetUId targetUId = iTarget->fUId;

      if (fPCPolarsByTargets.find(targetUId) != fPCPolarsByTargets.end()) {
         cout << ": " << PairAsPhpArray(fPCPolarsByTargets.find(targetUId)->second) << endl;
      }
   }

   printf("\n");

   //cout << "Polarimeter polar:" << endl;
   //cout << MapAsPhpArray<EPolarimeterId, ValErrPair>(fPCPolars) << endl;
   //cout << "Beam polar:" << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fBeamPolars) << endl;
   //cout << "Syst U vs D:" << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fPolarRatioU2D) << endl;
   //cout << "Profile polar:" << endl;
   //cout << MapAsPhpArray<EPolarimeterId, ValErrPair>(fPCProfPolars) << endl;
   //cout << MapAsPhpArray<ERingId, ValErrPair>(fHJPolars) << endl;
   //cout << MapAsPhpArray<ERingId, TgtOrient2ValErrMap>(fPolProfPs) << endl;
}


TGraphErrors* AnaFillResult::GetGrBluIntens() const { return fAnaFillExternResult.GetGrBluIntens(); }
TGraphErrors* AnaFillResult::GetGrYelIntens() const { return fAnaFillExternResult.GetGrYelIntens(); }


/** */
TGraphErrors* AnaFillResult::GetIntensGraph(ERingId ringId) const
{
   TGraphErrors *gr = NULL;

   if (ringId == kBLUE_RING)        gr = GetGrBluIntens();
   else if (ringId == kYELLOW_RING) gr = GetGrYelIntens();

   return gr;
}


/** */
ValErrPair AnaFillResult::GetIntensDecay(ERingId ringId) const
{
   ValErrPair decay(0, -1);

   TGraphErrors *gr = GetIntensGraph(ringId);
   TF1* func = gr->GetFunction("fitFunc");
   if (!func) {
      Warning("GetIntensDecay", "Fill %d. No fit function defined in intensity graph for ringId %d", fFillId, ringId);
      return decay;
   }

   decay.first  = func->GetParameter(1);
   decay.second = func->GetParError(1);

   return decay;
}


/** */
TGraphErrors* AnaFillResult::GetRotCurStarGraph(ERingId ringId) const
{
   TGraphErrors *gr = 0;

   if (ringId == kBLUE_RING && fAnaFillExternResult.fBluRotCurStarGraph)
      gr = fAnaFillExternResult.fBluRotCurStarGraph;
   else if (ringId == kYELLOW_RING && fAnaFillExternResult.fYelRotCurStarGraph)
      gr = fAnaFillExternResult.fYelRotCurStarGraph;

   return gr;
}


/** */
TGraphErrors* AnaFillResult::GetRotCurPhenixGraph(ERingId ringId) const
{
   TGraphErrors *gr = 0;

   if (ringId == kBLUE_RING && fAnaFillExternResult.fBluRotCurPhenixGraph)
      gr = fAnaFillExternResult.fBluRotCurPhenixGraph;
   else if (ringId == kYELLOW_RING && fAnaFillExternResult.fYelRotCurPhenixGraph)
      gr = fAnaFillExternResult.fYelRotCurPhenixGraph;

   return gr;
}


/** */
TGraphErrors* AnaFillResult::GetSnakeCurGraph(ERingId ringId) const
{
   TGraphErrors *gr = 0;

   if (ringId == kBLUE_RING && fAnaFillExternResult.fBluSnakeCurGraph)
      gr = fAnaFillExternResult.fBluSnakeCurGraph;
   else if (ringId == kYELLOW_RING && fAnaFillExternResult.fYelSnakeCurGraph)
      gr = fAnaFillExternResult.fYelSnakeCurGraph;

   return gr;
}


std::vector<TGraphErrors*> AnaFillResult::GetBCCurGraphs(EPolarimeterId polId) const
{
   if (fAnaFillExternResult.fBCCurGraph.count(polId))
     return fAnaFillExternResult.fBCCurGraph.at(polId);

   return vector<TGraphErrors*>();
}


TGraphErrors* AnaFillResult::GetAgsPolFitGraph() const
{
   return fAnaFillExternResult.fAgsPolFitGraph;
}


///** */
//void AnaFillResult::PrintAsPhp(FILE *f) const
//{
//}


/** */
void AnaFillResult::AddMeasResult(AnaMeasResult &result)
{
}


/** */
void AnaFillResult::AddMeasResult(EventConfig &mm)
{
   // this should go away when fanameasresult and measinfo are merged
   mm.fAnaMeasResult->SetStartTime(mm.fMeasInfo->fStartTime);

   string runName = mm.fMeasInfo->GetRunName();

   if (mm.fMeasInfo->fStartTime < fStartTime) fStartTime = mm.fMeasInfo->fStartTime;
   if (mm.fMeasInfo->fStartTime > fEndTime)   fEndTime   = mm.fMeasInfo->fStartTime;

   //Info("AddMeasResult", "fStartTime %d (meas)", mm.fMeasInfo->fStartTime);
   //Info("AddMeasResult", "fStartTime %d", GetStartTime());

   // add or overwrite new AnaFillResult
   fAnaMeasResults[runName] = *mm.fAnaMeasResult;
   fMeasInfos[runName]      = *mm.fMeasInfo;

   EPolarimeterId polId     = MeasInfo::ExtractPolarimeterId(runName);
   // These sets are guaranteed to be ordered by measurement time. See
   // MeasInfo.h for details
   fMeasInfosByPolId[polId].insert( &fMeasInfos[runName] );

   fMeasTgtOrients[runName] =  mm.fMeasInfo->GetTargetOrient();
   fMeasTgtIds[runName]     =  mm.fMeasInfo->GetTargetId();
   fMeasRingIds[runName]    =  mm.fMeasInfo->GetRingId();
}


/**
 * This func can be (should be?) merged with AddMeasResult
 */
void AnaFillResult::AddGraphMeasResult(EventConfig &mm, DrawObjContainer &ocIn)
{
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
}


/** */
void AnaFillResult::Process(DrawObjContainer *ocOut)
{
   // Loop over all measurements in this fill
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
      ValErrPair polarPC = anaMeasResult.GetPCPolar();
      ValErrPair profRPC = anaMeasResult.GetPCProfR();

      // Skip invalid results indicated by negative uncertainty
      if (polarPC.second < 0 || profRPC.second < 0) continue;

      time_t measRelTime = anaMeasResult.GetStartTime() - GetStartTime();

      // Pick the graph for this measurement and corresponding polarimeter
      // or create a new graph with polarization measurements as a func of time
      if (measInfo.GetBeamEnergy() == kINJECTION)
      {
         AppendToPCPolarInjGraph(polId, measRelTime, polarPC.first, 0, polarPC.second);
         AppendToPCProfRInjGraph(polId, tgtOrient, measRelTime, profRPC.first, 0, profRPC.second);
      } else {
         AppendToPCPolarGraph(polId, measRelTime, polarPC.first, 0, polarPC.second);
         AppendToPCProfRGraph(polId, tgtOrient, measRelTime, profRPC.first, 0, profRPC.second);

         TargetUId  newTarget(polId, tgtOrient, tgtId);
         TargetUId& curTarget = fPCTargets[polId];

         if ( curTarget.IsCompletelyUnknown() && !newTarget.IsCompletelyUnknown() )
            curTarget = newTarget;
         else if (curTarget != newTarget) // target was switched in this fill => partially invalidate the current target id
            curTarget.fTargetId = 0;
         // else do nothing
      }

      // The maximum energy achieved in this fill
      if (measInfo.GetBeamEnergy() > fFlattopEnergy)
         fFlattopEnergy = measInfo.GetBeamEnergy();

      //Info("Process", "xxx1");
      // Proceed only with flattop measurements
      if ( !IsValidFlattopMeas(measInfo) ) continue;

      //Info("Process", "zzz %p", (void*) iAnaMeasResult->second.fhAsymVsBunchId_X45);
      //iAnaMeasResult->second.fhAsymVsBunchId_X45->Print();
      if ((fAnaFillExternResult.GetGrBluIntens()->GetN() != 0)
       && (fAnaFillExternResult.GetGrYelIntens()->GetN() != 0))
      {
         //fFlattopEnergy = measInfo.GetBeamEnergy();
         fFillType = kFILLTYPE_PHYSICS;
      }

      if (ocOut)
         CalcAvrgAsymByBunch(anaMeasResult, measInfo, *ocOut);
   }

   // do whatever with the combined fill results
   UpdateExternGraphRange();
   FitExternGraphs();
   FitPCPolarGraphs();
   FitPCProfRGraphs();
   FitAgsPol();

   CalcRotatorPCPolarRatio();
   CalcRampPCPolarRatio();
   CalcRhicAgsPolarRatio();

   // Calculate average polarization and profiles
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId = *iPolId;

      // Calculate average polarizations
      fPCPolars[polId]     = CalcAvrgPolar(polId);
      fPCPolarUnWs[polId]  = CalcAvrgPolarUnweighted(polId);
      fPCProfPolars[polId] = CalcAvrgPolProfPolar(polId);

      fPCPolarsByTargets[fPCTargets[polId]] = fPCPolars[polId];

      // Now calculate average profiles: R and P_max
      // First new method using graphs
      TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();

      for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
      {
         ETargetOrient tgtOrient = *iTgtOrient;

         //fPCProfRInjs[polId][tgtOrient] = CalcAvrgProfRInj(polId, tgtOrient);
         fPCProfRs[polId][tgtOrient]    = CalcAvrgProfR(polId, tgtOrient);
         //printf("fPCProfRs: %f,  %f\n", fPCProfRs[polId][tgtOrient].first, fPCProfRs[polId][tgtOrient].second);
      }


      // Now older method
      // Get values only once for each beam (note we loop over polarimeters)
      ERingId ringId = RunConfig::GetRingId(polId);

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
}


/** */
Bool_t AnaFillResult::IsValidFlattopMeas(const MeasInfo &measInfo)
{
   if (measInfo.GetBeamEnergy() == kINJECTION) return kFALSE;

   //if (!fAnaFillExternResult) return kFALSE;

   //if (measInfo.fStartTime-fStartTime < lumion || measInfo.fStartTime-fStartTime > lumioff) return kFALSE;

   return kTRUE;
}


/** */
ValErrPair AnaFillResult::GetPCPolar(EPolarimeterId polId, PolId2ValErrMap *normJC) const
{
   Double_t norm = 1;

   // norm for polId must exist and the error (...->second.second) must be >= 0
   if ( normJC && normJC->find(polId) != normJC->end() && normJC->find(polId)->second.second >= 0)
      norm = normJC->find(polId)->second.first;

   ValErrPair pcPolar(0, -1);

   PolId2ValErrMapConstIter iPolar = fPCPolars.find(polId);

   if (iPolar != fPCPolars.end() )
   {
      pcPolar.first  = norm * iPolar->second.first;
      pcPolar.second = norm * iPolar->second.second;
   }

   return pcPolar;
}


/** */
ValErrPair AnaFillResult::GetPCPolarUnW(EPolarimeterId polId, PolId2ValErrMap *normJC) const
{
   Double_t norm = 1;

   if (normJC)
      norm = normJC->find(polId) == normJC->end() ? 1 : (*normJC)[polId].first;

   ValErrPair result(0, -1);

   PolId2ValErrMapConstIter iPolar = fPCPolarUnWs.find(polId);

   if (iPolar != fPCPolarUnWs.end() )
   {
      result.first  = norm * iPolar->second.first;
      result.second = norm * iPolar->second.second;
   }

   return result;
}


/** Function can return an empty graph. */
TGraphErrors* AnaFillResult::GetPCPolarGraph(EPolarimeterId polId) const
{
   if (fPCPolarGraphs.find(polId) == fPCPolarGraphs.end()) {
      return 0;
   }

   return (TGraphErrors*) fPCPolarGraphs.find(polId)->second;
}


/** Function cannot return empty graph. */
TGraphErrors* AnaFillResult::GetPCPolarInjGraph(EPolarimeterId polId)
{
   if (fPCPolarInjGraphs.find(polId) == fPCPolarInjGraphs.end()) {
      return 0;
   }

   return (TGraphErrors*) fPCPolarInjGraphs.find(polId)->second;
}


/** Function can return an empty graph. */
TGraphErrors* AnaFillResult::GetPCProfRGraph(EPolarimeterId polId, ETargetOrient tgtOrient) const
{
   TGraphErrors* graph = 0;

   PolId2TgtOrient2TGraphMapConstIter iPCProfRGraph = fPCProfRGraphs.find(polId);

   if (iPCProfRGraph == fPCProfRGraphs.end()) {
      return 0;
   } else {
      TgtOrient2TGraphMapConstIter iTgtPCProfRGraph = iPCProfRGraph->second.find(tgtOrient);

      if (iTgtPCProfRGraph == iPCProfRGraph->second.end()) {
         return 0;
      } else {
         graph = (TGraphErrors*) iTgtPCProfRGraph->second;
      }
   }

   return graph;
}


/** Function can return an empty graph. */
TGraphErrors* AnaFillResult::GetPCProfRInjGraph(EPolarimeterId polId, ETargetOrient tgtOrient) const
{
   TGraphErrors* graph = 0;

   PolId2TgtOrient2TGraphMapConstIter iPCProfRInjGraph = fPCProfRInjGraphs.find(polId);

   if (iPCProfRInjGraph == fPCProfRInjGraphs.end()) {
      return 0;
   } else {
      TgtOrient2TGraphMapConstIter iTgtPCProfRInjGraph = iPCProfRInjGraph->second.find(tgtOrient);

      if (iTgtPCProfRInjGraph == iPCProfRInjGraph->second.end()) {
         return 0;
      } else {
         graph = (TGraphErrors*) iTgtPCProfRInjGraph->second;
      }
   }

   return graph;
}


/** */
ValErrPair AnaFillResult::GetPCPolarP0(EPolarimeterId polId) const
{
   ValErrPair p0(0, -1);

   TGraphErrors *gr = GetPCPolarGraph(polId);
   if (!gr) return p0;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return p0;

   p0.first  = func->GetParameter(0)/100.;
   p0.second = func->GetParError(0)/100.;

   return p0;
}


/** */
ValErrPair AnaFillResult::GetPCPolarSlope(EPolarimeterId polId) const
{
   ValErrPair decay(0, -1);

   const TGraphErrors *gr = GetPCPolarGraph(polId);
   if (!gr) return decay;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return decay;

   decay.first  = func->GetParameter(1)/100.;
   decay.second = func->GetParError(1)/100.;

   return decay;
}


/** */
ValErrPair AnaFillResult::GetPCProfR0(EPolarimeterId polId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   TGraphErrors *gr = GetPCProfRGraph(polId, tgtOrient);
   if (!gr) return result;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return result;

   result.first  = func->GetParameter(0);
   result.second = func->GetParError(0);

   return result;
}


/** */
ValErrPair AnaFillResult::GetPCProfRSlope(EPolarimeterId polId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   TGraphErrors *gr = GetPCProfRGraph(polId, tgtOrient);
   if (!gr) return result;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return result;

   result.first  = func->GetParameter(1);
   result.second = func->GetParError(1);

   return result;
}


/** */
//ValErrPair AnaFillResult::GetPCPolarPmax(EPolarimeterId polId, ETargetOrient tgtOrient)
//{
//   ValErrPair result(0, -1);
//
//   TGraphErrors *gr = GetPCProfRGraph(polId, tgtOrient);
//   if (!gr) return result;
//
//   TF1* func = gr->GetFunction("fitFunc");
//   if (!func) return result;
//
//   result.first  = func->GetParameter(0);
//   result.second = func->GetParError(0);
//
//   return result;
//}


/** */
ValErrPair AnaFillResult::GetHJPolar(EPolarimeterId polId)
{
   ERingId ringId = RunConfig::GetRingId(polId);
   return GetHJPolar(ringId);
}


/** */
ValErrPair AnaFillResult::GetHJPolar(ERingId ringId) const
{
   return fHJPolars.find(ringId) == fHJPolars.end() ? ValErrPair(0, -1) : fHJPolars.find(ringId)->second;
}


/** */
ValErrPair AnaFillResult::GetHJAsym(EPolarimeterId polId)
{
   ERingId ringId = RunConfig::GetRingId(polId);
   return GetHJAsym(ringId);
}


/** */
ValErrPair AnaFillResult::GetHJAsym(ERingId ringId)
{
   return fHJAsyms.find(ringId) == fHJAsyms.end() ? ValErrPair(0, -1) : fHJAsyms[ringId];
}


/** */
ValErrPair AnaFillResult::GetBeamPolar(ERingId ringId) const
{
   RingId2ValErrMapConstIter iBeamPolar = fBeamPolars.find(ringId);
   return iBeamPolar == fBeamPolars.end() ? ValErrPair(0, -1) : iBeamPolar->second;
}


/** */
ValErrPair AnaFillResult::GetBeamPolarP0(ERingId ringId) const
{
   RingId2ValErrMapConstIter iBeamPolarP0 = fBeamPolarP0s.find(ringId);
   return iBeamPolarP0 == fBeamPolarP0s.end() ? ValErrPair(0, -1) : iBeamPolarP0->second;
}


/** */
ValErrPair AnaFillResult::GetBeamPolarDecay(ERingId ringId) const
{
   RingId2ValErrMapConstIter iBeamPolarDecay = fBeamPolarSlopes.find(ringId);
   return iBeamPolarDecay == fBeamPolarSlopes.end() ? ValErrPair(0, -1) : iBeamPolarDecay->second;
}


/** */
ValErrPair AnaFillResult::GetPolarUDRatio(ERingId ringId)
{
   return fPolarRatioU2D.find(ringId) == fPolarRatioU2D.end() ? ValErrPair(0, -1) : fPolarRatioU2D[ringId];
}


/** */
ValErrPair AnaFillResult::GetSystJvsCPolar(EPolarimeterId polId)
{
   return fSystJvsCPolar.find(polId) == fSystJvsCPolar.end() ? ValErrPair(0, -1) : fSystJvsCPolar[polId];
}


/** */
ValErrPair AnaFillResult::GetSystProfPolar(EPolarimeterId polId)
{
   return fSystProfPolar.find(polId) == fSystProfPolar.end() ? ValErrPair(0, -1) : fSystProfPolar[polId];
}


ValErrPair AnaFillResult::GetAgsCniPolar() const
{
   if (!fAgsPolFitRes->IsValid()) {
      return ValErrPair();
   }
   return ValErrPair(fAgsPolFitRes->Value(0), fAgsPolFitRes->FitResult::Error(0));
}


/** */
void AnaFillResult::CalcBeamPolar(Bool_t doNorm)
{
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId        = *iPolId;
      ValErrPair     pcPolar      = GetPCPolar(polId);
      ValErrPair     pcPolarP0    = GetPCPolarP0(polId);
      ValErrPair     pcPolarDecay = GetPCPolarSlope(polId);
      ERingId        ringId       = RunConfig::GetRingId(polId);

      if (!fAnaGlobResult)
         Warning("CalcBeamPolar", "fAnaGlobResult not defined in fill %d", fFillId);

      Double_t norm = doNorm && fAnaGlobResult ? fAnaGlobResult->GetNormJetCarbon(polId).first : 1;

      if (pcPolar.second >= 0)      { pcPolar.first      *= norm;  pcPolar.second      *= norm; }
      if (pcPolarP0.second >= 0)    { pcPolarP0.first    *= norm;  pcPolarP0.second    *= norm; }
      if (pcPolarDecay.second >= 0) { pcPolarDecay.first *= norm;  pcPolarDecay.second *= norm; }

      if ( fBeamPolars.find(ringId) == fBeamPolars.end() ) {
         fBeamPolars[ringId]      = pcPolar;
         fBeamPolarP0s[ringId]    = pcPolarP0;
         fBeamPolarSlopes[ringId] = pcPolarDecay;
      } else {
         fBeamPolars[ringId]      = utils::CalcWeightedAvrgErr(fBeamPolars[ringId],      pcPolar);
         fBeamPolarP0s[ringId]    = utils::CalcWeightedAvrgErr(fBeamPolarP0s[ringId],    pcPolarP0);
         fBeamPolarSlopes[ringId] = utils::CalcWeightedAvrgErr(fBeamPolarSlopes[ringId], pcPolarDecay);
      }
   }
}


/** */
RingId2ValErrMap AnaFillResult::CalcPolarRatioU2D(PolId2ValErrMap &normJC)
{
   RingId2ValErrMapConstIter iBeamPolar = fBeamPolars.begin();

   for ( ; iBeamPolar != fBeamPolars.end(); ++iBeamPolar)
   {
      ERingId    ringId = iBeamPolar->first;
      ValErrPair polarU(-1, -1);
      ValErrPair polarD(-1, -1);

      if (ringId == kBLUE_RING) {
         polarU = GetPCPolar(kB1U, &normJC);
         polarD = GetPCPolar(kB2D, &normJC);
      }

      if (ringId == kYELLOW_RING) {
         polarU = GetPCPolar(kY2U, &normJC);
         polarD = GetPCPolar(kY1D, &normJC);
      }

      if (polarU.second < 0 || polarD.second < 0) {
         fPolarRatioU2D[ringId] = ValErrPair(-1, -1);
         continue; // skip invalid result
      }

      // 1 = 100% correlation between U and D is a more conservative assumption
      // 0 =   0% correlation between U and D is more realistic?
      //fPolarRatioU2D[ringId] = utils::CalcDivision(polarU, polarD, 1);
      fPolarRatioU2D[ringId] = utils::CalcDivision(polarU, polarD, 0);
   }

   return fPolarRatioU2D;
}


/**
 * Calculates and returns the ratio of the HJ to PC polarization. The latter is
 * expected to be normalized.
 */
PolId2ValErrMap AnaFillResult::CalcSystJvsCPolar(PolId2ValErrMap &normJC)
{
   PolId2ValErrMapConstIter iPolar = fPCPolars.begin();

   for ( ; iPolar != fPCPolars.end(); ++iPolar)
   {
      EPolarimeterId polId   = iPolar->first;
      ERingId        ringId  = RunConfig::GetRingId(polId);
      ValErrPair     polarPC = GetPCPolar(polId, &normJC);  // Get normalized p-Carbon polarization
      ValErrPair     polarHJ = GetHJPolar(ringId);

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
}


/** */
PolId2ValErrMap AnaFillResult::CalcSystProfPolar(PolId2ValErrMap &normPP)
{
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
}


/** */
ValErrPair AnaFillResult::GetPCProfR(ERingId ringId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   RingId2TgtOrient2ValErrMapConstIter iPolProfR = fPolProfRs.find(ringId);
   if (iPolProfR == fPolProfRs.end()) return result;

   TgtOrient2ValErrMap tgtPolProfRs = iPolProfR->second;

   TgtOrient2ValErrMapConstIter iTgtPolProfR = tgtPolProfRs.find(tgtOrient);
   if (iTgtPolProfR == tgtPolProfRs.end()) return result;

   return iTgtPolProfR->second;
}


/** */
ValErrPair AnaFillResult::GetPCProfR(EPolarimeterId polId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   PolId2TgtOrient2ValErrMapConstIter iPCProfR = fPCProfRs.find(polId);
   if (iPCProfR == fPCProfRs.end()) return result;

   TgtOrient2ValErrMap tgtPCProfRs = iPCProfR->second;

   TgtOrient2ValErrMapConstIter iTgtPCProfR = tgtPCProfRs.find(tgtOrient);
   if (iTgtPCProfR == tgtPCProfRs.end()) return result;

   return iTgtPCProfR->second;
}


/** */
ValErrPair AnaFillResult::GetPCProfRInj(EPolarimeterId polId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   TGraphErrors *gr = GetPCProfRInjGraph(polId, tgtOrient);
   if (!gr) return result;

   TF1* func = gr->GetFunction("fitFunc");
   if (!func) return result;

   result.first  = func->GetParameter(0);
   result.second = func->GetParError(0);

   return result;
}


/** */
ValErrPair AnaFillResult::GetPCProfPMax(ERingId ringId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   RingId2TgtOrient2ValErrMapConstIter   iPolProfPMax = fPolProfPMaxs.find(ringId);
   if (iPolProfPMax == fPolProfPMaxs.end()) return result;

   TgtOrient2ValErrMap tgtPolProfPMaxs = iPolProfPMax->second;

   TgtOrient2ValErrMapConstIter iTgtPolProfPMax = tgtPolProfPMaxs.find(tgtOrient);
   if (iTgtPolProfPMax == tgtPolProfPMaxs.end()) return result;

   return iTgtPolProfPMax->second;
}


/** */
ValErrPair AnaFillResult::GetPCProfP(ERingId ringId, ETargetOrient tgtOrient) const
{
   ValErrPair result(0, -1);

   RingId2TgtOrient2ValErrMapConstIter   iPolProfP = fPolProfPs.find(ringId);
   if (iPolProfP == fPolProfPs.end()) return result;

   TgtOrient2ValErrMap tgtPolProfPs = iPolProfP->second;

   TgtOrient2ValErrMapConstIter iTgtPolProfP = tgtPolProfPs.find(tgtOrient);
   if (iTgtPolProfP == tgtPolProfPs.end()) return result;

   return iTgtPolProfP->second;
}


/** */
ValErrPair AnaFillResult::CalcAvrgPolar(EPolarimeterId polId)
{
   ValErrPair avrgPol(0, -1);

   // Check that we have graph
   const TGraphErrors *grPCPolar = GetPCPolarGraph(polId);
   if (!grPCPolar) {
      Warning("CalcAvrgPolar", "Fill %d. No graph defined for polId %d", fFillId, polId);
      return avrgPol;
   } else {
      Warning("CalcAvrgPolar", "Fill %d. Graph defined for polId %d", fFillId, polId);
      //grPCPolar->Print();
   }

   // Check that graph has fit func
   TF1* funcPCPolar = grPCPolar->GetFunction("fitFunc");
   if (!funcPCPolar) {
      Warning("CalcAvrgPolar", "Fill %d. No fit function found in graph for polId %d", fFillId, polId);
      return avrgPol;
   }

   // Get intens graph
   ERingId ringId = RunConfig::GetRingId(polId);

   TGraphErrors *grIntens = GetIntensGraph(ringId);
   if (grIntens->GetN() == 0) {
      Warning("CalcAvrgPolar", "No intensity values available for weighted average calculation %d. Unweighted average returned", polId);
      avrgPol = CalcAvrgPolarUnweighted(polId);
      return avrgPol;
   }

   Double_t time, intens, polar, polarErr;
   Double_t numer = 0, denom = 0, numerErr = 0, denomErr = 0;

   Double_t polarNormErr  = funcPCPolar->GetParError(0);
   Double_t polarSlopeErr = funcPCPolar->GetParError(1);

   for (UInt_t iPoint=0; iPoint < (UInt_t) grIntens->GetN(); iPoint++)
   {
      grIntens->GetPoint(iPoint, time, intens);
      polar = funcPCPolar->Eval(time);

      numer += polar*intens;
      denom += intens;

      // assume no correlation between the polarization slope (par1) and norm (par0)
      polarErr = sqrt( polarSlopeErr*polarSlopeErr*(time - GetLumiOnRelTime())*(time - GetLumiOnRelTime())/3600/3600 + polarNormErr*polarNormErr);
      numerErr += polarErr*intens;
      denomErr += intens;
   }

   // Internaly all polarization values are between 0 and 1 except graphs
   avrgPol.first  = numer/denom/100.;
   avrgPol.second = numerErr/denomErr/100.; // weighted error

   return avrgPol;
}


/** */
ValErrPair AnaFillResult::CalcAvrgPolarUnweighted(EPolarimeterId polId)
{
   // Create a container to keep all measurements for polarimeter polId
   ValErrPair avrgPol(0, -1);

   const TGraphErrors *grPCPolar = GetPCPolarGraph(polId);

   if (!grPCPolar) {
      Warning("CalcAvrgPolarUnweighted", "No graph defined for polId %d", polId);
      return avrgPol;
   }

   Double_t time, polar, polarErr;

   // Loop over all polarization points
   for (UInt_t iPoint=0; iPoint<(UInt_t) grPCPolar->GetN(); iPoint++)
   {
      grPCPolar->GetPoint(iPoint, time, polar);

      // Exclude points only if the lumi/phys period is defined && > 600 sec
      // Otherwise include all available measurements in the average
      if ( abs(GetLumiOffRelTime() - GetLumiOnRelTime()) > 600 &&
           (time < GetLumiOnRelTime() || time > GetLumiOffRelTime()) ) continue;

      polarErr = grPCPolar->GetErrorY(iPoint);

      // Internaly all polarization values are between 0 and 1 except graphs
      ValErrPair polarVE(polar/100., polarErr/100.);

      avrgPol = utils::CalcWeightedAvrgErr(avrgPol, polarVE);
   }

   return avrgPol;
}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfPolar(EPolarimeterId polId)
{
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
}


/** */
ValErrPair AnaFillResult::CalcAvrgProfR(EPolarimeterId polId, ETargetOrient tgtOrient)
{
   // Create a container to keep all measurements for polarimeter polId
   ValErrPair avrgProfR(0, -1);

   const TGraphErrors *grPCProfR = GetPCProfRGraph(polId, tgtOrient);

   if (!grPCProfR) {
      Warning("CalcAvrgProfR", "No graph defined for polId %d, tgtOrient %d", polId, tgtOrient);
      return avrgProfR;
   } else {
      Info("CalcAvrgProfR", "Found profile graph for polId %d, tgtOrient %d", polId, tgtOrient);
      //grPCProfR->Print();
   }

   // Loop over all data points
   for (UInt_t iPoint=0; iPoint<(UInt_t) grPCProfR->GetN(); iPoint++)
   {
      Double_t time, profR;
      grPCProfR->GetPoint(iPoint, time, profR);

      // Exclude points only if the lumi/phys period is defined && > 600 sec
      // Otherwise include all available measurements in the average
      if ( abs(GetLumiOffRelTime() - GetLumiOnRelTime()) > 600 &&
           (time < GetLumiOnRelTime() || time > GetLumiOffRelTime()) ) continue;

      Double_t profRErr = grPCProfR->GetErrorY(iPoint);

      ValErrPair profRVE(profR, profRErr);

      avrgProfR = utils::CalcWeightedAvrgErr(avrgProfR, profRVE);

      //printf("v, e: %f, %f, %f, %f\n", profR, profRErr, avrgProfR.first, avrgProfR.second);
   }

   return avrgProfR;
}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfR(ERingId ringId, ETargetOrient tgtOrient)
{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iAnaMeasResult = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient     = fMeasTgtOrients.begin();
   String2RingIdMapIter      iRingId        = fMeasRingIds.begin();

   for ( ; iAnaMeasResult!=fAnaMeasResults.end(); ++iAnaMeasResult, ++iTgtOrient, ++iRingId)
   {
      // Consider only results for ringId and tgtOrient
      if ( iRingId->second    != ringId)    continue;
      if ( iTgtOrient->second != tgtOrient) continue;

      ValErrPair    val_err(0, -1);
      TFitResultPtr fitres = iAnaMeasResult->second.fFitResProfilePvsI;

      if (fitres.Get())
      {
         val_err.first  = fitres->Value(1);
         val_err.second = fitres->FitResult::Error(1);
      } else {
         Error("CalcAvrgPolProfR", "No fit result found. Skipping...");
         continue;
      }

      allRunResults.insert(val_err);
   }

   return utils::CalcWeightedAvrgErr(allRunResults);
}


/** */
ValErrPair AnaFillResult::CalcAvrgPolProfPMax(ERingId ringId, ETargetOrient tgtOrient)
{
   ValErrSet allRunResults;

   AnaMeasResultMapConstIter iAnaMeasResult = fAnaMeasResults.begin();
   String2TgtOrientMapIter   iTgtOrient     = fMeasTgtOrients.begin();
   String2RingIdMapIter      iRingId        = fMeasRingIds.begin();

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

}


/** */
ValErrPair AnaFillResult::CalcPolProfP(ValErrPair R, ValErrPair PMax)
{
   ValErrPair polProfP(0, -1);

   if (PMax.second < 0 || R.second < 0 || R.first <= -1) return polProfP;

   polProfP.first = PMax.first / sqrt(1 + R.first);

   //relDelta2 = PMax.second * PMax.second / PMax.first / PMax.first + R.second * R.second / R.first / R.first;
   //polProfP.second = polProfP.first * sqrt(relDelta2);

   Double_t delta2 = PMax.first * PMax.first * R.second *
                     R.second/4./(1+R.first)/(1+R.first)/(1+R.first) +
                     PMax.second * PMax.second / (1 + R.first);

   polProfP.second = sqrt(delta2);

   return polProfP;
}


/** */
void AnaFillResult::CalcRotatorPCPolarRatio()
{
   Info("CalcRotatorPCPolarRatio", "Called");

   PolId2MeasInfoPtrSetMapConstIter iPolId2MeasInfoPtrSet = fMeasInfosByPolId.begin();

   for ( ; iPolId2MeasInfoPtrSet != fMeasInfosByPolId.end(); ++iPolId2MeasInfoPtrSet)
   {
      EPolarimeterId  polId           = iPolId2MeasInfoPtrSet->first;
      //cout << "polId: " << RunConfig::AsString(polId) << endl;
      const MeasInfoPtrSet &measInfos = iPolId2MeasInfoPtrSet->second;

      // Now loop over all measInfos for this polarimeter and select the first
      // one with small rotator current
      ValErrPair polarBeforeRot(0, -1);
      ValErrPair polarAfterRot(0, -1);
      time_t     timeBeforeRot = 0;

      MeasInfoPtrSetConstIter iMeasInfoPtr = measInfos.begin();

      for ( ; iMeasInfoPtr != measInfos.end(); ++iMeasInfoPtr)
      {
         const MeasInfo &measInfo = **iMeasInfoPtr;
         //cout << "runName: " << measInfo.GetRunName() << ", IsStarRotatorOn: " << measInfo.IsStarRotatorOn()<< endl;
         //cout << "runName: " << measInfo.GetRunName() << ", IsStarRotatorOn: " << measInfo.IsStarRotatorOn()<< endl;

         if (measInfo.GetBeamEnergy() <= kINJECTION) continue;

         if ( !measInfo.IsStarRotatorOn() )
         {
            polarBeforeRot = fAnaMeasResults[measInfo.GetRunName()].GetPCPolar();
            timeBeforeRot  = measInfo.GetStartTime();
            //cout << "polarBeforeRot: " << PairAsPhpArray(polarBeforeRot) << endl;
            continue;
         }

         // if found meas with rotators off
         if (measInfo.IsStarRotatorOn() && polarBeforeRot.second > 0 && fabs(measInfo.GetStartTime() - timeBeforeRot) < 3600)
         {
            polarAfterRot = fAnaMeasResults[measInfo.GetRunName()].GetPCPolar();
            //cout << "polarAfterRot : " << PairAsPhpArray(polarAfterRot) << endl;
         }
      }

      fRotatorPCPolarRatio[polId] = utils::CalcDivision(polarAfterRot, polarBeforeRot, 0);

      //cout << "polarBeforeRot: " << PairAsPhpArray(polarBeforeRot) << endl;
      //cout << "polarAfterRot : " << PairAsPhpArray(polarAfterRot) << endl;
      //cout << "fRotatorPCPolarRatio : " << PairAsPhpArray(fRotatorPCPolarRatio[polId]) << endl;
   }
}


/** */
void AnaFillResult::CalcRampPCPolarRatio()
{
   Info("CalcRampPCPolarRatio", "Called");

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId = *iPolId;

      TGraphErrors *grPCPolar    = GetPCPolarGraph(polId);
      TGraphErrors *grPCPolarInj = GetPCPolarInjGraph(polId);

      if (!grPCPolar || !grPCPolarInj) continue;

      // Get the last injection measurement 
      Double_t x, y, ye;
      Int_t nPoints = grPCPolarInj->GetN();
      grPCPolarInj->GetPoint(nPoints-1, x, y);
      ye = grPCPolarInj->GetErrorY(nPoints-1);

      ValErrPair pcPolarInj(y/100., ye/100.);
      ValErrPair pcPolarP0 = GetPCPolarP0(polId);

      fRampPCPolarRatio[polId] = utils::CalcDivision(pcPolarP0, pcPolarInj, 0);

      //cout << "pcPolarP0:         " << PairAsPhpArray(pcPolarP0) << endl;
      //cout << "pcPolarInj :       " << PairAsPhpArray(pcPolarInj) << endl;
      //cout << "fRampPCPolarRatio: " << PairAsPhpArray(fRampPCPolarRatio[polId]) << endl;
   }
}


void AnaFillResult::CalcRhicAgsPolarRatio()
{
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId = *iPolId;

      TGraphErrors *grPCPolarInj = GetPCPolarInjGraph(polId);

      if (!grPCPolarInj) continue;

      // Get the first injection measurement
      Double_t x, y, ye;
      grPCPolarInj->GetPoint(0, x, y);
      ye = grPCPolarInj->GetErrorY(0);

      ValErrPair pcPolarInj(y, ye);
      ValErrPair agsPolar = GetAgsCniPolar();

      fRhicAgsPolarRatio[polId] = utils::CalcDivision(pcPolarInj, agsPolar, 0);
   }
}


/** */
void AnaFillResult::CalcAvrgAsymByBunch(const AnaMeasResult &amr, const MeasInfo &mi, DrawObjContainer &ocOut) const
{
   MAsymFillHists *fillHists;

   if (ocOut.d.find("fills") == ocOut.d.end() ) {
      Error("CalcAvrgAsymByBunch", "No output container 'fills' found");
      return;
   } else {
      fillHists = (MAsymFillHists*) ocOut.d.find("fills")->second;
   }

   ERingId ringId = mi.GetRingId();
   string sRingId = RunConfig::AsString(ringId);

   stringstream ssFillId("");
   ssFillId << fFillId;
   string sFillId = ssFillId.str();

   //MAsymSingleFillHists *singleFillHists = (MAsymSingleFillHists*) fillHists->GetSingleFillHists(fFillId);
   MAsymSingleFillHists *singleFillHists = (MAsymSingleFillHists*) fillHists->GetSingleFillHists(*this);

   TH1* hAsymVsBunchId_X_  = (TH1*) singleFillHists->o["hAsymVsBunchId_X_" + sFillId + "_" + sRingId];
   TH1* hAsymVsBunchId_X90 = amr.fhAsymVsBunchId_X90;
   TH1* hAsymVsBunchId_X45 = amr.fhAsymVsBunchId_X45;

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      string grName = "grAsymVsBunchId_X_" + sSS;
      TGraphErrors* grAsymVsBunchId_X_fill   = (TGraphErrors*) hAsymVsBunchId_X_->GetListOfFunctions()->FindObject(grName.c_str());


      grName = "grAsymVsBunchId_X45_" + sSS;
      TGraphErrors* grAsymVsBunchId_X45 = hAsymVsBunchId_X45 ? (TGraphErrors*) hAsymVsBunchId_X45->GetListOfFunctions()->FindObject(grName.c_str()) : 0;

      utils::MergeGraphs(grAsymVsBunchId_X_fill, grAsymVsBunchId_X45);


      grName = "grAsymVsBunchId_X90_" + sSS;
      TGraphErrors* grAsymVsBunchId_X90 = hAsymVsBunchId_X90 ? (TGraphErrors*) hAsymVsBunchId_X90->GetListOfFunctions()->FindObject(grName.c_str()) : 0;

      utils::MergeGraphs(grAsymVsBunchId_X_fill, grAsymVsBunchId_X90);
   }

   //TGraphErrors *grAsymVsBunchId_X90 = new TGraphErrors(0);
   //utils::MergeGraphs(grAsymVsBunchId_X90, hAsymVsBunchId_X90->GetListOfFunctions());
   //utils::BinGraph(grAsymVsBunchId_X90, hAsymVsBunchId_X90_fill);
}


/** */
void AnaFillResult::UpdateExternGraphRange()
{
   // Fit the intensity graphs

   // Loop over polarimeters
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      Double_t x, y;
      vector<TGraphErrors*> grs = GetBCCurGraphs(*iPolId);

      for(vector<TGraphErrors*>::const_iterator it = grs.begin(); it != grs.end(); it++)
      {
         TGraphErrors *gr = *it;
         for (Int_t i=0; i<gr->GetN(); ++i) { gr->GetPoint(i, x, y); gr->SetPoint(i, x - fStartTime, y); }
      }
   }

   {
      Double_t x, y;
      TGraphErrors *gr = GetAgsPolFitGraph();
      if (gr) {
         for (Int_t i=0; i<gr->GetN(); ++i) { gr->GetPoint(i, x, y); gr->SetPoint(i, x - fStartTime, y); }
      }
   }

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
}


/** */
void AnaFillResult::FitExternGraphs()
{
   time_t lumion  = fAnaFillExternResult.fTimeEventLumiOn  - fStartTime;
   time_t lumioff = fAnaFillExternResult.fTimeEventLumiOff - fStartTime;

   if (lumion >= lumioff) {
      Error("FitExternGraphs", "Lumi-on and lumi-off markers are invalid");
      return;
   }

   Info("FitExternGraphs", "Using range %ld - %ld", lumion, lumioff);


   // Fit the intensity graphs
   // Loop over rings
   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId = *iRingId;

      TGraphErrors *grIntens = GetIntensGraph(ringId);

      if (grIntens->GetN() == 0) continue;

      //if ( fabs(lumioff - lumion) < 3600 ) continue;

      stringstream ssFormula("");
      //ssFormula << "[0] * 1./exp((x - " << lumion << ")/3600./[1])";
      ssFormula << "[0] * (1.- (x - " << lumion << ")/3600./[1])";

      TF1 fitFunc("fitFunc", ssFormula.str().c_str());
      fitFunc.SetParNames("I_{0}", "Lifetime, h");
      fitFunc.SetParameters(100, 50);
      fitFunc.SetParLimits(0, 50, 300);
      fitFunc.SetParLimits(1, 1, 200);

      grIntens->Fit(&fitFunc, "M E", "", lumion, lumioff);
   }
}


/** */
void AnaFillResult::FitPCPolarGraphs()
{
   // Fit the polarization decay graphs
   // Create a function to multiply graphs by 100% for aesthetic reasons
   TF2 *dummyScale = new TF2("dummyScale", "100.*y");

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId = *iPolId;

      // Fit injection measurements first...
      TGraphErrors *grPCPolarInj = GetPCPolarInjGraph(polId);

      // All of this has to be combined with FT measurements and processed the same way
      if (grPCPolarInj)
      {
         grPCPolarInj->Apply(dummyScale);

         if (grPCPolarInj->GetN() >= 2)
         {
            Double_t xmin, ymin, xmax, ymax;
            grPCPolarInj->ComputeRange(xmin, ymin, xmax, ymax);

            if (fabs(xmax - xmin) > 3600*3) // fit only if injection run > 3 hours
            {
               Info("FitPCPolarGraphs", "Fill %d. Fitting injection graph for polId %d", fFillId, polId);

               TF1 fitFunc("fitFunc", "pol0");
               fitFunc.SetParNames("P_{0}, %");
               grPCPolarInj->Fit(&fitFunc, "", "", xmin, xmax);
            }
         }
      }

      // Fit flattop measurements...
      TGraphErrors *grPCPolar  = GetPCPolarGraph(polId);

      if (!grPCPolar || grPCPolar->GetN() <= 0) continue; // require a valid graph

      grPCPolar->Apply(dummyScale);

      //Info("FitPCPolarGraphs", "Using range %d - %d", GetLumiOnRelTime(), GetLumiOffRelTime());

      stringstream ssFormula("");
      ssFormula << "[0] + [1]*(x - (" << GetLumiOnRelTime() << ") )/3600.";
      TF1 fitFunc("fitFunc", ssFormula.str().c_str());
      fitFunc.SetParNames("P_{0}, %", "Decay, %/h");
      Double_t xmin, ymin, xmax, ymax, first;
      grPCPolar->ComputeRange(xmin, ymin, xmax, ymax);
      first= xmax;
      if ( GetLumiOffRelTime() > GetLumiOnRelTime() && grPCPolar->GetN() > 1)
      {
	TGraph *tmpGraph = new TGraph(0);
	tmpGraph = (TGraph*) grPCPolar->Clone();
	Double_t time ,y, timediff;

	if (fabs(xmax - xmin) < 3600)
	  {
	    fitFunc.FixParameter(1, 0);
	  }
	for(Int_t i=0; i < tmpGraph->GetN(); ++i)
	  {
	    tmpGraph->GetPoint(i,time,y);
	    time = time + GetStartTime(); // time of each measurement
	    timediff  = GetLumiOnTime()-time; // time difference between the measurement and physics on
	    if(fabs(timediff) < first )
	      {
		first = timediff;
		xmin = time - GetStartTime();
	      }
	  }
	delete tmpGraph;
      }
      else {
	if (fabs(xmax - xmin) < 3600)
	  fitFunc.FixParameter(1, 0);
      }
      // fit with a const if only one data point is available
      Info("FitPCPolarGraphs", "Fill %d. Fitting flattop graph for polId %d", fFillId, polId);
      grPCPolar->Fit(&fitFunc, "", "", xmin, xmax);
   }

   delete dummyScale;
}


/** */
void AnaFillResult::FitPCProfRGraphs()
{

   // Fit the graphs
  PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
  for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
    {
      EPolarimeterId polId = *iPolId;
      // Now fit pol. profiles
      TargetOrientSetIter iTgtOrient = gRunConfig.fTargetOrients.begin();
      for ( ; iTgtOrient != gRunConfig.fTargetOrients.end(); ++iTgtOrient)
	{
	  ETargetOrient tgtOrient = *iTgtOrient;
	  // fit injection first...
	  TGraphErrors *grPCProfRInj = GetPCProfRInjGraph(polId, tgtOrient);
	  
	  if (grPCProfRInj && grPCProfRInj->GetN() >= 1) {
            Info("FitPCProfRGraphs", "Fill %d. Fitting injection graph for polId %d", fFillId, polId);
	    
            Double_t xmin, ymin, xmax, ymax;
            grPCProfRInj->ComputeRange(xmin, ymin, xmax, ymax);
	    
            TF1 fitFunc("fitFunc", "pol0");
            fitFunc.SetParNames("R_{0}, %");
            grPCProfRInj->Fit(&fitFunc, "", "", xmin, xmax);
	  }
	  TGraphErrors *grPCProfR = GetPCProfRGraph(*iPolId, tgtOrient);
	  if (!grPCProfR) continue;
	  Double_t xmin, ymin, xmax, ymax, first;
	  
	  if (!grPCProfR || grPCProfR->GetN() <= 0) continue; // require a valid graph

	  // //Info("FitPCPolarGraphs", "Using range %d - %d", GetLumiOnRelTime(), GetLumiOffRelTime());
	  
	  stringstream ssFormula("");
	  ssFormula << "[0] + [1]*(x - (" << GetLumiOnRelTime() << ") )/3600.";
	  TF1 fitFunc("fitFunc", ssFormula.str().c_str());
	  fitFunc.SetParNames("R_{0}, %", "Slope/h");
	  grPCProfR->ComputeRange(xmin, ymin, xmax, ymax);
	  first= xmax;
	  if ( GetLumiOffRelTime() > GetLumiOnRelTime() && grPCProfR->GetN() > 1)
	    {
	      TGraph *tmpGraph = new TGraph(0);
	      tmpGraph = (TGraph*) grPCProfR->Clone();
	      Double_t time,y, timediff;
	      //printf("FillId %i, xmax-xmin: %f \n", fFillId, xmax - xmin);
	      if (fabs(xmax - xmin) < 3600)
		{
		  fitFunc.FixParameter(1, 0);
		}
	      for(Int_t i=0; i < tmpGraph->GetN(); ++i)
		{
		  tmpGraph->GetPoint(i,time,y);
		  time = time + GetStartTime();
		  timediff  = GetLumiOnTime()-time;
		  if(fabs(timediff) < first )
		    {
		      first = timediff;
		      xmin = time - GetStartTime();
		    }
		}
	    }
	  else {
	    if (fabs(xmax - xmin) < 3600)
	      fitFunc.FixParameter(1, 0);
	  }
	  // fit with a const if only one data point is available
	  Info("FitPCProfR", "Fill %d. Fitting flattop graph for polId %d", fFillId, polId);
	  grPCProfR->Fit(&fitFunc, "", "", xmin, xmax);
	}
    }
}

/// This implements averaging of the polarization measurements via fitting. The
/// measurements are currently chosen in an interval [t-0.5h,t] where t is the
/// start of the fill. There will be, however, many measurements outside that
/// range. AGS does not store its beam, it is instead processed within a single
/// supercycle (which lasts ~4 seconds) and then either end up consumed by the
/// user (RHIC) or (more often) dumped. We are only looking for measurements of
/// the polarization done for a beam that is going to be injected in our
/// specific fill. For the purposes of this study we only only assume that the
/// settings are not changed during the interval mentioned above. This
/// definition can be improved, but it is good enough for the purposes of this
/// (small) study.
void AnaFillResult::FitAgsPol()
{
   TGraphErrors *gr = GetAgsPolFitGraph();
   if (gr) {
      fAgsPolFitRes = gr->Fit("pol0", "QS", "", -0.5 * 60 * 60, 0); // Q - Quiet, S - return FitResult
   }
}

/** */
void AnaFillResult::SetHJPolar(ERingId ringId, ValErrPair ve)
{
   fHJPolars[ringId] = ve;
}


/** */
void AnaFillResult::SetHJAsym(ERingId ringId, ValErrPair ve)
{
   fHJAsyms[ringId] = ve;
}


/** */
void AnaFillResult::AppendToPCPolarGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye)
{
   TGraphErrors *graph = GetPCPolarGraph(polId);

   if (!graph) {
      Info("AppendToPCPolarGraph", "Fill %d. Created new graph for polId %d", fFillId, polId);
      graph = new TGraphErrors();
      fPCPolarGraphs[polId] = graph;
   }

   Info("AppendToPCPolarGraph", "Fill %d. Added measurement to graph polId %d", fFillId, polId);
   utils::AppendToGraph(graph, x, y, xe, ye);
}


/** */
void AnaFillResult::AppendToPCPolarInjGraph(EPolarimeterId polId, Double_t x, Double_t y, Double_t xe, Double_t ye)
{
   TGraphErrors *graph = GetPCPolarInjGraph(polId);

   if (!graph) {
      Info("AppendToPCPolarInjGraph", "Fill %d. Created new injection graph for polId %d", fFillId, polId);
      graph = new TGraphErrors();
      fPCPolarInjGraphs[polId] = graph;
   }

   utils::AppendToGraph(graph, x, y, xe, ye);
}


/** */
void AnaFillResult::AppendToPCProfRGraph(EPolarimeterId polId, ETargetOrient tgtOrient, Double_t x, Double_t y, Double_t xe, Double_t ye)
{
   TGraphErrors *graph = GetPCProfRGraph(polId, tgtOrient);

   if (!graph) {
      Info("AppendToPCProfRGraph", "Fill %d. Created new graph for polId %d, tgtOrient %d", fFillId, polId, tgtOrient);
      graph = new TGraphErrors();
      fPCProfRGraphs[polId][tgtOrient] = graph;
   }

   Info("AppendToPCProfRGraph", "Fill %d. Added measurement to graph polId %d, tgtOrient %d", fFillId, polId, tgtOrient);
   utils::AppendToGraph(graph, x, y, xe, ye);
}


/** */
void AnaFillResult::AppendToPCProfRInjGraph(EPolarimeterId polId, ETargetOrient tgtOrient, Double_t x, Double_t y, Double_t xe, Double_t ye)
{
   TGraphErrors *graph = GetPCProfRInjGraph(polId, tgtOrient);

   if (!graph) {
      Info("AppendToPCProfRInjGraph", "Fill %d. Created new graph for polId %d, tgtOrient %d", fFillId, polId, tgtOrient);
      graph = new TGraphErrors();
      fPCProfRInjGraphs[polId][tgtOrient] = graph;
   }

   Info("AppendToPCProfRInjGraph", "Fill %d. Added measurement to graph polId %d, tgtOrient %d", fFillId, polId, tgtOrient);
   utils::AppendToGraph(graph, x, y, xe, ye);
}
