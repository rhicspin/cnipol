
//
// Class name  : Offline
// Method name : FillByFillAnalysis(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : 
// Input       : 
// Return      : 
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
// Class name  : Offline
// Method name : FillByFillAnalysis(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : 
// Input       : 
// Return      : 
//
Int_t 
OfflinePol::FillByFillAnalysis(Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps){
  

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

    Int_t array_index = fill[j].nRun-1;
    fill[j].Index[array_index]      = fill[j].nRun;
    fill[j].RunID[array_index]      = RunID[i];
    fill[j].P_online[array_index]   = P_online[i];
    fill[j].dP_online[array_index]  = dP_online[i];
    fill[j].P_offline[array_index]  = P_offline[i];
    fill[j].dP_offline[array_index] = dP_offline[i];
    fill[j].Clock[array_index]      = (Float_t(Time[i]) - fill[j].Clock0)/3600;

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
  Int_t nFill=j;


  TCanvas * C = new TCanvas("C","fill by fill", 800, 800);
  C->Divide(2,5);

  Char_t htitle[100];
  for (Int_t k=0; k<nFill; k++) {
    C->cd(k%10+1);
    sprintf(htitle,"Fill#%5d",fill[k].FillID);
    TH2F * fillbyfill = new TH2F("fillbyfill", htitle, 10, -1, 10, 100, -70, -20); 
    fillbyfill->Draw();
    fill[k].meas_vs_P[0] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_offline, fill[k].dum, fill[k].dP_offline);
    fill[k].meas_vs_P[0] -> SetMarkerStyle(20);
    fill[k].meas_vs_P[0] -> SetMarkerColor(Color);
    fill[k].meas_vs_P[0] -> Draw("PL");
    fill[k].meas_vs_P[1] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_online, fill[k].dum, fill[k].dP_online);
    fill[k].meas_vs_P[1] -> SetMarkerStyle(24);
    fill[k].meas_vs_P[1] -> SetMarkerColor(Color);
    fill[k].meas_vs_P[1] -> SetLineStyle(2);
    fill[k].meas_vs_P[1] -> Draw("PL");
    C->Update();
    if (k%10==0) ps.NewPage();
  }
    
  
  
  return 0;
    
}

