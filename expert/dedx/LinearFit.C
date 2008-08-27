//==============================================================//
//         LinearFit.C                                          //
//  Original: plotandfit.C (Auth. Osamu Jinnouchi)              //
//  Modified by (I.Nakagawa)                                    //
//  Function: Do Linear Parametrization fit on the energy loss  //
//            in dead-layer. Need to specify the deposited      //
//            energy range, corresponds to regular event        //
//            selection cut. For Run06, it is 400<E<900 keV.    //
//            Corresponding energy range is 204<E<583 keV for   //
//            typical dead-layer thickness 83ug/cm2 for Run06.  //
//            For more details, see 00README.LinearFit          // 
//==============================================================//

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>

#define Debug 0

using namespace std;
gROOT->Reset();

class parplot {
public:
    parplot();
    ~parplot();

    void Plot(Float_t Edep_min, Float_t Edep_max);

};


parplot::parplot(){};
parplot::~parplot(){};

void parplot::Plot(Float_t Edep_min, Float_t Edep_max) {

  // Initialize Fitting Range
  Float_t FitLwEdge[4]={300,400,500,Edep_min};
  Float_t FitUpEdge[4]={900,900,900,Edep_max};

    gStyle->SetGridColor(1);
    gStyle->SetGridStyle(2);
    gStyle->SetPalette(1,0);

    gStyle->SetLabelSize(0.05,"X");
    gStyle->SetLabelSize(0.05,"Y");
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerColor(4);
    gStyle->SetMarkerSize(.4);
    gStyle->SetTitleX(0.1);
    //gStyle->SetTitleH(0.08);
    gStyle->SetTitleW(0.45);

    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps("LinearFit.ps",112);

    gPad->SetLogx(0);
    gPad->SetLogy(0);
    
    gPad->SetBottomMargin(0.15);
    gPad->SetLeftMargin(0.15);
    
    CurC->SetGrid(1);


    TH2F* frame = new TH2F("frame"," ",10, 0., 1000., 10, 0., 1400.);
    //frame->SetTitle("Radial asymmetry component");
    frame -> SetTitle("Energy correlation (5-95 ug/cm2)");
    frame ->GetXaxis()->SetTitle("Deposit Energy (keV)");
    frame ->GetXaxis()->SetTitleSize(0.04);
    frame ->GetXaxis()->SetTitleOffset(1.4);

    frame ->GetYaxis()->SetTitle("Incident Energy (keV)");
    frame ->GetYaxis()->SetTitleSize(0.04);
    frame ->GetYaxis()->SetTitleOffset(1.4);
    
    frame ->SetStats(0);
    frame ->Draw();
 
    Float_t Thick[10];
    Float_t Edead[4][10];
    Float_t Ecoef[4][10];
    

    for (Int_t thick=5; thick<100; thick+=10) {

        cout << " CURRENT STRIP = "<< thick << endl; 

        Int_t indx =  (Int_t)(thick-5)/10;

        Thick[indx] = thick;

        Float_t edep[300];
        Float_t ekin[300];

        // ===================================
        // Reading the Data Points from files
        // ===================================

        ifstream finput;
        Char_t filename[100];
        sprintf(filename,"Integ_dat_moded/output%d.dat",thick);
        finput.open(filename);
	if (finput.fail()) { cerr << "ERROR: " << filename << " cannot be opened." << endl;}

        cout << " NOW READING = "<< filename << endl;  
        
        Int_t idx = 0;
        Float_t y_cood = 0.;   // just to know the final height

        while (!finput.eof()){
            finput >> ekin[idx] >> edep[idx]; 
            if (ekin[idx]<1000.&&ekin[idx]>0.) {y_cood = ekin[idx];};
            idx++;
        }
	
        finput.close();
        
        // ===================================
        // Plot the data points
        // ===================================
        TGraph *tgvar1 = new TGraph(idx-1, edep, ekin);
        tgvar1 -> SetLineColor(4);
        tgvar1 -> Draw("PL"); 

        //TLatex l;
        //Char_t string[10]; sprintf(string,"%d ug/cm2",thick);
        //l.SetTextSize(0.03);
        // l.DrawLatex(1005., y_cood, string);
        
        // ====================================== 
        // Fit with Linear Function
        // ====================================== 

        // Following Energy Ranges 300<E<900, 500<E<900, 400<E<900 
        // do quiet fit, and not make Energy Correlation plot. These are for reference.
        // 300 < E_dep < 900
        TF1 *f2 = new TF1("f2", "[0]+[1]*x", FitLwEdge[0], FitUpEdge[0]);
        tgvar1 -> Fit("f2","E","0", FitLwEdge[0], FitUpEdge[0]);
        Edead[1][indx] = f2 -> GetParameter(0);
        Ecoef[1][indx] = f2 -> GetParameter(1);
        
        // 500 < E_dep < 900
        tgvar1 -> Fit("f2","E","0", FitLwEdge[2], FitUpEdge[2]);
        Edead[2][indx] = f2 -> GetParameter(0);
        Ecoef[2][indx] = f2 -> GetParameter(1);

        // 400 < E_dep < 900
        TF1 *f1 = new TF1("f1", "[0]+[1]*x", FitLwEdge[1], FitUpEdge[1]);
        tgvar1 -> Fit("f1","E","", FitLwEdge[1], FitUpEdge[1]);
        Edead[0][indx] = f1 -> GetParameter(0);
        Ecoef[0][indx] = f1 -> GetParameter(1);
        f1 -> SetLineColor(2);

        // Do fit and make Energy Correlation Plot
        TF1 *f3 = new TF1("f3", "[0]+[1]*x", FitLwEdge[3], FitUpEdge[3]);
        tgvar1 -> Fit("f3","E","0", FitLwEdge[3], FitUpEdge[3]);
        Edead[3][indx] = f3 -> GetParameter(0);
        Ecoef[3][indx] = f3-> GetParameter(1);
        f3 -> SetLineColor(2);
	f3 -> Draw("same");

    }

    tl = new TLine(0,0,1000,1000);
    tl -> Draw();

    CurC->Update();

    //
    //  New Page (Result : function coeficients)
    // 

    Float_t rpar[4][5];
    ps.NewPage();
    CurC -> Clear();

    TGraph *tgedead0 = new TGraph(10, Thick, Edead[0]);
    TGraph *tgedead1 = new TGraph(10, Thick, Edead[1]);
    TGraph *tgedead2 = new TGraph(10, Thick, Edead[2]);
    TGraph *tgedead3 = new TGraph(10, Thick, Edead[3]);
    
    TGraph *tgecoef0 = new TGraph(10, Thick, Ecoef[0]);
    TGraph *tgecoef1 = new TGraph(10, Thick, Ecoef[1]);
    TGraph *tgecoef2 = new TGraph(10, Thick, Ecoef[2]);
    TGraph *tgecoef3 = new TGraph(10, Thick, Ecoef[3]);

    
    CurC -> Divide(1,2);
    
    // Edead Parameter

    CurC -> cd(1);
    TH2F* frame = new TH2F("frame"," ",10, 0., 100., 10, -10., 250.);
    frame -> SetTitle("Constant Term ");
    frame ->GetXaxis()->SetTitle("thickness (ug/cm2)");
    frame ->GetXaxis()->SetTitleSize(0.04);
    frame ->GetXaxis()->SetTitleOffset(1.4);
    
    frame ->GetYaxis()->SetTitle("Edead ");
    frame ->GetYaxis()->SetTitleSize(0.04);
    frame ->GetYaxis()->SetTitleOffset(1.4);
    frame ->SetStats(0);
    frame ->Draw();
 
    gStyle->SetLineWidth(0.3);
    tgedead0 -> SetMarkerStyle(20);
    tgedead0 -> SetMarkerColor(2);
    tgedead0 -> SetMarkerSize(1.);
    tgedead0 -> SetLineWidth(.5);
    tgedead1 -> SetMarkerStyle(21);
    tgedead1 -> SetMarkerColor(4);
    tgedead1 -> SetMarkerSize(1.);
    tgedead2 -> SetMarkerStyle(22);
    tgedead2 -> SetMarkerColor(3);
    tgedead2 -> SetMarkerSize(1.);
    tgedead3 -> SetMarkerStyle(23);
    tgedead3 -> SetMarkerColor(1);
    tgedead3 -> SetMarkerSize(1.);

    tgedead0 -> Fit("pol2","E", "",0., 100.);
    tgedead1 -> Fit("pol2","E", "", 0., 100.);
    tgedead2 -> Fit("pol2","E", "", 0., 100.);
    tgedead3 -> Fit("pol2","E", "", 0., 100.);

    TF1* fitfun = (TF1*)tgedead0 -> GetFunction("pol2");
    rpar[0][0] = fitfun -> GetParameter(0);
    rpar[0][1] = fitfun -> GetParameter(1);
    rpar[0][2] = fitfun -> GetParameter(2);
    fitfun -> SetLineWidth(.5);

    TF1* fitfun = (TF1*)tgedead1 -> GetFunction("pol2");
    rpar[1][0] = fitfun -> GetParameter(0);
    rpar[1][1] = fitfun -> GetParameter(1);
    rpar[1][2] = fitfun -> GetParameter(2);
    fitfun -> SetLineWidth(.5);

    TF1* fitfun = (TF1*)tgedead2 -> GetFunction("pol2");
    rpar[2][0] = fitfun -> GetParameter(0);
    rpar[2][1] = fitfun -> GetParameter(1);
    rpar[2][2] = fitfun -> GetParameter(2);
    fitfun -> SetLineWidth(.5);

    TF1* fitfun = (TF1*)tgedead3 -> GetFunction("pol2");
    rpar[3][0] = fitfun -> GetParameter(0);
    rpar[3][1] = fitfun -> GetParameter(1);
    rpar[3][2] = fitfun -> GetParameter(2);
    fitfun -> SetLineWidth(.5);

    tgedead0 -> Draw("P");
    tgedead1 -> Draw("P");
    tgedead2 -> Draw("P");
    tgedead3 -> Draw("P");
    
    leg = new TLegend(0.5, 0.2, 1.0, .5);
    leg -> SetTextSize(0.06);
    leg -> SetHeader("");
    Char_t comment[200];
    sprintf (comment, "300-900keV : p0=%6.4f p1=%6.4f p2=%6.5f ",
             rpar[1][0],rpar[1][1],rpar[1][2]);
    leg -> AddEntry(tgedead1, comment, "p");

    sprintf (comment, "400-900keV : p0=%6.4f p1=%6.4f p2=%6.5f ",
             rpar[0][0],rpar[0][1],rpar[0][2]);
    leg -> AddEntry(tgedead0, comment, "p");

    sprintf (comment, "500-900keV : p0=%6.4f p1=%6.4f p2=%6.5f ",
             rpar[2][0],rpar[2][1],rpar[2][2]);
    leg -> AddEntry(tgedead2, comment, "p");

    sprintf (comment, "%d-%dkeV : p0=%6.4f p1=%6.4f p2=%6.5f ",
             Int_t(Edep_min), Int_t(Edep_max), rpar[3][0],rpar[3][1],rpar[3][2]);
    leg -> AddEntry(tgedead3, comment, "p");

    leg -> Draw();

    



    // Ecoef Parameter
 
    CurC -> cd(2);
    TH2F* frame = new TH2F("frame"," ",10, 0., 100., 10, 0.9, 1.5);
    frame -> SetTitle("Slope Term ");
    frame ->GetXaxis()->SetTitle("thickness (ug/cm2)");
    frame ->GetXaxis()->SetTitleSize(0.04);
    frame ->GetXaxis()->SetTitleOffset(1.4);
    
    frame ->GetYaxis()->SetTitle("Ecoef ");
    frame ->GetYaxis()->SetTitleSize(0.04);
    frame ->GetYaxis()->SetTitleOffset(1.4);
    frame ->SetStats(0);
    frame ->Draw();
 
    tgecoef0 -> SetMarkerStyle(20);
    tgecoef0 -> SetMarkerColor(2);
    tgecoef0 -> SetMarkerSize(1.);
    tgecoef0 -> Draw("P");
    tgecoef1 -> SetMarkerStyle(21);
    tgecoef1 -> SetMarkerColor(4);
    tgecoef1 -> SetMarkerSize(1.);
    tgecoef1 -> Draw("P");
    tgecoef2 -> SetMarkerStyle(22);
    tgecoef2 -> SetMarkerColor(3);
    tgecoef2 -> SetMarkerSize(1.);
    tgecoef2 -> Draw("P");
    tgecoef3 -> SetMarkerStyle(23);
    tgecoef3 -> SetMarkerColor(1);
    tgecoef3 -> SetMarkerSize(1.);
    tgecoef3 -> Draw("P");
        
    tgecoef0 -> Fit("pol1","E", "",0., 100.);
    tgecoef1 -> Fit("pol1","E", "",0., 100.);
    tgecoef2 -> Fit("pol1","E", "",0., 100.);
    tgecoef3 -> Fit("pol1","E", "",0., 100.);

    TF1* fitfun = (TF1*)tgecoef0 -> GetFunction("pol1");
    rpar[0][3] = fitfun -> GetParameter(0);
    rpar[0][4] = fitfun -> GetParameter(1);
    fitfun -> SetLineWidth(.5);

    TF1* fitfun = (TF1*)tgecoef1 -> GetFunction("pol1");
    rpar[1][3] = fitfun -> GetParameter(0);
    rpar[1][4] = fitfun -> GetParameter(1);
    fitfun -> SetLineWidth(.5);

    TF1* fitfun = (TF1*)tgecoef2 -> GetFunction("pol1");
    rpar[2][3] = fitfun -> GetParameter(0);
    rpar[2][4] = fitfun -> GetParameter(1);
    fitfun -> SetLineWidth(.5);

    TF1* fitfun = (TF1*)tgecoef3 -> GetFunction("pol1");
    rpar[3][3] = fitfun -> GetParameter(0);
    rpar[3][4] = fitfun -> GetParameter(1);
    fitfun -> SetLineWidth(.5);

    leg = new TLegend(0.6, 0.7, 1.0, 1.0);
    leg -> SetTextSize(0.06);
    leg -> SetHeader("");
    Char_t comment[200];
    sprintf (comment, "300-900keV : p0=%6.4f p1=%6.4f  ",
             rpar[1][3],rpar[1][4]);
    leg -> AddEntry(tgedead1, comment, "p");

    sprintf (comment, "400-900keV : p0=%6.4f p1=%6.4f ",
             rpar[0][3],rpar[0][4]);
    leg -> AddEntry(tgedead0, comment, "p");

    sprintf (comment, "500-900keV : p0=%6.4f p1=%6.4f ",
             rpar[2][3],rpar[2][4]);
    leg -> AddEntry(tgedead2, comment, "p");

    sprintf (comment, "%d-%dkeV : p0=%6.4f p1=%6.4f ",
             Int_t(Edep_min), Int_t(Edep_max), rpar[3][3],rpar[3][4]);
    leg -> AddEntry(tgedead3, comment, "p");
    leg -> Draw();

    /*    
    for (Int_t i=0; i< 10; i++) {
        cout << Thick[i] << " "
             << Edead[0][i] << " " << Ecoef[0][i] << " "
             << Edead[1][i] << " " << Ecoef[1][i] << " "
             << Edead[2][i] << " " << Ecoef[2][i] << " " 
             << Edead[3][i] << " " << Ecoef[3][i] << " " 
             << endl;
    }
    
    */

    printf("\n\n");
    printf("===================================================================\n");
    printf(" Emin    Emax           Const                slope                 \n");
    printf(" [keV]   [keV]       p0        p1       p0        p1         p2    \n");
    printf("===================================================================\n");
    for (Int_t i=0; i<4 ; i++){
      printf("  %d   ", FitLwEdge[i]);
      printf("  %d   ", FitUpEdge[i]);
      printf(" %8.5f ", rpar[i][0]);
      printf(" %8.5f ", rpar[i][1]);
      printf(" %8.5f ", rpar[i][2]);
      printf(" %8.5f ", rpar[i][3]);
      printf(" %8.5f ", rpar[i][4]);
      printf("\n");
    }

    printf("===================================================================\n");
    printf("\n Update in cnipol/macro/KinFit.C        (Manually)               \n");
    printf("\n");

    ps.Close();

}









