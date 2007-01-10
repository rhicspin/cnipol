
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
// Input       : Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : (Int_t)Number of Fills
//
Int_t 
OfflinePol::FillByFill(Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps){

  Int_t nFill = FillByFillAnalysis(ndata);

  FillByFillPlot(nFill, Mode, ndata, Color, ps);


  return 0;

}


//
// Class name  : OfflinePol
// Method name : FillByFillAnalysis(Int_t ndata)
//
// Description : Stack data into Fill by Fill arrays
// Input       : Int_t ndata
// Return      : (Int_t)Number of Fills
//
Int_t 
OfflinePol::FillByFillAnalysis(Int_t ndata){
  

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
    fill[j].P_online[array_index]   = P_online[i];
    fill[j].dP_online[array_index]  = dP_online[i];
    fill[j].P_offline[array_index]  = P_offline[i];
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


  return j;

}

//
// Class name  : OfflinePol
// Method name : FillByFillPlot(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : Make Plots Fill by Fill 
// Input       : Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : (Int_t)Number of Fills
//
Int_t 
OfflinePol::FillByFillPlot(Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TPostScript *ps){

  ps->NewPage();

  Float_t xmin, xmax, ymin, ymax;
  Float_t margin=0.05;
  Float_t prefix_range=25; // Vertical Axis (Polariation) Range [%]. 
  Char_t text[50];

  // scaled rate 
  struct Rate {
    Float_t ymin;
    Float_t ymax;
    Int_t Color;
  } r;
  r.ymin=0;
  r.ymax=1.6;
  r.Color = 30;

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.08);
  TCanvas * C = new TCanvas("C","fill by fill", 1100, 850);
  C->Divide(2,5);
  
  Char_t htitle[100]; Int_t j=0;
  for (Int_t k=0; k<nFill; k++) {

    xmin=-0.5;  xmax=10;
      
      C->cd(j%10+1);
      // prepare 2D hist frame
      sprintf(htitle,"Fill#%5d",fill[k].FillID);
      if (fill[k].nRun>1) GetScale(fill[k].Clock, fill[k].nRun, margin, xmin, xmax);
      Float_t Range = GetScalePrefixRange(prefix_range, fill[k].P_offline, fill[k].nRun, margin, ymin, ymax);
      TH2F * fillbyfill = new TH2F("fillbyfill", htitle, 10, xmin, xmax, 100, ymin, ymax); 
      fillbyfill->GetXaxis()->SetTitle("Hours from first measurement at store");
      fillbyfill->GetYaxis()->SetTitle("Polarization [%]");

      // Print Vertical Range
      sprintf(text,"V-Range = %d", Range);
      TText * t = new TText(0, ymax*1.1, text);
      if (Range>prefix_range+1) {
	fillbyfill->GetYaxis()->SetLabelColor(2);
	fillbyfill->SetAxisColor(2,"Y");
	t->SetTextColor(2);
      }

      // Draw Histogram and text
      fillbyfill->Draw();
      t->Draw("same");


      // plot offline data points
      fill[k].meas_vs_P[0] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_offline, fill[k].dum, fill[k].dP_offline);
      fill[k].meas_vs_P[0] -> SetMarkerStyle(20);
      fill[k].meas_vs_P[0] -> SetMarkerColor(Color);
      fill[k].meas_vs_P[0] -> Draw("PL");

      // plot online data points
      fill[k].meas_vs_P[1] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_online, fill[k].dum, fill[k].dP_online);
      fill[k].meas_vs_P[1] -> SetMarkerStyle(24);
      fill[k].meas_vs_P[1] -> SetMarkerColor(Color);
      fill[k].meas_vs_P[1] -> SetLineStyle(2);
      fill[k].meas_vs_P[1] -> Draw("PL");
      C->Update();

      // ------------------------------------------------------------------- // 
      //                        Superpose Rate plot.                         // 
      // ------------------------------------------------------------------- // 
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
      C->Update();

      if ((j+1)%10==0) ps.NewPage();
      ++j;


      //    } // if (fill[k].nRun>1)


  }// for (k=0; k<nFill)
  
  return 0;
    
}

