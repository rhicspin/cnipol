
#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TStyle.h"

#include "DrawObjContainer.h"
#include "RunConfig.h"

#include "utils/utils.h"


TFile            *gRootOutFile;
DrawObjContainer *gH;

void Initialize();


/** */
void hjetPol()
{
   Initialize();

   //TFile *f = new TFile("hjet_out.root", "RECREATE");

   TGraphErrors* grYel = new TGraphErrors("hjet_pol_yel.txt");
   TGraphErrors* grBlu = new TGraphErrors("hjet_pol_blu.txt");

   Double_t xmin, ymin, xmax, ymax;

   grYel->ComputeRange(xmin, ymin, xmax, ymax);

	gH->o["hYel"] = new TH1F("hYel", "hYel", xmax-xmin+1, xmin-0.5, xmax+0.5);

   TH1* hYel = (TH1*) gH->o["hYel"];

   //hYel->GetListOfFunctions()->Add(grYel, "p");
   hYel->SetMarkerStyle(kFullCircle);
   hYel->SetMarkerSize(1);
   hYel->SetMarkerColor(RunConfig::AsColor(kY2U));
   hYel->SetOption("E1");

   grBlu->ComputeRange(xmin, ymin, xmax, ymax);

	gH->o["hBlu"] = new TH1F("hBlu", "hBlu", xmax-xmin+1, xmin-0.5, xmax+0.5);

   TH1* hBlu = (TH1*) gH->o["hBlu"];

   hBlu->SetMarkerStyle(kFullCircle);
   hBlu->SetMarkerSize(1);
   hBlu->SetMarkerColor(RunConfig::AsColor(kB1U));
   hBlu->SetOption("E1");

   //TCanvas *c = new TCanvas("cYel", "cYel", 800, 600);
   //grYel->Fit("pol0");
	//grYel->Draw("A*");

   //c = new TCanvas("cBlu", "cBlu", 800, 600);
   //grBlu->Fit("pol0");
	//grBlu->Draw("A*");

   utils::BinGraph(grYel, hYel);

   hYel->Scale(100);

   //c = new TCanvas("cYelHist", "cYelHist", 800, 600);
   hYel->Fit("pol0");
	//hYel->Draw("E1");

   utils::BinGraph(grBlu, hBlu);

   hBlu->Scale(100);

   //c = new TCanvas("cBluHist", "cBluHist", 800, 600);
   hBlu->Fit("pol0");
	//hBlu->Draw("E1");

   //f->Write();
   //f->Close();

   TCanvas canvas("cName2", "cName2", 1400, 600);
   gH->SaveAllAs(canvas);
   gH->Write();

   gRootOutFile->Close();
}


/** */
void Initialize()
{
   //gStyle->SetPadRightMargin(0.30);
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);
   gStyle->SetPadRightMargin(0.18);

   gRootOutFile  = new TFile("hjetPol_out.root", "RECREATE");
   gH = new DrawObjContainer(gRootOutFile);
}
