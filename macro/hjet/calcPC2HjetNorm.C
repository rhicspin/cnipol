
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "DrawObjContainer.h"
#include "RunConfig.h"

#include "utils/utils.h"

namespace pc2h {

TFile            *gRootOutFile;
DrawObjContainer *gH;


void  calcPC2HjetNorm();
void  Initialize();
TH1F* GetRatio(TH1F* hHjet, TH1F* hPC);

}


/** */
void pc2h::calcPC2HjetNorm()
{ //{{{
   pc2h::Initialize();

   //TGraphErrors* gr = new TGraphErrors("pcjet_norm.dat");
   //TGraphErrors* gr = new TGraphErrors("pcjet_norm_blu.dat");
   //TGraphErrors* gr = new TGraphErrors("pcjet_norm_yel.dat");

   //gr->Fit("pol0");
	//gr->Draw("A*");

   TFile *fHjet  = new TFile("hjetPol_out.root", "READ");

   TH1F* hHjetYel = (TH1F*) fHjet->FindObjectAny("hYel");
   TH1F* hHjetBlu = (TH1F*) fHjet->FindObjectAny("hBlu");

   hHjetYel->Print("");
   hHjetBlu->Print("");

   //fHjet->Close();


   //TFile *fPC = new TFile("~dsmirnov/eic0005/cnipol/anapc/masym_out.root", "READ");
   //TFile *fPC = new TFile("~dsmirnov/eic0005/cnipol/anapc/masym_run11_15XXX_1XX_2XX_3XX_4XX_out.root", "READ");
   //TFile *fPC = new TFile("~dsmirnov/cnipol/anapc/masym_run11_15XXX_1XX_2XX_3XX_4XX_out.root", "READ");
   TFile *fPC = new TFile("~dsmirnov/cnipol/anapc/masym_run11_tmp_goodruns_out.root", "READ");

   TH1F *hPC_B1U = (TH1F*) fPC->FindObjectAny("hPolarVsFill_B1U_250");
	//hPC_B1U->Scale(0.01);

   TH1F *hPC_Y1D = (TH1F*) fPC->FindObjectAny("hPolarVsFill_Y1D_250");
	//hPC_Y1D->Scale(0.01);

   TH1F *hPC_B2D = (TH1F*) fPC->FindObjectAny("hPolarVsFill_B2D_250");
	//hPC_B2D->Scale(0.01);

   TH1F *hPC_Y2U = (TH1F*) fPC->FindObjectAny("hPolarVsFill_Y2U_250");
	//hPC_Y2U->Scale(0.01);

	//TH1F *hRatio = GetRatio(hHjet, hPC);
	gH->o["hRatio_B1U"] = GetRatio(hHjetBlu, hPC_B1U);
   ((TH1*) gH->o["hRatio_B1U"])->SetName("hRatio_B1U");
   ((TH1*) gH->o["hRatio_B1U"])->SetMarkerStyle(kFullCircle);
   ((TH1*) gH->o["hRatio_B1U"])->SetMarkerSize(1);
   ((TH1*) gH->o["hRatio_B1U"])->SetMarkerColor(RunConfig::AsColor(kB1U));
   ((TH1*) gH->o["hRatio_B1U"])->SetOption("E1");

	gH->o["hRatio_Y1D"] = GetRatio(hHjetYel, hPC_Y1D);
   ((TH1*) gH->o["hRatio_Y1D"])->SetName("hRatio_Y1D");
   ((TH1*) gH->o["hRatio_Y1D"])->SetMarkerStyle(kFullCircle);
   ((TH1*) gH->o["hRatio_Y1D"])->SetMarkerSize(1);
   ((TH1*) gH->o["hRatio_Y1D"])->SetMarkerColor(RunConfig::AsColor(kY1D));
   ((TH1*) gH->o["hRatio_Y1D"])->SetOption("E1");

	gH->o["hRatio_B2D"] = GetRatio(hHjetYel, hPC_B2D);
   ((TH1*) gH->o["hRatio_B2D"])->SetName("hRatio_B2D");
   ((TH1*) gH->o["hRatio_B2D"])->SetMarkerStyle(kFullCircle);
   ((TH1*) gH->o["hRatio_B2D"])->SetMarkerSize(1);
   ((TH1*) gH->o["hRatio_B2D"])->SetMarkerColor(RunConfig::AsColor(kB2D));
   ((TH1*) gH->o["hRatio_B2D"])->SetOption("E1");

	gH->o["hRatio_Y2U"] = GetRatio(hHjetYel, hPC_Y2U);
   ((TH1*) gH->o["hRatio_Y2U"])->SetName("hRatio_Y2U");
   ((TH1*) gH->o["hRatio_Y2U"])->SetMarkerStyle(kFullCircle);
   ((TH1*) gH->o["hRatio_Y2U"])->SetMarkerSize(1);
   ((TH1*) gH->o["hRatio_Y2U"])->SetMarkerColor(RunConfig::AsColor(kY2U));
   ((TH1*) gH->o["hRatio_Y2U"])->SetOption("E1");

   //TCanvas *c = new TCanvas("cRatio", "cRatio", 800, 600);
	//hRatio->Draw();
	//utils::PrintNiceToFile(hRatio);

   //c = new TCanvas("cHjet", "cHjet", 800, 600);
	//hHjet->Draw();
	//utils::PrintNiceToFile(hHjet);

   //c = new TCanvas("cPC", "cPC", 800, 600);
	//hPC->Draw();
	//utils::PrintNiceToFile(hPC);

   TCanvas canvas("cName2", "cName2", 1400, 600);
   //gH->SaveAllAs(canvas);
   gH->Write();

   gRootOutFile->Close();
} //}}}


/** */
void pc2h::Initialize()
{ //{{{
   //gStyle->SetPadRightMargin(0.30);
   gStyle->SetOptTitle(0);
   gStyle->SetOptStat(0);
   gStyle->SetPadRightMargin(0.18);

   gRootOutFile  = new TFile("calcPC2HjetNorm_out.root", "RECREATE");
   gH = new DrawObjContainer(gRootOutFile);
} //}}}


/** Each bin is assumed to be a fill. */
TH1F* pc2h::GetRatio(TH1F* hHjet, TH1F* hPC)
{ //{{{
   //Int_t minFill = hPC->GetXaxis()->GetXMin() < hHjet->GetXaxis()->GetXMin() ? hPC->GetXaxis()->GetXMin() : hHjet->GetXaxis()->GetXMin();
   //Int_t maxFill = hPC->GetXaxis()->GetXMax() < hHjet->GetXaxis()->GetXMax() ? hPC->GetXaxis()->GetXMax() : hHjet->GetXaxis()->GetXMax();

   //Int_t nFills  = maxFill - minFill;

   // Copy hjet histogram
   TH1F *hPCRebinned = new TH1F(*hHjet);
	hPCRebinned->Reset();

   // Fill hPCRebinned with same fills as in hHjet
   for (Int_t ibPCR=1; ibPCR<=hPCRebinned->GetNbinsX(); ibPCR++)
	{
   //for (Int_t iFill=minFill; iFill<maxFill; iFill++)
	   
      Int_t iFill = (Int_t) hPCRebinned->GetBinCenter(ibPCR);

		Int_t ibPC         = hPC->FindBin(iFill);
		//Int_t ibPCRebinned = hPCRebinned->FindFixBin(iFill);
		//Int_t    ibPCR = hHjet->FindBin(iFill);

      // skip under/overflow bins as should never occur
		if (ibPC < 1 || ibPC > hPC->GetNbinsX() ) continue;

		Double_t polPC    = hPC->GetBinContent(ibPC);
		Double_t polPCErr = hPC->GetBinError(ibPC);

		if (polPC || polPCErr) {
		   hPCRebinned->SetBinContent(ibPCR, polPC);
		   hPCRebinned->SetBinError(ibPCR, polPCErr);
		} else // skip empty bins
		   continue;

		//Double_t polHj    = hHjet->GetBinContent(ibPCR);
		//Double_t polHjErr = hHjet->GetBinError(ibPCR);
   }

   TH1F *hRatio = new TH1F(*hHjet);
	hRatio->SetName("hRatio");
	hRatio->SetTitle("hRatio");

	hRatio->Divide(hPCRebinned);

	delete hPCRebinned;

	hRatio->Fit("pol0");

	return hRatio;
} //}}}
