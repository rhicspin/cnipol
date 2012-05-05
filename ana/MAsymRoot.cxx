
#include "MAsymRoot.h"

#include <climits>

ClassImp(MAsymRoot)

using namespace std;


MAsymRoot::MAsymRoot() : TFile(),
   fAnaInfo(0), fHists(0), 
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0)
{
}


MAsymRoot::MAsymRoot(MAsymAnaInfo &anaInfo) : TFile(anaInfo.GetRootFileName().c_str(), "RECREATE", "MAsymRoot output file"),
   fAnaInfo(&anaInfo), fHists(0),
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0)
{
   printf("Created ROOT file: %s\n", GetName());
}


MAsymRoot::~MAsymRoot()
{
}

void MAsymRoot::SetHists(DrawObjContainer &hists)
{ //{{{
   fHists = &hists;
} //}}}


/** */
void MAsymRoot::SaveAs(string pattern, string dir)
{ //{{{
   TCanvas canvas("canvas", "canvas", 1400, 600);

   stringstream ssSignature("signature not defined");

   char strAnaTime[25];
   time_t currentTime = time(0);
   tm *ltime = localtime( &currentTime );
   strftime(strAnaTime, 25, "%c", ltime);

   ssSignature << "Fills " << fMinFill << "--" << fMaxFill << ", Analyzed " << strAnaTime;
   ssSignature << ", Version " << fAnaInfo->fAsymVersion << ", " << fAnaInfo->fUserGroup.fUser;

   fHists->SetSignature(ssSignature.str());
   fHists->SaveAllAs(canvas, pattern, dir);
} //}}}


/** */
void MAsymRoot::SetMinMax(EventConfig &mm)
{ //{{{
   SetMinMaxFill((UInt_t) mm.fMeasInfo->RUNID);
   SetMinMaxTime(mm.fMeasInfo->fStartTime);
} //}}}


/** */
void MAsymRoot::SetMinMaxFill(UInt_t fillId)
{ //{{{
   if (fillId < fMinFill ) fMinFill = fillId;
   if (fillId > fMaxFill ) fMaxFill = fillId;
} //}}}


/** */
void MAsymRoot::SetMinMaxTime(UInt_t time)
{ //{{{
   if (time < fMinTime ) fMinTime = time;
   if (time > fMaxTime ) fMaxTime = time;
} //}}}
