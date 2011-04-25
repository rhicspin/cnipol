#ifndef MAsymRoot_h
#define MAsymRoot_h

#include <string>

#include "TFile.h"

class MAsymRoot : public TFile
{
public:

   MAsymRoot();
   MAsymRoot(std::string fileName);
   ~MAsymRoot();

   ClassDef(MAsymRoot, 1)
};
  
#endif
