#ifndef MAsymRoot_h
#define MAsymRoot_h

#include <string>

#include "TFile.h"

#include "DrawObjContainer.h"
#include "EventConfig.h"
#include "MAsymAnaInfo.h"

class MAsymRoot : public TFile
{
protected:

   AnaInfo          *fAnaInfo;
   DrawObjContainer *fHists;
   Float_t           fMinFill;
   Float_t           fMaxFill;
   time_t            fMinTime;
   time_t            fMaxTime;

public:

   MAsymRoot();
   MAsymRoot(MAsymAnaInfo &anaInfo);
   ~MAsymRoot();

   void SetHists(DrawObjContainer &hists);
   void SaveAs(std::string pattern, std::string dir);
   void SetMinMax(EventConfig &mm);
	void SetMinMaxFill(UInt_t fillId);
	void SetMinMaxTime(UInt_t time);


   ClassDef(MAsymRoot, 1)
};
  
#endif
