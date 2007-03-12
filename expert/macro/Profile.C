
Bool_t reject;
const Int_t N=30;

struct StructProfile {
  Int_t ndata;
  Float_t RunID[N];
  Float_t P[N];
  Float_t dP[N];
  Float_t rate[N];
  Float_t rate_norm[N];
  Float_t max_rate;
  Float_t time[N];
  Float_t yeild[N];
  Float_t wcm_sum[N];
  Float_t beam_center;
  Float_t xpos[N];
  Float_t xpos_norm[N];
  Float_t ex[N];
  Char_t hname[100];
  Char_t psfile[100];
  Float_t Chi2NDF[N];
  Int_t Threshold;
  Int_t Color;
  struct StructFitResults{
    Float_t Const;
    Float_t Mean;
    Float_t Sigma;
  } pol, R;
} yellow[2], Blue;

yellow[0].Color = yellow[1].Color = 95;
Blue.Color = 4;

yellow[0].beam_center  = 161.3;
yellow[1].beam_center  = 160.4;
Blue.beam_center       = 118.87;

yellow[0].max_rate     = 0.05307;
yellow[1].max_rate     = 0.03977;
Blue.max_rate          = 0.02633;

yellow[0].Threshold    = 4;
yellow[1].Threshold    = 7;
Blue.Threshold         =12;


Int_t GetData(Char_t *filename, StructProfile &profile)
{


  ifstream fin;
  fin.open(filename, ios::in);
  
  if (fin.fail()){
    cerr << "ERROR: " << filename << " doesn't exist. Force quit." << endl;
    exit(-1);
  }
  
  Int_t ch=0, i=0;
  while ( ( ch= fin.peek()) != EOF) {
    
    fin >> profile.RunID[i] >> profile.P[i] >> profile.dP[i] >> profile.rate[i] >> profile.time[i] >> profile.yeild[N]
	>> profile.wcm_sum[i] >> profile.xpos[i];

    profile.xpos[i] /= 10;
    profile.xpos_norm[i] = profile.xpos[i] - profile.beam_center;
    profile.rate_norm[i] = profile.rate[i] / profile.max_rate;
    profile.ex[i]=0;
    i++;

  }
  
  --i;
  fin.close();

  return i;

}




Float_t 
PolarizationProfile(StructProfile &profile, Int_t ndata, Int_t Color, Int_t FitMode){

  Float_t Chi2DOF = 0;
  Int_t MStyle = FitMode ? 31 : 20;
  Color = FitMode ? 1 : Color;

  TGraphErrors * Pol = new TGraphErrors(ndata, profile.xpos_norm, profile.P, profile.ex, profile.dP);
  Pol -> SetTitle(profile.hname);
  Pol -> SetMarkerColor(Color);
  Pol -> SetMarkerStyle(MStyle);
  Pol -> Draw("P");

  TF1 * f1 = new TF1("f1","pol0");
  TF1 * g1 = new TF1("g1","gaus");
  g1->FixParameter(1,0); // mean=0. force symmetric profile
  // if Dofit, then do polinomial-0 fit
  if (FitMode==1) {
    Pol->Fit(f1,"same");
    Chi2DOF=f1->GetChisquare()/f1->GetNDF();
  } else if (FitMode==2) {
    Pol->Fit(g1,"same");
    Chi2DOF=g1->GetChisquare()/g1->GetNDF();
    profile.pol.Const = g1->GetParameter(0);
    profile.pol.Mean  = g1->GetParameter(1);
    profile.pol.Sigma = g1->GetParameter(2);
  }

  return Chi2DOF ;

}


Double_t rate_vs_xpos(Double_t *x, Double_t par)
{
  return par[3]*sqrt(2*log(par[1]/x));
}

//
// Class name  : 
// Method name : 
//
// Description : 
//             : 
// Input       : 
// Return      : 
//
Int_t 
PolarizationVsRateCorrelation(StructProfile profile, TPostScript * ps){

  Float_t xmin=0.1, xmax=1;
  Char_t title[100], htitle[100];
  sprintf(title,"%s",profile.hname);
  TCanvas * C = new TCanvas("C","",1100, 850);
  ps->NewPage();


  C->Divide(1,3);

  C->cd(1);
  TF1 * rate_vs_xpos = new TF1("rate_vs_xpos","[1]*sqrt(2*log([0]/x))", xmin, xmax);
  sprintf(htitle,"%s : Rate vs. Target Position", title);
  rate_vs_xpos -> SetTitle(htitle);
  rate_vs_xpos -> SetParameters(profile.R.Const, profile.R.Sigma, profile.pol.Const, profile.pol.Sigma);
  rate_vs_xpos -> SetLineColor(profile.Color);
  rate_vs_xpos -> Draw("LA");
  rate_vs_xpos -> GetXaxis()->SetTitle("Rate relative to center");
  rate_vs_xpos -> GetYaxis()->SetTitle("Target position from center [mm]");

  C->cd(2);
  TF1 * corr_abs = new TF1("corr_abs","[2]*exp(-[1]*[1]/[3]/[3]*log([0]/x))", 0.01, 1);
  sprintf(htitle,"%s : Rate vs. Polarization", title);
  corr_abs -> SetTitle(htitle);
  corr_abs -> SetParameters(profile.R.Const, profile.R.Sigma, profile.pol.Const, profile.pol.Sigma);
  corr_abs -> SetLineColor(profile.Color);
  corr_abs -> Draw("LA");
  corr_abs -> GetXaxis()->SetTitle("Rate relative to center");
  corr_abs -> GetYaxis()->SetTitle("P(rate)[%]");

  C->cd(3);
  TF1 * corr_rel = new TF1("corr_rel","(([2]-[2]*exp(-[1]*[1]/[3]/[3]*log([0]/x)))/[2])*100", 0.01, 1);
  sprintf(htitle,"%s : Rate vs. Polarization drop from center (Relative)", title);
  corr_rel -> SetTitle(htitle);
  corr_rel -> SetParameters(profile.R.Const, profile.R.Sigma, profile.pol.Const, profile.pol.Sigma);
  corr_rel -> SetLineColor(profile.Color);
  corr_rel -> Draw("LA");
  corr_rel -> GetXaxis()->SetTitle("Rate relative to center");
  corr_rel -> GetYaxis()->SetTitle("{P(0)-P(rate)}/P(0) [%]");

  C->Update();

  return 0;

}



//
// Class name  : 
// Method name : MakePlot(StructProfile &profile, TPostScript * ps, Int_t k, Int_t FitMode)
//
// Description : Make Plots Polarization and Beam Intensity Profiles
//             : FitMode : 0  - no fit
//             : FitMode : 1  - linear polarization profile fit
//             : FitMode : 2  - gaussian polarization profile fit
// Input       : (StructProfile &profile, TPostScript * ps, Int_t k, Int_t FitMode)
// Return      :  StructProfile &profile (rate_norm calculated here)
//
Float_t 
MakePlot(StructProfile &profile, TPostScript * ps, Int_t k, Int_t FitMode)
{

  Float_t Chi2NDF=0;

  Float_t xmin=-2.1;
  Float_t xmax=fabs(xmin);

  gStyle->SetOptFit(111);
  gStyle->SetOptStat(kFALSE);

  TCanvas * C = new TCanvas("C","",1100, 850);
  C->Divide(1,2);

  C->cd(1);
  TH2F * frame = new TH2F("frame","",100, xmin, xmax, 100, -70, 0);
  frame -> SetTitle(profile.hname);
  frame -> GetXaxis()->SetTitle("target position [mm]");
  frame -> GetYaxis()->SetTitle("Polarization [%]");
  frame->Draw();

  PolarizationProfile(profile, profile.ndata, profile.Color, 0);    C->Update();

  if (FitMode>=1) Chi2NDF = PolarizationProfile(profile, profile.ndata-k, 2, FitMode);  C->Update();

  C->cd(2);
  TH2F * frame = new TH2F("frame","",100, xmin, xmax, 100, 0, 1.1);
  frame -> SetTitle(profile.hname);
  frame -> GetXaxis()->SetTitle("target position [mm]");
  frame -> GetYaxis()->SetTitle("Good Carbon Rate in Banana/WCM_sum");

  TF1 * g = new TF1("g","gaus");
  if (FitMode==0){
    TGraph * Norm = new TGraph(profile.ndata, profile.xpos_norm, profile.rate);
    Norm->GetXaxis()->SetTitle("Target Position [mm]");
    Norm->GetYaxis()->SetTitle("Carbon Rate/WCM_sum");
    Norm->SetMarkerColor(profile.Color);
    Norm->SetMarkerStyle(20);
    Norm->Fit(g);
    Norm->Draw("AP");
    profile.max_rate = g->GetParameter(0);
    for (Int_t i=0; i<profile.ndata; i++) profile.rate_norm[i] = profile.rate[i] / profile.max_rate;
  } else {
    frame->Draw();
    TGraph * Rate = new TGraph(profile.ndata, profile.xpos_norm, profile.rate_norm);
    Rate -> SetTitle(profile.hname);
    Rate -> SetMarkerColor(profile.Color);
    Rate -> SetMarkerStyle(20);
    Rate -> Draw("P");
    Rate -> Fit(g);
    profile.R.Const = g->GetParameter(0);
    profile.R.Mean = g->GetParameter(1);
    profile.R.Sigma = g->GetParameter(2);

    Float_t hline=profile.rate_norm[profile.ndata-k-1];
    TLine * t = new TLine(xmin, hline, xmax, hline);
    t->SetLineColor(23);
    t->Draw("same");
  }

  C->Update();
  ps->NewPage();

  return Chi2NDF;

}


Int_t PrintResults(StructProfile profile){

  printf("%s\n",profile.hname);
  for (Int_t k=0; k<profile.ndata; k++){
    printf("%d ", k);
    printf("%8.3f ", profile.RunID[k]);
    printf("%10.1f ", profile.P[k]);
    printf("%10.1f ", profile.dP[k]);
    printf("%10.4f ", profile.rate[k]);
    printf("%10.2f ", profile.rate_norm[k]);
    printf("%10.2f ", profile.xpos[k]);
    printf("%10.2f ", profile.xpos_norm[k]);
    printf("%10.3f ", profile.Chi2NDF[k]);
    printf("\n");
  }

  return 0;

}

Int_t Chi2Distribution(StructProfile profile, TPostScript * ps){

  TCanvas * C0 = new TCanvas();

  TGraph * gr = new TGraph(profile.ndata, profile.rate_norm, profile.Chi2NDF);
  gr -> SetTitle("Chi2/D.o.F dependence on Rate");
  gr -> SetMarkerStyle(20);
  gr -> SetMarkerColor(profile.Color);
  gr -> GetXaxis()->SetTitle("Normaized Rate");
  gr -> GetYaxis()->SetTitle("Chi2/d.o.f");
  gr -> Draw("APL");


  C0->Update(); ps->NewPage();
  
  //  TF1 * Gaus = new TF1("Gaus","gaus");
  //  Gaus->SetParameter(2,10);
  TH1F * PolDist = new TH1F("PolDist",profile.hname, 20, profile.P[0]-20, profile.P[0]+20);
  PolDist->SetFillColor(profile.Color);
  PolDist->GetXaxis()->SetTitle("Polarization [%]");
  PolDist->GetYaxis()->SetTitle("# of entry Weighted by stat error");
  for (Int_t i=0; i<profile.Threshold; i++) PolDist->Fill(profile.P[i], 1/profile.dP[i]);

  PolDist->Fit("gaus");
  C0->Update(); 


  return 0;
}



Int_t DataLoop(StructProfile profile, TPostScript * ps){

  // Make polarization and beam intensity profile plots without drawing fit
  MakePlot(profile, ps, 0, 0); 

  // Make polarization and beam intensity profile plots without drawing fit
  MakePlot(profile, ps, 0, -1); 

  // Linear fit polarization profile
  MakePlot(profile, ps, 0, 1);

  // Gaussian fit polarization profile
  MakePlot(profile, ps, 0, 2);

  PolarizationVsRateCorrelation(profile, ps);




  //  =========================================================================//
  //                Chi2-fits on selected data points                          //
  //  =========================================================================//
  /*
  // Profile linear fit excluding "k"th data and after 
  for (Int_t k=0; k<profile.ndata-1; k++)  profile.Chi2NDF[profile.ndata-1-k] = MakePlot(profile, ps, k, 1);
  Chi2Distribution(profile, ps);

  // Profile linear fit excluding "k"th data and after 
  for (Int_t k=0; k<profile.ndata-3; k++)  profile.Chi2NDF[profile.ndata-1-k] = MakePlot(profile, ps, k, 2);
  Chi2Distribution(profile, ps);
  */


  PrintResults(profile);



  return;

}


Int_t ProfileControl(StructProfile profile){

  TPostScript * ps = new TPostScript(profile.psfile,112);
  ps->NewPage();

  DataLoop(profile, ps);

  cout << "ps file: " << profile.psfile << endl;
  ps->Close();

}



Int_t Profile()
{

  Char_t filename[100];
  Int_t Fill=7133;
  Char_t Beam[10];

  //  gStyle->SetGridStyle(2);

  // Get Yellow 7133
  Fill=7133;
  sprintf(Beam, "Yellow");
  sprintf(yellow[0].hname,"%s - Fill%d", Beam, Fill);
  sprintf(filename,"dat/Profile_%s_%d.dat", Beam, Fill);
  sprintf(yellow[0].psfile,"ps/Profile_%s_%d.ps", Beam, Fill);
  yellow[0].ndata = GetData(filename, yellow[0]);

  // Get Yellow 7151
  Fill=7151;
  sprintf(yellow[1].hname,"%s - Fill%d", Beam, Fill);
  sprintf(filename,"dat/Profile_%s_%d.dat", Beam, Fill);
  sprintf(yellow[1].psfile,"ps/Profile_%s_%d.ps", Beam, Fill);
  yellow[1].ndata = GetData(filename, yellow[1]);
  
  // Get Blue 7151
  sprintf(Beam, "Blue");
  sprintf(Blue.hname,"%s - Fill%d", Beam, Fill);
  sprintf(filename,"dat/Profile_%s_%d.dat", Beam, Fill);
  sprintf(Blue.psfile,"ps/Profile_%s_%d.ps", Beam, Fill);
  Blue.ndata = GetData(filename, Blue);
  

  ProfileControl(yellow[0]); // Yellow - 7131
  ProfileControl(yellow[1]); // Yellow - 7151
  ProfileControl(Blue);      // Blue   - 7151



  return 0;

}
