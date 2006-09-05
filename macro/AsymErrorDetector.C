
Int_t 
AsymErrorDetector()
{

  // Root file open
  TFile * rootfile = new TFile("root/7325.007.root");

  // Cambus Setup
  TCanvas *CurC = new TCanvas("CurC","",1);
  CurC -> SetGridx();

  // postscript file
  Char_t psfile[100];
  sprintf(psfile,"ps/AsymErrorDetctor.ps");
  TPostScript *ps = new TPostScript(psfile,112);

  Asymmetry->cd();
  asym_vs_bunch_x90 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_vs_bunch_x45 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_vs_bunch_y45 -> Draw(); CurC -> Update(); ps->NewPage();

  rootfile->cd();
  ErrDet->cd();
  gStyle->SetOptFit(111);
  asym_bunch_x90 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_bunch_x45 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_bunch_y45 -> Draw(); CurC -> Update(); ps->NewPage();
  //  bunch_rate     -> Draw(); CurC -> Update(); ps->NewPage();
  rate_vs_bunch  -> Draw("AP"); CurC -> Update(); ps->NewPage();

  rootfile->cd();
  Bunch->cd();
  bunch_dist     -> Draw(); CurC -> Update();

  cout << "ps file : " << psfile << endl;
  ps->Close();

  gSystem->Exec("gv ps/AsymErrorDetctor.ps");

  return 0;

  return ;

}
  
