#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <stdio.h>

#include "mysql++.h"

#include "AsymDb.h"
#include "DbEntry.h"

#include "MseMeasInfo.h"
#include "MseRunPeriod.h"
#include "MseFillPolar.h"
#include "MseFillPolarNew.h"
#include "MseFillProfile.h"
#include "MseFillProfileNew.h"


class AsymDbSql : public AsymDb {

public:

	mysqlpp::Connection *fConnection;

   AsymDbSql();
   ~AsymDbSql();
   
   void                       OpenConnection();
   void                       CloseConnection();
   DbEntry*                   Select(std::string runName="");
   MseMeasInfoX*              SelectRun(std::string runName="");
   MseFillPolarX*             SelectFillPolar(UInt_t fill);
   MseFillPolarNewX*          SelectFillPolar(UInt_t fill, EPolarimeterId polId, ERingId ringId);
   MseFillPolarNewXSet        SelectFillPolars(UInt_t fill);
   MseFillProfileX*           SelectFillProfile(UInt_t fill);
   MseFillProfileNewX*        SelectFillProfile(UInt_t fill, EPolarimeterId polId, ETargetOrient tgtOrient);
   std::vector<MseMeasInfoX>  SelectPriorRuns(MseMeasInfoX& run);
   MseRunPeriodX*             SelectRunPeriod(MseMeasInfoX& run);
	void                       CompleteMeasInfo(MseMeasInfoX& run);
	MseRunPeriodX*             CompleteMeasInfoByRunPeriod(MseMeasInfoX& run);
   void                       Insert(DbEntry *dbrun);
   void                       UpdateInsert(MseMeasInfoX* orun, MseMeasInfoX* nrun);
   void                       UpdateInsert(MseFillPolarX*   ofill, MseFillPolarX*   nfill);
   void                       UpdateInsert(MseFillPolarNewX* ofill, MseFillPolarNewX* nfill);
   void                       UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill);
   void                       UpdateInsert(MseFillProfileNewX* ofill, MseFillProfileNewX* nfill);
   void                       Dump();
};

#endif
