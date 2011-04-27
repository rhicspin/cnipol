
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
#include "MAsymRunHists.h"
//#include "DrawObjContainer.h"
#include "CnipolProfileHists.h"

MAsymRoot           *gMAsymRoot = 0;
//DrawObjContainer    *gH;
MAsymRunHists       *gH;
EventConfig         *gRC;
UShort_t             gPolId     = 0;
UShort_t             gEnergyId  = 0;
map<UInt_t, UInt_t>  flattopTimes;

void manalyze(UShort_t polId=0, UShort_t eId=0);
void initialize();
