
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
