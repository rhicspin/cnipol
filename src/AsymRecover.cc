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
#include "RunConfig.h"


using namespace std;


// Description : Overwrite the spin pattern to save runs whose spin patterns
//             : were screwed up
void AsymRecover::OverwriteSpinPattern(int index)
{
   cerr << "Recovery: Spin pattern is ovewritten by user defined pattern index(" 
        << index << ")" << endl;
 
   for (int bid=1; bid<N_BUNCHES; bid++)
   {
      gMeasInfo->fBeamBunches[bid].SetBunchSpin( (ESpinState) UserDefinedFillPattern[index][bid-1] );
   }
}


// Description : Overwrite the fill pattern to save runs whose fill patterns
//             : were screwed up
void AsymRecover::OverwriteFillPattern(int index)
{
   cout << "Recovery: Fill pattern is ovewritten by user defined pattern index(" 
        << index << ")" << endl;
 
   for (int bid=1; bid<=N_BUNCHES; bid++)
   {
      gMeasInfo->fBeamBunches[bid].SetFilled( UserDefinedFillPattern[index][bid-1] );
   }
}


// Description : Mask fill pattern to disable suspicious bunches
void AsymRecover::MaskFillPattern()
{
   for (int i=0; i<gMeasInfo->NDisableBunch; i++) {

      for (int j=0; j<N_BUNCHES; j++) {
         //gFillPattern[j] = gMeasInfo->DisableBunch[i] == j ? 0 : gFillPattern[j];

         if (gMeasInfo->DisableBunch[i] == j) {
            gMeasInfo->fBeamBunches.find(i+1)->second.SetFilled(kFALSE);
         }
      }
   }
}
