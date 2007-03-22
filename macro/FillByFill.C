extern Float_t RATE_FIT_STANDARD_DEVIATION ;
extern Float_t RATE_FIT_STANDARD_DEVIATION_DATA ;
extern Float_t RATE_DROP_ALLOWANCE ;
extern Float_t REFERENCE_RATE_DROP_ALLOWANCE ;
extern Float_t POLARIZATION_FIT_CHI2 ;
extern Float_t POLARIZATION_FIT_SIGMA_DATA ;
extern Int_t   RUN;

//
// Class name  : Offline
// Method name : Int_t OfflinePol::TimeDecoder(){
//
// Description : Decode Character encoded time to total time from Jan. 1st in unit of [sec].
//             : This is not unix time yet, because it is not necessary. In case that run 
//             : continues from Dec. to Jan., need to imprement Year.
// Input       : 
// Return      : time in the unit of seconds
//
Int_t 
OfflinePol::TimeDecoder(){
  //               Jan,Feb,Mar,Apr,May,Jun,Jul,Aut,Sep,Oct,Nov,Dec
  Int_t day[13]={0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  Int_t days_from_Jan_1st=0;

  string str(time.Month_c);
  if (!str.compare("Jan")) time.Month =  1;
  if (!str.compare("Feb")) time.Month =  2;
  if (!str.compare("Mar")) time.Month =  3;
  if (!str.compare("Apr")) time.Month =  4;
  if (!str.compare("May")) time.Month =  5;
  if (!str.compare("Jun")) time.Month =  6; 
  if (!str.compare("Jul")) time.Month =  7;
  if (!str.compare("Aug")) time.Month =  8; 
  if (!str.compare("Sep")) time.Month =  9; 
  if (!str.compare("Oct")) time.Month = 10;
  if (!str.compare("Nov")) time.Month = 11;
  if (!str.compare("Dec")) {time.Month = 12; 
  cerr << "CAUTION!: Perent time format don't takes into account year. Rridge Dec. -> Jan. will reset the time stamp." << endl;
  cerr << "        : Add year in the time format. (TimeDecorder() @ FillByFill.C) " << endl;
  }
  if (!day) cerr << "OfflinePol::TimeDecoder()  Error in decoding time.Month_c" << endl;

  // count number of days from January 1st
  for (Int_t i=1;i<time.Month; i++) days_from_Jan_1st+=day[i];

  time.Hour   = atoi(strtok(time.Time,":"));
  time.Minute = atoi(strtok(NULL,":"));
  time.Sec    = atoi(strtok(NULL,":"));

  // total time from Jan. 1st in unit of [sec]
  Int_t time = time.Sec + 60*(time.Minute + 60*(time.Hour + 24*(time.Day + days_from_Jan_1st)));

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

  // Universal Rate Target by Target
  if (Mode>>5&1) TargetByTargetUniversalRate(nFill, Color, ps);

  MakeFillByFillPlot(nFill, Mode, ndata, Color, ps);


  return 0;

}

//
// Class name  : OfflinePol
// Method name : SingleFillPlot(Int_t Mode, Int_t RUN, Int_t ndata, Int_t FillID, Int_t Color)
//
// Description : Makes single fill plot
// Input       : Int_t Mode, Int_t RUN, Int_t ndata, Int_t FillID, Int_t Color
// Return      : (Int_t)Number of Fills
//
Int_t 
OfflinePol::SingleFillPlot(Int_t Mode, Int_t RUN, Int_t ndata, Int_t FillID, Int_t Color){

  Int_t nFill = FillByFillAnalysis(RUN, ndata);

  gStyle->SetOptStat(kFALSE);
  for (Int_t k=0; k<nFill; k++) {
    if (fill[k].FillID == FillID ) FillByFillPlot(Mode, k, Color); 
  }

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
    fill[j].GlobalIndex[array_index]= i;
    fill[j].Index[array_index]      = fill[j].nRun;
    fill[j].RunID[array_index]      = RunID[i];
    fill[j].P_online[array_index]   = P_online[i] * sign;
    fill[j].dP_online[array_index]  = dP_online[i];
    fill[j].P_offline[array_index]  = PROFILE_CORRECTION ? P_offline[i] * sign + dProf[i] : P_offline[i] * sign;
    fill[j].dP_offline[array_index] = dP_offline[i]; 
    fill[j].dProf[array_index]      = dProf[i];
    fill[j].dP_tot[array_index]     = PROFILE_ERROR  ? QuadraticSumSQRT(dP_offline[i], dProf[i]) : dP_offline[i];
    fill[j].Rate[array_index]       = Rate[i];
    fill[j].WCM[array_index]        = WCM[i];
    fill[j].Time[array_index]       = Float_t(Time[i]);
    fill[j].Clock[array_index]      = (Float_t(Time[i]) - fill[j].Clock0)/3600;
    fill[j].dum[array_index]        = 0;
    if (array_index) {
      fill[j].dT[array_index]       = fill[j].Clock[array_index]-fill[j].Clock[array_index-1];
    }else{
      fill[j].dT[array_index]       = 0;
    }

    // print out fill by fill arrays for debugging
    //    PrintFillByFillArray(i, j, array_index);

  } // end-of-for(i=0;ndata) lope



  //-------------------------------------------------------------------//
  //              Assign delta_t for each run                          //
  //-------------------------------------------------------------------//
  Int_t nFill=j+1;
  for (Int_t i=0; i<nFill; i++) {
    for (Int_t k=0; k<fill[i].nRun; k++) {
 
      // pre-define dt[fill[i].nRun] is zero 
      if (k==fill[i].nRun-1) fill[i].dt_err[k+1] = 0;
      
      // this is actually time interval between k+1 and k-1 data point for "k"th data point. 
      fill[i].dt[k] = fill[i].dT[k+1]/2 + fill[i].dT[k]/2;

      // further divide dt[k] by half for plotting purpose of horizontal bar
      fill[i].dt_err[k] = fill[i].dt[k]/2; 

      // assign 1 hour for datum where only one measurement per fill
      if (fill[i].nRun==1) fill[i].dt_err[k]=1;

      // weight by product of WCM_sum and dt
      fill[i].Weight[k]     = fill[i].WCM[k]*fill[i].dt[k];

      // some trick to plot horizonatal asymmetric error on plot
      if (k==0) {
	fill[i].ClockM[k] = fill[i].Clock[k] + fill[i].dt_err[k];
      } else {
	fill[i].ClockM[k] = fill[i].ClockM[k-1] + fill[i].dt_err[k-1] + fill[i].dt_err[k];
      }

    } // end-of-for (k<fill[i].nRun) loop

  } // end-of-for (i<nFill) loop

  cout << "Total Number of Fill = " << j+1 << endl;

  return nFill;

}






//
// Class name  : OfflinePol
// Method name : PrintFillByFillArray(Int_t i, Int_t j, Int_t array_index)
//
// Description : Print fill by fill arrays for diagnose
// Input       : Int_t i, Int_t j, Int_t array_index
// Return      : 
//
Int_t 
OfflinePol::PrintFillByFillArray(Int_t i, Int_t j, Int_t array_index){


    printf("i=%d ",i);
    printf("j=%d ",j);
    printf("fill[%d].nRun=%d ",j,fill[j].nRun);
    printf("fill[%d].Index[%d]=%d ",j, array_index, fill[j].Index[array_index]);
    printf("fill[%d].RunID[%d]=%8.3f ",j, array_index, fill[j].RunID[array_index]);
    printf("fill[%d].P_offline[%d]=%5.1f ",j, array_index, fill[j].P_offline[array_index]);
    printf("fill[%d].Time[%d]=%11d ",j, array_index, fill[j].Time[array_index]);
    printf("fill[%d].Clock[%d]=%4.1f ",j, array_index, fill[j].Clock[array_index]);
    //    printf("fill[%d].dt[%d]=%4.1f ",j,array_index, fill[j].dt[array_index]);
    //    printf("fill[%d].WCM[%d]=%4.1f ",j,array_index, fill[j].WCM[array_index]);
    printf("\n");

    return 0;

}



//
// Class name  : OfflinePol
// Method name : TargetByTarget(Int_t k, Int_t Color)
//
// Description : Make universal rate plots Target by Target and apply gaussian fit to
//             : define mean value. This mean will be reference to the expected rate 
//             : with a given target.
// Input       : TargetByTargetUniversalRate(Int_t nFill, Int_t Color, TPostScript *ps)
// Return      : 
//
Int_t 
OfflinePol::TargetByTargetUniversalRate(Int_t nFill, Int_t Color, TPostScript *ps){

  // Turn flag on 
  flag.UniversalRate = 1;

  Char_t htitle[100];
  Float_t xmax=0.10;

  ps->NewPage();

  //--------------------------------------------------------------------------//
  //            Define Universal Rate of Target Period Histograms             //
  //--------------------------------------------------------------------------//
  for (Int_t i=0; i<blue.Target.nPeriod; i++)   { // blue beam
    sprintf(htitle,"Blue:Universal Rate Period %8.3f - %8.3f", blue.target[i].Begin_RunID, blue.target[i].End_RunID);
    blue.target[i].UniversalRate = new TH1F("UniversalRate", htitle, 20, 0, xmax);
    blue.target[i].UniversalRate -> SetFillColor(Color);
    blue.target[i].UniversalRate -> SetXTitle("#12C_banana/Delta_t/WCM_sum");

    sprintf(htitle,"Blue:Universal Rate Period %8.3f - %8.3f", blue.target[i].Begin_RunID, blue.target[i].End_RunID);
    blue.target[i].UniversalRate_vs_P = new TH2F("UniversalRate_vs_P", htitle, 20, 0, xmax, 60, 20, 80);
    blue.target[i].UniversalRate_vs_P -> SetMarkerStyle(20);
    blue.target[i].UniversalRate_vs_P -> SetMarkerColor(Color);
    blue.target[i].UniversalRate_vs_P -> SetXTitle("#12C_banana/Delta_t/WCM_sum");
  }  
  for (Int_t i=0; i<yellow.Target.nPeriod; i++) { // yellow beam
    sprintf(htitle,"Yellow:Universal Rate Period %8.3f - %8.3f", yellow.target[i].Begin_RunID, yellow.target[i].End_RunID);
    yellow.target[i].UniversalRate = new TH1F("UniversalRate", htitle, 20, 0, xmax);
    yellow.target[i].UniversalRate -> SetFillColor(Color);
    yellow.target[i].UniversalRate -> SetXTitle("#12C_banana/Delta_t/WCM_sum");

    sprintf(htitle,"Yellow:Universal Rate Period %8.3f - %8.3f", yellow.target[i].Begin_RunID, yellow.target[i].End_RunID);
    yellow.target[i].UniversalRate_vs_P = new TH2F("UniversalRate_vs_P", htitle, 20, 0, xmax, 60, 20, 80);
    yellow.target[i].UniversalRate_vs_P -> SetMarkerStyle(20);
    yellow.target[i].UniversalRate_vs_P -> SetMarkerColor(Color);
    yellow.target[i].UniversalRate_vs_P -> SetXTitle("#12C_banana/Delta_t/WCM_sum");

  }




  //--------------------------------------------------------------------------//
  //                   Fill Universal Rate Histograms                         //
  //--------------------------------------------------------------------------//
  for (Int_t k=0; k<nFill; k++) TargetByTarget(k, Color);



  //--------------------------------------------------------------------------//
  // Plot Universal rate histograms and fit with gaussian to find the mean    //
  //--------------------------------------------------------------------------//
  gStyle->SetTitleFontSize(0.15);
  gStyle->SetOptStat(kTRUE);
  gStyle->SetOptFit(111);
  TCanvas * C2 = new TCanvas("C2","Universal Rate for Target Period", 1100, 800);
  
  TF1 * g = new TF1("gauss","gaus");
  g->SetLineColor(2);

  if (Color==4) {  // blue beam
    C2->Divide(1, blue.Target.nPeriod);
    for (Int_t i=0; i<blue.Target.nPeriod; i++) {
      C2->cd(i+1) ; blue.target[i].UniversalRate->Draw();
      g->SetParameter(0,blue.target[i].UniversalRate->GetMaximum());
      g->SetParameter(1,blue.target[i].UniversalRate->GetMean());
      g->SetParameter(2,blue.target[i].UniversalRate->GetRMS());
      blue.target[i].UniversalRate->Fit(g);
      blue.target[i].Mean = g->GetParameter(1);
      blue.target[i].Sigma = g->GetParameter(2);
      blue.target[i].Threshold = blue.target[i].Mean * RATE_DROP_ALLOWANCE;
      Float_t ymax = blue.target[i].UniversalRate->GetMaximum()*0.8;
      DrawLine(blue.target[i].UniversalRate, blue.target[i].Threshold, ymax, 1, 2);
      DrawLine(blue.target[i].UniversalRate, blue.target[i].Mean-blue.target[i].Sigma*REFERENCE_RATE_DROP_ALLOWANCE, ymax, 7, 2);
    }
  C2->Update(); ps->NewPage(); 
    for (Int_t i=0; i<blue.Target.nPeriod; i++) {
      C2->cd(i+1) ; blue.target[i].UniversalRate_vs_P->Draw("P");
    }
  }else{ // yellow beam
    C2->Divide(1, yellow.Target.nPeriod);
    for (Int_t i=0; i<yellow.Target.nPeriod; i++) {
      g->SetParameter(0,yellow.target[i].UniversalRate->GetMaximum());
      g->SetParameter(1,yellow.target[i].UniversalRate->GetMean());
      g->SetParameter(2,yellow.target[i].UniversalRate->GetRMS());
      // following line is some ugry trick to force fit to converge
      if ((i==3)&&(RUN==5)) { g->SetParLimits(1, 0.035, 0.05); g->SetParLimits(2, 0.0001, 0.01);}
      C2->cd(i+1) ; yellow.target[i].UniversalRate->Draw();
      yellow.target[i].UniversalRate->Fit(g);
      yellow.target[i].Mean = g->GetParameter(1);
      yellow.target[i].Sigma = g->GetParameter(2);
      yellow.target[i].Threshold = yellow.target[i].Mean * RATE_DROP_ALLOWANCE;
      Float_t ymax = yellow.target[i].UniversalRate->GetMaximum()*0.8;
      DrawLine(yellow.target[i].UniversalRate, yellow.target[i].Threshold, ymax, 1, 2);
      DrawLine(yellow.target[i].UniversalRate, yellow.target[i].Mean - yellow.target[i].Sigma*REFERENCE_RATE_DROP_ALLOWANCE, ymax, 7, 2);
    }
  C2->Update(); ps->NewPage(); 
    for (Int_t i=0; i<yellow.Target.nPeriod; i++) {
      C2->cd(i+1) ; yellow.target[i].UniversalRate_vs_P->Draw("P");
    }
  }

  C2->Update(); ps->NewPage(); 


  return 0;

}


//
// Class name  : OfflinePol
// Method name : TargetByTarget(Int_t k, Int_t Color)
//
// Description : Stack data for Target by Target period and fill the relevant universal rate histogram. 
// Input       : TargetByTarget(Int_t k, Int_t Color)
// Return      : 
//
Int_t 
OfflinePol::TargetByTarget(Int_t k, Int_t Color){


  for (Int_t j=0; j<fill[k].nRun; j++){

    if (Color==4){ // blue beam

      for (Int_t i=0;i<blue.Target.nPeriod; i++){
	if ((blue.target[i].Begin_RunID<=fill[k].RunID[j])&&(fill[k].RunID[j]<blue.target[i].End_RunID)) {
	  blue.target[i].UniversalRate->Fill(fill[k].Rate[j]);
	  blue.target[i].UniversalRate_vs_P->Fill(fill[k].Rate[j], fill[k].P_offline[j]);
	}
      } // end-of-(i<blue.Target.nPeriod) loop

    }else{ // yellow beam

      for (Int_t i=0;i<yellow.Target.nPeriod; i++){
	if ((yellow.target[i].Begin_RunID<=fill[k].RunID[j])&&(fill[k].RunID[j]<yellow.target[i].End_RunID)) {
	  yellow.target[i].UniversalRate->Fill(fill[k].Rate[j]);
	  yellow.target[i].UniversalRate_vs_P->Fill(fill[k].Rate[j], fill[k].P_offline[j]);
	}
      } // end-of-(i<yellow.Target.nPeriod) loop

    }// end-of-(j<fill[k].nRun) loop

  };// end-of-fill[k].nRun loop

  return 0;
}


Double_t pol_vs_rate(Double_t *x, Double_t *par)
{
  if (x[0]>1){
    TF1::RejectPoint();
    return 0;
  }
  return par[0]*exp(-par[2]*par[2]/par[1]/par[1]*log(1/x[0]));

}

//
// Class name  : 
// Method name : pol_vs_rate(Float_t x, Float_t par[0], Float_t par[1], Float_t par[2]) {
//
// Description : function to relate polarization and rate
// Input       : 
// Return      : 
//
Double_t pol_vs_rate(Float_t x, Float_t par[0], Float_t par[1], Float_t par[2]) {
  return par[0]*exp(-par[2]*par[2]/par[1]/par[1]*log(1/x));
}

//
// Class name  : 
// Method name : pol_vs_rate(Float_t x, Float_t par)
//
// Description : function to relate polarization and rate
// Input       : 
// Return      : 
//
Double_t pol_vs_rateR(Float_t x, Float_t par[0], Float_t par[1]){
  return par[0]*x**par[1];
}



//
// Class name  : OfflinePol
// Method name : PolarizationProfileNoFit(StructBeam beam, TCanvas * C2, Int_t ndata, Int_t Color, TPostScript *ps)
//
// Description : Make plots for Polarization Profile vs. Rate correlations, but no fit applied. Just draw pre-defined
//             : functions
// Input       : (StructBeam beam, TCanvas * C2, Int_t ndata, Int_t Color, TPostScript *ps)
// Return      : 
//
Int_t 
OfflinePol::PolarizationProfileNoFit(StructBeam beam, TCanvas * C2, Int_t ndata, Int_t Color, TPostScript *ps){

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.04);
  //========================================================================//
  //            Define Fitting Function pol_vs_rate()                       //
  //========================================================================//
  TF1 * f1 = new TF1("f1",pol_vs_rate, 0, 1, 3);  
  TF1 * f2 = new TF1("f2",pol_vs_rate, 0, 1, 3);  f2->SetLineColor(2);
  TF1 * f3 = new TF1("f3",pol_vs_rate, 0, 1, 3);  f3->SetLineColor(2); f3->SetLineStyle(2);
  TF1 * f4 = new TF1("f4",pol_vs_rate, 0, 1, 3);  f4->SetLineStyle(2);
  TF1 * f5 = new TF1("f5",pol_vs_rate, 0, 1, 3);  f5->SetLineStyle(2);

  //========================================================================//
  //            Pol vs. Rate 2-Dim Scatter Plot and Fit                     //
  //========================================================================//
  beam.UniversalRateDrop->Draw(); C2->Update(); ps->NewPage();
  beam.UniversalRateDrop_vs_p-> GetXaxis() -> SetTitle("Rate/Expected Universal Rate");
  beam.UniversalRateDrop_vs_p-> GetYaxis() -> SetTitle("Polarization [%]");
  beam.UniversalRateDrop_vs_p -> Draw(); 
  TGraphErrors * rate_vs_P = new TGraphErrors(ndata, RelRate, P_offline, dx, dP_offline); 
  rate_vs_P -> SetMarkerStyle(20);
  rate_vs_P -> SetMarkerColor(Color);
  rate_vs_P -> Draw("same,P"); 

    // draw predictions from profile scan
  if (Color==4){ // blue 7151
    f1->SetParameters(-49.9, 6000, 0);
    f1->Draw("same");
    f2->SetParameters(-49.9, 3.080, 0.8625); // blue 7151
    f2->Draw("same");
    f4->SetParameters(-49.9, 1/0.09, 1);     // best fit from Sasha - 0.08 systematic error
    f4->Draw("same");
  } else {   
    f1->SetParameters(-47.6, 1/SigR_SigP, 1);       // best fit from Sasha 
    f1->Draw("same");
    f2->SetParameters(-47.6, 1.100, 0.8229);   // yellow - 7151 
    f2->Draw("same");
    f3->SetParameters(-47.6, 1.541, 0.6913);   // yellow - 7133
    f3->Draw("same");
    f4->SetParameters(-47.6, 1/(SigR_SigP+0.07), 1);       // best fit from Sasha + 0.08 systematic error 
    f4->Draw("same"); 
    f5->SetParameters(-47.6, 1/(SigR_SigP-0.07), 1);       // best fit from Sasha - 0.08 systematic error
    f5->Draw("same");

  }

  C2->Update(); ps->NewPage();



  return;

}




//
// Class name  : OfflinePol
// Method name : PolarizationProfile(StructBeam beam, TCanvas * C2, Int_t ndata, Int_t Color, TPostScript *ps)
//
// Description : Make plots for Polarization Profile vs. Rate correlations
// Input       : 
// Return      : 
//
Int_t 
OfflinePol::PolarizationProfile(StructBeam beam, TCanvas * C2, Int_t ndata, Int_t Color, TPostScript *ps){

  TRandom * ran = new TRandom(0);
  dPdistGaus->SetFillColor(Color);

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.04);
  //========================================================================//
  //            Define Fitting Function pol_vs_rate()                       //
  //========================================================================//
  TF1 * f1 = new TF1("f1",pol_vs_rate, 0, 1, 3);  f1->SetParameters(0,1,0.7);  f1->SetParLimits(2,0.7,0.7);
  TF1 * f2 = new TF1("f2",pol_vs_rate, 0, 1, 3);  f2->SetLineColor(2);
  TF1 * f3 = new TF1("f3",pol_vs_rate, 0, 1, 3);  f3->SetLineColor(2); f3->SetLineStyle(2);
  Float_t xmin=-30, xmax=fabs(xmin);


  //========================================================================//
  //            Pol vs. Rate 2-Dim Scatter Plot and Fit                     //
  //========================================================================//
  if (!NORMALIZED_POL){
    beam.UniversalRateDrop->Draw(); C2->Update(); ps->NewPage();
    beam.UniversalRateDrop_vs_p -> Draw(); 
    TGraphErrors * rate_vs_P = new TGraphErrors(ndata, RelRate, P_offline, dx, dP_offline); 
  } else { // Vertical axis polarization is normalized by fill averaged one
    if (Color!=4) C2->Clear(); ps->NewPage();
    beam.NormP_vs_univR -> GetXaxis() -> SetTitle("Rate/Expected Universal Rate");
    beam.NormP_vs_univR -> GetYaxis() -> SetTitle("Pol_i / Average(Pol_fill)");
    beam.NormP_vs_univR->Draw(); C2->Update(); 
    TGraphErrors * rate_vs_P = new TGraphErrors(ndata, RelRate, P_norm_Ave, dx, dP_norm_Ave); 
    xmin=-0.5; xmax=fabs(xmin);
  }
  rate_vs_P -> SetMarkerStyle(20);
  rate_vs_P -> SetMarkerColor(Color);
  rate_vs_P -> Draw("same,P"); 
  rate_vs_P -> Fit(f1); 
  f1->SetParameters(f1->GetParameter(0), f1->GetParameter(1), f1->GetParameter(2));

    // draw predictions from profile scan
  if (Color==4){ // blue 7151
    f2->SetParameters(f1->GetParameter(0), 3.080, 0.8625); // blue 7151
    f2->Draw("same");
  } else {   
    f2->SetParameters(f1->GetParameter(0), 1.100, 0.8229);   // yellow - 7151 
    f2->Draw("same");
    f3->SetParameters(f1->GetParameter(0), 1.541, 0.6913);   // yellow - 7133
    f3->Draw("same");
  }

  C2->Update(); ps->NewPage();

  //========================================================================//
  //            Projection of Data onto Best Fit Plane                      //
  //========================================================================//
  Char_t title[100]="Data - Best Fit";
  NORMALIZED_POL ? sprintf(title,"Data - Best Fit (Normarized by Pol Average)");
  TH1F * dUnivDistH = new TH1F("dUnivDistH",title, 40, xmin, xmax);
  dUnivDistH->GetXaxis()->SetTitle("Data - Best Fit");
  NORMALIZED_POL ? dUnivDistH->GetYaxis()->SetTitle("# of Runs/error") : dUnivDistH->GetYaxis()->SetTitle("# of Runs") ; 
  for (Int_t i=0; i<ndata; i++) {
    Float_t  y = NORMALIZED_POL ?  P_norm_Ave[i] :  P_offline[i];
    Float_t dy = NORMALIZED_POL ? dP_norm_Ave[i] : dP_offline[i];
    if (RelRate[i]<1) {
      dUnivDist[i] = y - f1->Eval(RelRate[i]);
      dUnivDistH -> Fill(dUnivDist[i],1/dy);
      dProf[i] = (1-f1->Eval(RelRate[i])) * P_fillave[i] ; // fill-up array of profile error
      dProf_err[i] = dy * P_fillave[i];
      beam.dPdistGaus->Fill(ran->Gaus(0, dP_offline[i]/100));
    }else{
      dUnivDist[i] = 0;
    }      
  }

  gStyle->SetOptStat(kTRUE); gStyle->SetOptStat(1);
  dUnivDistH -> SetFillColor(Color);
  dUnivDistH -> Draw("");
  dUnivDistH -> Fit("gaus");

  C2->Update(); ps->NewPage();

  // In 2-dim histogram
  if (NORMALIZED_POL) {
    gStyle->SetOptStat(kTRUE); gStyle->SetOptStat(1);
    beam.dPdistGaus->SetFillColor(Color);
    beam.dPdistGaus->Fit("gaus");
    C2->Update(); ps->NewPage();

    gStyle->SetOptStat(kFALSE);
    beam.DataMinusFit_vs_univR -> GetXaxis() -> SetTitle("Rate/Expacted Universal Rate");
    beam.DataMinusFit_vs_univR -> GetYaxis() -> SetTitle("(Data - Best Fit) * P_ave(fill)");
    beam.DataMinusFit_vs_univR -> Draw(); 
    TGraphErrors * DataMinusFit = new TGraphErrors(ndata, RelRate, dProf, dx, dProf_err);
    DataMinusFit->SetMarkerStyle(20);
    DataMinusFit->SetMarkerColor(Color);
    DataMinusFit->Draw("same,P");
    C2->Update(); ps->NewPage();
  }


  return;

}


//
// Class name  : OfflinePol
// Method name : MakeFillByFillPlot(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : Make Plots Fill by Fill 
// Input       : Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
OfflinePol::MakeFillByFillPlot(Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TPostScript *ps){



  if (Color==4){
    fout << "\n Blue:" << endl;
    blue.UniversalRateDrop   = new TH1F("blue_UniversalRateDrop","Universal Rate Distribution (Blue)", 40, 0, 2);
    blue.UniversalRateDrop -> SetFillColor(Color);
    blue.UniversalRateDrop_vs_p= new TH2F("blue_UniversalRateDrop_vs_P","P vs Universal Rate (Blue)", 40, 0, 2, 40, -80, -20);
    blue.NormP_vs_univR = new TH2F("blue_NormP_vs_univR","P/P_ave vs. Universal Rate (Blue)", 40, 0, 1.5, 40, 0, 1.6);
    blue.DataMinusFit_vs_univR = new TH2F("DataMinusFit_vs_univR","Data - Fit vs. Universal Rate (Blue)", 10, 0, 1, -25, 25);
    blue.dPdistGaus  = new TH1D("blue_dPdistGaus",  "Offline Pol Error Distribution", 100, -0.5, 0.5);
  }else{
    fout << "\n Yellow:" << endl;
    yellow.UniversalRateDrop = new TH1F("yellow_UniversalRateDrop","Universal Rate Distribution (Yellow)", 40, 0, 2);
    yellow.UniversalRateDrop -> SetFillColor(Color);
    yellow.UniversalRateDrop_vs_p= new TH2F("yellow_UniversalRateDrop_vs_P","P vs. Universal Rate (Yellow)", 40, 0, 2, 40, -80, -20);
    yellow.NormP_vs_univR = new TH2F("yellow_NormP_vs_univR","P/P_ave  vs Universal Rate (Yellow)", 40, 0, 1.5, 40, 0, 1.6);
    yellow.DataMinusFit_vs_univR = new TH2F("DataMinusFit_vs_univR","Data - Fit vs. Universal Rate (Yellow)", 10, 0, 1, -25, 25);
    yellow.dPdistGaus  = new TH1D("yellow_dPdistGaus",  "Offline Pol Error Distribution", 100, -0.5, 0.5);
  };

  ps->NewPage();

  // Define Chi2 Distribution
  Float_t OverFlow=10;
  TH1F * FillByFillChi2[4];
  Char_t htitle[100];
  TH1F * nRunPerFill = new TH1F("nRunPerFill","Number of Runs per Fill", 21, -0.5, 20.5);
  nRunPerFill -> SetFillColor(Color);
  for (Int_t i=0; i<4; i++) {
    sprintf(htitle,"FillByFillChi2_%d",i);
    FillByFillChi2[i] = new TH1F(htitle,"Fill By Fill Polarization Chi2/D.o.F", 42, 0, OverFlow);
    FillByFillChi2[i] ->SetXTitle("Chi2/D.o.F.");
    FillByFillChi2[i] ->SetYTitle("# of Fill");
    if (i) {
      FillByFillChi2[1] ->SetLineColor(3);
      FillByFillChi2[i] ->SetLineWidth(2); 
      if (i!=2) FillByFillChi2[i] ->SetLineStyle(2);
    }else{
      FillByFillChi2[i] ->SetFillColor(Color);
      FillByFillChi2[i] ->SetStats(1111);
    }
  }


  // Reset counters
  Fill.bad.rate.fill = Fill.bad.rate.data = Fill.bad.P.fill = Fill.bad.P.data = 0;
  

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.08);
  TCanvas * C = new TCanvas("C","fill by fill", 1100, 850);
  C->Divide(2,5);
  
  // Fill By Fill loop
  Int_t j=0;
  for (Int_t k=0; k<nFill; k++) {
    nRunPerFill -> Fill(fill[k].nRun);

    // Fill Ch-2 distribution histograms
    if (fill[k].nRun>0) {
      C->cd(j%10+1);
      FillByFillPlot(Mode, k, Color); C->Update();
      for (Int_t i=0; i<2; i++)  fill[k].Chi2[i] = fill[k].Chi2[i]>OverFlow ? OverFlow*0.99 : fill[k].Chi2[i];
      if (fill[k].Chi2[1]) FillByFillChi2[1]->Fill(fill[k].Chi2[1]);   // Exponential Fit
      if (fill[k].Chi2[0]) {
	FillByFillChi2[0]->Fill(fill[k].Chi2[0]);                      // Linear Fit
	if (fill[k].DoF>1)  FillByFillChi2[2]->Fill(fill[k].Chi2[0]);  // D.o.F>1
	if (fill[k].DoF==1) FillByFillChi2[3]->Fill(fill[k].Chi2[0]);  // D.o.F=1
      }
      if ((j+1)%10==0) ps->NewPage();
      ++j;
    }

  } // for (k=0; k<nFill)

  C->Update(); ps->NewPage();

  // Plot Chi2 Distribution if Fit Mode
  if (Mode>>3&1) {
    gStyle->SetTitleFontSize(0.05);
    gStyle->SetOptStat(kTRUE);
    TCanvas * C2 = new TCanvas("C2","Chi2 Distribution", 1100, 800);
    nRunPerFill->Draw(""); C2-> Update(); ps->NewPage(); C2->Clear();
    C2->Divide(2);

    C2->cd(1);
    FillByFillChi2[0]->Draw();
    FillByFillChi2[1]->Draw("same");

    TLegend * lg = new TLegend(0.45,0.7,0.65,0.85);
    lg -> AddEntry(FillByFillChi2[0],"Linear Fit","F");
    lg -> AddEntry(FillByFillChi2[1],"Expo Fit","F");
    lg -> Draw("same");

    C2->cd(2);
    FillByFillChi2[0]->SetLineColor(Color);
    FillByFillChi2[0]->Draw("");
    FillByFillChi2[2]->Draw("same");
    FillByFillChi2[3]->Draw("same");

    TLegend * lg = new TLegend(0.45,0.7,0.65,0.85);
    lg -> AddEntry(FillByFillChi2[0],"Linear All;","F");
    lg -> AddEntry(FillByFillChi2[2],"D.o.F>1","F");
    lg -> AddEntry(FillByFillChi2[3],"D.o.F=1","F");
    lg -> Draw("same");


    C2->Update();    ps->NewPage();

    fout << "\n ===========================================================" << endl;
    fout << "  Large Chi2 Pol  fills               : " << Fill.bad.P.fill    << endl;
    fout << "  Large Chi2 Pol  data                : " << Fill.bad.P.data    << endl;
    fout << " ===========================================================" << endl;

  } else if ((Mode>>4&1)||(Mode>>5&1)) {

    TCanvas * C3 = new TCanvas("C3"); 
    //    Color == 4 ? PolarizationProfile(blue, C3, ndata, Color, ps) : PolarizationProfile(yellow, C, ndata, Color, ps);
    Color == 4 ? PolarizationProfileNoFit(blue, C3, ndata, Color, ps) : PolarizationProfileNoFit(yellow, C, ndata, Color, ps);

    fout << "\n ===========================================================" << endl;
    if (Fill.bad.rate.fill) fout << "  RATE Drop Suspicious fills : " << Fill.bad.rate.fill << endl;
    fout << "  RATE Drop Suspicoius data  : " << Fill.bad.rate.data << endl;
    fout << " ===========================================================" << endl;

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
//                   Bit 3 - P_Offline Fit
//                   Bit 4 - Rate Filter
//                   Bit 5 - Rater Filter by Universal rate on Target By Target
//                   Bit 6 - delta_t horizontal error (fill-by-fill average)
//               Mode=7 (Offline,Online,Rate), Mode=9 (Offline,fit)
// Input       : Int_t Mode, Int_t k, Int_t Color
// Return      : 
//
Int_t 
OfflinePol::FillByFillPlot(Int_t Mode, Int_t k, Int_t Color){

  static Int_t hid;
  Char_t text[50];
  Float_t xmin=-0.5, xmax=10, ymin, ymax;
  Float_t margin=0.05;
  Float_t prefix_range=25; // Vertical Axis (Polariation) Range [%]. 

  // scaled rate default parameters
  r.ymin=0;
  r.ymax=0.065;
  r.Color = 30;


  // 2D hist frame
  Char_t htitle[100], hname[100]; 
  Color == 4 ? sprintf(htitle,"Fill#%5d (Blue)",fill[k].FillID) : sprintf(htitle,"Fill#%5d (Yellow)",fill[k].FillID) ;
  sprintf(hname,"Fill%d",hid); ++hid;
  if (fill[k].nRun>1) GetScale(fill[k].Clock, fill[k].nRun, margin, xmin, xmax);
  Float_t Range = GetScalePrefixRange(prefix_range, fill[k].P_offline, fill[k].nRun, margin, ymin, ymax);
  if ((Mode>>4&1)||(Mode>>5&1)) {ymin=r.ymin; ymax=r.ymax;};
  TH2F * fillbyfill     = new TH2F(hname, htitle, 10, xmin, xmax, 100, ymin, ymax); 
  fillbyfill->GetXaxis()->SetTitle("Hours from first measurement at store");
  fillbyfill->GetYaxis()->SetTitle("Polarization [%]");
  if ((Mode>>4&1)||(Mode>>5&1))  fillbyfill->GetYaxis()->SetTitle("Good Carbon Rate/WCM_sum [MHz]");

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

  // Fill by Fill Average weighted by delta_t and WCM_sum 
  if (Mode>>6&1) {
    TGraphErrors * fill[k].meas_vs_fakeP = new TGraphErrors(fill[k].nRun, fill[k].ClockM, fill[k].P_offline, fill[k].dt_err, fill[k].dum);
    fill[k].meas_vs_fakeP ->Draw("P");

    // ------------------------------------------------------------------- // 
    //                WCM/dt Weighted Mean on Polarization                 // 
    // ------------------------------------------------------------------- // 
    calcWeightedMean(fill[k].P_offline, fill[k].dP_tot, fill[k].Weight, fill[k].nRun, fill[k].wAve[0], fill[k].wAve[1]);
    DrawLine(fillbyfill, xmin, xmax, fill[k].wAve[0], 2, 1, 3);
    DrawLine(fillbyfill, xmin, xmax, fill[k].wAve[0]+fill[k].wAve[1], 2, 3, 2);
    DrawLine(fillbyfill, xmin, xmax, fill[k].wAve[0]-fill[k].wAve[1], 2, 3, 2);
    if (!PROFILE_CORRECTION_ERROR){
      sprintf(text,"ave(P)=%.2f +/- %.2f ", fill[k].wAve[0], fill[k].wAve[1]);
      DrawText(fillbyfill, (xmin+xmax)/2, ymin*1.05, 2, text);
      fout << "    ====> FillID: " << std::setprecision(7) << fill[k].FillID 
	   << std::setprecision(4) << " P_ave = "  << fill[k].wAve[0] << " +/- " << fill[k].wAve[1] << endl;
    } else { // complete all errors
      fill[k].wAve[2] = fill[k].prof_corr.wAve - fill[k].wAve[0]; // profile error
      fill[k].wAve[3] = Color == 4 ? fill[k].wAve[0]*0.015 : fill[k].wAve[0]*0.017;
      fill[k].wAve[4] = QuadraticSumSQRT(fill[k].wAve[1], fill[k].wAve[2], fill[k].wAve[3]);
      DrawLine(fillbyfill, xmin, xmax, fill[k].wAve[0]+fill[k].wAve[4], 4, 3, 2);
      DrawLine(fillbyfill, xmin, xmax, fill[k].wAve[0]-fill[k].wAve[4], 4, 3, 2);
      sprintf(text,"ave(P)=%.2f +/- %.2f +/- %.2f +/- %.2f",  fill[k].wAve[0], fill[k].wAve[1], fill[k].wAve[2], fill[k].wAve[3]);
      DrawText(fillbyfill, (xmin+xmax)/2, ymin*1.05, 2, text);
      fout << fixed;
      fout << std::setprecision(2)    << fill[k].FillID   << "   " 
	   << fill[k].wAve[0] << "   "  << fill[k].wAve[1]  << "   " 
	   << fill[k].wAve[2] << "   "  << fill[k].wAve[3]  << "   " << fill[k].wAve[4] << endl;
      fill[k].wAve[0] = fill[k].wAve[1] = fill[k].wAve[2] = fill[k].wAve[3] = fill[k].wAve[4] = fill[k].prof_corr.wAve = 0;
    }

    // for profile correction error. Store fill average polarization with profile correction
    fill[k].prof_corr.wAve = fill[k].wAve[0] ;
      

  // ------------------------------------------------------------------- // 
  //                        Superpose WCM plot.                          // 
  // ------------------------------------------------------------------- // 
    if (!PLOT_PROFILE_ERROR){
      wcm.ymin=20; wcm.ymax=100; 
      TGaxis * A1 = new TGaxis(xmax, ymin, xmax, ymax, wcm.ymin, wcm.ymax, 510, "+L");
      A1 -> SetTitle("WCM Sum [10^11 protons]");
      A1 -> SetLineColor(r.Color);
      A1 -> SetLabelColor(r.Color);
      A1 -> Draw("same");
      
      // Scale wcm to vertical axis(polarization). fill[k].WCM[i] -> fill[k].sWCM[i] 
      for (Int_t i=0; i<fill[k].nRun; i++) 
	fill[k].sWCM[i] = (ymax - ymin)/(wcm.ymax - wcm.ymin)*(fill[k].WCM[i] - wcm.ymax) + ymax;

      fill[k].meas_vs_WCM = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].sWCM, fill[k].dum, fill[k].dum);
      fill[k].meas_vs_WCM -> SetMarkerStyle(22);
      fill[k].meas_vs_WCM -> SetMarkerColor(r.Color);
      fill[k].meas_vs_WCM -> SetLineColor(r.Color);
      fill[k].meas_vs_WCM -> Draw("P");
    }

  } // end-of-if (Mode>>6&1)



  // plot offline data points
  TGraphErrors *TotErr = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_offline, fill[k].dum, fill[k].dP_tot);
  fill[k].meas_vs_P[0] = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].P_offline, fill[k].dum, fill[k].dP_offline);
  fill[k].meas_vs_P[0] -> SetMarkerStyle(20);
  fill[k].meas_vs_P[0] -> SetMarkerColor(Color);
  if (Mode>>0&1) { 
    if (Mode>>6&1) {
      TotErr -> Draw("same");
      fill[k].meas_vs_P[0] -> Draw("P") ;
    }else{
      fill[k].meas_vs_P[0] -> Draw("PL"); };
  }

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
  //                            Rate Filter    .                         // 
  // ------------------------------------------------------------------- // 
  if ((Mode>>4&1)||(Mode>>5&1)){
    RateFilter(k, 0, xmin, xmax, Color);
    // RateFit(k, 1, xmin, xmax, Color);  // linear fit
  }


  // ------------------------------------------------------------------- // 
  //                     Fitting  on Polarization                        // 
  // ------------------------------------------------------------------- // 
  if (Mode>>3&1){

    Float_t P0, P1;
    gStyle->SetOptFit(111);
    if (fill[k].nRun>2) {

      TLegend * lg = new TLegend(0.15,0.1.5,0.35,0.35);

      // Linear Fit
      TF1 * f1 = new TF1("f1","pol1");
      f1 -> SetParLimits(1,-10000,0); // positive slope not allowed
      f1 -> SetLineColor(2);
      fill[k].meas_vs_P[0] -> Fit(f1,"QB");
      fill[k].DoF     = f1->GetNDF();
      fill[k].Chi2[0] = f1->GetChisquare()/(Float_t)fill[k].DoF;
      if (fill[k].Chi2[0]>POLARIZATION_FIT_CHI2){
	printf("Fill#%d Chi-2:%.1f\n",fill[k].FillID, fill[k].Chi2[0]);
	fout.precision(3);
	fout << "  Fill#" <<  fill[k].FillID << " Pol fit chi2 : " << fill[k].Chi2[0] 
	   << " Exceeds limit:" << POLARIZATION_FIT_CHI2 << endl;
        ++Fill.bad.P.fill;
      }
      P0      = f1->GetParameter(0);
      P1      = f1->GetParameter(1);
      lg->AddEntry(f1,"Linear Fit","L");

      // Exponential Fit
      TF1 * f2 = new TF1("f2","expo");
      f2 -> SetParLimits(1,-10000,0); // positive slope not allowed
      f2 -> SetLineColor(3);
      f2 -> SetLineStyle(2);
      fill[k].meas_vs_P[0] -> Fit(f2,"QNB");
      f2 -> Draw("same");
      fill[k].Chi2[1] = f2->GetChisquare()/(Float_t)f2->GetNDF();
      lg->AddEntry(f2,"Expo Fit","L");
      lg->Draw("same");

      // Test deviation of each data point from the Linear fit
      Int_t j=0;
      for (Int_t i=0; i<fill[k].nRun; i++) {

	Float_t sigma = ((P0 + P1*fill[k].Clock[i]) - fill[k].P_offline[i])/fill[k].dP_offline[i];
	if (fabs(sigma)>POLARIZATION_FIT_SIGMA_DATA) {
	  fill[k].bad.Clock[j] = fill[k].Clock[i];
	  fill[k].bad.P_offline[j] = fill[k].P_offline[i];
	  printf("%8.3f : %5.1f sigma off\n",fill[k].RunID[i], sigma);
	  sprintf(text,"%5.1f",sigma);
	  fout << "    ====> RunID:" << std::setprecision(7) << fill[k].RunID[i] << " polarization is off by " 
	       << std::setprecision(3) << sigma << " sigma limit "  << POLARIZATION_FIT_SIGMA_DATA << endl;
	  TText * tx = new TText(fill[k].Clock[i],fill[k].P_offline[i],text);
	  tx -> SetTextSize(0.08);
	  tx -> Draw("same");
	  ++j;
	} // end-of-fabs(sigma>3)

      } // end-of-for(Int_t i=0; i<fill[k].nRun)
      
      Fill.bad.P.data += j;
    } // end-of-if(fill[k].nRun>2)  


  } // end-of-if(Mode>>3&1)



  return 0;

}



//
// Class name  : OfflinePol
// Method name : RateFilter(Int_t k, Int_t Mode, Float_t xmin, Float_t xmax)
//
// Description : Extract major drop data in rate for fill[k].  
//             : Mode 1: Linear fit.and check the standard deviation
//             : Mode 0: No fit. Just compare with Maximum rate data point in the fill[k].
//                    Maximum rate option (Int_t Option)
//                       0 : Get maximum rate from data set after "i"th datum
//                       1 : Get maximum rate from all data set
//                       2 : Get maximum rate from gaussian mean fit on universal rate target by target

// Input       : Int_t k, Int_t Mode, Float_t xmin, Float_t xmax, Int_t Color
// Return      : 
//
void 
OfflinePol::RateFilter(Int_t k, Int_t Mode, Float_t xmin, Float_t xmax, Int_t Color){
  
  Int_t j=0; // bad run counter
  Char_t text[100];

  Float_t P0, P1;
  gStyle->SetOptFit(111);

  // Plot rate as a function of time
  fill[k].meas_vs_Rate = new TGraphErrors(fill[k].nRun, fill[k].Clock, fill[k].Rate, fill[k].dum, fill[k].dum);
  fill[k].meas_vs_Rate -> SetMarkerStyle(22);
  fill[k].meas_vs_Rate -> SetMarkerColor(r.Color);
  fill[k].meas_vs_Rate -> SetLineColor(r.Color);
  fill[k].meas_vs_Rate -> Draw("PL");

  //==============================================================================//
  //     No fit. Just check if the rate is not significantly low than expected    //
  //==============================================================================//
  if (Mode==0){
    // Maximum rate option
    //  0 : Get maximum rate from data set after "i"th datum
    //  1 : Get maximum rate from all data set
    //  2 : Get maximum rate from gaussian mean fit on universal rate target by target
    Int_t Option= flag.UniversalRate ? 2 : 0;

    for (Int_t i=0; i<fill[k].nRun; i++) {

      if (Option==0){
	Float_t DropRate = GetDropRate(k, i);
      }else if (Option==1){
	Int_t iMax;
	Float_t DropRate = GetDropRate(k, i, iMax);
	// draw max rate line
	TLine *l = new TLine(xmin, fill[k].Rate[iMax], xmax, fill[k].Rate[iMax]);
	l->SetLineColor(2);
	l->Draw("same");
      }else if (Option==2){
	Float_t RefRate;
	Float_t DropRate = GetDropRate(k, i, Color, RefRate);
	RelRate[fill[k].GlobalIndex[i]] = DropRate;
	if (PROFILE_ERROR){
	  if (RelRate[fill[k].GlobalIndex[i]]<1)  
	    dProf[fill[k].GlobalIndex[i]] = Color == 4 ? 0 : (1-RelRate[fill[k].GlobalIndex[i]]**(SigR_SigP*SigR_SigP))*fill[k].P_offline[i];
	  if (NORMALIZED_POL){
	    P_fillave[fill[k].GlobalIndex[i]]   = fill[k].wAve[0];
	    P_norm_Ave[fill[k].GlobalIndex[i]]  = fill[k].P_offline[i]/fill[k].wAve[0];
	    dP_norm_Ave[fill[k].GlobalIndex[i]] = 
	      QuadraticDivideError(fill[k].P_offline[i],fill[k].wAve[0],fill[k].dP_offline[i],fill[k].wAve[1]);
	  }
	}
	TLine *l = new TLine(xmin, RefRate, xmax, RefRate);
	l->SetLineColor(2);
	l->Draw("same");
      }


      // fill UnviversalRateDrop histograms
      Color == 4 ? blue.UniversalRateDrop -> Fill(DropRate) : yellow.UniversalRateDrop->Fill(DropRate);

      // Significant rate drop data extraction
      if ( DropRate < RATE_DROP_ALLOWANCE) {
	++j;
	fout << "    ====> RunID: " << std::setprecision(7) << fill[k].RunID[i] << " rate drop " 
	     << std::setprecision(3) << DropRate << " significant than limit :"  << RATE_DROP_ALLOWANCE << endl;
	sprintf(text,"%8.3f(%5.2f)",fill[k].RunID[i],DropRate);
	TText * tx = new TText(fill[k].Clock[i],fill[k].Rate[i],text);
	tx -> SetTextSize(0.08);
	tx -> Draw("same");
      }	
    }


  //==============================================================================//
  //                 Linear fit and check the standard deviation                  //
  //==============================================================================//
  } else if (Mode == 1){

    TF1 * f1 = new TF1("f1","pol1");
    f1 -> SetParLimits(1,-10000,0); // positive slope not allowed
    f1 -> SetLineColor(2);
    fill[k].meas_vs_Rate -> Fit(f1,"QB");
    if (fill[k].Chi2[0]>5) printf("Fill#%d Chi-2:%.1f\n",fill[k].FillID, fill[k].Chi2[0]);
    P0      = f1->GetParameter(0);
    P1      = f1->GetParameter(1);
      

    // Test deviation of each data point from the Linear fit
    for (Int_t i=0; i<fill[k].nRun; i++) fill[k].FitAve[i] = P0 + P1*fill[k].Clock[i];
    Float_t sdev = StandardDeviation(fill[k].nRun, fill[k].FitAve, fill[k].Rate);
    sprintf(text,"Standard Deviation %.2f",sdev);
    TText * tx = new TText(0,0.1,text);
    tx -> SetTextSize(0.08);
    tx -> Draw("same");


    // Find suspicious measurement for large standard deviation fills
    Int_t j=0;
    if (sdev > RATE_FIT_STANDARD_DEVIATION) {
      fout.precision(3);
      fout << "  Fill#" <<  fill[k].FillID << " Standard Deviation : " << sdev 
	   << " Exceeds limit:" << RATE_FIT_STANDARD_DEVIATION << endl;
      tx -> SetTextColor(2);
      tx -> Draw("same");
      ++Fill.bad.rate.fill;

      for (Int_t i=0; i<fill[k].nRun; i++) {
	Float_t diff = ((P0 + P1*fill[k].Clock[i]) - fill[k].Rate[i])/sdev;
	if (fabs(diff)>RATE_FIT_STANDARD_DEVIATION_DATA) {
	  fill[k].bad.Clock[j] = fill[k].Clock[i];
	  fill[k].bad.Rate[j]  = fill[k].Rate[i];
	  printf("%8.3f : %5.1f [MHz] off\n",fill[k].RunID[i], diff);
	  fout << "    ====> RunID:" << std::setprecision(7) << fill[k].RunID[i] << " is off by " 
	       << std::setprecision(3) << diff << " standard deviation limit "  << RATE_FIT_STANDARD_DEVIATION_DATA << endl;
	  sprintf(text,"%5.1f",diff);
	  TText * tx = new TText(fill[k].bad.Clock[j],fill[k].bad.Rate[j],text);
	  tx -> SetTextSize(0.08);
	  tx -> Draw("same");
	  ++j;
	} // end-of-fabs(diff>3)
	  
      } // end-of-for(Int_t i=0; i<fill[k].nRun)
      
    }// end-of-if (StandardDeviation > RATE_FIT_STANDARD_DEVIATION) 



    if (j) { // this superpose routine is not working 
      TGraph * tg = new TGraph(j, fill[k].bad.Clock, fill[k].bad.Rate);
      tg -> SetMarkerStyle(25);
      tg -> SetMarkerColor(6);
      tg -> Draw("same");
    }

    
  } // end-of-if (Mode)
  Fill.bad.rate.data += j;

  return;

}



//
// Class name  : OfflinePol
// Method name : GetDropRate(Int_t k, Int_t i)
//
// Description : Calculate Drop in Rate compared with the maximum rate exected after the "i"th measurement  
// Input       : Int_t k, Int_t i, Int_t &iMax
// Return      : Ratio between present "i"th Rate and max rate after "i"th measurement
//
Float_t
OfflinePol::GetDropRate(Int_t k, Int_t i){

  for (Int_t j=0; j<fill[k].nRun-i; j++) {
    fill[k].RateRest[j] = fill[k].Rate[i+j];
  }
  
  // GetMax(fill[k].RateRest, fill[k].nRun-i) returns the maximum rate from the
  // any measurements after "i"th one.
  return fill[k].Rate[i]/GetMax(fill[k].RateRest, fill[k].nRun-i); 

}




//
// Class name  : OfflinePol
// Method name : GetDropRate(Int_t k, Int_t i, Int_t &iMax)
//
// Description : Calculate Drop in Rate compared with the maximum rate of all the data in the fill
// Input       : Int_t k, Int_t i
// Return      : Ratio between present "i"th rate and max rate, and index "iMax"
//
Float_t
OfflinePol::GetDropRate(Int_t k, Int_t i, Int_t &iMax){

  Float_t rate =fill[k].Rate[i]/GetMax(fill[k].Rate, fill[k].nRun, iMax); 

  return rate;

}


//
// Class name  : OfflinePol
// Method name : GetDropRate(Int_t k, Int_t i, Int_t Color, Float_t &RefRate)
//
// Description : Calculate Drop in Rate compared with the reference rate determined from gaussian fit on 
//             : universal rate distribution target by target. This routine is custumized for blue/yellow 
//             : beams, respectively. The beam is distinguished by the argument "Int_t Color".
// Input       : Int_t k, Int_t i, Int_t Color, Float_t RefRate
// Return      : Reference rate and Ratio between present "i"th Rate and reference universal rate target by target
//
Float_t
OfflinePol::GetDropRate(Int_t k, Int_t i, Int_t Color, Float_t &RefRate){

  if (Color==4) { // blue beam
    if (RUN==5) blue.target[0].Mean = 0.02957 ; blue.target[1].Mean = 0.02313; blue.target[2].Mean = 0.02756;

    for (Int_t j=0;j<blue.Target.nPeriod; j++){
      if ((blue.target[j].Begin_RunID<=fill[k].RunID[i])&&(fill[k].RunID[i]<blue.target[j].End_RunID)) {
	RefRate = blue.target[j].Mean;
	break;
      }
    } // end-of-for(j<blue.Target.nPeriod) loop

  }else{ // yellow beam
    if (RUN==5) {
      yellow.target[0].Mean = 0.05133 ; yellow.target[1].Mean = 0.03301; yellow.target[2].Mean = 0.06175;
      yellow.target[3].Mean = 0.04321 ; yellow.target[4].Mean = 0.05000; yellow.target[5].Mean = 0.05000;
      yellow.target[6].Mean = 0.04353 ;
    }

    for (Int_t j=0;j<yellow.Target.nPeriod; j++){
      if ((yellow.target[j].Begin_RunID<=fill[k].RunID[i])&&(fill[k].RunID[i]<yellow.target[j].End_RunID)) {
	RefRate = yellow.target[j].Mean;
	break;
      }
    } // end-of-for(j<yellow.Target.nPeriod) loop

  } // end-of-if(Color)

  if (RefRate==0) {
    cerr << "GetDropRate::ERROR RefRate is zero. Check blue/yellow.target[i].Mean has finite value." << endl;
    exit;
  }

  return fill[k].Rate[i]/RefRate;

}
