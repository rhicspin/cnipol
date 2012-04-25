#ifndef MAsymRoot_h
#define MAsymRoot_h

#include <string>

#include "TFile.h"

#include "DrawObjContainer.h"

class MAsymRoot : public TFile
{
protected:

   DrawObjContainer *fHists;

public:

   MAsymRoot();
   MAsymRoot(std::string fileName);
   ~MAsymRoot();

   void SetHists(DrawObjContainer &hists);
   void SaveAs(std::string pattern, std::string dir);


   ClassDef(MAsymRoot, 1)
};
  
#endif
