#ifndef MAlphaRoot_h
#define MAlphaRoot_h

#include <string>

#include "TFile.h"

#include "DrawObjContainer.h"
#include "EventConfig.h"
#include "MAsymAnaInfo.h"


class AnaGlobResult;


class MAlphaRoot : public TFile
{
protected:

   AnaInfo          *fAnaInfo;
   DrawObjContainer *fHists;
   AnaGlobResult    *fAnaGlobResult;
   Float_t           fMinFill;
   Float_t           fMaxFill;
   time_t            fMinTime;
   time_t            fMaxTime;
   FILE             *fFilePhp;      //!

public:

   MAlphaRoot();
   MAlphaRoot(MAsymAnaInfo &anaInfo);
   ~MAlphaRoot();

   void SetHists(DrawObjContainer &hists);
   void SetAnaGlobResult(AnaGlobResult *agr);
   void SaveAs(std::string pattern, std::string dir);
   void UpdMinMax(EventConfig &mm);
	void UpdMinMaxFill(UInt_t fillId);
	void UpdMinMaxTime(time_t time);
   void Print(const Option_t* opt="") const;
   void PrintAsPhp(FILE *f=stdout) const;


   ClassDef(MAlphaRoot, 1)
};
  
#endif
