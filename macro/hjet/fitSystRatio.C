
#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualFitter.h"
#include "TFitResult.h"

#include "DrawObjContainer.h"
#include "RunConfig.h"

#include "utils/utils.h"


TFile            *gRootOutFile;
DrawObjContainer *gH;
TH1D             *hRatio;


void fitSystRatio();
extern void systRatioObjFunc(Int_t &npar, Double_t *gin, Double_t &f, Double_t *u, Int_t flag);
Double_t constFunc(Double_t *x, Double_t *par) {return 1;};


/** */
void fitSystRatio()
{ //{{{
   //Initialize();

   TFile *fPC = new TFile("~dsmirnov/cnipol/masym_run11_tmp_goodruns_out.root", "READ");
   //TFile *fPC = new TFile("~dsmirnov/cnipol/masym_run11_tmp_goodruns_small_out.root", "READ");

   hRatio = (TH1D*) fPC->FindObjectAny("hBluPolarRatioSystVsFill_250");
   //hRatio = (TH1D*) fPC->FindObjectAny("hYelPolarRatioSystVsFill_250");
   //hRatio = (TH1D*) fPC->FindObjectAny("hPolarHJPCRatioSystVsFill_B1U_250");
   //hRatio = (TH1D*) fPC->FindObjectAny("hProfPolarRatioSystVsFill_B1U_250");
   //hRatio = (TH1D*) fPC->FindObjectAny("hProfPolarRatioSystVsFill_Y2U_250");

	hRatio->Print();
	//hRatio->Fit("pol0");

	Double_t fMinFill = hRatio->GetXaxis()->GetXmin();
	Double_t fMaxFill = hRatio->GetXaxis()->GetXmax();
	//Double_t fMinFill = 15165.5;
	//Double_t fMaxFill = 15166.5;

   utils::SystRatioFitFunctor *srff = new utils::SystRatioFitFunctor(*hRatio);

   TF1 *systRatioFitFunc = new TF1("systRatioFitFunc", srff, -10, 10, 0, "SystRatioFitFunctor");

   TF1 *systRatioConstFunc = new TF1("systRatioConstFunc", constFunc, fMinFill, fMaxFill, 1);
	//systRatioConstFunc->FixParameter(0, 1);

	ROOT::Math::WrappedTF1 wf1(*systRatioFitFunc);
   ROOT::Math::BrentRootFinder brf;

   brf.SetFunction(wf1, 0, 1);
   brf.Solve();

   cout << brf.Root() << endl;

   //TVirtualFitter::Fitter(hRatio)->SetFCN(systRatioObjFunc);
   //hRatio->Fit(systRatioFitFunc, "U R");

	TH1D* hRatioCopy = new TH1D(*hRatio);
	hRatioCopy->Print();

   Double_t dSystStep = 0.001;

	for (Double_t dSyst=0; dSyst<=1; dSyst+=dSystStep) {
	   
      for (Int_t ib=1; ib!=hRatio->GetNbinsX(); ++ib)
	   {
	      Double_t bc = hRatio->GetBinContent(ib);
	      Double_t be = hRatio->GetBinError(ib);

		   if (!be && !bc) continue; // skip empty bins

			hRatioCopy->SetBinError(ib, sqrt(be*be + dSyst*dSyst));
	   }

	   hRatioCopy->Print();

	   TFitResultPtr fitres = hRatioCopy->Fit(systRatioConstFunc, "E S R Q", "");

		Double_t ndf  = fitres->Ndf();
		Double_t chi2 = fitres->Chi2();

		printf("%f: %f %f      \n", dSyst, chi2, ndf);

		if (chi2/ndf <= 1) break;
	}

	//hRatioCopy->Draw();

} //}}}


/** */
void systRatioObjFunc(Int_t &npar, Double_t *gin, Double_t &f, Double_t *u, Int_t flag)
{ //{{{

   if (gin)
      printf("gin: %f   ", *gin);

   Double_t chi2_sum = 0;
	Int_t    nPoints  = 0;

   for (Int_t ib=1; ib!=hRatio->GetNbinsX(); ++ib)
	{
	   if (ib != 67) continue;

	   Double_t bc = hRatio->GetBinContent(ib);
	   Double_t be = hRatio->GetBinError(ib);

		if (!be && !bc) continue; // skip empty bins

		printf("%5d: %f +/- %f      %f\n", nPoints, bc, be, u[0]);

	   Double_t chi2 = (bc - 1)*(bc - 1)/(be*be + u[0]*u[0]);
		chi2_sum += chi2;
		nPoints++;
	}

	f = -1*chi2_sum;///nPoints - 1.;

   //for (int i=0; i<npar; i++) {
   //   printf("%f   ", u[i]);
	//}
	printf("\n");
	printf("f: %f\n", f);

	//f = 0;
} //}}}
