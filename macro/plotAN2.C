void plotAN2(){
 FILE *ifile;
 int i;


 double mp=0.93827;
 double Ptarget = 0.924;
 double dPtarget = 0.018;
 double Data_t[6],Err_t[6];
 double DataAN[6],DataErr[6],DataErrStat[6];//final
 double DataSysErr[6];


 //////////////////////
 //TargetPhys,BeamPhys,Err
 double BeamPhysBlue[6],TargetPhysBlue[6],BlueErr[6];
 TargetPhysBlue[0]=-0.039243;BeamPhysBlue[0]=0.020341;BlueErr[0]=0.000914;
 TargetPhysBlue[1]=-0.040520;BeamPhysBlue[1]=0.020076;BlueErr[1]=0.000939;
 TargetPhysBlue[2]=-0.040819;BeamPhysBlue[2]=0.018495;BlueErr[2]=0.000967;
 TargetPhysBlue[3]=-0.040041;BeamPhysBlue[3]=0.020347;BlueErr[3]=0.000992;
 TargetPhysBlue[4]=-0.038616;BeamPhysBlue[4]=0.018231;BlueErr[4]=0.001038;
 TargetPhysBlue[5]=-0.037101;BeamPhysBlue[5]=0.016961;BlueErr[5]=0.001047;


 for(i=0;i<6;++i){
	 DataAN[i]=-1*TargetPhysBlue[i];
	 DataErr[i]=BlueErr[i];
	 DataSysErr[i]=0;

	 DataAN[i]=DataAN[i]/(double)(Ptarget); 
    DataErr[i]=DataErr[i]/(double)Ptarget;
 }  

 double Data_tB[6];
 Data_tB[0]=1.24;Data_tB[1]=1.74;Data_tB[2]=2.25;Data_tB[3]=2.74;Data_tB[4]=3.25;Data_tB[5]=3.75;
 double Err_t[6]; 
 Err_t[0]=0.5;Err_t[1]=0.5;Err_t[2]=0.5;Err_t[3]=0.5;Err_t[4]=0.5;Err_t[5]=0.5;

 for(i=0;i<6;++i){
	Data_t[i]=Data_tB[i]*1E-3*2*mp;
	Err_t[i]=0;
 }

 TGraphErrors *gr_AN;
 gr_AN=new TGraphErrors(6,Data_t,DataAN,Err_t,DataErr);

///////////////////////Fit 
//|t|=((1E-2)-(2E-4))/(double)64;
fitTheoAn = new TF1("fitTheoAn",fitfunc3,1E-6,4E-1,5);//rho = -0.09

//////////////////////Graph
 gROOT->Reset();
 gROOT->SetStyle("Plain"); 
 gROOT->ForceStyle();

// -- Don't show histogram titles
gStyle->SetOptTitle(0);

// -- Remove stats box
gStyle->SetOptStat(0);

 gStyle->SetCanvasBorderMode(0);
 gStyle->SetFrameLineColor(1);
 gStyle->SetPadColor(0);
 gStyle->SetTitleFillColor(0);
 gStyle->SetStatColor(0);
 gStyle->SetOptStat(0000000);
 gStyle->SetOptFit(00000000);
 //gStyle->SetOptFit(11111111);

 double max,min;
 int num;

  max=0.05;
  min=0.0;
  num=6;//7.5;

  TCanvas *cc = new TCanvas("cc","AN",10,10,600,450);

   TH2F* frame = new TH2F("frame"," ",50, 0., 0.05, num, min, max);
   frame->SetTitle("");
   frame->GetXaxis()->SetLabelSize(0.05);
   frame->GetXaxis()->SetTitleSize(0.05);
   frame->GetYaxis()->SetLabelSize(0.05);
   frame->GetYaxis()->SetTitleSize(0.05);
   frame->GetXaxis()->SetTitle("-t (GeV/c)^{2}");
	frame->GetYaxis()->SetTitle("A_{N}");
	frame->GetYaxis()->SetTitleOffset(1.1);
   frame->SetStats(0);
   frame->Draw();

	cc->SetBottomMargin(0.11);
   cc->SetLeftMargin(0.13);

  cc ->SetLogx();

 double Imr5=0;
 double Rer5=0;
 double Imr5_err=0;
 double Rer5_err=0;

///Data
   gr_AN->SetMarkerStyle(20);gr_AN->SetMarkerSize(1);
   gr_AN->SetMarkerColor(1);gr_AN->SetLineColor(1);gr_AN->Draw("P");


	TLine l1;
	l1.SetLineWidth(1);
   l1.DrawLine(0,0,0.05,0);//iimax,maxh/4,iimax,0

	leg2 = new TLegend(0.7, 0.75, 0.95, 0.9);
   leg2 -> SetTextSize(0.04);
//    leg2 -> SetHeader("title");
   leg2 -> AddEntry(gr_AN, "RUN8 BLUE", "p");
	leg2 -> AddEntry(fitTheoAn, "fit", "l");
   leg2 -> Draw("");

   leg2 -> SetFillColor(0);//white filled box


fitTheoAn->SetRange(1E-3,5E-2);
fitTheoAn->FixParameter(0,1);
fitTheoAn->SetParameter(1,0);//Imr5
fitTheoAn->SetParameter(2,0);//Rer5
fitTheoAn->FixParameter(3,0);//Imr2
fitTheoAn->FixParameter(4,0);//Rer2
fitTheoAn->SetLineColor(3);
fitTheoAn->SetLineWidth(1);
fitTheoAn->SetLineStyle(1);

gr_AN->Fit(fitTheoAn,"R");


Imr5 = fitTheoAn ->GetParameter(1);
Rer5 = fitTheoAn ->GetParameter(2);
Imr5_err = fitTheoAn ->GetParError(1);
Rer5_err = fitTheoAn ->GetParError(2);



///////////////////////////
printf("Imr5 =%lf Rer5=%lf\n",Imr5,Rer5);
printf("Imr5_err =%lf Rer5_err=%lf\n",Imr5_err,Rer5_err);


 double cal_AN[256];
 double cal_t[256];

 double rho=-0.08;
 double fac=1; 
 double B=12;
 double sigma=38.4;

 f_AN(cal_t,cal_AN,rho,B,sigma,0,0,fac); //write 256 points


TGraph *gr;
//calc
 gr=new TGraph(256,cal_t,cal_AN);//r5=0 N=1, rho=-0.08

 cc-> cd();

 gr->SetLineColor(1);
 gr->SetLineWidth(2); gr->SetLineStyle(1); gr->Draw("L");



/////contour plot ImR5 vs. ReR5
double cRer5[1],cImr5[1];
double cRer5_err[1],cImr5_err[1];
cRer5[0]=fitTheoAn->GetParameter(2);
cImr5[0]=fitTheoAn->GetParameter(1);
cRer5_err[0]=0;//fitTheoAn2->GetParError(2);
cImr5_err[0]=0;//fitTheoAn2->GetParError(1);

TGraphErrors *gr_r5;
gr_r5=new TGraphErrors(1,cRer5,cImr5,cRer5_err,cImr5_err);
gr_r5->SetMarkerStyle(5);
gr_r5->SetMarkerSize(1);
gr_r5->SetMarkerColor(4);



TCanvas *c2 = new TCanvas("c2","AN",10,10,450,450);
int padcolor=0;
	c2->SetBottomMargin(0.15);
   c2->SetLeftMargin(0.2);

int padcolor=0;
//// inset ///////////////////////////////////////////////////////////////
//TPad *npad = new TPad("npad", "", 0.631, 0.539, 0.931, 0.939);		//
//npad->Draw("same");													//
//npad->SetFillStyle(0);												//
//npad->SetFillColor(2);// fill inside of "inset" plot					//
//npad->cd();															//
																		//
//gPad->SetGridx(1);  gPad->SetGridy(1);								//
																		//
// -- make enough space so that axis titles are not cut off				//
//gPad->SetBottomMargin(0.25);											//
//gPad->SetLeftMargin(0.25);											//
//////////////////////////////////////////////////////////////////////////


// -- make enough space so that axis titles are not cut off
int InsetMin=-0.1;
int InsetMax=0.02;
TH2F* frame2 = new TH2F("frame2"," ",10, -0.2, 0.2, 10, -0.4, 0.2);//E704
frame2->SetTitle("");
frame2->GetXaxis()->SetLabelSize(0.06);
frame2->GetXaxis()->SetTitleSize(0.06);
frame2->GetYaxis()->SetLabelSize(0.06);
frame2->GetYaxis()->SetTitleSize(0.06);
frame2->GetXaxis()->SetTitle("Re r5");
frame2->GetYaxis()->SetTitle("Im r5");
frame2->GetYaxis()->SetTitleOffset(1.5);
frame2->GetXaxis()->SetTitleOffset(1.2);
frame2->GetYaxis()->SetLabelOffset(0.02);
frame2->GetXaxis()->SetLabelOffset(0.03);
frame2->SetStats(0);
frame2->GetXaxis()->SetNdivisions(-2);
frame2->GetYaxis()->SetNdivisions(-3);//sigma-123
//frame2->GetYaxis()->SetNdivisions(-4);//sigma-1
//frame2->GetXaxis()->CenterTitle();
//frame2->GetYaxis()->CenterTitle();
frame2->Draw();

//get first contour for Re_r5(par2) versus Im_r5(par1)
  TGraph *gr12_1 = (TGraph*)gMinuit->Contour(300,2,1);//fineness, param1(Rer5), param2(Imr5)
  //Get contour for ERRDEF=1 
  gMinuit->SetErrorDef(1);// 1sigma
  gr12_1->SetLineColor(4);
  gr12_1->SetFillColor(7);
  gr12_1->SetLineWidth(1);
  gr12_1->Draw("+l");

 double matrix[2][2];
 gMinuit->mnemat(&matrix[0][0],2);

 for(i=0;i<2;++i){
	  for(j=0;j<2;++j){
       printf("%lf ",matrix[i][j]);
	  }printf("\n");
 }
 printf("matrix----------\n");//getchar();

   // Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
	//void mnstat(Double_t &fmin, Double_t &fedm, Double_t &errdef, Int_t &npari, Int_t &nparx, Int_t &istat)
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   gMinuit->mnprin(0,amin);    

	printf("amin=%lf edm =%lf errdef =%lf\n",amin,edm,errdef);
	printf("nvpar=%d nparx=%d icstat=%d\n",nvpar,nparx,icstat);
	
  //void mnerrs(Int_t number, Double_t &eplus, Double_t &eminus, Double_t &eparab, Double_t &gcc)
	Double_t eplus,eminus,eparab,gcc;
	int number = 1; //Imr5
	gMinuit->mnerrs(number,eplus,eminus,eparab,gcc);
	printf("param1 (Imr5) eplus=%lf eminus=%lf eparab=%lf gcc=%lf\n",eplus,eminus,eparab,gcc);
	number = 2;//Rer5
	gMinuit->mnerrs(number,eplus,eminus,eparab,gcc);
	printf("param2 (Rer5) eplus=%lf eminus=%lf eparab=%lf gcc=%lf\n",eplus,eminus,eparab,gcc);
	number = 0;//Rer5
	gMinuit->mnerrs(number,eplus,eminus,eparab,gcc);
	printf("param0 (Ptarget) eplus=%lf eminus=%lf eparab=%lf gcc=%lf\n",eplus,eminus,eparab,gcc);
	number = 3;//Imr2
	gMinuit->mnerrs(number,eplus,eminus,eparab,gcc);
	printf("param3 (Imr2) eplus=%lf eminus=%lf eparab=%lf gcc=%lf\n",eplus,eminus,eparab,gcc);
	number = 4;//Rer2
	gMinuit->mnerrs(number,eplus,eminus,eparab,gcc);
	printf("param4 (Rer2) eplus=%lf eminus=%lf eparab=%lf gcc=%lf\n",eplus,eminus,eparab,gcc);
	//getchar();

	double ccc=gMinuit->mnwerr();
   printf("ccc=%lf\n",ccc);

// Example illustrating how to draw the n-sigma contour of a Minuit fit.
// To get the n-sigma contour the ERRDEF parameter in Minuit has to set
// to n^2. The fcn function has to be set before the routine is called.
//
// WARNING!!! This test works only with TMinuit

   gMinuit->mnmatu(1);

  //Get contour for ERRDEF=2  
  gMinuit->SetErrorDef(4);//2sigma   
  TGraph *gr12_2 = (TGraph*)gMinuit->Contour(300,2,1);
  gr12_2->Draw("l");

  //Get contour for ERRDEF=3  
  gMinuit->SetErrorDef(9);//3sigma   
  TGraph *gr12_3 = (TGraph*)gMinuit->Contour(300,2,1);
  gr12_3->SetLineColor(1);
  gr12_3->Draw("l");

  gr_r5->Draw("P");


//TLine *lx = new TLine(-0.04, 0.0, 0.04, 0.);//JET-sigma1
TLine *lx = new TLine(-0.2, 0, 0.2, 0.);
lx -> SetLineColor(14);
lx->SetLineStyle(1);
lx -> Draw("same");

//TLine *ly = new TLine(0., -0.06, 0.0, 0.02);//JET-sigma1
TLine *ly = new TLine(0., -0.4, 0.0, 0.2);
ly -> SetLineColor(14);
ly->SetLineStyle(1);
ly -> Draw("same");




}

int f_AN(double cal_t[],double cal_AN[],double rho,double B, double sigma,double Imr5,double Rer5,double fac){
	double ans;
	int i;
double pi=3.14159;
double mp=0.93827;
double alpha=1/(double)137;
double mb2GeV_inv2=0.389;// 1GeV-2=0.389mb
double myuP=2.7938;
double sigma_totP;
double xx;
double ttt;
double tc;
double beta=0;
double delta=0;
double lamda2 = 0.71;//GeV^2
// double B = 12;//(GeV/c)^2
double ganma = 0.5772156;//Euler's constant
double light_c = 3E8;//m/s
double delta_t=((5E-2)-(1E-4))/(double)256;
//double Imr5=0;
//double Rer5=0;
double Imr2=0;
double Rer2=0;

beta = 0;
delta=0.02;

sigma_totP=sigma/(double)mb2GeV_inv2;

tc=-8*pi*alpha/(double)(sigma_totP*sqrt(1+rho*rho));
for(i=0;i<256;++i){
  cal_t[i]=0.5E-3+delta_t*i;
  xx=-1*tc/(double)cal_t[i];
  ttt=pow(xx,2)-2*(rho+delta)*xx+(1+rho*rho)*(1+beta*beta);

  ans=xx*((myuP-1)*(1-delta*rho)-2*(Imr5-delta*Rer5))-2*(Rer5-rho*Imr5);

  //ans=xx*((myuP-1)*(1-delta*rho+Imr2-delta*Rer2)-2*(Imr5-delta*Rer5))-2*(1+Imr2)*Rer5+2*(rho+Rer2)*Imr5;
  //ans=xx*(myuP-1)*(1-delta*rho);
  cal_AN[i]=fac*ans*sqrt(cal_t[i])/(double)(ttt*mp);
//   printf("cal_t=%lf cal_AN=%lf\n",cal_t[i],cal_AN[i]);
//   if(i%10==0 && i>0)getchar();
}
return 0;


}




Double_t fitfunc3(Double_t *x,Double_t *par){
 Double_t ans;

int i;
double pi=3.14159;
double mp=0.93827;
double alpha=1/(double)137;
double mb2GeV_inv2=0.389;// 1GeV-2=0.389mb
double myuP=2.7938;
double sigma_totP;
double xx;
double ttt;
double tc;
double rho=0;
double beta=0;
double delta=0;
double lamda2 = 0.71;//GeV^2
double B = 11;//13;//(GeV/c)^2
double ganma = 0.5772156;//Euler's constant
double light_c = 3E8;//m/s
double Imr2,Imr5,Rer2,Rer5;

Imr5=par[1];
Rer5=par[2];
Imr2=par[3];
Rer2=par[4];



rho = -0.0286;//pp
beta = 0;
delta=0.02;

 sigma_totP=38.4;//mb
sigma_totP=sigma_totP/(double)mb2GeV_inv2;

tc=-8*pi*alpha/(double)(sigma_totP*sqrt(1+rho*rho));
xx=-1*tc/(double)x[0];
ttt=pow(xx,2)-2*(rho+delta)*xx+(1+rho*rho)*(1+beta*beta);

ans=xx*((myuP-1)*(1-delta*rho+Imr2-delta*Rer2)-2*(Imr5-delta*Rer5))-2*(1+Imr2)*Rer5+2*(rho+Rer2)*Imr5;
//ans=xx*((myuP-1)*(1-delta*rho)-2*(Imr5-delta*Rer5))-2*(Rer5-rho*Imr5);

ans=ans*sqrt(x[0])/(double)(ttt*mp);
ans=ans*par[0];

 return(ans);
}
