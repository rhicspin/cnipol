// fit one set of spin U/D detector hits for aymmetry etc.
//
//  input: nup[6] - # Up hits in [det = 0-5]
//         nup[6] - # Dn hits in [det = 0-5]
//         fixpar - integer, bit(s) 0-2 = 1  => fix paramater(2) 0-2 to value in par[0-2]
//
// output:  par[3] - results [0]=asymmetry magnitude, [1]=spin tilt (rad.), [2]=lumi asym.
//         epar[3] - error
//            chi2 - minimized chi-square
//         
//

#include <iostream>
#include <fstream>

using namespace std;

namespace afterburner{
  // function prototypes:
  void myFcn(int & /*nPar*/, double * /*grad*/ , double &fval, double *par, int /*iflag */  );
  //void myFcn(Int_t & /*nPar*/, Double_t * /*grad*/ , Double_t &fval, Double_t *par, Int_t /*iflag */  );
  pair<double,double> asym_err(double lambda, double nup, double ndn);
  void fit1asym(double nup[] , double ndn[] , double par[], double epar[] , double &chi2, int fixpar = 0);
}
