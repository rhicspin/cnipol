
#include "MAsymRoot.h"

#include <climits>

#include "TROOT.h"

#include "AnaGlobResult.h"


ClassImp(MAsymRoot)

using namespace std;


MAsymRoot::MAsymRoot() : TFile(),
   fAnaInfo(0), fHists(0), fAnaGlobResult(0), 
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0),
   fFilePhp(0)
{
   gROOT->SetMacroPath("./:~/rootmacros/:");
   gROOT->Macro("styles/style_asym.C");
}


MAsymRoot::MAsymRoot(MAsymAnaInfo &anaInfo) : TFile(anaInfo.GetRootFileName().c_str(), "RECREATE", "MAsymRoot output file"),
   fAnaInfo(&anaInfo), fHists(0),
   fMinFill(UINT_MAX), fMaxFill(0),
   fMinTime(UINT_MAX), fMaxTime(0),
   fFilePhp(anaInfo.GetAnaInfoFile())
{
   printf("Created ROOT file: %s\n", GetName());

   gROOT->SetMacroPath("./:~/rootmacros/:");
   gROOT->Macro("styles/style_asym.C");
}


MAsymRoot::~MAsymRoot()
{
}

void MAsymRoot::SetHists(DrawObjContainer &hists)
{ //{{{
   fHists = &hists;
} //}}}


/** */
void MAsymRoot::SetAnaGlobResult(AnaGlobResult *agr)
{ //{{{
   fAnaGlobResult = agr;
} //}}}


/** */
void MAsymRoot::SaveAs(string pattern, string dir)
{ //{{{

   TCanvas canvas("canvas", "canvas", 1400, 600);
   canvas.UseCurrentStyle();

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
void MAsymRoot::UpdMinMax(EventConfig &mm)
{ //{{{
   UpdMinMaxFill((UInt_t) mm.fMeasInfo->RUNID);
   UpdMinMaxTime(mm.fMeasInfo->fStartTime);
} //}}}


/** */
void MAsymRoot::UpdMinMaxFill(UInt_t fillId)
{ //{{{
   if (fillId < fMinFill ) fMinFill = fillId;
   if (fillId > fMaxFill ) fMaxFill = fillId;
} //}}}


/** */
void MAsymRoot::UpdMinMaxTime(time_t time)
{ //{{{
   if (time < fMinTime ) fMinTime = time;
   if (time > fMaxTime ) fMaxTime = time;
} //}}}


/** */
void MAsymRoot::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Called");
   PrintAsPhp(fFilePhp);
} //}}}


/** */
void MAsymRoot::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "<?php\n");

   fprintf(f, "\n// AnaGlobResult data\n");
   if (!fAnaGlobResult) {
      Error("PrintAsPhp", "fAnaGlobResult not defined");
   } else {
      fAnaGlobResult->PrintAsPhp(f);
   }

   fprintf(f, "?>\n");
} //}}}
