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

const Int_t N=200;

class Template
{

private:
    Float_t RunID[N],Dl[N],DlE[N],ReadRate[N],WCM[N],SpeLumi[N],NBunch[N]; 
    Float_t dx[N],dy[N];

public:
    Int_t Plot(Int_t, Int_t);
    Int_t Template();
    Int_t GetData(Char_t * DATAFILE);


}; // end-class Template



//
// Class name  : Template
// Method name : GetData(Char_t *DATAFILE)
//
// Description : Get Data from DATAFILE
// Input       : Char_t *filename
// Return      : Number of lines read
//
Int_t
Template::GetData(Char_t * DATAFILE){
                 
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
        
        fin >> RunID[i] >> Dl[i] >> DlE[i] >> ReadRate[i] >> WCM[i] >> SpeLumi[i] >> NBunch[i]
            >> dum[0] >> dum[1] >> dum[2] >> dum[3] >> dum[4] 
            >> dum[5] >> dum[6] >> dum[7] >> dum[8] >> dum[9];

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
// Class name  : Template
// Method name : Plot(Int_t Mode)
//
// Description : Plot data
// Input       : Int_t Mode, Int_t ndata
// Return      : 
//
Int_t
Template::Plot(Int_t Mode, Int_t ndata){

    Int_t Mtyp=20;
    Int_t Color=4;

    TGraphErrors* tgae = new TGraphErrors(ndata, RunID, Dl, dx, DlE);
    tgae -> SetMarkerStyle(Mtyp);
    tgae -> SetMarkerSize(1.5);
    tgae -> SetLineWidth(2);
    tgae -> SetMarkerColor(Color);
    tgae -> Draw("P");

    return 0;

} // end-of-Plot()


//
// Class name  : Template
// Method name : Template
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
Template::Template()
{

    Float_t xmin=7475.0;
    Float_t xmax=7475.05;
    Float_t ymin=20;
    Float_t ymax=80;

    Char_t xtitle[100]="Run ID";
    Char_t ytitle[100]="DeadLayer Thickness [ug/cm^2]";

    // postscript file
    Char_t psfile[100];
    sprintf(psfile,"ps/dLmonitor.ps");
    TPostScript ps(psfile,112);

    // Cambus Setup
    TCanvas *CurC = new TCanvas("CurC","",1);
    CurC -> SetGridy();
    Char_t title[100];
    sprintf(title,"DeadLayer History ");

    TH2D* frame = new TH2D("frame",title, 10, xmin, xmax, 10, ymin, ymax);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle(xtitle);
    frame -> GetYaxis()->SetTitle(ytitle);
    frame -> Draw();

    Char_t DATAFILE[256]="dlayer/dl.summary.dat";
    Int_t ndata = GetData(DATAFILE);
    Plot(1, ndata);
    CurC -> Update();

    cout << "ps file : " << psfile << endl;
    ps.Close();
    
    //  frame->Delete();

    return 0;

}



