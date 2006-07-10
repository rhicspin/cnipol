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
  Int_t ControlCenter(TCanvas *CurC, TPostScript *ps, Char_t *Energy);

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
        
      fin >> index[i] >> T[i] >> AN[i];
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

    Int_t Color=2;

    TGraphErrors* tgae = new TGraphErrors(ndata, T, AN, dx, dy);
    tgae -> SetLineColor(Color);
    tgae -> SetLineWidth(2);
    tgae -> Draw("C");

    return 0;

} // end-of-Plot()


//
// Class name  : A_N
// Method name : ControlCenter
//
// Description : 
// Input       : 
// Return      : 
//

Int_t 
A_N::ControlCenter(TCanvas *CurC, TPostScript *ps, Char_t * Energy)
{
  // frame dimensions
  Float_t xmin=0.;
  Float_t xmax=1200.;
  Float_t tmin=2*xmin*11.36e-6;
  Float_t tmax=2*xmax*11.36e-6;
  Float_t ymin=0.0;
  Float_t ymax=0.05;

    Char_t ytitle[100]="A_N";
    Char_t xtitle[100]="Kinetic Energy [keV]";

    Char_t title[100];
    sprintf(title,"A_N(%s[GeV])",Energy);

    TH2D* frame = new TH2D("frame",title, 10, xmin, xmax, 10, ymin, ymax);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle(xtitle);
    frame -> GetYaxis()->SetTitle(ytitle);
    frame -> Draw();

    // -t axis on the top of plot
    TF1 *f1=new TF1("f1","x",tmin,tmax);
    TGaxis *A1 = new TGaxis(xmin,ymax,xmax,ymax,"f1",510,"-");
    A1->SetTitle("-t [(GeV/c)^2]");
    A1->Draw("same");

    Char_t DATAFILE[256];
    sprintf(DATAFILE,"%s/phys/A_N_%sGeV.dat",gSystem->Getenv("SHAREDIR"),Energy);
    Int_t ndata = GetData(DATAFILE);
    Plot(1, ndata);
    CurC -> Update();

    return 0;
}


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



    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/A_N.ps");
    TPostScript * ps = new TPostScript(psfile,112);

    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();

    ControlCenter(CurC, ps, "24");
    ps->NewPage();
    ControlCenter(CurC, ps, "100");


    cout << "ps file : " << psfile << endl;
    ps->Close();
    
    //  frame->Delete();

    return 0;

}



