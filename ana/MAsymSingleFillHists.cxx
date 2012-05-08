/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TDirectoryFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"
#include "TF2.h"

#include "utils/utils.h"

#include "MAsymSingleFillHists.h"

#include "MseMeasInfo.h"


ClassImp(MAsymSingleFillHists)

using namespace std;

/** Default constructor. */
MAsymSingleFillHists::MAsymSingleFillHists() : DrawObjContainer(), fFillId(0)
{
   BookHists();
}


MAsymSingleFillHists::MAsymSingleFillHists(TDirectory *dir) : DrawObjContainer(dir), fFillId(0)
{
   BookHists();
}


/** Default destructor. */
MAsymSingleFillHists::~MAsymSingleFillHists()
{
   //delete grHTargetVsMeas;
   //delete grVTargetVsMeas;
}


/** */
void MAsymSingleFillHists::BookHists()
{ //{{{
   fDir->cd();

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {
      BookHistsPolarimeter((EPolarimeterId) i);
   }

   string   shName;
   TH1     *hist;
   string   strDirName(fDir->GetName());

   // This piece of code may go to another function
   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId = *iRingId;
      string sRingId = RunConfig::AsString(ringId);
      Color_t  color = RunConfig::AsColor(ringId);

      TAttMarker styleMarker;
      styleMarker.SetMarkerStyle(kFullCircle);
      styleMarker.SetMarkerSize(1);
      styleMarker.SetMarkerColor(color);

      shName = "hIntensVsFillTime_" + strDirName + "_" + sRingId;
      hist = new TH2C(shName.c_str(), shName.c_str(), 48, 0, 12*3600, 100, -10, 300);
      hist->SetOption("DUMMY");
      hist->SetTitle("; Time in Fill, hours; Beam Intensity, x10^{9} Protons;");
      hist->GetXaxis()->SetTimeOffset(3600*6, "gmt");
      //hist->GetXaxis()->SetTimeOffset(0, "local");
      hist->GetXaxis()->SetTimeDisplay(1);
      hist->GetXaxis()->SetTimeFormat("%H");
      styleMarker.Copy(*hist);
      o[shName] = hist;

      shName = "hAsymVsBunchId_X_" + strDirName + "_" + sRingId;
      hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0.5, N_BUNCHES+0.5, 1, -0.02, 0.02);
      hist->SetTitle("; Bunch Id; X Asymmetry;");
      hist->SetOption("DUMMY GRIDX");
      hist->GetYaxis()->SetRangeUser(-0.05, 0.05);
      styleMarker.Copy(*hist);
      o[shName] = hist;

      SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

      for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
      {
         string  sSS   = gRunConfig.AsString(*iSS);
         Color_t color = RunConfig::AsColor(*iSS);

         // Create graphs for different spin states
         // X45
         shName = "grAsymVsBunchId_X_" + sSS;

         TGraphErrors *grAsymVsBunchId_X = new TGraphErrors();
         grAsymVsBunchId_X->SetName(shName.c_str());
         grAsymVsBunchId_X->SetMarkerStyle(kFullCircle);
         grAsymVsBunchId_X->SetMarkerSize(1);
         grAsymVsBunchId_X->SetMarkerColor(color);

         // Add graphs to histos
         ((TH1*) o["hAsymVsBunchId_X_" + strDirName + "_" + sRingId])->GetListOfFunctions()->Add(grAsymVsBunchId_X, "p");
      }
   }


   shName = "hExternInfoVsFillTime_" + strDirName;
   hist = new TH2C(shName.c_str(), shName.c_str(), 48, 0, 12*3600, 100, -10, 300);
   hist->SetOption("DUMMY");
   hist->SetTitle("; Time in Fill, hours; ;");
   hist->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   hist->GetXaxis()->SetTimeDisplay(1);
   hist->GetXaxis()->SetTimeFormat("%H");
   o[shName] = hist;
} //}}}


/** */
void MAsymSingleFillHists::BookHistsPolarimeter(EPolarimeterId polId)
{ //{{{
   char hName[256];
   string   shName;
   TH1     *hist;
   string   strPolId = RunConfig::AsString(polId);
   //string  strBeamE = RunConfig::AsString(beamE);
   Color_t  color    = RunConfig::AsColor(polId);

   TAttMarker styleMarker;
   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(color);

   // Get a unique dir name which should be also the same as the fill number
   string strDirName(fDir->GetName());

   // Asymmetry
   sprintf(hName, "grAsymVsEnergy");
   TGraphErrors *grAsymVsEnergy = new TGraphErrors();
   grAsymVsEnergy->SetName(hName);
   grAsymVsEnergy->SetMarkerStyle(kFullCircle);
   grAsymVsEnergy->SetMarkerSize(1);
   grAsymVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAsymVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 0.01);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;Asymmetry;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAsymVsEnergy, "p");

   // Analyzing power
   sprintf(hName, "grAnaPowerVsEnergy");
   TGraphErrors *grAnaPowerVsEnergy = new TGraphErrors();
   grAnaPowerVsEnergy->SetName(hName);
   grAnaPowerVsEnergy->SetMarkerStyle(kFullCircle);
   grAnaPowerVsEnergy->SetMarkerSize(1);
   grAnaPowerVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAnaPowerVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0.01, 0.02);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;A_{N};");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   sprintf(hName, "hAnaPowerVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
   Float_t energyBins[10] = {0, 14, 34, 90, 110, 240, 260, 390, 410, 450};
   o[hName] = new TH1F(hName, hName, 9, energyBins);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0.01, 0.02);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;A_{N};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("hist NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   // Polarization
   TGraphErrors *grPolarVsEnergy = new TGraphErrors();
   grPolarVsEnergy->SetName("grPolarVsEnergy");
   grPolarVsEnergy->SetMarkerStyle(kFullCircle);
   grPolarVsEnergy->SetMarkerSize(1);
   grPolarVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hPolarVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;Polarization, %;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsEnergy, "p");

   TGraphErrors *grPolarVsFillTime = new TGraphErrors();
   grPolarVsFillTime->SetName("grPolarVsFillTime");
   styleMarker.Copy(*grPolarVsFillTime);

   shName = "hPolarVsFillTime_" + strDirName + "_" + strPolId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 12, 0, 12*3600, 100, 0, 100);
   hist->SetOption("DUMMY");
   hist->SetTitle("; Time in Fill, hours; Polarization, %;");
   hist->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //hist->GetXaxis()->SetTimeOffset(0, "local");
   hist->GetXaxis()->SetTimeDisplay(1);
   hist->GetXaxis()->SetTimeFormat("%H");
   //hist->GetListOfFunctions()->Add(grPolarVsFillTime, "p");
   styleMarker.Copy(*hist);
   o[shName] = hist;

   // Profile r
   TGraphErrors *grRVsEnergy = new TGraphErrors();
   grRVsEnergy->SetName("grRVsEnergy");
   grRVsEnergy->SetMarkerStyle(kFullCircle);
   grRVsEnergy->SetMarkerSize(1);
   grRVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hRVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");

   sprintf(hName, "hRVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
   o[hName] = new TH1F(hName, hName, 50, 0, 450);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1 NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");
} //}}}


/** */
void MAsymSingleFillHists::SetSignature(std::string signature)
{ //{{{
   stringstream ssSignature("signature not defined");

   char strAnaTime[25];
   time_t currentTime = time(0);
   tm *ltime = localtime( &currentTime );
   strftime(strAnaTime, 25, "%c", ltime);

   ssSignature << "Fill " << fDir->GetName() << ", Analyzed " << strAnaTime;
   //ssSignature << ", Version " << fAnaInfo->fAsymVersion << ", " << fAnaInfo->fUserGroup.fUser;

   fSignature = ssSignature.str();
} //}}}


/** */
void MAsymSingleFillHists::Fill(EventConfig &rc)
{ //{{{
   Double_t runId            = rc.fMeasInfo->RUNID;
   UInt_t   fFillId           = (UInt_t) runId;
   UInt_t   beamEnergy       = rc.fMeasInfo->GetBeamEnergy();
   Short_t  polId            = rc.fMeasInfo->fPolId;
   time_t   runStartTime     = rc.fMeasInfo->fStartTime;
   //Short_t  targetId         = rc.fMseMeasInfoX->target_id;
   //Char_t   targetOrient     = rc.fMseMeasInfoX->target_orient[0];
   Float_t  anaPower         = rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetry        = rc.fAnaMeasResult->sinphi[0].P[0] * rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetryErr     = rc.fAnaMeasResult->sinphi[0].P[1] * rc.fAnaMeasResult->A_N[1];
   Float_t  polarization     = rc.fAnaMeasResult->sinphi[0].P[0] * 100.;
   Float_t  polarizationErr  = rc.fAnaMeasResult->sinphi[0].P[1] * 100.;
   Float_t  profileRatio     = rc.fAnaMeasResult->fProfilePolarR.first;
   Float_t  profileRatioErr  = rc.fAnaMeasResult->fProfilePolarR.second;
   //Float_t  max_rate         = rc.fAnaMeasResult->max_rate;
   //Float_t  tzero            = rc.fCalibrator->fChannelCalibs[0].fT0Coef;
   //Float_t  tzeroErr         = rc.fCalibrator->fChannelCalibs[0].fT0CoefErr;
   //Float_t  dl               = rc.fCalibrator->fChannelCalibs[0].fDLWidth;
   //Float_t  dlErr            = rc.fCalibrator->fChannelCalibs[0].fDLWidthErr;

   // Get a unique dir name which should be also the same as the fill number
   string strDirName = fDir->GetName();

   // Process common histograms
   char hName[256];

   string strPolId = RunConfig::AsString((EPolarimeterId) polId);

   TGraphErrors *graphErrs = 0;
   UInt_t        graphNEntries;

   // Asymmetry
   sprintf(hName, "hAsymVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAsymVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, asymmetry);
   graphErrs->SetPointError(graphNEntries, 0, asymmetryErr);

   // Analyzing power
   sprintf(hName, "hAnaPowerVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, anaPower);
   graphErrs->SetPointError(graphNEntries, 1, 1);

   // Polarization
   sprintf(hName, "hPolarVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, polarization);
   graphErrs->SetPointError(graphNEntries, 0, polarizationErr);

   // polarization vs fill time
   //char grName[256];
   //sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
   //sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, strPolId.c_str());
   //graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject(grName);
   //graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
   //
   // Each histogram of this type contains multiple graphs for each fill
   //if (!graphErrs) {
   //   TGraphErrors *graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
   //   TGraphErrors *graphFill = new TGraphErrors(*graph); // Copy the default graph and its properties, style, colors, ...
   //   graphFill->SetName(grName);
   //   ((TH1*) o[hName])->GetListOfFunctions()->Add(graphFill, "p");
   //   graphErrs = graphFill;
   //}
   //if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_250) {
   //   graphNEntries = graphErrs->GetN();
   //   graphErrs->SetPoint(graphNEntries, runStartTime, polarization);
   //   graphErrs->SetPointError(graphNEntries, 0, polarizationErr);
   //}

   // Profiles r
   sprintf(hName, "hRVsEnergy_%s_%s", strDirName.c_str(), strPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, profileRatio);
   graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);
} //}}}


/** */
void MAsymSingleFillHists::PostFill()
{ //{{{
/*
   string strDirName = fDir->GetName();

   char hName[256];

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      string strPolId = RunConfig::AsString((EPolarimeterId) i);

      TGraphErrors *graph;

      // Analyzing power
      sprintf(hName, "hAnaPowerVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");

      utils::BinGraph(graph, (TH1*) o[hName]);

      ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

      // Profiles r
      sprintf(hName, "hRVsEnergyBinned_%s_%s", strDirName.c_str(), strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsEnergy");

      //utils::RemoveOutliers(graph, 2, 3);
      utils::BinGraph(graph, (TH1*) o[hName]);

      ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

      TF1 *funcRVsEnergy = new TF1("funcRVsEnergy", "[0] + [1]*x");
      funcRVsEnergy->SetParameters(0, 0);
      funcRVsEnergy->SetParNames("offset", "slope");
      ((TH1*) o[hName])->Fit("funcRVsEnergy");
      delete funcRVsEnergy;

      // Polarization vs fill time
      sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
      //TList* list = ((TH1*) o[hName])->GetListOfFunctions();
      //graph->Merge(list);

      TF1 *funcPolarVsFillTime = new TF1("funcPolarVsFillTime", "[0] + [1]*x");
      //funcPolarVsFillTime->SetParameters(0, 0);
      funcPolarVsFillTime->SetParNames("offset", "slope");
      graph->Fit("funcPolarVsFillTime");
      delete funcPolarVsFillTime;

      //((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);
   }
*/
} //}}}


/** */
void MAsymSingleFillHists::PostFill(AnaFillResult &afr)
{ //{{{
   string strDirName = fDir->GetName();

   // Loop over polarimeters
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      string sPolId = RunConfig::AsString(*iPolId);

      // Get graph of p-Carbon polar values
      TGraphErrors *graphErrs      = afr.GetPCPolarGraph(*iPolId);
      ValErrPair    avrgPCPolar    = afr.GetPolarPC(*iPolId); // unnormalized value
      ValErrPair    avrgPCPolarUnW = afr.GetPolarPCUnW(*iPolId); // unnormalized value

      if (graphErrs && graphErrs->GetN() > 0) {
         TH1* hPolarVsFillTime_ = (TH1*) o["hPolarVsFillTime_" + strDirName + "_" + sPolId];
         ((TAttMarker*) hPolarVsFillTime_)->Copy(*graphErrs);
         hPolarVsFillTime_->GetListOfFunctions()->Add(graphErrs, "p");
         utils::UpdateLimitsFromGraphs(hPolarVsFillTime_, 2);

         if (avrgPCPolar.second >= 0) {
            TLine* avrgLine = new TLine(0, avrgPCPolar.first*100, 12*3600, avrgPCPolar.first*100);
            avrgLine->SetLineWidth(3);
            avrgLine->SetLineColor(hPolarVsFillTime_->GetMarkerColor());
            hPolarVsFillTime_->GetListOfFunctions()->Add(avrgLine);

            //TLine* avrgLineUnW = new TLine(0, avrgPCPolarUnW.first, 12*3600, avrgPCPolarUnW.first);
            //avrgLineUnW->SetLineWidth(1);
            ////avrgLineUnW->SetLineColor(hPolarVsFillTime_->GetMarkerColor());
            //hPolarVsFillTime_->GetListOfFunctions()->Add(avrgLineUnW);
         }
      } else
         Error("PostFill", "hPolarVsFillTime_ graph is not properly defined in %s", strDirName.c_str());

   }


   TH1* hExternInfoVsFillTime_ = (TH1*) o["hExternInfoVsFillTime_" + strDirName];

   // Loop over rings
   RingIdSetIter iRingId = gRunConfig.fRings.begin();

   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId = *iRingId;
      string sRingId = RunConfig::AsString(ringId);
      Color_t  color = RunConfig::AsColor(ringId);

      TGraphErrors *graphErrs = afr.GetIntensGraph(ringId);

      TH1* hIntensVsFillTime_ = (TH1*) o["hIntensVsFillTime_" + strDirName + "_" + sRingId];

      if (graphErrs && graphErrs->GetN() > 0) {
         ((TAttMarker*) hIntensVsFillTime_)->Copy(*graphErrs);
         graphErrs->SetMarkerSize(0.8);
         hIntensVsFillTime_->GetListOfFunctions()->Add(graphErrs, "p");
         utils::UpdateLimitsFromGraphs(hIntensVsFillTime_, 2);
      } else
         Error("PostFill", "hIntensVsFillTime_ graph is not properly defined in %s", strDirName.c_str());


      // add external info graphs
      graphErrs = afr.GetRotCurStarGraph(ringId);
      if (graphErrs) {
         graphErrs->SetMarkerSize(1);
         graphErrs->SetMarkerStyle(34);
         graphErrs->SetMarkerColor(color+2);
         hExternInfoVsFillTime_->GetListOfFunctions()->Add(graphErrs, "p");
      }

      graphErrs = afr.GetRotCurPhenixGraph(ringId);
      if (graphErrs) {
         graphErrs->SetMarkerSize(1);
         graphErrs->SetMarkerStyle(34);
         graphErrs->SetMarkerColor(color-2);
         hExternInfoVsFillTime_->GetListOfFunctions()->Add(graphErrs, "p");
      }

      graphErrs = afr.GetSnakeCurGraph(ringId);
      if (graphErrs) {
         graphErrs->SetMarkerSize(1);
         graphErrs->SetMarkerStyle(34);
         graphErrs->SetMarkerColor(color);
         hExternInfoVsFillTime_->GetListOfFunctions()->Add(graphErrs, "p");
      }

      // Fit the graphs
      TH1* hAsymVsBunchId_X_ = (TH1*) o["hAsymVsBunchId_X_" + strDirName + "_" + sRingId];

      SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

      for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
      {
         string sSS = gRunConfig.AsString(*iSS);
         Color_t color = RunConfig::AsColor(*iSS);

         string grName = "grAsymVsBunchId_X_" + sSS;
         TGraphErrors* grAsymVsBunchId_X = (TGraphErrors*) hAsymVsBunchId_X_->GetListOfFunctions()->FindObject(grName.c_str());

         TF1 *funcConst= new TF1("funcConst", "[0]");
         funcConst->SetParNames("const");
         funcConst->SetLineColor(color);
         grAsymVsBunchId_X->Fit("funcConst");
         delete funcConst;
      }
   }
} //}}}


/** */
/*
void MAsymSingleFillHists::UpdateLimits()
{ //{{{
   string strDirName = fDir->GetName();

   char hName[256];

   for (PolarimeterIdSetIter iPolId=gRunConfig.fPolarimeters.begin(); iPolId!=gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      //for (BeamEnergySetIter iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      //{
         string strPolId = RunConfig::AsString(*iPolId);
         //string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hPolarVsFillTime_%s_%s", strDirName.c_str(), strPolId.c_str());
         TList* list = ((TH1*) o[hName])->GetListOfFunctions();
         TIter  next(list);

         while ( TGraphErrors *graphErrs = (TGraphErrors*) next() ) {
            utils::SubtractMinimum(graphErrs);

            //Double_t xmin, ymin, xmax, ymax;
            //graphErrs->ComputeRange(xmin, ymin, xmax, ymax);
         }

         //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(xmin, "gmt");
         //((TH1*) o[hName])->GetXaxis()->SetLimits(xmin, xmax);

         //sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, strPolId.c_str());
         //sprintf(hName, "hPolarVsTime_%s_%s", strPolId.c_str(), strBeamE.c_str());

         //((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);
      //}
   }
} //}}}
*/
