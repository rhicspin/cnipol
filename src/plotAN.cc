
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
{ //{{{
   //gROOT->Reset();
   //gROOT->SetStyle("Plain");
   //gROOT->SetMacroPath("./:~/rootmacros/:");
   gROOT->Macro("styles/style_anapow.C");
   gStyle->SetLineWidth(2);

   //double targetPol    = 0.924;
   //double targetPolErr = 0.018;

   //double DataAN[6], DataErr[6];//, DataErrStat[6]; //final
   //double DataSysErr[6];

   // TargetPhys, BeamPhys, Err
   double BeamPhysBlue[6], TargetPhysBlue[6], BlueErr[6];

   TargetPhysBlue[0] = -0.039243;   BeamPhysBlue[0] = 0.020341;   BlueErr[0] = 0.000914;
   TargetPhysBlue[1] = -0.040520;   BeamPhysBlue[1] = 0.020076;   BlueErr[1] = 0.000939;
   TargetPhysBlue[2] = -0.040819;   BeamPhysBlue[2] = 0.018495;   BlueErr[2] = 0.000967;
   TargetPhysBlue[3] = -0.040041;   BeamPhysBlue[3] = 0.020347;   BlueErr[3] = 0.000992;
   TargetPhysBlue[4] = -0.038616;   BeamPhysBlue[4] = 0.018231;   BlueErr[4] = 0.001038;
   TargetPhysBlue[5] = -0.037101;   BeamPhysBlue[5] = 0.016961;   BlueErr[5] = 0.001047;

   //for (int i = 0; i < 6; ++i)
   //{
   //   DataAN[i]     = -1 * TargetPhysBlue[i];
   //   DataErr[i]    = BlueErr[i];
   //   DataSysErr[i] = 0;
   //   DataAN[i]  = DataAN[i]  / targetPol;
   //   DataErr[i] = DataErr[i] / targetPol;
   //}

   //double Data_tB[6], Err_t[6], Data_t[6];

   //Data_tB[0] = 1.24;   Err_t[0] = 0.5;
   //Data_tB[1] = 1.74;   Err_t[1] = 0.5;
   //Data_tB[2] = 2.25;   Err_t[2] = 0.5;
   //Data_tB[3] = 2.74;   Err_t[3] = 0.5;
   //Data_tB[4] = 3.25;   Err_t[4] = 0.5;
   //Data_tB[5] = 3.75;   Err_t[5] = 0.5;

   //for (int i = 0; i < 6; ++i) {
   //   Data_t[i] = Data_tB[i] * 1E-3 * 2 * MASS_PROTON;
   //   Err_t[i] = 0;
   //}

   //string dataFileName = "pp_ANvst_s006.8_Hjet_2004_24GeV";
   //string dataFileName = "pp_ANvst_s006.8_Hjet_2004_24GeV_toterr";
   //string dataFileName = "pp_ANvst_s006.8_Hjet_2012_24GeV";
   //string dataFileName = "pp_ANvst_s013.7_Hjet_2004_100GeV";
   //string dataFileName = "pp_ANvst_s013.7_Hjet_2004_100GeV_toterr";
   //string dataFileName = "pp_ANvst_s013.7_Hjet_2009_100GeV_blu";
   //string dataFileName = "pp_ANvst_s013.7_Hjet_2009_100GeV_yel";
   //string dataFileName = "pp_ANvst_s013.7_Hjet_2012_100GeV";
   //string dataFileName = "pp_ANvst_s019.4_E704";
   //string dataFileName = "pp_ANvst_s021.7_Hjet_2011_250GeV";
   string dataFileName = "pp_ANvst_s021.9_Hjet_2012_255GeV";
   //string dataFileName = "pp_ANvst_s200.0_pp2pp_2003";
   //string dataFileName = "pp_ANvst_s200.0_pp2pp_2003_toterr";
   //string dataFileName = "pp_ANvst_s200.0_pp2pp_2009";
   //string dataFileName = "pp_ANvst_s200.0_pp2pp_2009_toterr";


   //TGraphErrors *grANStat = new TGraphErrors(6, Data_t, DataAN, Err_t, DataErr);
   TGraphErrors *grANStat  = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");
   TGraphErrors *grANSyst  = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");
   TGraphErrors *grANSystP = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");
   TGraphErrors *grANSystM = new TGraphErrors(("/eic/u/dsmirnov/cnipol_results/" + dataFileName).c_str(), "%lg %lg %lg");

   // Add systematic errors
   Double_t x, y, y_systM, y_systP;
   Double_t xe, ye, ye_syst;
   Double_t xmin, xmax, ymin, ymax;

   for (UInt_t i=0; i!=grANStat->GetN(); i++) {
      grANStat->ComputeRange(xmin, ymin, xmax, ymax);
      grANStat->GetPoint(i, x, y);
      xe = grANStat->GetErrorX(i);
      ye = grANStat->GetErrorY(i);

      //xe = TMath::Sqrt( xe*xe + );
      ye_syst = y*0.03;
      ye_syst = TMath::Sqrt( ye*ye + ye_syst*ye_syst);        // uncorrelated across -t syst. error
      y_systM = y - y*0.04 - 0.002*(x - xmin)/(xmax - xmin);  // correlated across -t syst. error
      y_systP = y + y*0.04 + 0.002*(x - xmin)/(xmax - xmin);  // correlated across -t syst. error

      //grANStat->SetPointError(i, xe, ye);
      grANSyst->SetPointError(i, xe, ye_syst);
      //grANSyst->SetPoint(i, x, y);
      grANSystM->SetPoint(i, x, y_systM);
      //grANSystM->SetPointError(i, xe, ye);
      grANSystP->SetPoint(i, x, y_systP);
      //grANSystP->SetPointError(i, xe, ye);
   }


   //|t| = ((1E-2)-(2E-4))/ (Double_t) 64;
   TF1 *funcModelAN = new TF1("funcModelAN", modelAN, 1E-6, 4E-1, 2);

   TCanvas *canvas1 = new TCanvas("canvas1", "canvas1", 100, 100, 800, 480);

   //canvas1->SetBottomMargin(0.11);
   //canvas1->SetLeftMargin(0.13);
   canvas1->SetLogx();

   //TH2C* frame1 = new TH2C("frame1", "frame1", 50, 0, 0.05, 100, 0, 0.08);
   TH2C* frame1 = new TH2C("frame1", "frame1", 50, 0, 0.05, 100, 0, 0.06);

   frame1->GetXaxis()->SetTitle("-t, (GeV/c)^{2}");
   frame1->GetYaxis()->SetTitle("A_{N}");
   frame1->SetStats(kFALSE);
   frame1->Draw();

   funcModelAN->SetRange(1E-3, 5E-2);
   funcModelAN->SetParNames("Re r_{5}", "Im r_{5}");
   //funcModelAN->FixParameter(0, 1);
   funcModelAN->SetParameter(0, 0); // Rer5
   funcModelAN->SetParameter(1, 0); // Imr5
   //funcModelAN->SetParameter(2, 1); // Norm
   //funcModelAN->FixParameter(3, 0); // Imr2
   //funcModelAN->FixParameter(4, 0); // Rer2
   //funcModelAN->SetParLimits(2, 0.99, 1.01); // Norm

   funcModelAN->SetLineColor(kBlue);
   //funcModelAN->SetLineWidth(2);
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

   //TLine l1;
   //l1.SetLineWidth(1);
   //l1.DrawLine(0, 0, 0.05, 0); //iimax,maxh/4,iimax,0

   //TLegend* legend = new TLegend(0.82, 0.3, 0.99, 0.5);
   ////legend->SetTextSize(0.04);
   //legend->AddEntry(grANStat, "Data", "p");
   //legend->AddEntry(funcModelAN, "fit", "l");
   //legend->SetFillColor(0);//white filled box
   //legend->Draw("");

   TF1* funcModelAN_noflip = new TF1();
   funcModelAN->Copy(*funcModelAN_noflip);
   funcModelAN_noflip->SetParameter(0, 0); // Imr5
   funcModelAN_noflip->SetParameter(1, 0); // Rer5
   //funcModelAN_noflip->SetParameter(2, 1); // norm
   funcModelAN_noflip->SetLineColor(kBlack);
   funcModelAN_noflip->SetLineWidth(2);
   funcModelAN_noflip->SetLineStyle(1);
   funcModelAN_noflip->Draw("same");

   canvas1->SaveAs((dataFileName + "_AN.png").c_str());

   // contour plot Im_r5 vs. Re_r5
   TCanvas *canvas2 = new TCanvas("canvas2", "canvas2", 120, 120, 640, 480);
   //canvas2->SetBottomMargin(0.15);
   //canvas2->SetLeftMargin(0.2);

   double cRer5[1], cImr5[1];
   double cRer5_err[1], cImr5_err[1];

   cRer5[0]     = fitResStat->Value(0);    //funcModelAN->GetParameter(0);
   cImr5[0]     = fitResStat->Value(1);    //funcModelAN->GetParameter(1);
   cRer5_err[0] = fitResStat->ParError(0); //0; //fitTheoAn2->GetParError(2);
   cImr5_err[0] = fitResStat->ParError(1); //0; //fitTheoAn2->GetParError(1);

   TGraphErrors *gr_r5 = new TGraphErrors(1, cRer5, cImr5, cRer5_err, cImr5_err);
   gr_r5->SetMarkerStyle(kFullDotLarge);
   gr_r5->SetMarkerSize(0.2);
   //gr_r5->SetLineWidth(2);
   gr_r5->SetMarkerColor(kBlue);

   //// inset ///////////////////////////////////////////////////////////////
   //TPad *npad = new TPad("npad", "", 0.631, 0.539, 0.931, 0.939);		//
   //npad->Draw("same");													//
   //npad->SetFillStyle(0);												//
   //npad->SetFillColor(2);// fill inside of "inset" plot					//
   //npad->cd();															//
   //
   //gPad->SetGridx(1);  gPad->SetGridy(1);								//
   //
   // -- make enough space so that axis titles are not cut off				//
   //gPad->SetBottomMargin(0.25);											//
   //gPad->SetLeftMargin(0.25);											//
   //////////////////////////////////////////////////////////////////////////

   // -- make enough space so that axis titles are not cut off
   //int InsetMin = -0.1;
   //int InsetMax = 0.02;

   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.2, 0.2, 100, -0.4, 0.4); //E704
   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.2, 0.4, 100, -0.4, 0.2);       // H-jet, 24 GeV  like in paper
   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.10, 0.10, 100, -0.15, 0.15);   // H-jet, 2012: 24 GeV, 255 GeV
   TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.05, 0.05, 100, -0.15, 0.15);   // H-jet, 2012: 24 GeV, 255 GeV
   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.05, 0.05, 100, -0.07, 0.07);   // H-jet: 100 GeV, 255 GeV, pp2pp: 2009
   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.04, 0.04, 100, -0.12, 0.08);   // H-jet: 100 GeV
   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.2, 0.15, 100, -3, 2);         //pp2pp 2003 
   //TH2C* frame2 = new TH2C("frame2", "frame2", 100, -0.009, 0.009, 100, -0.07, 0.07); //pp2pp 2009  like in paper

   frame2->GetXaxis()->SetTitle("Re r_{5}");
   frame2->GetYaxis()->SetTitle("Im r_{5}");
   //frame2->GetYaxis()->SetTitleOffset(1.5);
   //frame2->GetXaxis()->SetTitleOffset(1.2);
   //frame2->GetYaxis()->SetLabelOffset(0.02);
   //frame2->GetXaxis()->SetLabelOffset(0.03);
   frame2->SetStats(kFALSE);
   frame2->GetXaxis()->SetNdivisions(505);
   frame2->GetYaxis()->SetNdivisions(505);//sigma-123
   //frame2->GetYaxis()->SetNdivisions(-4);//sigma-1
   //frame2->GetXaxis()->CenterTitle();
   //frame2->GetYaxis()->CenterTitle();
   frame2->Draw();

   //Get contour for ERRDEF=1
   gMinuit->SetErrorDef(1);// 1sigma

   //get first contour for Re_r5(par2) versus Im_r5(par1)
   //TGraph *gr12_1 = (TGraph*) gMinuit->Contour(300, 0, 1); //fineness, param1(Rer5), param2(Imr5)
   //gr12_1->SetLineColor(kBlue);
   //gr12_1->SetFillColor(7);
   //gr12_1->SetLineWidth(2);
   //gr12_1->Draw("+l");


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

   //number = 0;//Rer5
   //gMinuit->mnerrs(number, eplus, eminus, eparab, gcc);
   //printf("param0 (targetPol) eplus=%10f eminus=%10f eparab=%10f gcc=%10f\n", eplus, eminus, eparab, gcc);

   //number = 3;//Imr2
   //gMinuit->mnerrs(number, eplus, eminus, eparab, gcc);
   //printf("param3 (Imr2) eplus=%10f eminus=%10f eparab=%10f gcc=%10f\n", eplus, eminus, eparab, gcc);

   //number = 4;//Rer2
   //gMinuit->mnerrs(number, eplus, eminus, eparab, gcc);
   //printf("param4 (Rer2) eplus=%10f eminus=%10f eparab=%10f gcc=%10f\n", eplus, eminus, eparab, gcc);

   gMinuit->mnwerr();
   //printf("ccc=%10f\n", ccc);

   // Example illustrating how to draw the n-sigma contour of a Minuit fit.
   // To get the n-sigma contour the ERRDEF parameter in Minuit has to set
   // to n^2. The fcn function has to be set before the routine is called.
   //
   // WARNING!!! This test works only with TMinuit

   //gMinuit->mnmatu(1);

   //Get contour for ERRDEF=2
   //gMinuit->SetErrorDef(4); //2sigma
   //TGraph *gr12_2 = (TGraph*) gMinuit->Contour(300, 0, 1);
   //gr12_2->SetLineColor(kBlack);
   //gr12_2->SetLineWidth(2);
   //gr12_2->Draw("l");

   //Get contour for ERRDEF=3
   //gMinuit->SetErrorDef(9); //3sigma
   //TGraph *gr12_3 = (TGraph*) gMinuit->Contour(300, 0, 1);
   //gr12_3->SetLineColor(kBlack);
   //gr12_3->SetLineWidth(2);
   //gr12_3->Draw("l");

   gr_r5->Draw("P");

   //TLine *lx = new TLine(-0.04, 0.0, 0.04, 0.);//JET-sigma1
   TLine *lx = new TLine(frame2->GetXaxis()->GetXmin(), 0, frame2->GetXaxis()->GetXmax(), 0.);
   //lx->SetLineColor(kBlack);
   //lx->SetLineStyle(1);
   lx->Draw("same");

   //TLine *ly = new TLine(0., -0.06, 0.0, 0.02);//JET-sigma1
   TLine *ly = new TLine(0, frame2->GetYaxis()->GetXmin(), 0, frame2->GetYaxis()->GetXmax());
   //ly->SetLineColor(kBla);
   //ly->SetLineStyle(1);
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
} //}}}


/** */
void f_AN(double cal_t[], double cal_AN[], double rho, double sigma, double Imr5, double Rer5, double scale)
{ //{{{
   int i;
   double mb2GeV_inv2 = 0.389379; // 1GeV-2=0.389mb factor to convert mb to GeV
   double beta = 0;
   double delta = 0.02;
   //double lamda2 = 0.71;//GeV^2
   // double B = 12;//(GeV/c)^2
   //double ganma = 0.5772156;//Euler's constant
   //double light_c = 3E8;//m/s
   double delta_t = ((5E-2) - (1E-4)) / (double)256;
   //double Imr5=0;
   //double Rer5=0;
   //double Imr2 = 0;
   //double Rer2 = 0;


   double sigma_totP = sigma / mb2GeV_inv2;

   double tc = -8 * TMath::Pi() * ALPHA / (sigma_totP * sqrt(1 + rho * rho));

   for (i = 0; i < 256; ++i) {
      cal_t[i] = 0.5E-3 + delta_t * i;

      double tt = -1 * tc / cal_t[i];
      double ttt = tt * tt - 2 * (rho + delta) * tt + (1 + rho * rho) * (1 + beta * beta);
      double ans = tt * ((MAGMOM_PROTON - 1) * (1 - delta * rho) - 2 * (Imr5 - delta * Rer5)) - 2 * (Rer5 - rho * Imr5);

      //ans=tt*((MAGMOM_PROTON-1)*(1-delta*rho+Imr2-delta*Rer2)-2*(Imr5-delta*Rer5))-2*(1+Imr2)*Rer5+2*(rho+Rer2)*Imr5;
      //ans=tt*(MAGMOM_PROTON-1)*(1-delta*rho);
      cal_AN[i] = scale * ans * sqrt(cal_t[i]) / (double)(ttt * MASS_PROTON);
      //printf("cal_t = %lf; cal_AN = %lf\n", cal_t[i], cal_AN[i]);
      //if(i%10==0 && i>0)getchar();
   }
} //}}}


/** */
Double_t modelAN(Double_t *x, Double_t *par)
{ //{{{
   double minus_t = x[0];

   double Rer5 = par[0];
   double Imr5 = par[1];
   //double norm = par[2];
   //double Imr2 = par[3];
   //double Rer2 = par[4];

   double mb2GeV_inv2 = 0.389379;          // 1GeV-2    = 0.389mb

   // sqrt(s) = 6.8 GeV, 24 GeV beam
   //double B          = 11;
   //double rho        = -0.245;             // at sqrt(s) = 6.8 GeV, 24 GeV beam
   ////double rho        = -0.25;             // at sqrt(s) = 6.8 GeV, 24 GeV beam - older
   //double sigma_totP = 38.73;              // mb
   ////double sigma_totP = 38.4;              // mb - older

   // sqrt(s) = 13.7 GeV, 100 GeV beam on H-jet target
   //double B          = 12;
   //double rho        = -0.0835;             // at sqrt(s) = 13.7 GeV, 100 GeV beam on H-jet target
   ////double rho        = -0.08;             // at sqrt(s) = 13.7 GeV, 100 GeV beam on H-jet target - older
   //double sigma_totP = 38.477;              // mb
   ////double sigma_totP = 38.4;              // mb - older

   // sqrt(s) = 19.418 GeV, 200 GeV beam, E704 FNAL
   // These data have to be checked!!!
   //double B          = 12; //????
   //double rho        = -0.0286;
   //double sigma_totP = 38.977;

   // sqrt(s) = 21.7 GeV, 250 GeV beam on H-jet target
   // These values have to be checked!!!
   //double B          = 12;
   //double rho        = -0.0136;
   //double sigma_totP = 39.221;

   // sqrt(s) = 21.9 GeV, 255 GeV beam on H-jet target
   double B          = 12;
   double rho        = -0.0123;
   double sigma_totP = 39.245;

   // sqrt(s) = 200 GeV, pp2pp
   //double B          = 16.3;
   //double rho        = 0.1278;
   //double sigma_totP = 51.79;


   double Lambda2 = 0.71;      //GeV^2
   double gamma   = 0.5772156;
   double kappa   = MAGMOM_PROTON - 1; // anomalous magnetic moment of proton

   sigma_totP /= mb2GeV_inv2;

   //double tc    = -8 * TMath::Pi() * ALPHA / (sigma_totP * sqrt(1 + rho * rho));
   double delta = ALPHA*TMath::Log( 2./ minus_t / (B + 8./Lambda2) ) - ALPHA*gamma ;
   //double delta = 0.02;
   double tc    = -8 * TMath::Pi() * ALPHA / sigma_totP ;
   double tt    = -1 * tc / minus_t;
   double denom = tt * tt - 2 * (rho + delta) * tt + (1 + rho * rho);     // * (1 + beta * beta);
   //double nom = tt * (kappa * (1 - delta * rho + Imr2 - delta * Rer2) - 2 * (Imr5 - delta * Rer5)) - 2 * (1 + Imr2) * Rer5 + 2 * (rho + Rer2) * Imr5;
   double nom   = tt * (kappa * (1 - delta * rho) + 2 * (delta * Rer5 - Imr5)) - 2 * (Rer5 - rho * Imr5);

   return TMath::Sqrt(minus_t) * nom / MASS_PROTON / denom;
} //}}}


/** */
TFitResultPtr FitGraph(TGraph *gr, TF1 *func, TEllipse *ell)
{ //{{{
   Info("FitGraph", "\n********************************************************************************\n");

   TFitResultPtr fitRes = gr->Fit(func, "R S B");

   double Rer5     = func->GetParameter(0);
   double Rer5_err = func->GetParError(0);
   double Imr5     = func->GetParameter(1);
   double Imr5_err = func->GetParError(1);
   //double norm     = func->GetParameter(2);
   //double normErr  = func->GetParError(2);
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

   //par1.e *= 3;
   //par2.e *= 3;
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
   //printf("Norm   = % 10.8lf +/- % 10.8lf\n", norm, normErr);
   printf("\n");

   //double cal_AN[256];
   //double cal_t[256];
   //double rho   = -0.08;
   //double scale =  1;
   //double B     =  12;
   //double sigma =  38.4;
   //f_AN(cal_t, cal_AN, rho, B, sigma, 0, 0, scale); // write 256 points
   //f_AN(cal_t, cal_AN, rho, sigma, 0, 0, scale); // write 256 points
   //TGraph *gr = new TGraph(256, cal_t, cal_AN); //r5=0 N=1, rho=-0.08
   return fitRes;
} //}}}
