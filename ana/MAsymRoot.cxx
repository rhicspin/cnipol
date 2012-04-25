
#include "MAsymRoot.h"

ClassImp(MAsymRoot)

using namespace std;


MAsymRoot::MAsymRoot() : TFile()
{
}


MAsymRoot::MAsymRoot(string fileName) : TFile(fileName.c_str(), "RECREATE", "MAsymRoot histogram file")
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

   fHists->SaveAllAs(canvas, pattern, dir);
} //}}}
