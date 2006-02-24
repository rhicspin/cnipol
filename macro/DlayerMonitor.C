#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TMinuit.h>
#include <iostream.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <iomanip>

#define Debug 0

Float_t ylgd=0.35;

void 
ReadPlot(Int_t Opt, Char_t Beam[], Char_t Mode[], Int_t Filln, 
	 Int_t Profile, Char_t DataSet[], Int_t Misc){

  
  Int_t Color=4;
  if (Beam[0] == 'Y') Color=95;
  if (Filln == 7133) Color=5;
  Int_t Mtyp=20;
  if (Mode == "INJ") Mtyp=24;

  if (Misc) {
    Color=Misc;
  }


    const Int_t N=60;
    Float_t Fill[N],day[N], rates[N], rdrates[N], wcmave[N], FillBunch[N];
    Float_t ave[N],wm[N],dwm[N],wcmtot[N],order[N],stprates[N],dev[N],wcmE[N];
    Float_t ratescrr[N],wmcrr[N];
    Float_t dx[N],dy[N];
    ifstream fin;
    Char_t filename[50];
    Char_t legend[20];
    Float_t ymax=0.25;
    if (!Profile){
      sprintf(filename,"dat/SampledExt-%s-%s_%s.dat",DataSet,Beam,Mode);
      sprintf(legend,"%s",Mode);
      if (Mode=="INJ") ymax=0.2;
      if (Misc){
	sprintf(legend,"%s",DataSet);
	if (strchr(DataSet,'Full')) sprintf(legend,"300to900");
      }
    } else {
      if (Profile==1) {
	sprintf(filename,"dat/Profile_%4d-%s.dat",Filln,Beam);
      }else{
	sprintf(filename,"dat/Profile_%2.1fsigma_%4d-%s.dat",Profile,Filln,Beam);
      }
      sprintf(legend,"Fill %4d",Filln);
      if (Filln==7151) ymax=0.2;
    }
    cout << "datafile: " << filename << endl;

    fin.open(filename);
    //    for (Int_t i=0; i<N; i++){
    Int_t i=0;
    while (!fin.eof()){
      fin >> Fill[i] >> day[i] 
	  >> rates[i] >> rdrates[i] >> wcmave[i] >> FillBunch[i]
	  >> ave[i] >> wm[i] >> dwm[i];
      //      rates[i]/=1e6;
      rdrates[i]/=1e6;  // [Hz]->[MHz]
      wcmtot[i]=wcmave[i]*FillBunch[i];
      wcmE[i] = wcmave[i]*0.05;
      order[i]=i+1;
      stprates[i]=rdrates[i]/70*1e3;
      dev[i]= (!Misc) ? rates[i] : 0;
      dx[i] = dy[i] = 0;
      ratescrr[i] = - (3.66*rdrates[i]*rdrates[i]-0.02*rdrates[i]);
      wmcrr[i] = wm[i] - (3.66*rdrates[i]*rdrates[i]-0.02*rdrates[i]);
      ++i;
    }

    Int_t NLINE=i-1;

    if (Opt==-1){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, ave, dx, dy);
    }else if (Opt==-70){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, wcmave, dx, dy);
    }else if (Opt==-60){
      TGraphErrors* tgae = new TGraphErrors(NLINE, wcmave, wm, dx, dwm);
    }else if (Opt==-50){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, wcmave, dx, dy);
    }else if (Opt==-40){
      TGraphErrors* tgae = new TGraphErrors(NLINE, rates, wcmtot, dx, dy);
    }else if (Opt==-30){
      TGraphErrors* tgae = new TGraphErrors(NLINE, rates, wcmave, dx, dy);
    }else if (Opt==-20){
      TGraphErrors* tgae = new TGraphErrors(NLINE, rates, FillBunch, dx, dy);
    }else if (Opt==-10){
      TGraphErrors* tgae = new TGraphErrors(NLINE, rates, rdrates, dx, dy);
    }else if (Opt==-2){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, ratescrr, dx, dy);
    }else if (Opt==0){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, wm, dx, dwm);
    }else if (Opt==1){
      TGraphErrors* tgae = new TGraphErrors(NLINE, rdrates, wm, dx, dwm);
    }else if (Opt==2){
      TGraphErrors* tgae = new TGraphErrors(NLINE, wcmave, wm, dx, dwm);
    }else if (Opt==3){
      TGraphErrors* tgae = new TGraphErrors(NLINE, wcmtot, wm, dx, dwm);
    }else if (Opt==4){
      TGraphErrors* tgae = new TGraphErrors(NLINE, order, wm, dx, dwm);
    }else if (Opt==5){
      TGraphErrors* tgae = new TGraphErrors(NLINE, stprates, wm, dx, dwm);
    }else if (Opt==6){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, stprates, dx, dwm);
    }else if (Opt==7){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, wcmave, dx, dwm);
    }else if (Opt==8){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, FillBunch, dx, dy);
    }else if (Opt==9){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, rdrates, dx, dy);
    }else if (Opt==10){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, wm, dx, dev);
    }else if (Opt==11){
      TGraphErrors* tgae = new TGraphErrors(NLINE, rdrates, wm, dx, dev);
    }else if (Opt==12){
      TGraphErrors* tgae = new TGraphErrors(NLINE, wcmave, wm, wcmE, dev);
    }else if (Opt==13){
      TGraphErrors* tgae = new TGraphErrors(NLINE, day, wmcrr, dx, dev);
    }else if (Opt==14){
      TGraphErrors* tgae = new TGraphErrors(NLINE, wcmave, wmcrr, wcmE, dev);
    }

    tgae -> SetMarkerStyle(Mtyp);
    tgae -> SetMarkerSize(1.5);
    tgae -> SetLineWidth(2);
    tgae -> SetMarkerColor(Color);
    tgae -> Draw("P");

    if (Misc) {
      if (strchr(Mode,'FTP')) {
	ylgd-=0.05;
	aLegend = new TLegend(0.70,ylgd-0.05,0.85,ylgd);
	aLegend ->AddEntry(tgae,legend,"P");
	aLegend ->Draw("same");
      }
    } else {

      aLegend = new TLegend(0.70,ymax-0.05,0.85,ymax);
      aLegend ->AddEntry(tgae,legend,"P");
      aLegend ->Draw("same");

      if ((Opt==13)||(Opt==14)) {
	aLegend = new TLegend(0.70,ymax-0.05,0.85,ymax);
	aLegend ->AddEntry(tgae,legend,"P");
	aLegend ->Draw("same");
      }

    } // end of if(Misc)
    fin.close();

}



// -------------------------------------------------------------------
//     SampledDlayers()
// -------------------------------------------------------------------
void SampledDlayers()
{


  Int_t Profile=0;
  Float_t RunID=10;
  Char_t Beam[10];
  Char_t DataSet[15];

  cout << "==========================" << endl;
  cout << " History           :    0 " << endl;
  cout << " Profile           :    1 " << endl;
  cout << " Profile(3.0sigma) :    3 " << endl;
  cout << "==========================" << endl;
  cin >> Profile;

  if (!Profile) {
    cout << "==========================" << endl;
    cout << "3.0sigma                  " << endl;
    cout << "ConstLumi                 " << endl;
    cout << "Chi2 (ConstLumi)          " << endl;
    cout << "Vtgt (Chi2 data)          " << endl;
    cout << "Full                      " << endl;
    cout << "Yellow                    " << endl;
    cout << "SpeLumi                   " << endl;
    cout << "TgtPos                    " << endl;
    //    cout << "300to700                  " << endl;
    //    cout << "300to600                  " << endl;
    cout << "H-ECutDep                  " << endl;
    cout << "L-ECutDep                  " << endl;
    cout << "==========================" << endl;
    cin >> DataSet ;
  }

  cout << "=======================" << endl;
  cout << " Blue or Yellow        " << endl;
  cout << "=======================" << endl;
  cin >> Beam;

  Int_t Opt;
  cout << "===========================" << endl;
  cout << " tgtpos vs. date     : -70 " << endl;
  cout << " dlayer vs. spelumi  : -60 " << endl;
  cout << " spelumi .cs date    : -50 " << endl;
  cout << " rates vs. WCM tot   : -40 " << endl;
  cout << " rates vs. WCM ave   : -30 " << endl;
  cout << " rates vs. fill bunch: -20 " << endl;
  cout << " rates vs. read rates: -10 " << endl;
  cout << " eve corr. vs. date  : -2  " << endl;
  cout << " dlayer vs. date     : -1  " << endl;
  cout << " dlayer (wm)vs. date :  0  " << endl;
  cout << " dlayer vs.read rates:  1  " << endl;
  cout << " dlayer vs.wcm ave   :  2  " << endl;
  cout << " dlayer vs.wcm tot   :  3  " << endl;
  cout << " dlayer vs. run #    :  4  " << endl;
  cout << " dlayer vs.rates/strp:  5  " << endl;
  cout << " strp rates vs. date :  6  " << endl;
  cout << " wcm ave  vs. date   :  7  " << endl;
  cout << " # of bunch vs. date :  8  " << endl;
  cout << " read rates vs. date :  9  " << endl;
  cout << " dlayer vs. date+chi2:  10 " << endl;
  cout << " dlayer vs. rdrt+chi2:  11 " << endl;
  cout << " dlayer vs.wcmav+chi2:  12 " << endl;
  cout << " rate corrected            " << endl;
  cout << " dlayer vs. date+chi2:  13 " << endl;
  cout << " dlayer vs.wcmav+chi2:  14 " << endl;
  cout << "===========================" << endl;
  cin >> Opt;
  
  Float_t xmin=1;
  Float_t xmax=90;
  Float_t ymin=20;
  Float_t ymax=80;
  if (strchr(DataSet,'ECutDep')){ ymin=30 ; ymax=60;}
  Char_t pstitle[30]="dates";
  Char_t xtitle[100]="Days from April 1st";
  Char_t ytitle[200]="Entrance Window Thickness [ug/cm^2]";
  if (Profile){
    ymin=35;
    ymax=60;
  }
  if ((Opt==1)||(Opt==11)) {
    sprintf(pstitle,"rates");
    xmin=0.;
    xmax=1.5;
    sprintf(xtitle,"Rates [MHz]"); 
  } else if ((Opt==2)||(Opt==12)||(Opt==14)) {
    sprintf(pstitle,"wcmave");
    xmin=0.;
    xmax=120;
    sprintf(xtitle,"Wall Current Monitor Average [10^9 protons]"); 
  } else if (Opt==3) {
    sprintf(pstitle,"wcmtot");
    xmin=0.;
    xmax=10000;
    sprintf(xtitle,"Wall Current Monitor Total [10^9 protons]"); 
  } else if (Opt==4) {
    sprintf(pstitle,"order");
    xmin=0.;
    xmax=10;
    sprintf(xtitle,"Measurement Order"); 
  } else if (Opt==5) {
    sprintf(pstitle,"rates_per_strip");
    xmin=0.;
    xmax=22;
    sprintf(xtitle,"Rates per Strip [kHz]"); 
  } else if (Opt==6) {
    sprintf(pstitle,"stprates_vs_date");
    ymin=0.;
    ymax=30;
    sprintf(xtitle,"dates from April 1st "); 
    sprintf(ytitle,"Rates per Strip [kHz] "); 
  } else if (Opt==7) {
    sprintf(pstitle,"wcmave_vs_date");
    ymin=0.;
    ymax=120;
    sprintf(xtitle,"dates from April 1st "); 
    sprintf(ytitle,"Average Wall Current Monitor [10^9 protons]"); 
  } else if (Opt==8) {
    sprintf(pstitle,"fillbunch_vs_date");
    ymin=0.;
    ymax=120;
    sprintf(xtitle,"dates from April 1st "); 
    sprintf(ytitle,"# of Bunches"); 
  } else if (Opt==9) {
    sprintf(pstitle,"rdrates_vs_date");
    ymin=0.;
    ymax=1.5;
    sprintf(ytitle,"Read Rates [MHz]"); 
  } else if (Opt==-2) {
    sprintf(pstitle,"RateCorr_vs_date");
    ymin=-8.;
    ymax=0;
    sprintf(xtitle,"Rates Correction[ug/cm^2]"); 
  }else if (Opt==-10) {
    sprintf(pstitle,"rates_vs_read");
    xmin=0.;
    xmax=1.5;
    ymin=0.;
    ymax=1.5;
    sprintf(xtitle,"Rates [MHz]"); 
    sprintf(ytitle,"Read Rates [MHz]"); 
  }else if (Opt==-20) {
    sprintf(pstitle,"rates_vs_fillbunch");
    xmin=0.;
    xmax=1.5;
    ymin=0.;
    ymax=120;
    sprintf(xtitle,"Rates [MHz]"); 
    sprintf(ytitle,"Fill Bunch"); 
  }else if (Opt==-30) {
    sprintf(pstitle,"rates_vs_wcmave");
    xmin=0.;
    xmax=1.5;
    ymin=0.;
    ymax=120;
    sprintf(xtitle,"Rates [MHz]"); 
    sprintf(ytitle,"WCM Average [10^9 protons]"); 
  }else if (Opt==-40) {
    sprintf(pstitle,"rates_vs_wcmtot");
    xmin=0.;
    xmax=1.5;
    ymin=0.;
    ymax=12000;
    sprintf(xtitle,"Rates [MHz]"); 
    sprintf(ytitle,"WCM total [10^9 protons]"); 
  }else if (Opt==-50) {
    sprintf(pstitle,"spelumi_vs_date");
    ymin=0.;
    ymax=0.2;
    sprintf(ytitle,"Normarized Specific Luminosity RMS "); 
  }else if (Opt==-60) {
    sprintf(pstitle,"dlayer_vs_spelumi");
    xmin=0;
    xmax=0.2;
    sprintf(ytitle,"Dlayer vs. Normarized Specific Luminosity RMS"); 
    sprintf(xtitle,"Normarized Specific Luminosity RMS "); 
  }else if (Opt==-70) {
    sprintf(pstitle,"tgtpos_vs_date");
    ymin = (!strcmp(Beam,"Blue")) ? 1100 : 1550;
    ymax = (!strcmp(Beam,"Blue")) ? 1200 : 1650;
    sprintf(ytitle,"Target Position [tgt counts]"); 
  }

    // ps file
    Char_t psfile[100];
    if (!Profile) {
      sprintf(psfile,"ps/SampledExtDlayers-%s_%s_%s.ps",DataSet,pstitle,Beam);
    } else if (Profile==1){
      sprintf(psfile,"ps/Profile_%s_%s.ps",pstitle,Beam);
    } else {
      sprintf(psfile,"ps/Profile_%2.1fsigma_%s_%s.ps",Profile,pstitle,Beam);
    }      
    TCanvas *CurC = new TCanvas("CurC","",1);
    TPostScript ps(psfile,112);

    // Cambus Setup
    CurC -> SetGridy();
    Char_t title[100];
    if (!Profile){
      sprintf(title,"Sampled DeadLayer Runs (%s) - %s",Beam,DataSet);
    }else{
      sprintf(title,"DeadLayers from Profile Runs (%s)",Beam);
    }
    TH2D* frame = new TH2D("frame",title, 10, xmin, xmax, 10, ymin, ymax);
    frame -> SetStats(0);
    frame -> GetXaxis()->SetTitle(xtitle);
    frame -> GetYaxis()->SetTitle(ytitle);
    frame -> Draw();


    // Call Main Read and Plotting Routine
    if (Profile) {
      ReadPlot(Opt, Beam,"FTP", 7151, Profile, DataSet, 0);
      if (Beam[0]=='Y')   ReadPlot(Opt, Beam,"FTP", 7133, Profile, DataSet, 0);
    } else {
      if (strchr(DataSet,'H-ECutDep')) {
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "300to600", 2);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "300to600", 2);
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "300to700", 3);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "300to700", 3);
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "Full", 1);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "Full", 1);
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "300to1000", 4);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "300to1000", 4);
	cout << "1" << DataSet << endl;
      }	else if (strchr(DataSet,'L-ECutDep')) {
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "250to900", 2);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "250to900", 2);
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "Full", 1);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "Full", 1);
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "400to900", 3);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "400to900", 3);
	ReadPlot(Opt, Beam,"FTP", 0, Profile, "500to900", 4);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, "500to900", 4);
	cout << "2" << DataSet << endl;
      } else {
	ReadPlot(Opt, Beam,"FTP", 0, Profile, DataSet, 0);
	ReadPlot(Opt, Beam,"INJ", 0, Profile, DataSet, 0);
      }
    }

    CurC -> Update();
    cout << "ps file" << psfile << endl;
    ps.Close();


}



