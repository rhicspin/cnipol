#ifndef AsymDb_h
#define AsymDb_h

#include <string>

#include "TObject.h"

#include "DbEntry.h"
#include "MseRunInfo.h"


class AsymDb : public TObject {

public:

   AsymDb();
   ~AsymDb();

   virtual DbEntry* Select(std::string runName="");
   virtual MseRunInfo* SelectRun(std::string runName="");
   virtual void Insert(DbEntry *dbrun);
   virtual void Dump();
};

#endif
