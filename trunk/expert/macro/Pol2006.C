
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <iomanip>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TMinuit.h>

#define Debug 0

const Int_t N=200;

class Pol2006
{

private:
  Float_t Fill[N],Pol[N],dSta[N],dSys[N];
  Float_t dx[N],dy[N];

public:
    Int_t Plot(Int_t, Int_t);
    Int_t Pol2006();
    Int_t GetData(Char_t * DATAFILE);
};


//
// Class name  : Pol2006
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
Pol2006::GetData(Char_t * DATAFILE){
                 
    ifstream fin;
    fin.open(DATAFILE,ios::in);
    if (fin.fail()){
        cerr << "ERROR: " << DATAFILE << " doesn't exist. Force exit." << endl;
        exit(-1);
    }

    Float_t dum[10];
    Int_t i=0;
    Int_t ch=0;
    while (!fin.eof()) {
        
      fin >> Fill[i] >> Pol[i] >> dSta[i] >> dSys[i] ;

      ++i; dx[i]=dy[i]=0; 
      if (i>N-1){
          cerr << "WARNING : input data exceed the size of array " << N << endl;
          cerr << "          Ignore beyond line " << N << endl;
          break;
      } // if-(i>N)

    }// end-of-while(!fin.eof())

    fin.close();
    return i-1;

}

//
// Class name  : Pol2006
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
Pol2006::Plot(Int_t Mode, Int_t ndata){

    Int_t Mtyp=20;
    Int_t Color=4;
    //    Int_t Color=94;

    TGraphErrors* tgae = new TGraphErrors(ndata, Fill, Pol, dx, dSta);
    tgae -> SetMarkerStyle(Mtyp);
    tgae -> SetMarkerSize(1.5);
    tgae -> SetLineWidth(2);
    tgae -> SetMarkerColor(Color);
    tgae -> Draw("P");

    return 0;

} // end-of-Plot()


//
// Class name  : Pol2006
// Method name : Pol2006
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
Pol2006::Pol2006()
{

    Float_t xmin=7580.0;
    Float_t xmax=7980.0;
    Float_t ymin=0.2;
    Float_t ymax=0.8;

    Char_t xtitle[100]="RHIC Fill Number";
    Char_t ytitle[100]="Polarization";

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/Pol2006.ps");
    TPostScript ps(psfile,112);

    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();
    Char_t title[100];
    sprintf(title,"Polarization ");

    TH2D* frame = new TH2D("frame",title, 10, xmin, xmax, 10, ymin, ymax);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle(xtitle);
    frame -> GetYaxis()->SetTitle(ytitle);
    frame -> Draw();

    Char_t DATAFILE[256]="summary/Pol2006_blue_phenix_brahms.dat";
    //    Char_t DATAFILE[256]="summary/Pol2006_yellow_phenix_brahms.dat";
    Int_t ndata = GetData(DATAFILE);
    Plot(1, ndata);
    CurC -> Update();

    cout << "ps file : " << psfile << endl;
    ps.Close();
    
    //  frame->Delete();

    return 0;

}



