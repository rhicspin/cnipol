
//
// Class name  : Offline
// Method name : Int_t OfflinePol::TimeDecoder(){
//
// Description : Decode Character encoded time to integer time
// Input       : 
// Return      : time in the unit of seconds
//
Int_t 
OfflinePol::TimeDecoder(){

  time.Hour   = atoi(strtok(time.Time,":"));
  time.Minute = atoi(strtok(NULL,":"));
  time.Sec    = atoi(strtok(NULL,":"));

  Int_t time = time.Sec + 60*(time.Minute + 60*(time.Hour + 24*time.Day));

  return time;

}

//
// Class name  : OfflinePol
// Method name : FillByFillAnalysis(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : Fill by Fill Analysis 
// Input       : Int_t Mode, Int_t RUN, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : (Int_t)Number of Fills
//
Int_t 
OfflinePol::FillByFill(Int_t Mode, Int_t RUN, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps){

  Int_t nFill = FillByFillAnalysis(RUN, ndata);

  MakeFillByFillPlot(nFill, Mode-1000, ndata, Color, ps);


  return 0;

}


//
// Class name  : OfflinePol
// Method name : FillByFillAnalysis(Int_t ndata)
//
// Description : Stack data into Fill by Fill arrays
// Input       : Int_t RUN, Int_t ndata
// Return      : (Int_t)Number of Fills
//
Int_t 
OfflinePol::FillByFillAnalysis(Int_t RUN, Int_t ndata){

  // Flip Polarization sign for RUN5 (to give the same slope sign with rate)
  Int_t sign = RUN==5 ? -1 : 1;

  // Variables Initiarizations //
  Int_t j=0;
  fill[0].FillID = Int_t(RunID[0]);
  fill[0].RunID[0] = RunID[0];
  fill[0].nRun = 1;

  // Fill-by-Fill loop
  for (Int_t i=0; i<ndata; i++){
    if ( fill[j].FillID != Int_t(RunID[i]) ) {
      j++;  fill[j].FillID = Int_t(RunID[i]);
      fill[j].nRun=1;
      fill[j].Clock0 = Float_t(Time[i]);
    }else{
      if (i!=0) fill[j].nRun++;
    }

    // stack polarizations into fill by fill arrays
    Int_t array_index = fill[j].nRun-1;
    fill[j].Index[array_index]      = fill[j].nRun;
    fill[j].RunID[array_index]      = RunID[i];
    fill[j].P_online[array_index]   = P_online[i] * sign;
    fill[j].dP_online[array_index]  = dP_online[i];
    fill[j].P_offline[array_index]  = P_offline[i] * sign;
    fill[j].dP_offline[array_index] = dP_offline[i];
    fill[j].Clock[array_index]      = (Float_t(Time[i]) - fill[j].Clock0)/3600;
    fill[j].Rate[array_index]       = Rate[i];
    /*
    printf("i=%d ",i);
    printf("j=%d ",j);
    printf("fill[%d].nRun=%d ",j,fill[j].nRun);
    printf("fill[%d].Index[%d]=%d ",j, array_index, fill[j].Index[array_index]);
    printf("fill[%d].RunID[%d]=%8.3f ",j, array_index, fill[j].RunID[array_index]);
    printf("fill[%d].P_offline[%d]=%f",j, array_index, fill[j].P_offline[array_index]);
    printf("fill[%d].Clock[%d]=%5.1f",j, array_index, fill[j].Clock[array_index]);
    printf("\n");
    */

  }

  cout << "Total Number of Fill = " << j << endl;

  return j;

}

//
// Class name  : OfflinePol
// Method name : FillByFillPlot(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : Make Plots Fill by Fill 
// Input       : Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
OfflinePol::MakeFillByFillPlot(Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TPostScript *ps){

  ps->NewPage();

  // Define Chi2 Distribution
  Float_t OverFlow=10;
  TH1F * FillByFillChi2[2];
  FillByFillChi2[0] = new TH1F("FillByFillChi2-0","Fill By Fill Polarization Chi2", 21, 0, OverFlow);
  FillByFillChi2[1] = new TH1F("FillByFillChi2-1","Fill By Fill Polarization Chi2", 21, 0, OverFlow);
  FillByFillChi2[0] ->SetFillColor(Color);
  FillByFillChi2[0] ->SetXTitle("Chi2/D.o.F.");
  FillByFillChi2[0] ->SetYTitle("# of Fill");
  FillByFillChi2[1] ->SetLineColor(3);
  FillByFillChi2[1] ->SetLineStyle(2);

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.08);
  TCanvas * C = new TCanvas("C","fill by fill", 1100, 850);
  C->Divide(2,5);
  
  Int_t j=0;
  for (Int_t k=0; k<nFill; k++) {

      C->cd(j%10+1);
      C->Update();

      if (fill[k].nRun>1) {
	FillByFillPlot(Mode, k, Color);
	for (Int_t i=0; i<2; i++)  fill[k].Chi2[i] = fill[k].Chi2[i]>OverFlow ? OverFlow*0.99 : fill[k].Chi2[i];
	if (fill[k].Chi2[0]) FillByFillChi2[0]->Fill(fill[k].Chi2[0]); // Linear Fit
	if (fill[k].Chi2[0]) FillByFillChi2[1]->Fill(fill[k].Chi2[1]); // Exponential Fit
	if ((j+1)%10==0) ps->NewPage();
	++j;
      }

  }// for (k=0; k<nFill)

  // Plot Chi2 Distribution if Fit Mode
  if (Mode>>3&1) {
    gStyle->SetTitleFontSize(0.05);
    gStyle->SetOptStat(kTRUE);
    TCanvas * C2 = new TCanvas("C2");
    FillByFillChi2[0]->SetStats(1111);
    FillByFillChi2[0]->Draw();
    FillByFillChi2[1]->Draw("same");

    TLegend * lg = new TLegend(0.45,0.7,0.65,0.85);
    lg -> AddEntry(FillByFillChi2[0],"Linear Fit","F");
    lg -> AddEntry(FillByFillChi2[1],"Expo Fit","F");
    lg -> Draw("same");

  }



  return 0;
    
}


//
// Class name  : OfflinePol
// Method name : FillByFillPlot(Int_t Mode, Int_t k)
//
// Description : Make Plots Fill by Fill 
//               Mode Description
//                   Bit 0 - P_Offline 
//                   Bit 1 - P_Online
//                   Bit 2 - Rate
//                   Bit 3 - Fit
//               Mode=7 (Offline,Online,Rate), Mode=9 (Offline,fit)
// Input       : Int_t Mode, Int_t k, Int_t Color
// Return      : 
//
Int_t 
OfflinePol::FillByFillPlot(Int_t Mode, Int_t k, Int_t Color){

  Char_t text[50];
  Float_t xmin, xmax, ymin, ymax;
  Float_t margin=0.05;
  Float_t prefix_range=25; // Vertical Axis (Polariation) Range [%]. 

  // scaled rate default parameters
  r.ymin=0;
  r.ymax=1.6;
  r.Color = 30;
  xmin=-0.5;  
  xmax=10;

  // 2D hist frame
  Char_t htitle[100]; 
  sprintf(htitle,"Fill#%5d",fill[k].FillID);
  if (fill[k].nRun>1) GetScale(fill[k].Clock, fill[k].nRun, margin, xmin, xmax);
  Float_t Range = GetScalePrefixRange(prefix_range, fill[k].P_offline, fill[k].nRun, margin, ymin, ymax);
  TH2F * fillbyfill = new TH2F("fillbyfill", htitle, 10, xmin, xmax, 100, ymin, ymax); 
  fillbyfill->GetXaxis()->SetTitle("Hours from first measurement at store");
  fillbyfill->GetYaxis()->SetTitle("Polarization [%]");

  // Print Vertical Range
  sprintf(text,"V-Range = %d", Range);
  TText * t1 = new TText(0, ymin*1.1, text);
  if (Range>prefix_range+1) {
    fillbyfill->GetYaxis()->SetLabelColor(2);
    fillbyfill->SetAxisColor(2,"Y");
    t1->SetTextColor(2);
  }

  // Draw Histogram and texts
  fillbyfill->Draw();
  if (Mode>>3&0) t1->Draw("same");

  // plot offline data points
  fill[k].meas_vs_P[0] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_offline, fill[k].dum, fill[k].dP_offline);
  fill[k].meas_vs_P[0] -> SetMarkerStyle(20);
  fill[k].meas_vs_P[0] -> SetMarkerColor(Color);
  if (Mode>>0&1) fill[k].meas_vs_P[0] -> Draw("PL");
  
  // plot online data points
  fill[k].meas_vs_P[1] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_online, fill[k].dum, fill[k].dP_online);
  fill[k].meas_vs_P[1] -> SetMarkerStyle(24);
  fill[k].meas_vs_P[1] -> SetMarkerColor(Color);
  fill[k].meas_vs_P[1] -> SetLineStyle(2);
  if (Mode>>1&1)  fill[k].meas_vs_P[1] -> Draw("PL");

  // ------------------------------------------------------------------- // 
  //                        Superpose Rate plot.                         // 
  // ------------------------------------------------------------------- // 
  if (Mode>>2&1){
    TGaxis * A1 = new TGaxis(xmax, ymin, xmax, ymax, r.ymin, r.ymax, 510, "+L");
    A1 -> SetTitle("Event Rate [MHz]");
    A1 -> SetLineColor(r.Color);
    A1 -> SetLabelColor(r.Color);
    A1 -> Draw("same");
      
    // Scale Rate to vertical axis(polarization). fill[k].Rate[i] -> fill[k].sRate[i] 
    for (Int_t i=0; i<fill[k].nRun; i++) 
      fill[k].sRate[i] = (ymax - ymin)/(r.ymax - r.ymin)*(fill[k].Rate[i] - r.ymax) + ymax;
  
    fill[k].meas_vs_Rate = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].sRate, fill[k].dum, fill[k].dum);
    fill[k].meas_vs_Rate -> SetMarkerStyle(22);
    fill[k].meas_vs_Rate -> SetMarkerColor(r.Color);
    fill[k].meas_vs_Rate -> SetLineColor(r.Color);
    fill[k].meas_vs_Rate -> Draw("PL");
  }

  // ------------------------------------------------------------------- // 
  //                              Fitting                                // 
  // ------------------------------------------------------------------- // 
  if (Mode>>3&1){
    Float_t P0, P1;
    gStyle->SetOptFit(111);
    if (fill[k].nRun>2) {

      TLegend * lg = new TLegend(0.15,0.1.5,0.35,0.35);

      // Linear Fit
      TF1 * f1 = new TF1("f1","pol1");
      f1 -> SetLineColor(2);
      fill[k].meas_vs_P[0] -> Fit(f1,"Q");
      fill[k].Chi2[0] = f1->GetChisquare()/(Float_t)f1->GetNDF();
      P0      = f1->GetParameter(0);
      P1      = f1->GetParameter(1);
      lg->AddEntry(f1,"Linear Fit","L");

      // Exponential Fit
      TF1 * f2 = new TF1("f2","expo");
      f2 -> SetLineColor(3);
      f2 -> SetLineStyle(2);
      fill[k].meas_vs_P[0] -> Fit(f2,"QN");
      f2 -> Draw("same");
      fill[k].Chi2[1] = f2->GetChisquare()/(Float_t)f2->GetNDF();
      lg->AddEntry(f2,"Expo Fit","L");

      lg->Draw("same");

      // Test deviation of each data point from the Linear fit
      Int_t j=0;
      for (Int_t i=0; i<fill[k].nRun; i++) {
	Float_t sigma = ((P0 + P1*fill[k].Clock[i]) - fill[k].P_offline[i])/fill[k].dP_offline[i];
	if (fabs(sigma)>3) {
	  fill[k].bad.Clock[j] = fill[k].Clock[i];
	  fill[k].bad.P_offline[j] = fill[k].P_offline[i];
	  printf("%8.3f : %.1f sigma off\n",fill[k].RunID[i], sigma);
	  sprintf(text,"%5.1f",sigma);
	  TText * tx = new TText(fill[k].bad.Clock[j],fill[k].bad.P_offline[j],text);
	  tx -> SetTextSize(0.08);
	  tx -> Draw("same");
	  ++j;
	}
      } 

      if (j) { // this superpose routine is not working 
	TGraph * tg = new TGraph(j, fill[k].bad.Clock, fill[k].bad.P_offline);
	tg -> SetMarkerStyle(25);
	tg -> SetMarkerColor(6);
	tg -> Draw("same");
      }

    } // end-of-if(fill[k].nRun>2)  

    } // end-of-if(Mode>>3&1)

  return 0;

}
