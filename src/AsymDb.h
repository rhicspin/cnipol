#ifndef AsymDb_h
#define AsymDb_h

#include <string>

#include "TObject.h"

#include "DbEntry.h"
#include "MseRunInfo.h"
#include "MseRunPeriod.h"


class AsymDb : public TObject {

public:

   AsymDb();
   ~AsymDb();

   virtual DbEntry*       Select(std::string runName="") { return 0; }
   virtual MseRunInfoX*   SelectRun(std::string runName="") { return 0; }
   virtual MseRunPeriodX* SelectRunPeriod(MseRunInfoX& run) { return 0; }
	virtual void           CompleteRunInfo(MseRunInfoX& run) {}
	virtual MseRunPeriodX* CompleteRunInfoByRunPeriod(MseRunInfoX& run) { return 0; }
   virtual void           Insert(DbEntry *dbrun) {}
   virtual void           UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun) {}
   virtual void           Dump() {}
};

#endif
