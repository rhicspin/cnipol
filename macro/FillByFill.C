extern Float_t RATE_FIT_STANDARD_DEVIATION ;
extern Float_t RATE_FIT_STANDARD_DEVIATION_DATA ;
extern Float_t POLARIZATION_FIT_CHI2 ;
extern Float_t POLARIZATION_FIT_SIGMA_DATA ;

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

  MakeFillByFillPlot(nFill, Mode-1000, ndata, Color, ps);


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
    if (fill[k].FillID == FillID ) FillByFillPlot(Mode-1000, k, Color); 
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
    fill[j].Index[array_index]      = fill[j].nRun;
    fill[j].RunID[array_index]      = RunID[i];
    fill[j].P_online[array_index]   = P_online[i] * sign;
    fill[j].dP_online[array_index]  = dP_online[i];
    fill[j].P_offline[array_index]  = P_offline[i] * sign;
    fill[j].dP_offline[array_index] = dP_offline[i];
    fill[j].Rate[array_index]       = Rate[i];
    fill[j].WCM[array_index]        = WCM[i];
    fill[j].Time[array_index]       = Float_t(Time[i]);
    fill[j].Clock[array_index]      = (Float_t(Time[i]) - fill[j].Clock0)/3600;
    fill[j].dum[array_index]        = 0;
    if (array_index) {
      fill[j].dt[array_index]       = fill[j].Clock[array_index]-fill[j].Clock[array_index-1];
    }else{
      fill[j].dt[array_index]       = 0;
    }


    // print out fill by fill arrays for debugging
    //    PrintFillByFillArray(i, j, array_index);

  } // end-of-for(i=0;ndata) lope

  cout << "Total Number of Fill = " << j+1 << endl;

  return j+1;

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
// Method name : FillByFillPlot(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : Make Plots Fill by Fill 
// Input       : Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
OfflinePol::MakeFillByFillPlot(Int_t nFill, Int_t Mode, Int_t ndata, Int_t Color, TPostScript *ps){

  if (Color==4){
    fout << "\n Blue:" << endl;
  }else{
    fout << "\n Yellow:" << endl;
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
  
  Int_t j=0;
  for (Int_t k=0; k<nFill; k++) {
    nRunPerFill -> Fill(fill[k].nRun);

    // Fill Ch-2 distribution histograms
      if (fill[k].nRun>1) {
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

  } else if (Mode>>4&1) {

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
//                   Bit 4 - Rate Fit
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
  if (Mode>>4&1) {ymin=r.ymin; ymax=r.ymax;};
  TH2F * fillbyfill     = new TH2F(hname, htitle, 10, xmin, xmax, 100, ymin, ymax); 
  fillbyfill->GetXaxis()->SetTitle("Hours from first measurement at store");
  fillbyfill->GetYaxis()->SetTitle("Polarization [%]");
  if (Mode>>4&1)  fillbyfill->GetYaxis()->SetTitle("Good Carbon Rate/WCM_sum [MHz]");

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
  //                            Rate Fit       .                         // 
  // ------------------------------------------------------------------- // 
  if (Mode>>4&1){
    RateFit(k, 0, xmin, xmax);
    // RateFit(k, 1, xmin, xmax);  // linear fit
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
// Method name : RateFit(Int_t k, Int_t Mode, Float_t xmin, Float_t xmax)
//
// Description : Extract major drop data in rate for fill[k].  
//             : Mode 1: Linear fit.and check the standard deviation
//             : Mode 0: No fit. Just compare with Maximum rate data point in the fill[k].
//                    Maximum rate option (Int_t Option)
//                       0 : Get maximum rate from data set after "i"th datum
//                       1 : Get maximum rate from all data set
// Input       : Int_t k, Int_t Mode, Float_t xmin, Float_t xmax
// Return      : 
//
void 
OfflinePol::RateFit(Int_t k, Int_t Mode, Float_t xmin, Float_t xmax){
  
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
    Int_t Option=0;

    for (Int_t i=0; i<fill[k].nRun; i++) {

      if (!Option){
	Float_t DropRate = GetDropRate(k, i);
      }else{
	Int_t iMax;
	Float_t DropRate = GetDropRate(k, i, iMax);
	// draw max rate line
	TLine *l = new TLine(xmin, fill[k].Rate[iMax], xmax, fill[k].Rate[iMax]);
	l->SetLineColor(2);
	l->Draw("same");
      }

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
