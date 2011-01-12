//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRecover.cc
//
//  Decscription: Subroutines to handle non-standard analysis
// 
//  Authors   :   Itaru Nakagawa
//                Dmitri Smirnov
// 
//  Creation  :   4/12/2006
//                

#include "AsymRecover.h"
#include "AsymRecoverUserDefined.h"

using namespace std;

//
// Class name  : AsymRecover
// Method name : OverwriteSpinPattern(int index)
//
// Description : Overwrite the spin pattern to save runs whose spin patterns
//             : were screwed up
// Input       : int index
// Return      : 
//
int AsymRecover::OverwriteSpinPattern(int index)
{
  cerr << "Recovery: Spin pattern is ovewritten by user defined pattern index(" 
       << index << ")" << endl;

  for (int i=0; i<NBUNCH; i++) spinpat[i] = UserDefinedSpinPattern[index][i] ;

  return 1 ;
}

//
// Class name  : AsymRecover
// Method name : OverwriteFillPattern(int index)
//
// Description : Overwrite the fill pattern to save runs whose fill patterns
//             : were screwed up
// Input       : int index
// Return      : 
//
int AsymRecover::OverwriteFillPattern(int index)
{
  cout << "Recovery: Fill pattern is ovewritten by user defined pattern index(" 
       << index << ")" << endl;

  for (int i=0; i<NBUNCH; i++)  fillpat[i] = UserDefinedFillPattern[index][i] ;

  return 1 ;
}


//
// Class name  : AsymRecover
// Method name : MaskFillPattern(int index)
//
// Description : Mask fill pattern to disable suspicious bunches
//             : 
// Input       : 
// Return      : 
//
int AsymRecover::MaskFillPattern()
{
   for (int i=0; i<runinfo.NDisableBunch; i++) {

      for (int j=0; j<NBUNCH; j++) {
         fillpat[j] = runinfo.DisableBunch[i] == j ? 0 : fillpat[j];
      }
   }

  return 0;
}
