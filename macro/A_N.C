#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TMinuit.h>
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <iomanip>

#define Debug 0

const Int_t N=50;

class A_N
{

private:
  Float_t index[N],T[N],AN[N];
  Float_t dx[N],dy[N];

public:
    Int_t Plot(Int_t, Int_t);
    Int_t A_N();
    Int_t GetData(Char_t * DATAFILE);


}; // end-class A_N



//
// Class name  : A_N
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
A_N::GetData(Char_t * DATAFILE){
                 
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
        
        fin >> index[i] >> T[i] >> AN[i] 
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
// Class name  : A_N
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
A_N::Plot(Int_t Mode, Int_t ndata){

    Int_t Mtyp=20;
    Int_t Color=4;

    TGraphErrors* tgae = new TGraphErrors(ndata, T, AN, dx, dy);
    tgae -> SetMarkerStyle(Mtyp);
    tgae -> SetMarkerSize(1.5);
    tgae -> SetLineWidth(2);
    tgae -> SetMarkerColor(Color);
    tgae -> Draw("P");

    return 0;

} // end-of-Plot()


//
// Class name  : A_N
// Method name : A_N
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
A_N::A_N()
{

    Float_t xmin=7475.0;
    Float_t xmax=7475.05;
    Float_t ymin=20;
    Float_t ymax=80;

    Char_t ytitle[100]="A_N";
    Char_t xtitle[100]="Kinetic Energy [keV]";

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/A_N.ps");
    TPostScript ps(psfile,112);

    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();
    Char_t title[100];
    sprintf(title,"A_N");

    TH2D* frame = new TH2D("frame",title, 10, xmin, xmax, 10, ymin, ymax);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle(xtitle);
    frame -> GetYaxis()->SetTitle(ytitle);
    frame -> Draw();

    
    Char_t DATAFILE[256];
    sprintf(DATAFILE,"%s/phys/A_N_100GeV.dat",gSystem->Getenv("SAHREDIR"));
    Int_t ndata = GetData(DATAFILE);
    Plot(1, ndata);
    CurC -> Update();

    cout << "ps file : " << psfile << endl;
    ps.Close();
    
    //  frame->Delete();

    return 0;

}



