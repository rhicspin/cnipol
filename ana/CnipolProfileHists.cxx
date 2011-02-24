/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolProfileHists.h"

#include "AsymGlobals.h"


ClassImp(CnipolProfileHists)

using namespace std;

/** Default constructor. */
CnipolProfileHists::CnipolProfileHists() : DrawObjContainer()
{
   BookHists();
}


CnipolProfileHists::CnipolProfileHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolProfileHists::~CnipolProfileHists()
{
}


/** */
void CnipolProfileHists::BookHists(string sid)
{ //{{{
   fDir->cd();

   if (!sid.empty()) sid = "_" + sid;

   char hName[256];

   sprintf(hName, "hPolarizationProfile");
   o[hName] = new TH1D(hName, hName, 1, 0, 1); // The number of delimeters is not known beforehand
   ((TH1*) o[hName])->GetXaxis()->SetTitle("Target Steps");
   ((TH1*) o[hName])->GetYaxis()->SetTitle("Polarization");
   ((TH1*) o[hName])->SetBit(TH1::kCanRebin);

} //}}}


/** */
void CnipolProfileHists::PostFill()
{
   ((TH1*) o["hPolarizationProfile"])->SetBins(nTgtIndex, 0, nTgtIndex);
}
