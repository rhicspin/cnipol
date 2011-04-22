
#include <sstream>
#include <string>

#include "TObject.h"
#include "TFile.h"
#include "TList.h"
#include "TString.h"

#include "DrawObjContainer.h"
#include "CnipolProfileHists.h"

DrawObjContainer *gH;

void manalyze(UShort_t polId=0, UShort_t eId=0);
void initialize();
