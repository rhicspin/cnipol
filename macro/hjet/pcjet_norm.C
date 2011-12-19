
void pcjet_norm() {

   //TGraphErrors* gr = new TGraphErrors("pcjet_norm.dat");
   TGraphErrors* gr = new TGraphErrors("pcjet_norm_blu.dat");
   //TGraphErrors* gr = new TGraphErrors("pcjet_norm_yel.dat");

   gr->Fit("pol0");
	gr->Draw("A*");

}
