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

   //Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
   //Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
   

   ClassDef(MAsymRoot, 1)
};
  
#endif
