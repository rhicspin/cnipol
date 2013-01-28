//--------------------------------
// MinuitFit()
// Maximum Likelihood fitting
//--------------------------------

#include "TMinuit.h"
#include "TStopwatch.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"

#include <iostream>
#include <iomanip>
#include <time.h>

#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <map>

using namespace std;
using std::cout;
using std::endl;
using std::setw;


/*
  Jan. 9, 2012 : change rho to 0.1278 and sigma_tot = 51.79
*/
Double_t  AN_theory(Double_t minus_t, Double_t Rer5, Double_t Imr5) {

  //  Double_t alpha = 7.297352568E-3 ;
  Double_t alpha = 7.2973525376E-3 ;
  //  Double_t tc = -8 * TMath::Pi() * alpha / 51.6  * 0.389379 ;  // last factor converting mb to GeV
  Double_t tc = -8 * TMath::Pi() * alpha / 51.79  * 0.389379 ;  // last factor converting mb to GeV

  //  Double_t rho = 0.13 ;
  Double_t rho   = 0.1278 ; // +/- 0.0015
  Double_t delta = alpha*TMath::Log( 2./( TMath::Abs(minus_t)*(16.3+8./0.71) ) ) - alpha*0.5772 ;

  Double_t nom   = ( (1.792847351*(1-rho*delta) + 2*(delta*Rer5 - Imr5))*tc/(-1.*minus_t)  - 2*(Rer5-rho*Imr5) );
  Double_t denom = (tc*tc/minus_t/minus_t) + 2*(rho+delta)*tc/minus_t + (1+rho*rho) ;

  //  return TMath::Sqrt(minus_t)*nom/denom/0.93827203 ;
  return TMath::Sqrt(minus_t)*nom/denom/0.938272013 ;

}


Double_t derivative(Double_t minus_t_l, Double_t minus_t_h, Double_t Rer5, Double_t Imr5)
{
  return (  AN_theory(minus_t_h, Rer5, Imr5) -  AN_theory(minus_t_l, Rer5, Imr5) ) / (minus_t_h - minus_t_l) ;
}


//fcn loop
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{ //{{{
  Double_t ChiSquare = 0 ;

  /* 2003
  const Int_t maxdim = 3 ;
  Double_t minus_t[maxdim] = { 0.0127, 0.0175, 0.0236 } ;
  Double_t AN[maxdim] = { 0.0277, 0.0250, 0.0178 } ;
  //  Double_t ANerror[maxdim] = { 0.00765, 0.00599, 0.00422 } ;
  Double_t ANerror[maxdim] = { 0.00652, 0.00479, 0.00335 } ;
  */

  // 2009
  //  const Int_t maxdim = 4 ;
  // Mar. 2010 (for STAR collaboration meeting)
  //  Double_t minus_t[maxdim] = { 0.0075, 0.0127, 0.0175, 0.0236 } ;
  //  Double_t AN[maxdim] = { 0.03314, 0.02268, 0.01638, 0.01584 } ;
  //  Double_t ANerror[maxdim] = { 0.00202, 0.00188, 0.00188, 0.00201 } ;
  //
  // Apr. 20, 2010:
  //  Double_t minus_t[maxdim] = { 0.00768, 0.01253, 0.01741, 0.02317 } ;

  //  Double_t AN[maxdim] = { 0.03014, 0.02363, 0.01893, 0.01705 } ;
  //  Double_t ANerror[maxdim] = { 0.00081, 0.00070, 0.00069, 0.00072 } ;

  // May 3, 2010 (new TAC/fixing last 5 runs):
  //  Double_t AN[maxdim] = { 0.03027, 0.02322, 0.01916, 0.01718 } ;
  //  Double_t ANerror[maxdim] = { 0.00077, 0.00068, 0.00068, 0.00071 } ;

  // Not-Gold (May 25, 2010)
  //  Double_t AN[maxdim] = { 0.02949, 0.02392, 0.02034, 0.0174 } ;
  //  Double_t ANerror[maxdim] = { 0.00130, 0.00101, 0.00094, 0.00090 } ;

  // Gold (May 25, 2010)
  //  Double_t AN[maxdim] = { 0.03097, 0.02292, 0.01757, 0.01689 } ;
  //  Double_t ANerror[maxdim] = { 0.00100, 0.00095, 0.00102, 0.00125 } ;

  // No-Max-Cluster-Cut (June 1, 2010) and 18 bins
  /*
  const Int_t maxdim = 4 ;
  Double_t minus_t[maxdim] = { 0.00768, 0.01253, 0.01741, 0.02317 } ;
  Double_t AN[maxdim] = { 0.03016, 0.02318, 0.01918, 0.01705 } ;
  Double_t ANerror[maxdim] = { 0.00077, 0.00068, 0.00068, 0.00072 } ;
  */

  /* Also June 1, 2010 --- if we make use of -t < 0.005
  const Int_t maxdim = 5 ;
  Double_t minus_t[maxdim] = { 0.003975, 0.00768, 0.01253, 0.01741, 0.02317 } ;
  Double_t AN[maxdim] = { 0.03942, 0.03016, 0.02318, 0.01918, 0.01705 } ;
  Double_t ANerror[maxdim] = { 0.0015, 0.00077, 0.00068, 0.00068, 0.00072 } ;
  */

  /* June 10, 2010 (divided into 6 bins)
  const Int_t maxdim = 6 ;
  Double_t minus_t[maxdim] = { 0.005513, 0.009293, 0.01251, 0.01573, 0.01943, 0.02442 } ;
  Double_t AN[maxdim] =      { 0.03609, 0.02577, 0.02354, 0.02078, 0.01841, 0.01684 } ;
  Double_t ANerror[maxdim] = { 0.00091, 0.00086, 0.00087, 0.00080, 0.00077, 0.00086 } ;
  */

  /* Also June 23, 2010 --- modified 5 -t bins (including < -t < 0.005 and -t > 0.03
  const Int_t maxdim = 5 ;
  Double_t minus_t[maxdim] = { 0.003975, 0.00768, 0.01253, 0.01741, 0.02336 } ;
  Double_t AN[maxdim] = { 0.03942, 0.03016, 0.02318, 0.01918, 0.01706 } ;
  Double_t ANerror[maxdim] = { 0.0015, 0.00077, 0.00068, 0.00068, 0.00070 } ;
  */

  /* Shouldn't have included polarization errors
  // June 29, 2010 --- modified 5 -t bins (including < -t < 0.005 and -t > 0.03
  // Different polarizations in different periods ...
  const Int_t maxdim = 5 ;
  Double_t minus_t[maxdim] = { 0.003975, 0.00768, 0.01253, 0.01741, 0.02336 } ;
  Double_t AN[maxdim] = { 0.03834, 0.02951, 0.02273, 0.01880, 0.01664 } ;
  Double_t ANerror[maxdim] = { 0.00149, 0.00077, 0.00067, 0.00070, 0.00070 } ;
  */

  /* June 30, 2010 --- modified 5 -t bins (including < -t < 0.005 and -t > 0.03
  // Different polarizations in different periods ... (no pol. error involved)
  const Int_t maxdim = 5 ;
  Double_t minus_t[maxdim] = { 0.003975, 0.00768, 0.01253, 0.01741, 0.02336 } ;
  Double_t AN[maxdim] = { 0.03860, 0.02975, 0.02290, 0.01894, 0.01677 } ;
  Double_t ANerror[maxdim] = { 0.00148, 0.00076, 0.00067, 0.00067, 0.00069 } ;
  */

  // Oct. 4, 2010 5 -t bins
  const Int_t maxdim = 5 ;
  //  Double_t minus_t[maxdim] = { 0.003927, 0.007775, 0.01255, 0.01741, 0.02335 } ;
  // putting in polarization = 1.22402
  //  Double_t AN[maxdim] = {       0.0379049, 0.0289560,   0.0225266,   0.0182762,    0.0165966 } ;
  //  Double_t ANerror[maxdim] = { 0.00154632, 0.000774588, 0.000669098, 0.000669893, 0.000731630 } ;
  // Dividing raw_asymmetry by polarization of that store
  //  Double_t AN[maxdim] = {       0.0379359,  0.0291207,   0.0227017,   0.0184137,    0.0166414 } ;
  //  Double_t ANerror[maxdim] = { 0.00154989, 0.000781428, 0.000673336, 0.000673475,  0.000735353 } ;
  // After phi~ pi/-pi correction
  //  Double_t AN[maxdim] = {       0.0398449,  0.0299221,   0.0230140,   0.0188722,    0.0167721 } ;
  //  Double_t ANerror[maxdim] = { 0.00153445, 0.000777224, 0.000672145, 0.000672611,  0.000733029 } ;
  //  Double_t AN[maxdim] = {       0.0398557,  0.0299213,   0.0230122,   0.0188697,    0.0167720 } ;
  //  Double_t ANerror[maxdim] = { 0.00153449, 0.000777229, 0.000672130, 0.000672590,  0.000732946 } ;

  // Feb. 2, 2011 5 -t bins
  //  Double_t AN[maxdim] = {       0.0396061,  0.0299866,   0.0228288,   0.0187340,    0.0171175 } ;
  //  Double_t ANerror[maxdim] = { 0.00150063, 0.000763740, 0.000665213, 0.000667173,  0.000736930 } ;

  // July 26, 2011 (5 -t bins, after hotspot/fiducial -- wrong due to -ve sigma for hotspot) : t10
  //  Double_t AN[maxdim] = {      0.0396370,  0.0297613,   0.0228931,   0.0184411,    0.0171746 } ;
  //  Double_t ANerror[maxdim] = { 0.00148228, 0.000759952, 0.000664114, 0.000665168,  0.000740819 } ;

  /* July 27, 2011 (5 -t bins, after hotspot/fiducial) : t10
  Double_t minus_t[maxdim] = { 0.00390777, 0.00777526, 0.0125324, 0.0174174, 0.0231780 } ;
  Double_t AN[maxdim] = {      0.0395247,  0.0297218,   0.0228926,   0.0184411,    0.0171746 } ;
  Double_t ANerror[maxdim] = { 0.00148536, 0.000759619, 0.000664120, 0.000665168,  0.000740819 } ;
  */

  /* Aug. 1, 2011 (5 -t bins, after hotspot/fiducial) : t12 -- using the TM of Steve of July 28, 2011
  Double_t minus_t[maxdim] = { 0.00390528, 0.00777765, 0.0125332, 0.0174164, 0.0231845 } ;
  Double_t AN[maxdim] = {      0.0398523,  0.0298750,   0.0225254,   0.0184632,    0.0175347 } ;
  Double_t ANerror[maxdim] = { 0.00148627, 0.000759684, 0.000664311, 0.000665435,  0.000755378 } ;
  */

  /* Aug. 4, 2011 ( t13 -- using the TM of Steve of July 28, 2011 but increasing Leff by 3% )
  Double_t minus_t[maxdim] = { 0.00386735, 0.00775179, 0.0125195, 0.0173703, 0.0226619 } ;
  Double_t AN[maxdim] = {      0.0396947,  0.0284620,   0.0221335,   0.0184170,    0.0168262 } ;
  Double_t ANerror[maxdim] = { 0.00136805, 0.000718769, 0.000641561, 0.000652818,  0.000906987 } ;
  */

  /* Aug. 4, 2011 ( t14 -- using the TM of Steve of July 28, 2011 but decreasing Leff by 3% )
  Double_t minus_t[maxdim] = { 0.00392876, 0.00771226, 0.0125226, 0.0174251, 0.0232064 } ;
  Double_t AN[maxdim] = {      0.0400385,  0.0307538,   0.0230289,   0.0199025,    0.0171693 } ;
  Double_t ANerror[maxdim] = { 0.00163703, 0.000806277, 0.000690028, 0.000682538,  0.000656206 } ;
  */

  /* Aug. 16, 2011 ( t16 -- using the TM of Steve of July 28, 2011 but decreasing Leff by 3% )
  Double_t minus_t[maxdim] = { 0.00390622, 0.00777650, 0.0125331, 0.0174175, 0.0231786 } ;
  Double_t AN[maxdim] = {      0.0396302,  0.0297428,   0.0228979,   0.0184502,    0.0171983 } ;
  Double_t ANerror[maxdim] = { 0.00148545, 0.000760212, 0.000664198, 0.000665270,  0.000739967 } ;
  */

  // Oct. 27, 2011 ( t22 -- using the TM of Steve of Sep29, 2011 and Igor's Table-2a and crossing angle )
  //  Double_t minus_t[maxdim] = { 0.0038868132, 0.0077462566, 0.012549681, 0.017456393, 0.023195816 } ; // get_t_ave.C used
  //  Double_t AN[maxdim] = {      0.0403030,  0.0302288,   0.0228515,   0.0196280,    0.0172375 } ;
  //  Double_t ANerror[maxdim] = { 0.00166526, 0.000819272, 0.000684569, 0.000672232,  0.000662139 } ;


  // Nov. 8, 2011 --- including error of 3 radians in phi
  //  Double_t AN[maxdim] = {      0.0403167,  0.0302942,   0.0228655,   0.0196288,    0.0172437 } ;
  //  Double_t ANerror[maxdim] = { 0.00167142, 0.000831774, 0.000692986, 0.000678488,  0.000668959 } ;

  // Dec. 11, 2011 --- t24, including error of 3 radians in phi
  /*
  Double_t minus_t[maxdim] = { 0.0038866511, 0.0077448888, 0.012550182, 0.017455728, 0.023197279 } ; // get_t_ave.C used
  Double_t AN[maxdim] = {      0.0404398,  0.0302077,   0.0228507,   0.0195908,    0.0172306 } ;
  Double_t ANerror[maxdim] = { 0.00166865, 0.000831349, 0.000693085, 0.000678539,  0.000668520 } ;

  // in % => needed to be multipled by minus_t (JH's Sept. 21, 2011 Slides)
  Double_t terror[maxdim] = { 0.095, 0.070, 0.0575, 0.0525, 0.0475 } ;
  */

  // Jan. 5, 2012 --- t24, including error of 3 radians in phi
  // But fixing phi0 = 0
  //  Double_t AN[maxdim] = {      0.0403459,  0.0299179,   0.0226807,   0.0195662,    0.0170466 } ;
  //  Double_t ANerror[maxdim] = { 0.00164617, 0.000819439, 0.000689972, 0.000678284,  0.000660720 } ;



  /*
  const Int_t maxdim = 8 ;

  Double_t minus_t[maxdim] = { 0.0038866511, 0.0071832176, 0.010537208, 0.01325603, 0.015750569, 0.018233077, 0.020928152, 0.024856866 } ; // get_t_ave.C used
  Double_t AN[maxdim] = {      0.0404398,  0.0316643,   0.0242445,   0.0225497,   0.0216847,    0.0181807,   0.0180504,   0.0166590 } ;
  Double_t ANerror[maxdim] = { 0.00166865, 0.000961698, 0.000929353, 0.000965007, 0.000955824,  0.000954917, 0.000907286, 0.000890162 } ;

  Double_t terror[maxdim] = { 0.095, 0.0725, 0.0625, 0.0575, 0.0550, 0.0525, 0.0500, 0.0475 } ;
  */

  /* Jan. 9, 2012 --- t24, including NO error on phi when extracting AN (still with phi0)
  Double_t minus_t[maxdim] = { 0.0038866511, 0.0077448888, 0.012550182, 0.017455728, 0.023197279 } ; // get_t_ave.C used
  Double_t AN[maxdim] = {      0.0404306,  0.0301417,   0.0228388,   0.0195866,    0.0172281 } ;
  Double_t ANerror[maxdim] = { 0.00166257, 0.000818789, 0.000684798, 0.000672201,  0.000661959 } ;
  */

  // Jan. 19, 2012 --- t24, including NO error on phi and without phi0 when extracting AN
  Double_t minus_t[maxdim] = { 0.0038859772, 0.0077443228, 0.012551763, 0.017456002, 0.023195601 } ; // using hUU & hDD
  Double_t AN[maxdim] = {      0.0403424,  0.0298590,   0.0226589,   0.0195613,    0.0170307 } ;
  Double_t ANerror[maxdim] = { 0.00164122, 0.000806966, 0.000681473, 0.000672144,  0.000652752 } ;

  // Jan. 22, 2012 --- t24, pi+phi
  //  Double_t AN[maxdim] = {      0.0407855,  0.0302280,   0.0231259,   0.0197839,    0.0172828 } ;
  //  Double_t ANerror[maxdim] = { 0.00180860, 0.000823298, 0.000688538, 0.000674690,  0.000662684 } ;

  // in % => needed to be multipled by minus_t (JH's Sept. 21, 2011 Slides)
  Double_t terror[maxdim] = { 444045, 2091977, 2854764, 2882893, 2502703 } ; // no. of events in hUU & hDD for each -t bin


  Int_t i ;
  for ( i=0; i<maxdim ; i++ ) {

    // Leff
    //    cout << i << ": Leff : " << 0.02*minus_t[i] << endl ;
    //    cout << i << ": Leff : " << 0.02/TMath::Sqrt(2)*minus_t[i] << endl ;
    //    minus_t[i] = 1.02*minus_t[i] ;
    //    minus_t[i] = 0.98*minus_t[i] ;
    // Feb. 23, 2012 : since two sides are uncorrelated => can be divided by 1/sqrt(2)
    //    minus_t[i] = (1.+ TMath::Sqrt(2)/100.)*minus_t[i] ;
    //    minus_t[i] = (1.- TMath::Sqrt(2)/100.)*minus_t[i] ;

    // Alignment
    //    cout << i << ": alignment : " << 0.002*TMath::Sqrt(minus_t[i]) << endl ;
    //    minus_t[i] = minus_t[i] + 0.002*TMath::Sqrt(minus_t[i]);
    //    minus_t[i] = minus_t[i] - 0.002*TMath::Sqrt(minus_t[i]);

    //    terror[i] = 0.05*minus_t[i];
    //    terror[i] = 0.1*minus_t[i];
    //    terror[i] = terror[i]*minus_t[i];
    terror[i] = 0.011*TMath::Sqrt(minus_t[i]/terror[i]) ;
    //    cout << i << ": dt : " << terror[i] << endl ;

    /* Either (higher polarization => lower AN and lower ANerror
    cout << i << ": dAN : " << AN[i]/1.054 - AN[i] << endl ;
    AN[i] = AN[i]/1.054 ;
    ANerror[i] = ANerror[i]/1.054 ;
    */
    /* OR lower polarization => higher AN and higher ANerror
    cout << i << ": dAN : " << AN[i]*1.054 - AN[i] << endl ;
    AN[i] = AN[i]*1.054 ;
    ANerror[i] = ANerror[i]*1.054 ;
    */

  }

  Double_t Chi, Error2 ;

  for ( i = 0; i < maxdim; i++){//evt loop
    //    Chi = ( AN_theory(minus_t[i], par[0], par[1]) - AN[i] )/ANerror[i] ;
    //    ChiSquare += Chi*Chi ;

    /*
   The following is done mainly according to  http://root.cern.ch/root/html/TGraph.html#TGraph:Fit .
   Maybe search for the words "effective variance method" ...
    */

    Chi =  AN_theory(minus_t[i], par[0], par[1]) - AN[i] ;

    if ( terror[i] > 0 )
      Error2 = terror[i]*derivative( minus_t[i]-terror[i]*0.043, minus_t[i]+terror[i]*0.043, par[0], par[1] );
    else
      Error2 = 0.;

    Error2 = Error2*Error2 + ANerror[i]*ANerror[i] ;
    ChiSquare += Chi*Chi/Error2 ;

  }

  f = ChiSquare;
} //}}}


//minuit loop
Double_t MinuitFit()
{//{{{
  TStopwatch timer; timer.Start();

  TMinuit *gMinuit = new TMinuit(2);
  gMinuit->SetFCN(fcn);

  Double_t arglist[10];
  Double_t p1=1, p3=3;
  Int_t ierflg = 0;
  arglist[0] = 1.;

  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg); // 1 for ChiSquare, 0.5 for log likelihood

  gMinuit->SetErrorDef(1); // 1 for ChiSquare, 0.5 for log likelihood
  gMinuit->mnparm(0, "a0", -0.033, 0.00005, -1.50, 1.50, ierflg);
  gMinuit->mnparm(1, "a1", -0.430, 0.00005, -2.50, 2.50, ierflg);

   // Now ready for minimization step
   arglist[0] = 3000;
   arglist[1] = 0.;
   gMinuit->mnexcm("CALL FCN", &p1, 1, ierflg);
   //gMinuit->mnexcm("SIMPLEX", arglist ,2,ierflg);
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
   gMinuit->mnexcm("CALL FCN", &p3, 1, ierflg);
   //gMinuit->mnexcm("SHOW COVARIANCE", &p0, 0, ierflg); // Double_t p0=0 ;

   gMinuit->SetErrorDef(1); // 1 for ChiSquare, 0.5 for log likelihood

   // Print results
   Double_t  amin,edm,errdef;
   Int_t     nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   cout<<"amin    ***  from mnstat  "<<amin<<endl;
   cout<<"amin    ***  from mnstat  "<<amin<<endl;
   gMinuit->mnprin(3,amin);

   cout<<"Time at the end of job = "<<timer.CpuTime()<<" seconds"<<endl;

   cout<<endl<<endl;
   cout<<"   ********************************      "<<endl;
   cout<<"amin      "<<amin<<endl;
   cout<<"edm       "<<edm<<endl;
   cout<<"errdef    "<<errdef<<endl;
   cout<<"nvpar     "<<nvpar<<endl;
   cout<<"nparx     "<<nparx<<endl;
   cout<<"icstat    "<<icstat<<endl;
   cout<<"   ********************************      "<<endl;
   cout<<endl<<endl;
   printf("amin = %0.4f \n", amin);

   return amin;
} //}}}
