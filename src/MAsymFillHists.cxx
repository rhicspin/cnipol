#include "TDirectoryFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2F.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "MAsymFillHists.h"
#include "MAsymSingleFillHists.h"

#include "MseMeasInfo.h"


ClassImp(MAsymFillHists)

using namespace std;

/** Default constructor. */
MAsymFillHists::MAsymFillHists() : DrawObjContainer(), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


MAsymFillHists::MAsymFillHists(TDirectory *dir) : DrawObjContainer(dir), fHTargetVsRun(), fVTargetVsRun()
{
   BookHists();
}


/** Default destructor. */
MAsymFillHists::~MAsymFillHists()
{
   //delete grHTargetVsMeas;
   //delete grVTargetVsMeas;
}


/** */
void MAsymFillHists::BookHists()
{
   fDir->cd();

   // By polarimeter
   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      BookHistsByPolarimeter(*iPolId);
   }

   // By ring
   RingIdSetIter iRingId = gRunConfig.fRings.begin();
   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      BookHistsByRing(*iRingId);
   }

   TH1          *hist;
   string       shName;
   TAttMarker   styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);

   grAgsPolarVsFillId = new TGraphErrors();
   grAgsPolarVsFillId->SetName("grAgsPolarVsFillId");
   styleMarker.Copy(*grAgsPolarVsFillId);
   grAgsPolarVsFillId->SetMarkerColor(kRed);

   shName = "hAgsPolarVsFillId";
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0., 1., 1, 0., 1.);
   hist->SetTitle("; Fill Id; AGS polarization, %;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grAgsPolarVsFillId, "p");
   hist->GetListOfFunctions()->SetOwner(kTRUE);
   o[shName] = hist;
   hAgsPolarVsFillId = hist;

}


/** */
void MAsymFillHists::BookHistsByPolarimeter(EPolarimeterId polId)
{
   char    hName[256];
   string  shName;
   TH1    *hist;
   string  sPolId = RunConfig::AsString(polId);
   //string  strBeamE = RunConfig::AsString(beamE);
   Color_t color  = RunConfig::AsColor(polId);

   //TStyle  styleMarker;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(color);


   // Asymmetry
   sprintf(hName, "grAsymVsEnergy");
   TGraphErrors *grAsymVsEnergy = new TGraphErrors();
   grAsymVsEnergy->SetName(hName);
   grAsymVsEnergy->SetMarkerStyle(kFullCircle);
   grAsymVsEnergy->SetMarkerSize(1);
   grAsymVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAsymVsEnergy_%s", sPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 0.01);
   ((TH1*) o[hName])->SetTitle("; Beam Energy; Asymmetry;");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAsymVsEnergy, "p");

   // Analyzing power
   sprintf(hName, "grAnaPowerVsEnergy");
   TGraphErrors *grAnaPowerVsEnergy = new TGraphErrors();
   grAnaPowerVsEnergy->SetName(hName);
   grAnaPowerVsEnergy->SetMarkerStyle(kFullCircle);
   grAnaPowerVsEnergy->SetMarkerSize(1);
   grAnaPowerVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hAnaPowerVsEnergy_%s", sPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 0.02);
   ((TH1*) o[hName])->SetTitle("; Beam Energy; A_{N};");
   ((TH1*) o[hName])->SetOption("NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   sprintf(hName, "hAnaPowerVsEnergyBinned_%s", sPolId.c_str());
   Float_t energyBins[10] = {0, 14, 34, 90, 110, 240, 260, 390, 410, 450};
   o[hName] = new TH1F(hName, hName, 9, energyBins);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(0.00, 0.02);
   ((TH1*) o[hName])->SetTitle("; Beam Energy; A_{N};");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grAnaPowerVsEnergy, "p");

   // Polarization
   TGraphErrors *grPolarVsEnergy = new TGraphErrors();
   grPolarVsEnergy->SetName("grPolarVsEnergy");
   grPolarVsEnergy->SetMarkerStyle(kFullCircle);
   grPolarVsEnergy->SetMarkerSize(1);
   grPolarVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hPolarVsEnergy_%s", sPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, 0, 100);
   ((TH1*) o[hName])->SetTitle("; Beam Energy; Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsEnergy, "p");

   TGraphErrors *grPolarVsFillTime = new TGraphErrors();
   grPolarVsFillTime->SetName("grPolarVsFillTime");
   grPolarVsFillTime->SetMarkerStyle(kFullCircle);
   grPolarVsFillTime->SetMarkerSize(1);
   grPolarVsFillTime->SetMarkerColor(color);

   sprintf(hName, "hPolarVsFillTime_%s", sPolId.c_str());
   //o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 60, 20, 80);
   o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 100, 20, 100);
   ((TH1*) o[hName])->SetTitle("; Time in Fill, hours; Polarization, %;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grPolarVsFillTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "local");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H");

   // Polarization per measurement number (first five measurements)
   shName = "hPolarVsMeasId_" + sPolId;
   hist = new TH1D(shName.c_str(), shName.c_str(), 10, 0.5, 10.5);
   hist->GetYaxis()->SetRangeUser(0.7, 1.3);
   hist->SetTitle("; Measurement Id; P/P_{first meas.};");
   hist->SetOption("E1");
   styleMarker.Copy(*hist); o[shName] = hist;

   // Polarization per measurement number (first five measurements)
   shName = "hProfilePolarMaxVsMeasId_" + sPolId;
   o[shName] = new TH1D(shName.c_str(), shName.c_str(), 10, 0.5, 10.5);
   ((TH1*) o[shName])->GetYaxis()->SetRangeUser(0.7, 1.3);
   ((TH1*) o[shName])->SetTitle("; Measurement Id; P_{0}/P_{0, first meas.};");
   ((TH1*) o[hName])->SetOption("NOIMG");

   // Profile r
   TGraphErrors *grRVsEnergy = new TGraphErrors();
   grRVsEnergy->SetName("grRVsEnergy");
   grRVsEnergy->SetMarkerStyle(kFullCircle);
   grRVsEnergy->SetMarkerSize(1);
   grRVsEnergy->SetMarkerColor(color);

   sprintf(hName, "hRVsEnergy_%s", sPolId.c_str());
   o[hName] = new TH2F(hName, hName, 1, 0, 450, 1, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");
   ((TH1*) o[hName])->SetOption("NOIMG");

   sprintf(hName, "hRVsEnergyBinned_%s", sPolId.c_str());
   o[hName] = new TH1F(hName, hName, 50, 0, 450);
   ((TH1*) o[hName])->GetYaxis()->SetRangeUser(-0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Beam Energy;r;");
   ((TH1*) o[hName])->SetMarkerStyle(kFullCircle);
   ((TH1*) o[hName])->SetMarkerSize(1);
   ((TH1*) o[hName])->SetMarkerColor(color);
   ((TH1*) o[hName])->SetOption("E1 NOIMG");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsEnergy, "p");

   TGraphErrors *grRVsFillTime = new TGraphErrors();
   grRVsFillTime->SetName("grRVsFillTime");
   grRVsFillTime->SetMarkerStyle(kFullCircle);
   grRVsFillTime->SetMarkerSize(1);
   grRVsFillTime->SetMarkerColor(color);

   sprintf(hName, "hRVsFillTime_%s", sPolId.c_str());
   o[hName] = new TH2F(hName, hName, 12, 0, 12*3600, 100, -0.1, 1);
   ((TH1*) o[hName])->SetTitle(";Time in Fill, hours;r;");
   ((TH1*) o[hName])->GetListOfFunctions()->Add(grRVsFillTime, "p");
   ((TH1*) o[hName])->GetXaxis()->SetTimeOffset(3600*6, "gmt");
   //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(0, "local");
   ((TH1*) o[hName])->GetXaxis()->SetTimeDisplay(1);
   ((TH1*) o[hName])->GetXaxis()->SetTimeFormat("%H");

   // Profile R
   shName = "hProfileRVsMeasId_" + sPolId;
   hist = new TH1D(shName.c_str(), shName.c_str(), 10, 0.5, 10.5);
   hist->SetTitle("; Measurement Id; R;");
   hist->SetOption("E1 NOIMG");
   hist->GetYaxis()->SetRangeUser(-0.5, 0.5);
   styleMarker.Copy(*hist);
   o[shName] = hist;

   TGraphErrors *grRotatorPCPolarRatio = new TGraphErrors();
   grRotatorPCPolarRatio->SetName("grRotatorPCPolarRatio");
   styleMarker.Copy(*grRotatorPCPolarRatio);

   shName = "hRotatorPCPolarRatio_" + sPolId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
   hist->SetTitle("; Fill Id; After/Before Rotator Pol. Ratio;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grRotatorPCPolarRatio, "p");
   hist->GetListOfFunctions()->SetOwner(kTRUE);
   o[shName] = hist;
   hRotatorPCPolarRatiosByPol[polId]  = hist;
   grRotatorPCPolarRatiosByPol[polId] = grRotatorPCPolarRatio;

   TGraphErrors *grRampPCPolarRatio = new TGraphErrors();
   grRampPCPolarRatio->SetName("grRampPCPolarRatio");
   styleMarker.Copy(*grRampPCPolarRatio);

   shName = "hRampPCPolarRatio_" + sPolId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
   hist->SetTitle("; Fill Id; Flattop/Injection Pol. Ratio;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grRampPCPolarRatio, "p");
   hist->GetListOfFunctions()->SetOwner(kTRUE);
   o[shName] = hist;
   hRampPCPolarRatiosByPol[polId]  = hist;
   grRampPCPolarRatiosByPol[polId] = grRampPCPolarRatio;
}


/** */
void MAsymFillHists::BookHistsByRing(ERingId ringId)
{
   string  sRingId = RunConfig::AsString(ringId);
   Color_t color   = RunConfig::AsColor(ringId);

   TAttMarker styleMarker;
   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(color);

   string   shName;
   TH1     *hist;

   TGraphErrors *grRotatorPCPolarRatio = new TGraphErrors();
   grRotatorPCPolarRatio->SetName("grRotatorPCPolarRatio");
   styleMarker.Copy(*grRotatorPCPolarRatio);

   shName = "hRotatorPCPolarRatio_" + sRingId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
   hist->SetTitle("; Fill Id; After/Before Rotator Pol. Ratio;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grRotatorPCPolarRatio, "p");
   hist->GetListOfFunctions()->SetOwner(kTRUE);
   o[shName] = hist;
   hRotatorPCPolarRatiosByRing[ringId]  = hist;
   grRotatorPCPolarRatiosByRing[ringId] = grRotatorPCPolarRatio;


   TGraphErrors *grRampPCPolarRatio = new TGraphErrors();
   grRampPCPolarRatio->SetName("grRampPCPolarRatio");
   styleMarker.Copy(*grRampPCPolarRatio);

   shName = "hRampPCPolarRatio_" + sRingId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
   hist->SetTitle("; Fill Id; Flattop/Injection Pol. Ratio;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grRampPCPolarRatio, "p");
   hist->GetListOfFunctions()->SetOwner(kTRUE);
   o[shName] = hist;
   hRampPCPolarRatiosByRing[ringId]  = hist;
   grRampPCPolarRatiosByRing[ringId] = grRampPCPolarRatio;

   TGraphErrors *grRhicAgsPolarRatio = new TGraphErrors();
   grRhicAgsPolarRatio->SetName("grRhicAgsPolarRatio");
   styleMarker.Copy(*grRhicAgsPolarRatio);

   shName = "hRhicAgsPolarRatio_" + sRingId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0., 1., 1, 0., 1.);
   hist->SetTitle("; Fill Id; RHIC Injection/AGS Pol. Ratio;");
   hist->SetOption("DUMMY GRIDX GRIDY");
   hist->GetListOfFunctions()->Add(grRhicAgsPolarRatio, "p");
   hist->GetListOfFunctions()->SetOwner(kTRUE);
   o[shName] = hist;
   hRhicAgsPolarRatiosByRing[ringId]  = hist;
   grRhicAgsPolarRatiosByRing[ringId] = grRhicAgsPolarRatio;


   // Chi2 of bunch asym
   shName = "hBunchAsymChi2_" + sRingId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 1, 0, 1);
   hist->SetTitle("; Fill Id; Bunch Asym. #chi^{2};");
   hist->SetOption("DUMMY GRIDX GRIDY");
   o[shName] = hist;

   // 
   shName = "hBunchAsymSigma_" + sRingId;
   hist = new TH2C(shName.c_str(), shName.c_str(), 1, 0, 1, 100, 0, 1);
   hist->SetTitle("; Fill Id; Bunch Asym. #sigma_{Y};");
   hist->SetOption("DUMMY GRIDX GRIDY");
   o[shName] = hist;
   
   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   
   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string  sSS   = gRunConfig.AsString(*iSS);
      Color_t color = RunConfig::AsColor(*iSS);
   
      styleMarker.SetMarkerColor(color);

      // Create graphs for different spin states
      TGraph *grBunchAsymChi2_ = new TGraph();
      grBunchAsymChi2_->SetName(string("grBunchAsymChi2_" + sSS).c_str());
      styleMarker.Copy(*grBunchAsymChi2_);
   
      // Add graph to hist
      ((TH1*) o["hBunchAsymChi2_" + sRingId])->GetListOfFunctions()->Add(grBunchAsymChi2_, "p");

      // Create graphs for different spin states
      TGraph *grBunchAsymSigma_ = new TGraph();
      grBunchAsymSigma_->SetName(string("grBunchAsymSigma_" + sSS).c_str());
      styleMarker.Copy(*grBunchAsymSigma_);

      // Add graph to hist
      ((TH1*) o["hBunchAsymSigma_" + sRingId])->GetListOfFunctions()->Add(grBunchAsymSigma_, "p");
   }
}


/** */
void MAsymFillHists::Fill(const EventConfig &rc)
{
   Double_t runId              = rc.fMeasInfo->RUNID;
   UInt_t   fillId             = (UInt_t) runId;
   UInt_t   beamEnergy         = rc.fMeasInfo->GetBeamEnergy();
   Short_t  polId              = rc.fMeasInfo->fPolId;
   time_t   measStartTime       = rc.fMeasInfo->fStartTime;
   Float_t  anaPower           = rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetry          = rc.fAnaMeasResult->sinphi[0].P[0] * rc.fAnaMeasResult->A_N[1];
   Float_t  asymmetryErr       = rc.fAnaMeasResult->sinphi[0].P[1] * rc.fAnaMeasResult->A_N[1];
   Float_t  polar              = rc.fAnaMeasResult->sinphi[0].P[0] * 100.;
   Float_t  polarErr           = rc.fAnaMeasResult->sinphi[0].P[1] * 100.;
   Float_t  profileRatio       = rc.fAnaMeasResult->fProfilePolarR.first;
   Float_t  profileRatioErr    = rc.fAnaMeasResult->fProfilePolarR.second;

   // Process common histograms
   char hName[256];

   string sPolId = RunConfig::AsString((EPolarimeterId) polId);

   TGraphErrors *graphErrs = 0;
   UInt_t        graphNEntries;

   // Asymmetry
   sprintf(hName, "hAsymVsEnergy_%s", sPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAsymVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, asymmetry);
   graphErrs->SetPointError(graphNEntries, 0, asymmetryErr);

   // Analyzing power
   sprintf(hName, "hAnaPowerVsEnergy_%s", sPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, anaPower);
   graphErrs->SetPointError(graphNEntries, 0, 0.05*anaPower); // This error is fake

   // Polarization
   sprintf(hName, "hPolarVsEnergy_%s", sPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, polar);
   graphErrs->SetPointError(graphNEntries, 0, polarErr);

   // polarization vs fill time
   char grName[256];
   sprintf(hName, "hPolarVsFillTime_%s", sPolId.c_str());
   sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, sPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject(grName);

   // Each histogram of this type contains a graph for each fill
   if (!graphErrs) {
      TGraphErrors *graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
      // Create new graph for this fill
      TGraphErrors *graphFill = new TGraphErrors(*graph); // Copy the default graph and its properties, style, colors, ...
      graphFill->SetName(grName);
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphFill, "p");
      graphErrs = graphFill;
   }

   //if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_255)
   //if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_100) {
      graphNEntries = graphErrs->GetN();
      graphErrs->SetPoint(graphNEntries, measStartTime, polar);
      graphErrs->SetPointError(graphNEntries, 0, polarErr);
   //}

   // Profiles r
   sprintf(hName, "hRVsEnergy_%s", sPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsEnergy");
   graphNEntries = graphErrs->GetN();
   graphErrs->SetPoint(graphNEntries, beamEnergy, profileRatio);
   graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);


   // r vs fill time
   sprintf(hName, "hRVsFillTime_%s", sPolId.c_str());
   sprintf(grName, "grRVsFillTime_%05d_%s", fillId, sPolId.c_str());
   graphErrs = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject(grName);

   // Each histogram of this type contains multiple graphs for each fill
   if (!graphErrs) {
      TGraphErrors *graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsFillTime");
      TGraphErrors *graphFill = new TGraphErrors(*graph); // Copy the default graph and its properties, style, colors, ...
      graphFill->SetName(grName);
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphFill, "p");
      graphErrs = graphFill;
   }

   //if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_250)
   if ((EBeamEnergy) beamEnergy == kBEAM_ENERGY_100) {
      graphNEntries = graphErrs->GetN();
      graphErrs->SetPoint(graphNEntries, measStartTime, profileRatio);
      graphErrs->SetPointError(graphNEntries, 0, profileRatioErr);
   }
}


/** */
void MAsymFillHists::PostFill()
{
   char hName[256];

   for (UInt_t i=0; i!=N_POLARIMETERS; i++) {

      string sPolId = RunConfig::AsString((EPolarimeterId) i);

      TGraphErrors *graph;

      // Analyzing power
      sprintf(hName, "hAnaPowerVsEnergyBinned_%s", sPolId.c_str());
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grAnaPowerVsEnergy");

      utils::BinGraph(graph, (TH1*) o[hName]);

      ((TH1*) o[hName])->GetListOfFunctions()->Remove(graph);

      // Profiles r
      sprintf(hName, "hRVsEnergyBinned_%s", sPolId.c_str());
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
      sprintf(hName, "hPolarVsFillTime_%s", sPolId.c_str());
      TList* list = ((TH1*) o[hName])->GetListOfFunctions();

      // For each fill subtract the minimum time from all graph values
      TIter next(list);
      while ( TGraphErrors *graphErrs = (TGraphErrors*) next() ) {
         utils::SubtractMinimum(graphErrs);
      }

      // Fill a histogram with values per consequent measurement number
      utils::BinGraphsByMeasId(list, (TH1*) o["hPolarVsMeasId_" + sPolId], kTRUE);

      // Create a single graph from graphs for individual fills and fit it
      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");
      //graph->Merge(list);
      TGraphErrors *graphMerged = new TGraphErrors(*graph);
      utils::MergeGraphs(graphMerged, list);
      ((TH1*) o[hName])->GetListOfFunctions()->Clear();
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphMerged, "p");
      //delete graphMerged;

      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grPolarVsFillTime");

      TF1 *funcPolarVsFillTime = new TF1("funcPolarVsFillTime", "[0] + [1]*x", 0, 11*3600);
      funcPolarVsFillTime->SetParNames("offset", "slope");
      graph->Fit("funcPolarVsFillTime", "R");
      delete funcPolarVsFillTime;

      // r vs fill time
      sprintf(hName, "hRVsFillTime_%s", sPolId.c_str());
      list = ((TH1*) o[hName])->GetListOfFunctions();

      // For each fill subtract the minimum time from all graph values
      TIter nextRlist(list);
      while ( TGraphErrors *graphErrs = (TGraphErrors*) nextRlist() ) {
         utils::SubtractMinimum(graphErrs);
      }

      // Fill a histogram with values per consequent measurement number
      utils::BinGraphsByMeasId(list, (TH1*) o["hProfileRVsMeasId_" + sPolId], kFALSE);

      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsFillTime");
      //graph->Merge(list);
      graphMerged = new TGraphErrors(*graph);
      utils::MergeGraphs(graphMerged, list);
      ((TH1*) o[hName])->GetListOfFunctions()->Clear();
      ((TH1*) o[hName])->GetListOfFunctions()->Add(graphMerged, "p");
      //*graph = *graphMerged;
      //delete graphMerged;

      graph = (TGraphErrors*) ((TH1*) o[hName])->GetListOfFunctions()->FindObject("grRVsFillTime");

      TF1 funcRVsFillTime("funcRVsFillTime", "[0] + [1]*x");
      funcRVsFillTime.SetParNames("offset", "slope");
      graph->Fit(&funcRVsFillTime);
   }

   //DrawObjContainer::PostFill();
}


/** */
void MAsymFillHists::PostFill(AnaGlobResult &agr)
{
   utils::SetXAxisIntBinsLabels(hAgsPolarVsFillId, agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);

   // Should be moved to a new function
   RingIdSetIter iRingId = gRunConfig.fRings.begin();
   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId  = *iRingId;
      string  sRingId = RunConfig::AsString(ringId);

      utils::SetXAxisIntBinsLabels((TH1*) o["hBunchAsymChi2_"  + sRingId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);
      utils::SetXAxisIntBinsLabels((TH1*) o["hBunchAsymSigma_" + sRingId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);

      utils::SetXAxisIntBinsLabels(hRotatorPCPolarRatiosByRing[ringId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);
      utils::SetXAxisIntBinsLabels(hRampPCPolarRatiosByRing[ringId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);
      utils::SetXAxisIntBinsLabels(hRhicAgsPolarRatiosByRing[ringId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);
   }


   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();
   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      EPolarimeterId polId  = *iPolId;
      string         sPolId = RunConfig::AsString(polId);

      utils::SetXAxisIntBinsLabels(hRotatorPCPolarRatiosByPol[polId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);
      utils::SetXAxisIntBinsLabels(hRampPCPolarRatiosByPol[polId], agr.GetMinFill()-0.5, agr.GetMaxFill()+0.5);
   }


   // Now loop over fills
   AnaFillResultMapIter iafr = agr.fAnaFillResults.begin();

   for ( ; iafr != agr.fAnaFillResults.end(); ++iafr)
   {
      UInt_t fillId      = iafr->first;
      AnaFillResult &afr = iafr->second;

      // Set individual fill sub dirs
      string sFillId(5, ' ');
      sprintf(&sFillId[0], "%05d", fillId);

      DrawObjContainer *oc = GetSingleFillHists(afr);

      ((MAsymSingleFillHists*) oc)->PostFill(afr);

      // By polarimeter
      PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

      for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
      {
         EPolarimeterId polId  = *iPolId;
         ERingId        ringId = RunConfig::GetRingId(polId);

         if (afr.fRotatorPCPolarRatio[polId].second >= 0)
         {
            utils::AppendToGraph(grRotatorPCPolarRatiosByPol[polId], fillId, afr.fRotatorPCPolarRatio[polId].first, 0, afr.fRotatorPCPolarRatio[polId].second);
            utils::AppendToGraph(grRotatorPCPolarRatiosByRing[ringId], fillId, afr.fRotatorPCPolarRatio[polId].first, 0, afr.fRotatorPCPolarRatio[polId].second);
         }

         if (afr.fRampPCPolarRatio[polId].second >= 0)
         {
            utils::AppendToGraph(grRampPCPolarRatiosByPol[polId], fillId, afr.fRampPCPolarRatio[polId].first, 0, afr.fRampPCPolarRatio[polId].second);
            utils::AppendToGraph(grRampPCPolarRatiosByRing[ringId], fillId, afr.fRampPCPolarRatio[polId].first, 0, afr.fRampPCPolarRatio[polId].second);
         }

         if (afr.fRhicAgsPolarRatio[polId].second >= 0)
         {
            if (afr.fRhicAgsPolarRatio[polId].first >= 2)
            {
               Error("PostFill", "RHIC/AGS polarization ratio outlier: %f", afr.fRhicAgsPolarRatio[polId].first);
            } else {
               utils::AppendToGraph(grRhicAgsPolarRatiosByRing[ringId], fillId, afr.fRhicAgsPolarRatio[polId].first, 0, afr.fRhicAgsPolarRatio[polId].second);
            }
         }

         ValErrPair agsPol = afr.GetAgsCniPolar();
         Info("PostFill", "AGS polar %lg+/-%lg", agsPol.first, agsPol.second);
         if (agsPol.second >= 0)
         {
            utils::AppendToGraph(grAgsPolarVsFillId, fillId, agsPol.first, 0, agsPol.second);
         }
      }

      // now fill histograms using the above results
      RingIdSetIter iRingId = gRunConfig.fRings.begin();

      for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
      {
         ERingId ringId  = *iRingId;
         string  sRingId = RunConfig::AsString(ringId);

         SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();

         for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
         {
            if (*iSS == kSPIN_NULL) continue;

            string sSS = gRunConfig.AsString(*iSS);

            string grName = "grBunchAsymChi2_" + sSS;
            TGraph* grBunchAsymChi2_ = (TGraph*) ((TH1*) o["hBunchAsymChi2_" + sRingId])->GetListOfFunctions()->FindObject(grName.c_str());

            grName = "grBunchAsymSigma_" + sSS;
            TGraph* grBunchAsymSigma_ = (TGraph*) ((TH1*) o["hBunchAsymSigma_" + sRingId])->GetListOfFunctions()->FindObject(grName.c_str());

            // now extract the chi2 from single fill hist
            TH1*    hAsymVsBunchId_X_  = (TH1*) ((MAsymSingleFillHists*) oc)->o["hAsymVsBunchId_X_" + sFillId + "_" + sRingId];
            grName = "grAsymVsBunchId_X_" + sSS;
            TGraph* grAsymVsBunchId_X_ = (TGraph*) hAsymVsBunchId_X_->GetListOfFunctions()->FindObject(grName.c_str());

            if (grAsymVsBunchId_X_->GetN() <= 0) continue;

            TF1* fitFunc   = (TF1*) grAsymVsBunchId_X_->GetListOfFunctions()->FindObject("fitFunc");
            Double_t chi2ndf = fitFunc->GetNDF() > 0 ? fitFunc->GetChisquare()/fitFunc->GetNDF() : -1;
            Double_t sigma_y = fitFunc->GetParameter(1); // extract sigma_y, parameter 1

            utils::AppendToGraph(grBunchAsymChi2_,  fillId, chi2ndf);
            utils::AppendToGraph(grBunchAsymSigma_, fillId, sigma_y);
         }

         utils::UpdateLimitsFromGraphs((TH1*) o["hBunchAsymChi2_"  + sRingId], 2);
         utils::UpdateLimitsFromGraphs((TH1*) o["hBunchAsymSigma_" + sRingId], 2);
      }
   }
}


/** */
void MAsymFillHists::UpdateLimits()
{
   char hName[256];

   utils::UpdateLimitsFromGraphs(hAgsPolarVsFillId, 2);

   PolarimeterIdSetIter iPolId = gRunConfig.fPolarimeters.begin();

   for ( ; iPolId != gRunConfig.fPolarimeters.end(); ++iPolId)
   {
      //for (BeamEnergySetIter iBE=gRunConfig.fBeamEnergies.begin(); iBE!=gRunConfig.fBeamEnergies.end(); ++iBE)
      //{
         string sPolId = RunConfig::AsString(*iPolId);
         //string strBeamE = RunConfig::AsString(*iBE);

         sprintf(hName, "hPolarVsFillTime_%s", sPolId.c_str());
         TList* list = ((TH1*) o[hName])->GetListOfFunctions();
         TIter  next(list);

         while ( TGraphErrors *graphErrs = (TGraphErrors*) next() ) {
            utils::SubtractMinimum(graphErrs);
         }

         // Profile r
         sprintf(hName, "hRVsFillTime_%s", sPolId.c_str());
         list = ((TH1*) o[hName])->GetListOfFunctions();
         TIter nextRlist(list);

         while ( TGraphErrors *graphErrs = (TGraphErrors*) nextRlist() ) {
            utils::SubtractMinimum(graphErrs);
         }

         //((TH1*) o[hName])->GetXaxis()->SetTimeOffset(xmin, "gmt");
         //((TH1*) o[hName])->GetXaxis()->SetLimits(xmin, xmax);

         //sprintf(grName, "grPolarVsFillTime_%05d_%s", fillId, sPolId.c_str());
         //sprintf(hName, "hPolarVsTime_%s_%s", sPolId.c_str(), strBeamE.c_str());

         //((TH1*) o[hName])->GetXaxis()->SetLimits(fMinTime, fMaxTime);

         grRotatorPCPolarRatiosByPol[*iPolId]->Fit("pol0");
         utils::UpdateLimitsFromGraphs(hRotatorPCPolarRatiosByPol[*iPolId], 2);

         grRampPCPolarRatiosByPol[*iPolId]->Fit("pol0");
         utils::UpdateLimitsFromGraphs(hRampPCPolarRatiosByPol[*iPolId], 2);
      //}
   }

   RingIdSetIter iRingId = gRunConfig.fRings.begin();
   for ( ; iRingId != gRunConfig.fRings.end(); ++iRingId)
   {
      ERingId ringId  = *iRingId;

      TF1 fitFunc("fitFunc", "[0] + [1]*0");
      fitFunc.SetParNames("const", "#sigma_{Y}");
      Double_t sigma;

      sigma = grRotatorPCPolarRatiosByRing[ringId]->GetRMS(2);
      fitFunc.FixParameter(1, sigma);
      grRotatorPCPolarRatiosByRing[ringId]->Fit(&fitFunc);
      utils::UpdateLimitsFromGraphs(hRotatorPCPolarRatiosByRing[ringId], 2);

      sigma = grRampPCPolarRatiosByRing[ringId]->GetRMS(2);
      fitFunc.FixParameter(1, sigma);
      grRampPCPolarRatiosByRing[ringId]->Fit(&fitFunc);
      utils::UpdateLimitsFromGraphs(hRampPCPolarRatiosByRing[ringId], 2);

      sigma = grRhicAgsPolarRatiosByRing[ringId]->GetRMS(2);
      fitFunc.FixParameter(1, sigma);
      grRhicAgsPolarRatiosByRing[ringId]->Fit(&fitFunc);
      utils::UpdateLimitsFromGraphs(hRhicAgsPolarRatiosByRing[ringId], 2);
   }

   DrawObjContainer::UpdateLimits();
}


/** */
DrawObjContainer *MAsymFillHists::GetSingleFillHists(const AnaFillResult &afr)
{
   UInt_t fillId = afr.GetFillId();

   // Set individual fill sub dirs
   string sFillId(5, ' ');
   sprintf(&sFillId[0], "%05d", fillId);
   string dName = sFillId;

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir = d.find(dName);

   if ( isubdir == d.end()) { // if dir not found
      TDirectoryFile *tdir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      oc = new MAsymSingleFillHists(tdir, &afr);
      d[dName] = oc;
   } else {
      oc = isubdir->second;
   }

   return oc;
}
