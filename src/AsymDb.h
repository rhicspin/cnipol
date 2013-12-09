#ifndef AsymDb_h
#define AsymDb_h

#include <string>

#include "TObject.h"

#include "DbEntry.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"
#include "MseFillPolar.h"
#include "MseFillPolarNew.h"
#include "MseFillProfile.h"


/**
 * An abstract class (not really) to provide a sort of interface for database access.
 */
class AsymDb : public TObject
{

public:

   AsymDb();

   virtual void              OpenConnection() {}
   virtual void              CloseConnection() {}
   virtual DbEntry*          Select(std::string runName="") { return 0; }
   virtual MseMeasInfoX*     SelectRun(std::string runName="") { return 0; }
   virtual MseFillPolarX*    SelectFillPolar(UInt_t fill) { return 0; }
   virtual MseFillPolarNewX* SelectFillPolar(UInt_t fill, EPolarimeterId polId, ERingId ringId) { return 0; }
   virtual MseFillProfileX*  SelectFillProfile(UInt_t fill) { return 0; }
   virtual MseRunPeriodX*    SelectRunPeriod(MseMeasInfoX& run) { return 0; }
	virtual void              CompleteMeasInfo(MseMeasInfoX& run) {}
	virtual MseRunPeriodX*    CompleteMeasInfoByRunPeriod(MseMeasInfoX& run) { return 0; }
   virtual void              Insert(DbEntry *dbrun) {}
   virtual void              UpdateInsert(MseMeasInfoX* orun, MseMeasInfoX* nrun) {}
   virtual void              UpdateInsert(MseFillPolarX* ofill, MseFillPolarX* nfill) {}
   virtual void              UpdateInsert(MseFillPolarNewX* ofill, MseFillPolarNewX* nfill) {}
   virtual void              UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill) {}
};

#endif
