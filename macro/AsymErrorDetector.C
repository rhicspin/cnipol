
Int_t 
AsymErrorDetector()
{

  // Root file open
  TFile * rootfile = new TFile("7642.101.root");

  // Cambus Setup
  TCanvas *CurC = new TCanvas("CurC","",1);

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
  asym_bunch_x90 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_bunch_x45 -> Draw(); CurC -> Update(); ps->NewPage();
  asym_bunch_y45 -> Draw(); CurC -> Update(); ps->NewPage();
  bunch_rate     -> Draw(); CurC -> Update(); ps->NewPage();
  rate_vs_bunch  -> Draw("AP"); CurC -> Update(); 

  cout << "ps file : " << psfile << endl;
  ps->Close();

  gSystem->Exec("gv ps/AsymErrorDetctor.ps");

  return 0;

  return ;

}
  
