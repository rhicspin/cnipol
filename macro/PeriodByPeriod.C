extern const Int_t MAX_NMEAS_PER_PERIOD;



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

  // Fill up Fill-by-Fill arrays first
  Int_t nFill   = FillByFillAnalysis(RUN, ndata);

  // Fill up Period-by-Period arrays next
  Int_t nPeriod = PeriodByPeriodAnalysis(RUN, nFill);

  // Make Period By Period Plot
  MakePeriodByPeriodPlot(nPeriod, Mode+15, Color, ps);

  return 0;

}


//
// Class name  : OfflinePol
// Method name : PeriodByPeriodAnalysis(Int_t nFill)
//
// Description : Stack data into Period by Period arrays
// Input       : Int_t RUN, Int_t ndata
// Return      : (Int_t)Number of Periods
//
Int_t 
OfflinePol::PeriodByPeriodAnalysis(Int_t RUN, Int_t nFill){

  // Inititiarization
  for (Int_t j=0; j<Period.nPeriod; j++) {
    period[j].nRun = 0;
    period[j].flag = 0;
  }


  // Fill-by-Fill loop
  for (Int_t i=0; i<nFill; i++){

    for (Int_t j=0; j<Period.nPeriod; j++) {

      if ((fill[i].FillID>=period[j].Begin_FillID)&&(fill[i].FillID<period[j].End_FillID)) {
	period[j].PeriodID = j+1;

	for (Int_t k=0; k<fill[i].nRun; k++) {

	  // reset clock to be zero for the first measurement for the period[j]
	  if (!period[j].flag){
	    period[j].Clock0 = fill[i].Time[k];
	    period[j].flag = 1;
	    cout << "========" << endl;
	  }

	  Int_t array_index = period[j].nRun;
	  period[j].RunID[array_index]      = fill[i].RunID[k];
	  period[j].P_online[array_index]   = fill[i].P_online[k];
	  period[j].dP_online[array_index]  = fill[i].dP_online[k];
	  period[j].P_offline[array_index]  = fill[i].P_offline[k];
	  period[j].dP_offline[array_index] = fill[i].dP_offline[k];
	  period[j].WCM[array_index]        = fill[i].WCM[k];
	  peroid[j].dum[array_index]        = 0;
	  if (k==fill[i].nRun-1) fill[i].dt[k+1] = 0;
	  period[j].dt[array_index]         = (fill[i].dt[k+1]/2+fill[i].dt[k]/2)/2/24;
	  period[j].Clock[array_index]      = (fill[i].Time[k] - period[j].Clock0)/3600/24;
	  //	  period[j].Weight[array_index]     = period[j].WCM[array_index]*period[j].dt[array_index];
	  period[j].Weight[array_index]     = period[j].WCM[array_index];

	  // some trick to plot horizonatal asymmetric error on plot
	  if (array_index==0) {
	    period[j].ClockM[array_index] = period[j].dt[array_index];
	  } else {
	    if (k) {
	      period[j].ClockM[array_index] = period[j].ClockM[array_index-1] + period[j].dt[array_index-1] + period[j].dt[array_index];
	    }else{
	      period[j].ClockM[array_index] = period[j].Clock[array_index] + period[j].dt[array_index];
	    }
	  }

	  // Print out period by period arrays for debugging purpose
	  //	  PrintPeriodByPeriodArray(i,k,j, array_index);
	  ++period[j].nRun;

	} // end-of-for(k=0;fill[i].nRun)

      } // end-of-for(i=0;period.nPeriod);

    }//end-of-if(fill[i].FillID)

  } // end-of-for(i=0;ndata) lope


  if (j==Period.nPeriod) {
    cout << "Total Number of Period = " << j << endl;
  }else{
    cerr << "Total Number of Period counter " << j << " mismatches with defined " << Period.nPeriod;
  }

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
    printf("period[%d].ClockM[%d]=%4.2f ",j, array_index, period[j].ClockM[array_index]);
    //    printf("period[%d].WCM[%d]=%4.1f ",j, array_index, period[j].WCM[array_index]);
    printf("\n");

    return 0;

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


  gStyle->SetOptStat(kFALSE);
  gStyle->SetTitleFontSize(0.08);
  TCanvas * C = new TCanvas("C","fill by fill", 1100, 850);
  C->Divide(1,4);

  ps->NewPage();

  Int_t j=0;
  for (Int_t k=0; k<nPeriod; k++) {

      if (period[k].nRun>1) {

	C->cd(j%4+1);

	PeriodByPeriodPlot(Mode, k, Color); C->Update();
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
//                   Bit 0 - P_Offline 
//                   Bit 1 - P_Offline polinomial-0 fit
//                   Bit 2 - Wall Current Monitor
//                   Bit 3 - 
//                   Bit 4 - 
//               Mode=7 (Offline,Online,Rate), Mode=9 (Offline,fit)
// Input       : Int_t Mode, Int_t k, Int_t Color
// Return      : 
//
Int_t 
OfflinePol::PeriodByPeriodPlot(Int_t Mode, Int_t k, Int_t Color){

  static Int_t hid;
  Char_t text[50];
  Float_t xmin=-0.05, xmax=10.5, ymin, ymax;
  Float_t margin=0.05;
  Float_t prefix_range=30; // Vertical Axis (Polariation) Range [%]. 

  // scaled intensity default parameters
  r.ymin=20;
  r.ymax=100;
  r.Color = 36;

  // 2D hist frame
  Char_t htitle[100], hname[100]; 
  sprintf(htitle,"Period#%d (Fill#%d - Fill#%d)",period[k].PeriodID, period[k].Begin_FillID, period[k].End_FillID);
  sprintf(hname,"Period%d",hid); ++hid;
  GetScale(period[k].Clock, period[k].nRun, margin, xmin, xmax);
  Float_t Range = GetScalePrefixRange(prefix_range, period[k].P_offline, period[k].nRun, margin, ymin, ymax);
  TH2F * periodbyperiod     = new TH2F(hname, htitle, 10, xmin, xmax, 100, ymin, ymax); 
  periodbyperiod->GetXaxis()->SetTitle("Duration from the first Measurement [day]");
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
  //  if (Mode>>4&1) period[k].meas_vs_P[1] -> Draw("P");

  // plot offline data points
  period[k].meas_vs_P[0] = new TGraphErrors(period[k].nRun, period[k].Clock, period[k].P_offline, period[k].dum, period[k].dP_offline);
  period[k].meas_vs_P[0] -> SetMarkerStyle(20);
  period[k].meas_vs_P[0] -> SetMarkerColor(Color);
  if (Mode>>0&1) period[k].meas_vs_P[0] -> Draw("P");


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
    A1 -> SetTitle("WCM Average [10^11 protons]");
    A1 -> SetLineColor(r.Color);
    A1 -> SetLabelColor(r.Color);
    A1 -> Draw("same");
      
    // Scale Rate to vertical axis(polarization). fill[k].Rate[i] -> fill[k].sRate[i] 
    for (Int_t i=0; i<period[k].nRun; i++) 
      period[k].sWCM[i] = (ymax - ymin)/(r.ymax - r.ymin)*(period[k].WCM[i] - r.ymax) + ymax;
  
    period[k].meas_vs_WCM = new TGraphErrors(period[k].nRun, period[k].Clock, period[k].sWCM, period[k].dum, period[k].dum);
    period[k].meas_vs_WCM -> SetMarkerStyle(22);
    period[k].meas_vs_WCM -> SetMarkerColor(r.Color);
    period[k].meas_vs_WCM -> SetLineColor(r.Color);
    period[k].meas_vs_WCM -> Draw("P");

  }


  // ------------------------------------------------------------------- // 
  //                WCM/dt Weighted Mean on Polarization                 // 
  // ------------------------------------------------------------------- // 

  Float_t Ave[2];
  calcWeightedMean(period[k].P_offline, period[k].dP_offline, period[k].Weight, period[k].nRun, Ave[0], Ave[1]);
  cout << period[k].PeriodID << " " << Ave[0] << " " << Ave[1] << endl;
  DrawLine(periodbyperiod, xmin, xmax, Ave[0], 2, 1, 3);
  sprintf(text,"ave(P)=%.2f +/- %.3f ", Ave[0], Ave[1]);
  DrawText(periodbyperiod, (xmin+xmax)/2, ymin*1.05, 2, text);

  return 0;

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

//
// Class name  : 
// Method name : DrawLine()
//
// Description : DrawLines in TH2F histogram
//             : Assumes  (x1,x2) y=y0=y1
// Input       : TH2F * h, float x0, float x1, float y, int color, int lstyle
// Return      : 
//
void 
DrawLine(TH2F * h, float x0, float x1, float y, int color, int lstyle, int lwidth){

  TLine * l = new TLine(x0, y, x1, y);
  l -> SetLineStyle(lstyle);
  l -> SetLineColor(color);
  l -> SetLineWidth(lwidth);
  h -> GetListOfFunctions()->Add(l);

  return;
}


//
// Class name  : 
// Method name : DrawText(TH2F * h, float x, float y, int color, char * text)
//
// Description : draw text on histogram. 
//             : 
// Input       : TH2F * h, float x, float y, int color, char * text
// Return      : 
//
void 
DrawText(TH2F * h, float x, float y, int color, char * text){

  TText * t = new TText(x, y, text);
  t->SetTextColor(color);
  t->SetTextSize(0.1);
  t->SetTextAlign(21);
  h->GetListOfFunctions()->Add(t);

  return;
}
