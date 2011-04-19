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
   virtual MseRunInfoX* SelectRun(std::string runName="");
	virtual void CompleteRunInfo(MseRunInfoX& run) {}
	virtual void CompleteRunInfoByRunPeriod(MseRunInfoX& run) {}
   virtual void Insert(DbEntry *dbrun);
   virtual void UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun) {}
   virtual void Dump();
};

#endif
