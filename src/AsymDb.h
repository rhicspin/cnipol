#ifndef AsymDb_h
#define AsymDb_h

#include <string>

//#include "TObject.h"

#include "DbEntry.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"
#include "MseFillPolar.h"
#include "MseFillProfile.h"


//class AsymDb : public TObject {
class AsymDb {

public:

   AsymDb();
   ~AsymDb();

   virtual void             OpenConnection() {}
   virtual void             CloseConnection() {}
   virtual DbEntry*         Select(std::string runName="") { return 0; }
   virtual MseMeasInfoX*    SelectRun(std::string runName="") { return 0; }
   virtual MseFillPolarX*   SelectFillPolar(UInt_t fill) { return 0; }
   virtual MseFillProfileX* SelectFillProfile(UInt_t fill) { return 0; }
   virtual MseRunPeriodX*   SelectRunPeriod(MseMeasInfoX& run) { return 0; }
	virtual void             CompleteMeasInfo(MseMeasInfoX& run) {}
	virtual MseRunPeriodX*   CompleteMeasInfoByRunPeriod(MseMeasInfoX& run) { return 0; }
   virtual void             Insert(DbEntry *dbrun) {}
   virtual void             UpdateInsert(MseMeasInfoX* orun, MseMeasInfoX* nrun) {}
   virtual void             UpdateInsert(MseFillPolarX* ofill, MseFillPolarX* nfill) {}
   virtual void             UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill) {}
   virtual void             Dump() {}
};

#endif
