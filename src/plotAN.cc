
#include <string>

#include "TCanvas.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMath.h"
#include "TMatrixDSym.h"
#include "TMinuit.h"
#include "TPaveStats.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TVector2.h"

#include "plotAN.h"

#include "utils/utils.h"


using namespace std;


/** */
int main(int argc, char *argv[])
{
   //gROOT->Reset();
   //gROOT->SetStyle("Plain");
   //gROOT->SetMacroPath("./:~/rootmacros/:");
   gROOT->Macro("styles/style_anapow.C");
   gStyle->SetLineWidth(2);

   double BeamPhysBlue[6], TargetPhysBlue[6], BlueErr[6];

   TargetPhysBlue[0] = -0.039243;   BeamPhysBlue[0] = 0.020341;   BlueErr[0] = 0.000914;
   TargetPhysBlue[1] = -0.040520;   BeamPhysBlue[1] = 0.020076;   BlueErr[1] = 0.000939;
   TargetPhysBlue[2] = -0.040819;   BeamPhysBlue[2] = 0.018495;   BlueErr[2] = 0.000967;
   TargetPhysBlue[3] = -0.040041;   BeamPhysBlue[3] = 0.020347;   BlueErr[3] = 0.000992;
   TargetPhysBlue[4] = -0.038616;   BeamPhysBlue[4] = 0.018231;   BlueErr[4] = 0.001038;
   TargetPhysBlue[5] = -0.037101;   BeamPhysBlue[5] = 0.016961;   BlueErr[5] = 0.001047;

   string dataFileName = "pp_ANvst_s021.9_Hjet_2012_255GeV";

   TGraphErrors *grANStat  = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");
   TGraphErrors *grANSyst  = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");
   TGraphErrors *grANSystP = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");
   TGraphErrors *grANSystM = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");

   // Add systematic errors
   Double_t x, y, y_systM, y_systP;
   Double_t xe, ye, ye_syst;
   Double_t xmin, xmax, ymin, ymax;

   for (Int_t i=0; i!=grANStat->GetN(); i++) {
      grANStat->ComputeRange(xmin, ymin, xmax, ymax);
      grANStat->GetPoint(i, x, y);
      xe = grANStat->GetErrorX(i);
      ye = grANStat->GetErrorY(i);

      ye_syst = y*0.03;
      ye_syst = TMath::Sqrt( ye*ye + ye_syst*ye_syst);        // uncorrelated across -t syst. error
      y_systM = y - y*0.04 - 0.002*(x - xmin)/(xmax - xmin);  // correlated across -t syst. error
      y_systP = y + y*0.04 + 0.002*(x - xmin)/(xmax - xmin);  // correlated across -t syst. error

      grANSyst->SetPointError(i, xe, ye_syst);
      grANSystM->SetPoint(i, x, y_systM);
      grANSystP->SetPoint(i, x, y_systP);
   }

   //|t| = ((1E-2)-(2E-4))/ (Double_t) 64;
   TF1 *funcModelAN = new TF1("funcModelAN", modelAN, 1E-6, 4E-1, 2);

   TCanvas *canvas1 = new TCanvas("canvas1", "canvas1", 100, 100, 800, 480);

   canvas1->SetLogx();

   TH2C* frame1 = new TH2C("frame1", "frame1", 50, 0, 0.05, 100, 0, 0.06);

   frame1->GetXaxis()->SetTitle("-t, (GeV/c)^{2}");
   frame1->GetYaxis()->SetTitle("A_{N}");
   frame1->SetStats(kFALSE);
   frame1->Draw();

   funcModelAN->SetRange(1E-3, 5E-2);
   funcModelAN->SetParNames("Re r_{5}", "Im r_{5}");
   funcModelAN->SetParameter(0, 0); // Rer5
   funcModelAN->SetParameter(1, 0); // Imr5

   funcModelAN->SetLineColor(kBlue);
   funcModelAN->SetLineStyle(1);

   TEllipse* errEllipseStat  = new TEllipse();
   TEllipse* errEllipseSyst  = new TEllipse();
   TEllipse* errEllipseSystP = new TEllipse();
   TEllipse* errEllipseSystM = new TEllipse();

   TFitResultPtr fitResStat  = FitGraph(grANStat,  funcModelAN, errEllipseStat);
   TFitResultPtr fitResSyst  = FitGraph(grANSyst,  funcModelAN, errEllipseSyst);
   TFitResultPtr fitResSystP = FitGraph(grANSystP, funcModelAN, errEllipseSystP);
   TFitResultPtr fitResSystM = FitGraph(grANSystM, funcModelAN, errEllipseSystM);

   errEllipseStat->Print();
   errEllipseSyst->Print();
   errEllipseSystP->Print();
   errEllipseSystM->Print();

   Double_t reR5Syst    = TMath::Max(fabs(errEllipseStat->GetX1() - errEllipseSystP->GetX1()), fabs(errEllipseStat->GetX1() - errEllipseSystM->GetX1()));
   Double_t imR5Syst    = TMath::Max(fabs(errEllipseStat->GetY1() - errEllipseSystP->GetY1()), fabs(errEllipseStat->GetY1() - errEllipseSystM->GetY1()));

   Double_t corr        = fitResStat->GetCorrelationMatrix()[0][1];
   Double_t reR5        = fitResStat->Value(0);
   Double_t reR5ErrStat = fitResStat->ParError(0);
   Double_t imR5        = fitResStat->Value(1);
   Double_t imR5ErrStat = fitResStat->ParError(1);

   Double_t reR5ErrTot  = sqrt(fitResSyst->ParError(0)*fitResSyst->ParError(0) + reR5Syst*reR5Syst);
   Double_t imR5ErrTot  = sqrt(fitResSyst->ParError(1)*fitResSyst->ParError(1) + imR5Syst*imR5Syst);

   Double_t r5          = sqrt(reR5*reR5 + imR5*imR5);
   Double_t r5ErrStat   = sqrt(reR5ErrStat*reR5ErrStat + imR5ErrStat*imR5ErrStat + 2*corr * reR5ErrStat * imR5ErrStat);
   Double_t r5ErrTot    = sqrt(reR5ErrTot*reR5ErrTot   + imR5ErrTot*imR5ErrTot   + 2*corr * reR5ErrTot * imR5ErrTot);

   printf("\nResult:\n********************************************************************************\n");

   printf("\n");
   printf("corr   = % 10.8lf\n", corr);
   printf("Re(r5) = % 10.8lf +/- % 10.8lf\n", reR5, reR5ErrStat);
   printf("Im(r5) = % 10.8lf +/- % 10.8lf\n", imR5, imR5ErrStat);
   printf("   r5  = % 10.8lf +/- % 10.8lf\n",   r5,   r5ErrStat);
   printf("\n");

   printf("Stat. + Syst:\n");
   printf("Re(r5) = % 10.8lf +/- % 10.8lf\n", reR5, reR5ErrTot);
   printf("Im(r5) = % 10.8lf +/- % 10.8lf\n", imR5, imR5ErrTot);
   printf("   r5  = % 10.8lf +/- % 10.8lf\n",   r5,   r5ErrTot);
   printf("\n");

   ValErrPair par1(reR5, reR5ErrTot);
   ValErrPair par2(imR5, imR5ErrTot);

   TEllipse* errEllipseSystTot = utils::GetErrorEllipse(par1, par2, corr);

   grANStat->SetMarkerStyle(kFullDotLarge);
   grANStat->SetMarkerSize(1);
   grANStat->SetMarkerColor(kBlack);
   grANStat->Draw("P");

   TF1* funcANSystP = grANSystP->GetFunction("funcModelAN");
   funcANSystP->SetLineStyle(2);
   funcANSystP->SetLineColor(kMagenta);
   funcANSystP->Draw("same");

   TF1* funcANSystM = grANSystM->GetFunction("funcModelAN");
   funcANSystM->SetLineStyle(2);
   funcANSystM->SetLineColor(kMagenta);
   funcANSystM->Draw("same");

   canvas1->Update();

   TPaveStats *stats = (TPaveStats*) grANStat->FindObject("stats");

   if (stats) {
      stats->SetX1NDC(0.72);
      stats->SetX2NDC(0.99);
      stats->SetY1NDC(0.42);
      stats->SetY2NDC(0.65);
   }

   TF1* funcModelAN_noflip = new TF1();
   funcModelAN->Copy(*funcModelAN_noflip);
   funcModelAN_noflip->SetParameter(0, 0); // Imr5
   funcModelAN_noflip->SetParameter(1, 0); // Rer5
   funcModelAN_noflip->SetLineColor(kBlack);
   funcModelAN_noflip->SetLineWidth(2);
   funcModelAN_noflip->SetLineStyle(1);
   funcModelAN_noflip->Draw("same");

   canvas1->SaveAs((dataFileName + "_AN.png").c_str());

   // contour plot Im_r5 vs. Re_r5
   TCanvas *canvas2 = new TCanvas("canvas2", "canvas2", 120, 120, 640, 480);

   double cRer5[1], cImr5[1];
   double cRer5_err[1], cImr5_err[1];

   cRer5[0]     = fitResStat->Value(0);    //funcModelAN->GetParameter(0);
   cImr5[0]     = fitResStat->Value(1);    //funcModelAN->GetParameter(1);
   cRer5_err[0] = fitResStat->ParError(0); //0; //fitTheoAn2->GetParError(2);
   cImr5_err[0] = fitResStat->ParError(1); //0; //fitTheoAn2->GetParError(1);

   TGraphErrors *gr_r5 = new TGraphErrors(1, cRer5, cImr5, cRer5_err, cImr5_err);
   gr_r5->SetMarkerStyle(kFullDotLarge);
   gr_r5->SetMarkerSize(0.2);
   gr_r5->SetMarkerColor(kBlue);

   TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.05, 0.05, 100, -0.15, 0.15);   // H-jet, 2012: 24 GeV, 255 GeV

   frame2->GetXaxis()->SetTitle("Re r_{5}");
   frame2->GetYaxis()->SetTitle("Im r_{5}");
   frame2->SetStats(kFALSE);
   frame2->GetXaxis()->SetNdivisions(505);
   frame2->GetYaxis()->SetNdivisions(505);//sigma-123
   frame2->Draw();

   //Get contour for ERRDEF=1
   gMinuit->SetErrorDef(1);// 1sigma

   // Print results
   Double_t amin, edm, errdef;
   Int_t nvpar, nparx, icstat;
   //void mnstat(Double_t &fmin, Double_t &fedm, Double_t &errdef, Int_t &npari, Int_t &nparx, Int_t &istat)
   gMinuit->mnstat(amin, edm, errdef, nvpar, nparx, icstat);
   gMinuit->mnprin(0, amin);

   printf("\n");
   printf("amin  = %10f   edm   = %10f   errdef = %10f\n", amin, edm, errdef);
   printf("nvpar = %10d   nparx = %10d   icstat = %10d\n", nvpar, nparx, icstat);
   printf("\n");

   //void mnerrs(Int_t number, Double_t &eplus, Double_t &eminus, Double_t &eparab, Double_t &gcc)
   Double_t eplus, eminus, eparab, gcc;

   int number = 0; //Imr5
   gMinuit->mnerrs(number, eplus, eminus, eparab, gcc);
   printf("param1 (Imr5) eplus=%10f eminus=%10f eparab=%10f gcc=%10f\n", eplus, eminus, eparab, gcc);

   number = 1;     //Rer5
   gMinuit->mnerrs(number, eplus, eminus, eparab, gcc);
   printf("param2 (Rer5) eplus=%10f eminus=%10f eparab=%10f gcc=%10f\n", eplus, eminus, eparab, gcc);

   gMinuit->mnwerr();

   gr_r5->Draw("P");

   TLine *lx = new TLine(frame2->GetXaxis()->GetXmin(), 0, frame2->GetXaxis()->GetXmax(), 0.);
   lx->Draw("same");

   TLine *ly = new TLine(0, frame2->GetYaxis()->GetXmin(), 0, frame2->GetYaxis()->GetXmax());
   ly->Draw("same");

   errEllipseStat->SetFillStyle(0);
   errEllipseStat->SetLineColor(kBlue);
   errEllipseStat->SetLineWidth(2);
   errEllipseStat->Draw();

   errEllipseSystTot->SetFillStyle(0);
   errEllipseSystTot->SetLineStyle(2);
   errEllipseSystTot->SetLineColor(kMagenta);
   errEllipseSystTot->SetLineWidth(2);
   errEllipseSystTot->Draw();

   canvas2->SaveAs((dataFileName + "_ell.png").c_str());
}


/** */
void f_AN(double cal_t[], double cal_AN[], double rho, double sigma, double Imr5, double Rer5, double scale)
{
   int i;
   double mb2GeV_inv2 = 0.389379; // 1GeV-2=0.389mb factor to convert mb to GeV
   double beta = 0;
   double delta = 0.02;
   double delta_t = ((5E-2) - (1E-4)) / (double)256;

   double sigma_totP = sigma / mb2GeV_inv2;

   double tc = -8 * TMath::Pi() * ALPHA / (sigma_totP * sqrt(1 + rho * rho));

   for (i = 0; i < 256; ++i) {
      cal_t[i] = 0.5E-3 + delta_t * i;

      double tt = -1 * tc / cal_t[i];
      double ttt = tt * tt - 2 * (rho + delta) * tt + (1 + rho * rho) * (1 + beta * beta);
      double ans = tt * ((MAGMOM_PROTON - 1) * (1 - delta * rho) - 2 * (Imr5 - delta * Rer5)) - 2 * (Rer5 - rho * Imr5);

      cal_AN[i] = scale * ans * sqrt(cal_t[i]) / (double)(ttt * MASS_PROTON);
   }
}


/** */
Double_t modelAN(Double_t *x, Double_t *par)
{
   double minus_t = x[0];

   double Rer5 = par[0];
   double Imr5 = par[1];

   double mb2GeV_inv2 = 0.389379;          // 1GeV-2    = 0.389mb

   // sqrt(s) = 21.9 GeV, 255 GeV beam on H-jet target
   double B          = 12;
   double rho        = -0.0123;
   double sigma_totP = 39.245;

   double Lambda2 = 0.71;      //GeV^2
   double gamma   = 0.5772156;
   double kappa   = MAGMOM_PROTON - 1; // anomalous magnetic moment of proton

   sigma_totP /= mb2GeV_inv2;

   double delta = ALPHA*TMath::Log( 2./ minus_t / (B + 8./Lambda2) ) - ALPHA*gamma ;
   double tc    = -8 * TMath::Pi() * ALPHA / sigma_totP ;
   double tt    = -1 * tc / minus_t;
   double denom = tt * tt - 2 * (rho + delta) * tt + (1 + rho * rho);     // * (1 + beta * beta);
   double nom   = tt * (kappa * (1 - delta * rho) + 2 * (delta * Rer5 - Imr5)) - 2 * (Rer5 - rho * Imr5);

   return TMath::Sqrt(minus_t) * nom / MASS_PROTON / denom;
}


/** */
TFitResultPtr FitGraph(TGraph *gr, TF1 *func, TEllipse *ell)
{
   Info("FitGraph", "\n********************************************************************************\n");

   TFitResultPtr fitRes = gr->Fit(func, "R S B");

   double Rer5     = func->GetParameter(0);
   double Rer5_err = func->GetParError(0);
   double Imr5     = func->GetParameter(1);
   double Imr5_err = func->GetParError(1);
   Double_t corr   = 0;

   if (fitRes.Get()) {
      TMatrixDSym covMatrix  = fitRes->GetCovarianceMatrix();
      covMatrix.Print();
      TMatrixDSym corrMatrix = fitRes->GetCorrelationMatrix();
      corrMatrix.Print();

      Rer5     = fitRes->Value(0);
      Rer5_err = fitRes->ParError(0);
      Imr5     = fitRes->Value(1);
      Imr5_err = fitRes->ParError(1);

      corr = corrMatrix[0][1];
   }

   ValErrPair par1(Rer5, Rer5_err);
   ValErrPair par2(Imr5, Imr5_err);

   TEllipse* ellTmp = utils::GetErrorEllipse(par1, par2, corr);

   ellTmp->Copy(*ell);
   delete ellTmp;

   if (ell)
      ell->Print();
   else
      Error("plotAN", "Invalid ellipse");

   Double_t r5     = sqrt(Rer5*Rer5 + Imr5*Imr5);
   Double_t r5_err = sqrt(  Rer5_err*Rer5_err + Imr5_err*Imr5_err + 2*corr * Rer5_err * Imr5_err);

   printf("\n");
   printf("corr   = % 10.8lf\n", corr);
   printf("Re(r5) = % 10.8lf +/- % 10.8lf\n", Rer5, Rer5_err);
   printf("Im(r5) = % 10.8lf +/- % 10.8lf\n", Imr5, Imr5_err);
   printf("   r5  = % 10.8lf +/- % 10.8lf\n",   r5,   r5_err);
   printf("\n");

   return fitRes;
}
