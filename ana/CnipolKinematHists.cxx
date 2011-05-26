/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "CnipolKinematHists.h"

#include "RunInfo.h"


ClassImp(CnipolKinematHists)

using namespace std;

/** Default constructor. */
CnipolKinematHists::CnipolKinematHists() : DrawObjContainer()
{
   BookHists();
   BookHists("_cut1");
   BookHists("_cut2");
}


CnipolKinematHists::CnipolKinematHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
   BookHists("_cut1");
   BookHists("_cut2");
}


/** Default destructor. */
CnipolKinematHists::~CnipolKinematHists()
{
}


/** */
void CnipolKinematHists::BookHists(string cutid)
{ //{{{
   char hName[256];

   fDir->cd();

   sprintf(hName, "hPseudoMass%s", cutid.c_str());
   o[hName] = new TH1D(hName, hName, 50, 0, 20);
   ((TH1*) o[hName])->SetOption("hist NOIMG");

   DrawObjContainer        *oc;
   DrawObjContainerMapIter  isubdir;

   for (int iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {

      string sChId("  ");
      sprintf(&sChId[0], "%02d", iCh);

      string dName = "channel" + sChId;

      isubdir = d.find(dName);

      if ( isubdir == d.end()) { // if dir not found
         oc = new DrawObjContainer();
         oc->fDir = new TDirectoryFile(dName.c_str(), dName.c_str(), "", fDir);
      } else {
         oc = isubdir->second;
      }

      sprintf(hName, "hPseudoMass%s_ch%02d", cutid.c_str(), iCh);
      oc->o[hName] = new TH1D(hName, hName, 50, 0, 20);
      ((TH1*) oc->o[hName])->SetOption("hist NOIMG");

      // If this is a new directory then we need to add it to the list
      if ( isubdir == d.end()) {
         d[dName] = oc;
      }
   }
} //}}}


/** */
void CnipolKinematHists::Fill(ChannelEvent *ch, string cutid)
{ //{{{
   UChar_t chId  = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   DrawObjContainer *sd = d["channel"+sChId];

   //Float_t mass = ch->GetCarbonMassEstimate();
   Float_t mass = ch->GetCarbonMass();
   //cout << "mass: " << mass << endl;

   ((TH1*) sd->o["hPseudoMass"+cutid+"_ch"+sChId]) -> Fill(mass);
} //}}}


/** */
void CnipolKinematHists::PostFill()
{
   //char hName[256];

   vector<string> cutIds;
   vector<string>::const_iterator icut;
   cutIds.push_back("");
   cutIds.push_back("_cut1");
   cutIds.push_back("_cut2");

   for (icut=cutIds.begin(); icut!=cutIds.end(); ++icut) {

      string sCutId = *icut;

      TH1* hPseudoMass      = (TH1*) o["hPseudoMass"+sCutId];
   
      for (UShort_t iCh=1; iCh<=N_SILICON_CHANNELS; iCh++) {


         string sChId("  ");
         sprintf(&sChId[0], "%02d", iCh);

         DrawObjContainer *oc = d.find("channel"+sChId)->second;

         TH1* hPseudoMass_channel = (TH1*) oc->o["hPseudoMass"+sCutId+"_ch"+sChId];
         hPseudoMass->Add(hPseudoMass_channel);
      }
   }
}
