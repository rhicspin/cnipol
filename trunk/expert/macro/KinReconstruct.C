
const Int_t N=200;
Float_t edep[N],t[N],Tkin[N], dx[N], dy[N];

//
// Class name  : KinReconstruct
// Method name : KinReconstruct
//
// Description : Main program
// Input       : 
// Return      : 
//

Int_t 
KinReconstruct()
{

  TFile * rfile = new TFile("7279.005.root");
  t_vs_e_st0->SetMarkerColor(3);
  t_vs_e_st0->Draw();


  ifstream fin;
  fin.open("strip0.dat");
  
  Int_t i;
  while (!fin.eof()) {

    fin >> edep[i] >> t[i] >> Tkin[i] ;
    dx[i] = dy[i] = 0;
    i++;

  }
  
  int nevent=i-1;

  TGraphErrors * tg = new TGraphErrors(nevent, Tkin, t, dx, dy);
  tg -> SetMarkerStyle(20);
  tg -> SetMarkerColor(2);
  tg -> SetMarkerSize(1.0);
  tg -> Draw("same,P");

  
  return 0;

}



