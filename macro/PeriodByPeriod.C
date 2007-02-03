extern const Int_t MAX_NMEAS_PER_PERIOD;


Float_t JetPol[2][3]  = {43.8, 48.2, 49.4,     // Blue
			 41.9, 41.1, 45.4};    // Yellow
Float_t JetdPol[2][3] = { 2.8,  3.8,  1.7,     // Blue   statistical error
			  2.9,  2.9,  1.4};    // Yellow statistical error
Float_t JetdTotPol[2][3] = { 3.0,  4.0,  2.0,  // Blue   total error (SysErr=1.1 Added in Quadruture)
			     3.1,  3.1,  1.8}; // Yellow total error (SysErr=1.1 Added in Quadruture)



//
// Class name  : OfflinePol
// Method name : PeroidByPeriodPlot(Int_t Mode, Int_t ndata, TCanvas *CurC, TPostScript *ps)
//
// Description : Main function of Peroid by Peroid Analysis 
// Input       : Int_t Mode, Int_t RUN, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps
// Return      : 
//
Int_t 
OfflinePol::PeroidByPeriod(Int_t Mode, Int_t RUN, Int_t ndata, Int_t Color, TCanvas *CurC, TPostScript *ps){

  Mode -= 2000;

  // Fill up Fill-by-Fill arrays first
  Int_t nFill   = FillByFillAnalysis(RUN, ndata);

  // Fill up Period-by-Period arrays next
  Int_t nPeriod = PeriodByPeriodAnalysis(RUN, nFill, Mode);

  // Make Period By Period Plot
  MakePeriodByPeriodPlot(nPeriod, Mode+15, Color, ps);

  // Compare with Jet
  if (!(Mode>>7&1)) JetComparison(nPeriod, Mode+15, Color, ps);

  if (Color!=4) Normalization(nPeriod, Mode+15, Color, ps);

  return 0;

}


//
// Class name  : OfflinePol
// Method name : PeriodByPeriodAnalysis(Int_t RUN, Int_t nFill, Int_t Mode)
//
// Description : Stack data into Period by Period arrays
//             : Mode bit-7 :
//             :        TRUE  - Don't combine periods
//             :        FALSE - Combine Same Jet Operation Types
//             : 
//	       : *** NOTE *** following line allocates 1 hour duration to duration dt for only one Run in thru the fill.
//             : This should make very small impact though, could be modified for more sophisticated manner
//             :  if (fill[i].nRun==1) period[index].dt[array_index]=1/24;  
//             : 
// Input       : Int_t RUN, Int_t ndata
// Return      : (Int_t)Number of Periods
//
Int_t 
OfflinePol::PeriodByPeriodAnalysis(Int_t RUN, Int_t nFill, Int_t Mode){

  // Inititiarization
  for (Int_t j=0; j<Period.nPeriod; j++) {
    period[j].nRun = 0;
    period[j].flag = 0;
  }


  // Fill-by-Fill loop
  Int_t index;
  for (Int_t i=0; i<nFill; i++){

    for (Int_t j=0; j<Period.nPeriod; j++) {

      if ((fill[i].FillID>=period[j].Begin_FillID)&&(fill[i].FillID<period[j].End_FillID)) {
	period[j].PeriodID = j+1;
	index=j;
	if (!(Mode>>7&1)) {period[j].PeriodID = period[j].Type; index=period[j].Type;}

	for (Int_t k=0; k<fill[i].nRun; k++) {

	  // reset clock to be zero for the first measurement for the period[index]
	  if (!period[index].flag){
	    period[index].Clock0 = fill[i].Time[k];
	    period[index].flag = 1;
	    cout << "========" << endl;
	  }

	  Int_t array_index = period[index].nRun;
	  period[index].RunID[array_index]      = fill[i].RunID[k];
	  period[index].P_online[array_index]   = fill[i].P_online[k];
	  period[index].dP_online[array_index]  = fill[i].dP_online[k];
	  period[index].P_offline[array_index]  = fill[i].P_offline[k];
	  period[index].dP_offline[array_index] = fill[i].dP_offline[k];
	  period[index].WCM[array_index]        = fill[i].WCM[k];
	  peroid[index].dum[array_index]        = 0;
	  if (k==fill[i].nRun-1) fill[i].dt[k+1] = 0;
	  period[index].dt[array_index]         = (fill[i].dt[k+1]/2+fill[i].dt[k]/2)/2/24;
	  if (fill[i].nRun==1) period[index].dt[array_index]=1./24.;  // alocate 1 hour 
	  period[index].Clock[array_index]      = (fill[i].Time[k] - period[index].Clock0)/3600/24;
	  period[index].Weight[array_index]     = period[index].WCM[array_index]*period[index].dt[array_index];

	  // some trick to plot horizonatal asymmetric error on plot
	  if (array_index==0) {
	    period[index].ClockM[array_index] = period[index].dt[array_index];
	  } else {
	    if (k) {
	      period[index].ClockM[array_index] = period[index].ClockM[array_index-1] + period[index].dt[array_index-1] + period[index].dt[array_index];
	    }else{
	      period[index].ClockM[array_index] = period[index].Clock[array_index] + period[index].dt[array_index];
	    }
	  }

	  // Print out period by period arrays for debugging purpose
	  //	  PrintPeriodByPeriodArray(i, k, index, array_index);
	  ++period[index].nRun;

	} // end-of-for(k=0;fill[i].nRun)

      } // end-of-for(i=0;period.nPeriod);

    }//end-of-if(fill[i].FillID)

  } // end-of-for(i=0;ndata) lope


  if (j==Period.nPeriod) {
    cout << "Total Number of Period = " << j << endl;
  }else{
    cerr << "Total Number of Period counter " << j << " mismatches with defined " << Period.nPeriod;
  }

  if (Mode==1) j=Period.nType;

  return j;

}




//
// Class name  : OfflinePol
// Method name : PrintPeriodByPeriodArray(Int_t i, Int_t k, Int_t j, Int_t array_index)
//
// Description : Print period by period arrays for diagnose
// Input       : Int_t i, Int_t k, Int_t j, Int_t array_index
// Return      : 
//
Int_t 
OfflinePol::PrintPeriodByPeriodArray(Int_t i, Int_t k, Int_t j, Int_t array_index){


    printf("i=%d ",i);
    printf("k=%d ",k);
    printf("j=%d ",j);
    printf("fill[%d].nRun=%d ",i,fill[i].nRun);
    printf("fill[%d].Index[%d]=%d ",i, k, fill[i].Index[k]);
    printf("fill[%d].RunID[%d]=%8.3f ",i, k, fill[i].RunID[k]);
    //    printf("fill[%d].P_offline[%d]=%5.1f ",i, k, fill[i].P_offline[k]);
    printf("fill[%d].dt[%d]=%4.2f ",i,k, fill[i].dt[k]);
    //    printf("fill[%d].Time[%d]=%4.1f ",i,k, fill[i].Time[k]);
    printf("period[%d].dt[%d]=%4.3f ",j, array_index, period[j].dt[array_index]);
    printf("period[%d].Clock[%d]=%4.1f ",j, array_index, period[j].Clock[array_index]);
    //    printf("period[%d].ClockM[%d]=%4.2f ",j, array_index, period[j].ClockM[array_index]);
   printf("period[%d].WCM[%d]=%4.1f ",j, array_index, period[j].WCM[array_index]);
    printf("\n");

    return 0;

}

//
// Class name  : OfflinePol
// Method name : PlotJet(Int_t Beam, Int_t index, Float_t xmin)
//
// Description : Plot jet average data point on the current frame
// Input       : (Int_t Beam, Int_t index, Float_t xmin)
// Return      : 
//
Int_t 
OfflinePol::PlotJet(Int_t Beam, Int_t index, Float_t xmin){


  // Total Error
  TGraphErrors * JetPlotTot = new TGraphErrors();
  JetPlotTot -> SetPoint(     0, xmin,  JetPol[Beam][index]);
  JetPlotTot -> SetPointError(0,    0, JetdTotPol[Beam][index]);
  JetPlotTot -> SetLineColor(6);
  JetPlotTot -> SetLineWidth(5);
  JetPlotTot -> Draw("P");

  // Statistical Error Only
  TGraphErrors * JetPlot = new TGraphErrors();
  JetPlot -> SetPoint(     0, xmin,  JetPol[Beam][index]);
  JetPlot -> SetPointError(0,    0, JetdPol[Beam][index]);
  JetPlot -> SetMarkerStyle(20);
  JetPlot -> SetMarkerColor(7);
  JetPlot -> SetMarkerSize(1.2);
  JetPlot -> SetLineColor(7);
  JetPlot -> SetLineWidth(5);
  JetPlot -> Draw("P");


  return 0; 

}


//
// Class name  : OfflinePol
// Method name : JetComparison(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps)
//
// Description : Compare Jet average and pC polarizations over the JetType combined periods
// Input       : (Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps)
// Return      : 
//
Int_t 
OfflinePol::JetComparison(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps){

  Int_t Beam = Color == 4 ? 0 : 1;

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.08);
  TCanvas * C = new TCanvas("C","fill by fill", 1100, 850);
  C->Divide(1,3);

  ps->NewPage();

  Float_t xmin; 

  if (Color==4){ // Blue
    C->cd(1) ; xmin=PeriodByPeriodPlot(Mode, 0, Color, Period.pC_Ave[Beam][0]); 
    PlotJet(Beam, 0, xmin); C->Update();
    C->cd(2) ; xmin=PeriodByPeriodPlot(Mode, 4, Color, Period.pC_Ave[Beam][1]); 
    PlotJet(Beam, 1, xmin); C->Update();
    C->cd(3) ; xmin=PeriodByPeriodPlot(Mode, 5, Color, Period.pC_Ave[Beam][2]); 
    PlotJet(Beam, 2, xmin); C->Update();
  } else {
    C->cd(1) ; xmin=PeriodByPeriodPlot(Mode, 0, Color, Period.pC_Ave[Beam][0]); 
    PlotJet(Beam, 0, xmin); C->Update();
    C->cd(2) ; xmin=PeriodByPeriodPlot(Mode, 2, Color, Period.pC_Ave[Beam][1]); 
    PlotJet(Beam, 1, xmin); C->Update();
    C->cd(3) ; xmin=PeriodByPeriodPlot(Mode, 3, Color, Period.pC_Ave[Beam][2]); 
    PlotJet(Beam, 2, xmin); C->Update();
  }

  C->Update(); ps->NewPage();

  return 0 ;

}

//
// Class name  : OfflinePol
// Method name : Normalization(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps)
//
// Description : Compare Jet average and pC polarizations over the JetType combined periods
// Input       : (Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps)
// Return      : 
//
Int_t 
OfflinePol::Normalization(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps){
  

  // =========================================================================
  //                   Normalization 
  // =========================================================================
  TCanvas * C1 = new TCanvas();
  TH2F * norm = new TH2F("norm","Jet vs. pC", 6, 0.5, 6.5, 10, 38, 55); 
  norm -> Draw("");

  Int_t Beam=0;
  TGraphErrors * pC = new TGraphErrors();
  pC -> SetPoint(0, 1, Period.pC_Ave[Beam][0][0]); pC -> SetPointError(0, 0, Period.pC_Ave[Beam][0][1]); 
  pC -> SetPoint(1, 2, Period.pC_Ave[Beam][1][0]); pC -> SetPointError(1, 0, Period.pC_Ave[Beam][1][1]); 
  pC -> SetPoint(2, 3, Period.pC_Ave[Beam][2][0]); pC -> SetPointError(2, 0, Period.pC_Ave[Beam][2][1]); 
  pC -> SetMarkerStyle(20);
  pC -> SetMarkerColor(4);
  pC -> Draw("P"); 

  Int_t Beam=1;
  TGraphErrors * pC2 = new TGraphErrors();
  pC2 -> SetPoint(0, 4, Period.pC_Ave[Beam][0][0]); pC -> SetPointError(0, 0, Period.pC_Ave[Beam][0][1]); 
  pC2 -> SetPoint(1, 5, Period.pC_Ave[Beam][1][0]); pC -> SetPointError(1, 0, Period.pC_Ave[Beam][1][1]); 
  pC2 -> SetPoint(2, 6, Period.pC_Ave[Beam][2][0]); pC -> SetPointError(2, 0, Period.pC_Ave[Beam][2][1]); 
  pC2 -> SetMarkerStyle(20);
  pC2 -> SetMarkerColor(Color);
  pC2 -> Draw("P"); 

  PlotJet(0, 0, 1.1); 
  PlotJet(0, 1, 2.1); 
  PlotJet(0, 2, 3.1); 
  PlotJet(1, 0, 4.1); 
  PlotJet(1, 1, 5.1); 
  PlotJet(1, 2, 6.1); 


  /*
  C->Update(); ps->NewPage();

  TH2F * Ratio = new TH2F("Ratio","Normarization", 6, 0.5, 6.5, 10, 0.5, 1.5); 
  Ratio -> Draw("");

  Float_t ratio[6], dratio[6], dx[6]; 
  Float_t x[6]={1,2,3,4,5,6};
  ratio[0] = Period.pC_Ave[0][0][0]/JetPol[0][0];
  ratio[1] = Period.pC_Ave[0][1][0]/JetPol[0][1];
  ratio[2] = Period.pC_Ave[0][2][0]/JetPol[0][2];
  ratio[3] = Period.pC_Ave[1][0][0]/JetPol[1][0];
  ratio[4] = Period.pC_Ave[1][1][0]/JetPol[1][1];
  ratio[5] = Period.pC_Ave[1][2][0]/JetPol[1][2];

  dratio[0] = JetPol[0][0];
  dratio[1] = JetPol[0][1];
  dratio[2] = JetPol[0][2];
  dratio[3] = JetPol[1][0];
  dratio[4] = JetPol[1][1];
  dratio[5] = JetPol[1][2];

  TGraphErrors * tg = new TGraphErrors(6, x, ratio, dx, dratio);
  tg->SetMarkerStyle(20);
  tg->SetMarkerColor(2);
  tg->Draw("P");
  */



  return 0 ;

}



//
// Class name  : OfflinePol
// Method name : MakePeriodByPeriodPlot(Int_t nPeriod, Int_t Mode, Int_t ndata, Int_t Color)
//
// Description : Make peroid by period plot
// Input       : Int_t nPeriod, Int_t Mode, Int_t ndata, Int_t Color
// Return      : 
//
Int_t 
OfflinePol::MakePeriodByPeriodPlot(Int_t nPeriod, Int_t Mode, Int_t Color, TPostScript *ps){

  Float_t Ave[2];

  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.08);
  TCanvas * C = new TCanvas("C","fill by fill", 1100, 850);
  C->Divide(1,4);

  ps->NewPage();

  Int_t j=0;
  for (Int_t k=0; k<nPeriod; k++) {

      if (period[k].nRun>1) {

	C->cd(j%4+1);

	PeriodByPeriodPlot(Mode, k, Color, Ave); C->Update();
	if ((j+1)%4==0) ps->NewPage();
	++j;

      }

  } // for (k=0; k<nPeroid)

  C->Update(); ps->NewPage();

  return 0;

}



//
// Class name  : OfflinePol
// Method name : PeriodByPeriodPlot(Int_t Mode, Int_t k)
//
// Description : Make Plots Period by Period 
//               Mode Description
//                   Bit 0 -
//                   Bit 1 - P_Offline Liniear fit
//                   Bit 2 - Plot Wall Current Monitor 
//                   Bit 3 - 
//                   Bit 4 - 
//                   Bit 5 - 
//                   Bit 6 - 
//                   Bit 7 - TRUE:duration[day], FALSE:FillID on x-axis
// Input       : Int_t Mode, Int_t k, Int_t Color
// Return      : Float_t xmin
//
Float_t 
OfflinePol::PeriodByPeriodPlot(Int_t Mode, Int_t k, Int_t Color, Float_t Ave[]){


  static Int_t hid;
  Char_t text[50];
  Float_t xmin=-0.05, xmax=10.5, ymin, ymax;
  Float_t margin=0.05;
  Float_t prefix_range=30; // Vertical Axis (Polariation) Range [%]. 
  Float_t xarray[MAX_NMEAS_PER_PERIOD];

  // scaled intensity default parameters
  r.ymin=20;
  r.ymax=100;
  r.Color = 36;

  // 2D hist frame
  Char_t htitle[100], hname[100], xtitle[100]; 
  if (Mode>>7&1){
    sprintf(htitle,"Period#%d (Fill#%d - Fill#%d)",period[k].PeriodID, period[k].Begin_FillID, period[k].End_FillID);
    sprintf(hname,"Period%d",hid); ++hid;
    sprintf(xtitle,"Duration from the first Measurement [day]");
    for (Int_t i=0; i<period[k].nRun; i++) xarray[i] = period[k].Clock[i];
  } else {
    sprintf(htitle,"Period Type-%d", k);
    sprintf(hname,"Period%d",hid); ++hid;
    sprintf(xtitle,"Fill Number");
    for (Int_t i=0; i<period[k].nRun; i++) xarray[i] = period[k].RunID[i];
  }

  GetScale(xarray, period[k].nRun, margin, xmin, xmax);
  Float_t  Range = GetScalePrefixRange(prefix_range, period[k].P_offline, period[k].nRun, margin, ymin, ymax);
  TH2F * periodbyperiod = new TH2F(hname, htitle, 10, xmin, xmax, 100, ymin, ymax); 
  periodbyperiod->GetXaxis()->SetTitle(xtitle);
  periodbyperiod->GetYaxis()->SetTitle("Polarization [%]");

  // Print Vertical Range
  sprintf(text,"V-Range = %d", Range);
  TText * t1 = new TText(0, ymin*1.1, text);
  if (Range>prefix_range+1) {
    periodbyperiod->GetYaxis()->SetLabelColor(2);
    periodbyperiod->SetAxisColor(2,"Y");
    t1->SetTextColor(2);
  }

  // Draw Histogram and texts
  periodbyperiod->Draw();
  if (Mode>>3&0) t1->Draw("same");

  // plot offline data points horizontal error bars
  period[k].meas_vs_P[1] = new TGraphErrors(period[k].nRun, period[k].ClockM, period[k].P_offline, period[k].dt, period[k].dum);
  if (Mode>>7&1) period[k].meas_vs_P[1] -> Draw("P");

  // plot offline data points
  period[k].meas_vs_P[0] = new TGraphErrors(period[k].nRun, xarray, period[k].P_offline, period[k].dum, period[k].dP_offline);
  period[k].meas_vs_P[0] -> SetMarkerStyle(20);
  period[k].meas_vs_P[0] -> SetMarkerColor(Color);
  period[k].meas_vs_P[0] -> Draw("P");

  // ------------------------------------------------------------------- // 
  //                    Linear Fit on Polarization                       // 
  // ------------------------------------------------------------------- // 
  if (Mode>>1&&1){
    gStyle->SetOptFit(111);
    TF1 * f1 = new TF1("f1","pol0");
    f1 -> SetLineStyle(2);
    f1 -> SetLineWidth(2);
    f1 -> SetLineColor(3);
    period[k].meas_vs_P[0] -> Fit(f1,"Q");
  }


  // ------------------------------------------------------------------- // 
  //                        Superpose WCM plot.                          // 
  // ------------------------------------------------------------------- // 
  if (Mode>>2&1){
    TGaxis * A1 = new TGaxis(xmax, ymin, xmax, ymax, r.ymin, r.ymax, 510, "+L");
    A1 -> SetTitle("WCM Sum [10^11 protons]");
    A1 -> SetLineColor(r.Color);
    A1 -> SetLabelColor(r.Color);
    A1 -> Draw("same");
      
    // Scale Rate to vertical axis(polarization). fill[k].Rate[i] -> fill[k].sRate[i] 
    for (Int_t i=0; i<period[k].nRun; i++) 
      period[k].sWCM[i] = (ymax - ymin)/(r.ymax - r.ymin)*(period[k].WCM[i] - r.ymax) + ymax;
  
    period[k].meas_vs_WCM = new TGraphErrors(period[k].nRun, xarray, period[k].sWCM, period[k].dum, period[k].dum);
    period[k].meas_vs_WCM -> SetMarkerStyle(22);
    period[k].meas_vs_WCM -> SetMarkerColor(r.Color);
    period[k].meas_vs_WCM -> SetLineColor(r.Color);
    period[k].meas_vs_WCM -> Draw("P");

  }


  // ------------------------------------------------------------------- // 
  //                WCM/dt Weighted Mean on Polarization                 // 
  // ------------------------------------------------------------------- // 

  calcWeightedMean(period[k].P_offline, period[k].dP_offline, period[k].Weight, period[k].nRun, Ave[0], Ave[1]);
  cout << period[k].PeriodID << " " << Ave[0] << " " << Ave[1] << endl;
  DrawLine(periodbyperiod, xmin, xmax, Ave[0], 2, 1, 3);
  DrawLine(periodbyperiod, xmin, xmax, Ave[0]+Ave[1], 2, 3, 2);
  DrawLine(periodbyperiod, xmin, xmax, Ave[0]-Ave[1], 2, 3, 2);
  sprintf(text,"ave(P)=%.2f +/- %.3f ", Ave[0], Ave[1]);
  DrawText(periodbyperiod, (xmin+xmax)/2, ymin*1.05, 2, text);

  return xmin;

}


//
// Class name  : 
// Method name : WeightedMean(Float_t A[N], Float_t dA[N], int NDAT)
//
// Description : calculate weighted mean
// Input       : Float_t A[N], Float_t dA[N], int NDAT
// Return      : weighted mean
//
Float_t
WeightedMean(Float_t A[MAX_NMEAS_PER_PERIOD], Float_t dA[MAX_NMEAS_PER_PERIOD], Float_t W[MAX_NMEAS_PER_PERIOD], Int_t NDAT){

  Float_t sum1, sum2, dA2, WM;
  sum1 = sum2 = dA2 = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){  // skip dA=0 data
      dA2 = dA[i]*dA[i];
      sum1 += W[i]*A[i]/dA2 ;
      sum2 += W[i]/dA2 ;
    }
  }

  WM = dA2 == 0 ? 0 : sum1/sum2 ;
  return WM ;

} // end-of-WeightedMean()


//
// Class name  : 
// Method name : WeightedMeanError(Float_t dA[N], int NDAT)
//
// Description : calculate weighted mean error 
// Input       : Float_t dA[N], int NDAT
// Return      : weighted mean error
//
Float_t 
WeightedMeanError(Float_t dA[MAX_NMEAS_PER_PERIOD], Float_t W[MAX_NMEAS_PER_PERIOD], Int_t NDAT){

  Float_t sum, dA2, dWM;
  sum = dA2 = dWM = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){
      dA2 = dA[i]*dA[i];
      sum += 1/dA2 ;
    }
  }

  dWM = sum == 0 ? 0 : sqrt(1/sum);
  return dWM ;

} // end-of-WeightedMeanError()




//
// Class name  : 
// Method name : calcWeightedMean(Float_t A[MAX_NMEAS_PER_PERIOD], Float_t dA[MAX_NMEAS_PER_PERIOD], 
//                                Float_t W[MAX_NMEAS_PER_PERIOD], Int_t NDAT, Float_t &Ave, Float_t &dAve);
//
// Description : call weighted mean for weight array W[] and error 
// Input       : Float_t A[], Float_t dA[], Float_t W[], Float_t Ave, int NDAT
// Return      : Ave, dAve
//
Int_t
calcWeightedMean(Float_t A[MAX_NMEAS_PER_PERIOD], Float_t dA[MAX_NMEAS_PER_PERIOD], Float_t W[MAX_NMEAS_PER_PERIOD], 
		 Int_t NDAT, Float_t &Ave, Float_t &dAve){

  Ave  = WeightedMean(A, dA, W, NDAT);
  dAve = WeightedMeanError(dA, W, NDAT);

  return 0;

}
