
const Int_t N=200;

struct StructBeam {
  Int_t ndata;
  Float_t fillID[N];
  Float_t pol_STAR[N];
  Float_t pol_PHENIX[N];
  Float_t dpol[N];
  Float_t diff[N];
  Float_t ratio[N];
  Float_t dum[N];
  TH1F * hratio_unweighted;
  TH1F * hratio_weighted;
  TH2F * fillID_vs_pol;
  Int_t Color;
} blue, yellow;


Int_t GetData(Char_t *filename, StructBeam &beam){


  ifstream fin;
  fin.open(filename, ios::in);
  
  if (fin.fail()){
    cerr << "ERROR: " << filename << " doesn't exist. Force quit." << endl;
    exit(-1);
  }
  
  Int_t ch=0, i=0;
  while ( ( ch= fin.peek()) != EOF) {

    beam.dum[i]=0;
    fin >> beam.fillID[i] >> beam.pol_STAR[i] >> beam.pol_PHENIX[i] >> beam.dpol[i] 
	>> beam.diff[i] >> beam.ratio[i];

    // fill histograms
    beam.hratio_unweighted->Fill(beam.ratio[i]);
    if(beam.dpol[i]) beam.hratio_weighted->Fill(beam.ratio[i],1/beam.dpol[i]);

    i++;

  }

  return --i; 

}


Int_t 
Initiarize(StructBeam &beam){

  beam.hratio_unweighted = new TH1F("h_ratio","(P_STAR-P_PHENIX)/P_STAR*100", 30, -10, 10);
  beam.hratio_unweighted -> SetLineColor(beam.Color);
  beam.hratio_unweighted -> SetLineStyle(2);
  beam.hratio_weighted = new TH1F("h_ratio","(P_STAR-P_PHENIX)/P_STAR*100", 30, -10, 10);
  beam.hratio_weighted -> SetFillColor(beam.Color);
  beam.hratio_weighted -> GetYaxis() -> SetTitle("Entries weighted by DeltaP");

  beam.fillID_vs_pol = new TH2F("fillID_vs_pol","FillID vs. Pol", 100, 6920, 7330, 100, 35, 70);
  beam.fillID_vs_pol -> GetYaxis() -> SetTitle("Polarization[%]");
  beam.fillID_vs_pol -> GetXaxis() -> SetTitle("Fill ID");

  return 0;

}

Int_t Plot2D(StructBeam &beam){
  
  gStyle->SetOptStat(kFALSE);
  TLegend * lg = new TLegend(0.6,0.75,0.88,0.88);

  beam.fillID_vs_pol->Draw("");

  TGraphErrors * tg_STAR = new TGraphErrors(beam.ndata, beam.fillID, beam.pol_STAR, beam.dum, beam.dpol);
  tg_STAR -> SetMarkerStyle(20);
  tg_STAR -> SetMarkerColor(beam.Color);
  tg_STAR ->Draw("same,P");
  lg->AddEntry(tg_STAR,"STAR");

  TGraphErrors * tg_PHENIX = new TGraphErrors(beam.ndata, beam.fillID, beam.pol_PHENIX, beam.dum, beam.dpol);
  tg_PHENIX -> SetMarkerStyle(20);
  tg_PHENIX -> SetMarkerColor(23);
  tg_PHENIX ->Draw("same,P");
  lg->AddEntry(tg_PHENIX,"PHENIX & BRAHMS");

  lg->Draw("same");

  return 0;
}


Int_t 
PHENIX_STAR(){

  TPostScript * ps = new TPostScript("ps/PHENIX_STAR.ps",112);
  TCanvas * C = new TCanvas("C");

  blue.Color = 4;  yellow.Color = 95;

  Char_t filename[100];

  // blue
  sprintf(filename,"summary/PHENIX_STAR_blue.dat");
  Initiarize(blue);
  blue.ndata = GetData(filename, blue);
  blue.hratio_weighted->Draw(""); ps->NewPage();
  Plot2D(blue); C->Update(); ps->NewPage();

  // yellow
  sprintf(filename,"summary/PHENIX_STAR_yellow.dat");
  Initiarize(yellow);
  yellow.ndata = GetData(filename, yellow);
  yellow.hratio_weighted->Draw(""); ps->NewPage();
  Plot2D(yellow); C->Update();

  ps->Close();

  return 0;
}
