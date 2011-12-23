#ifndef AsymDb_h
#define AsymDb_h

#include <string>

#include "TObject.h"

#include "DbEntry.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"
#include "MseFillPolar.h"
#include "MseFillProfile.h"


class AsymDb : public TObject {

public:

   AsymDb();
   ~AsymDb();

   virtual DbEntry*         Select(std::string runName="") { return 0; }
   virtual MseRunInfoX*     SelectRun(std::string runName="") { return 0; }
   virtual MseFillPolarX*   SelectFillPolar(UInt_t fill) { return 0; }
   virtual MseFillProfileX* SelectFillProfile(UInt_t fill) { return 0; }
   virtual MseRunPeriodX*   SelectRunPeriod(MseRunInfoX& run) { return 0; }
	virtual void             CompleteRunInfo(MseRunInfoX& run) {}
	virtual MseRunPeriodX*   CompleteRunInfoByRunPeriod(MseRunInfoX& run) { return 0; }
   virtual void             Insert(DbEntry *dbrun) {}
   virtual void             UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun) {}
   virtual void             UpdateInsert(MseFillPolarX* ofill, MseFillPolarX* nfill) {}
   virtual void             UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill) {}
   virtual void             Dump() {}
};

#endif
