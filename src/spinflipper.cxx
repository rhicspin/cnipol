#include <fstream>
#include <sstream>
#include <string>

#include "TGraphErrors.h"
#include "TFile.h"
#include "EventConfig.h"
#include "DrawObjContainer.h"
#include "CnipolSpinStudyHists.h"
#include "TCanvas.h"
#include "TROOT.h"

#include "utils/utils.h"

//static const double _TWO_PI= 6.2831853071795864769252867665590057683943387987502116419498891846156328125724179972560696;
static const uint _SPIN_FLIPPER_PERIOD = 33554432; // 2^25

using namespace std;

// Define some helper functions
void   changePhase(TGraphErrors* gr, Double_t phase);
void   removePoints(TGraphErrors* gr);
double calcSpinFlipperPhase(uint markerAbsRev, uint markerRelRev, uint spinFlipperTune);


/** */
int main(int argc, char *argv[])
{
   TDirectory *myDir = gDirectory;
   gROOT->Macro(CNIPOL_ROOT_DIR "/contrib/styles/style_masym.C");

   // Pick the input file
   string spinFlipperFileName("spinflipper_study_1");
   //string spinFlipperFileName("spinflipper_study_2");
   //string spinFlipperFileName("spinflipper_study_3");

   std::stringstream fullPath("");
   fullPath << "/eic/u/dsmirnov/cnipol_results/" + spinFlipperFileName;
   ifstream fileSpinFlipperCombo(fullPath.str().c_str());

   uint markerAbsRev, spinFlipperTune;            // in revolutions
   uint iline = 0;
   string measId;
   vector<string> measIds;
   vector<uint>   markerAbsRevs;

   while ( fileSpinFlipperCombo.good() )
   {
      iline++;

      if (iline == 1) {
         fileSpinFlipperCombo >> spinFlipperTune;
         continue;
      }

      fileSpinFlipperCombo >> measId >> markerAbsRev;

      if (fileSpinFlipperCombo.eof()) break;

      measIds.push_back(measId);
      markerAbsRevs.push_back(markerAbsRev);
   }

   printf("spinFlipperTune: %d\n", spinFlipperTune);

   TH2* hAsymVsOscillPhase_Y45_phys = NULL;
   TGraphErrors* graph = NULL;

   vector<string>::const_iterator iMeasId = measIds.begin();
   vector<uint>::const_iterator   iMarkerAbsRev = markerAbsRevs.begin();

   for ( ; iMeasId!=measIds.end(); ++iMeasId, ++iMarkerAbsRev)
   {
      printf("\n\nmeasId: %s, markerAbsRev: %d\n", (*iMeasId).c_str(), *iMarkerAbsRev);

      string fileName = "/eicdata/eic0005/run13/root/" + *iMeasId + "/" + *iMeasId + ".root";

      TFile *f = new TFile(fileName.c_str(), "READ");

      if (!f) { Error("spinflipper", "File not found. Skipping..."); delete f; continue; }

      Info("masym", "Found file: %s", fileName.c_str());

      EventConfig *gMM = (EventConfig*) f->FindObjectAny("measConfig");

      if (!gMM) { Error("spinflipper", "MM not found. Skipping..."); f->Close(); delete f; continue; }

      // Calculate the position of the marker in the data set w.r.t. the first revolution in the data
      const std::vector<UInt_t>& spinFlipperMarkers = gMM->fMeasInfo->GetSpinFlipperMarkers();
      UInt_t spinFlipperMarkerRevId    = spinFlipperMarkers.size() > 0 ? spinFlipperMarkers[0] : 0;
      UInt_t spinFlipperMarkerRelRevId = spinFlipperMarkerRevId - gMM->fMeasInfo->GetFirstRevolution();

      cout << "spinFlipperMarkerRelRevId: " << spinFlipperMarkerRelRevId << endl;

      double phase = calcSpinFlipperPhase(*iMarkerAbsRev, spinFlipperMarkerRelRevId, spinFlipperTune);
      //phase += 0.01;

      cout << "phase: " << phase << endl;

      DrawObjContainer *gHIn = new DrawObjContainer(f);
      gHIn->d["studies"] = new CnipolSpinStudyHists();
      gHIn->ReadFromDir();
      //gHIn->Print();

      TH1* hAsymVsOscillPhase_Y45_phys_ = (TH1*) gHIn->d["studies"]->o["hAsymVsOscillPhase_Y45_phys"];

      if ( iMeasId == measIds.begin() ) {
         myDir->cd();
         hAsymVsOscillPhase_Y45_phys = (TH2*) hAsymVsOscillPhase_Y45_phys_->Clone("hAsymVsOscillPhase_Y45_phys_new");

         // temporarily remove all graphs
         //hAsymVsOscillPhase_Y45_phys->GetListOfFunctions()->Clear();

         graph = (TGraphErrors*) hAsymVsOscillPhase_Y45_phys->GetListOfFunctions()->FindObject("grhAsymVsOscillPhase_Y45_phys");
         graph->GetListOfFunctions()->Clear();
         removePoints(graph);

         hAsymVsOscillPhase_Y45_phys->Print();
         //phase = 0.05;
      }

      TGraphErrors* graph_ = (TGraphErrors*) hAsymVsOscillPhase_Y45_phys_->GetListOfFunctions()->FindObject("grhAsymVsOscillPhase_Y45_phys");
      graph_->Print();

      changePhase(graph_, phase);

      //graph_->Print();
      utils::MergeGraphs(graph, graph_);

      // For test add all graphs
      //hAsymVsOscillPhase_Y45_phys->GetListOfFunctions()->Add((TGraphErrors*) graph_->Clone(), "p");
      //hAsymVsOscillPhase_Y45_phys->GetListOfFunctions()->SetOwner();
      //graph->Print();

      TCanvas canvas("canvas", "canvas", 1200, 600);
      //gHIn->SaveAllAs(canvas, "^.*$", ("./tmp_images/" + spinFlipperFileName + "_" + *iMeasId).c_str());

      delete gHIn;
      f->Close();
      delete f;

      //hAsymVsOscillPhase_Y45_phys->Print();
   }

   utils::UpdateLimitsFromGraphs(hAsymVsOscillPhase_Y45_phys, 2);

   TF1 fitFunc("fitFunc", "[0] + [1]*sin(x + [2])", 0, _TWO_PI);
   fitFunc.SetParNames("Offset", "Amplitude", "Phase");
   graph->Fit(&fitFunc);

   TCanvas canvas("canvas", "canvas", 1200, 600);
   hAsymVsOscillPhase_Y45_phys->SetStats(kFALSE);
   hAsymVsOscillPhase_Y45_phys->Draw();
   canvas.SaveAs((string(hAsymVsOscillPhase_Y45_phys->GetName())+".png").c_str());
}


void changePhase(TGraphErrors* gr, Double_t phase)
{
   double x, y;

   phase = phase < 0 ? _TWO_PI + phase : phase;

   for (Int_t i=0; i<gr->GetN(); i++)
   {
      gr->GetPoint(i, x, y);
      x = fmod(x - phase, _TWO_PI);
      x = x < 0 ? _TWO_PI + x : x;
      gr->SetPoint(i, x, y);
   }
}


void removePoints(TGraphErrors* gr)
{
   for (Int_t i=0; i<gr->GetN(); i++)
   {
      gr->RemovePoint(i);
      i--;
   }
}


double calcSpinFlipperPhase(uint markerAbsRev, uint markerRelRev, uint spinFlipperTune)
{
   double phase  = _TWO_PI * spinFlipperTune/_SPIN_FLIPPER_PERIOD * ( (Double_t) markerAbsRev - markerRelRev);
   double spinFlipperPhase = fmod(phase, _TWO_PI);
   return spinFlipperPhase;
}
