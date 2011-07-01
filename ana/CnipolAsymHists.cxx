/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolAsymHists.h"

#include "RunInfo.h"


ClassImp(CnipolAsymHists)

using namespace std;

/** Default constructor. */
CnipolAsymHists::CnipolAsymHists() : DrawObjContainer()
{
   BookHists();
}


CnipolAsymHists::CnipolAsymHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolAsymHists::~CnipolAsymHists()
{
}


/** */
void CnipolAsymHists::BookHists(string cutid)
{ //{{{
   //char hName[256];

   fDir->cd();

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iDet=1; iDet<=N_DETECTORS; iDet++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iDet);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      //sprintf(hName, "hTvsA_ch%02d", iDet);
      //oc->o[hName] = new TH2F(hName, hName, 255, 0, 255, 80, 10, 90);
      //((TH1*) oc->o[hName])->SetOption("colz LOGZ");
      //((TH1*) oc->o[hName])->SetTitle(";Amplitude, ADC;TDC;");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
} //}}}


/** */
void CnipolAsymHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{
} //}}}

