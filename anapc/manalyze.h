
#include <sstream>
#include <string>

#include "TObject.h"
#include "TFile.h"
#include "TList.h"
#include "TString.h"
#include "TColor.h"
#include "TStyle.h"

#include "EventConfig.h"
#include "MAsymRoot.h"
#include "DrawObjContainer.h"
#include "CnipolProfileHists.h"

MAsymRoot           *gMAsymRoot = 0;
EventConfig         *gRC;
map<UInt_t, UInt_t>  flattopTimes;

void manalyze();
void initialize();
