#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <stdio.h>

#include "mysql++.h"

#include "AsymDb.h"
#include "DbEntry.h"

//#include "MstMeasInfo.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"
#include "MseFillPolar.h"
#include "MseFillProfile.h"


class AsymDbSql : public AsymDb {

public:

   //MseMeasInfo fMstMeasInfo; 
	mysqlpp::Connection *fConnection;

   AsymDbSql();
   ~AsymDbSql();
   
   DbEntry*                 Select(std::string runName="");
   MseMeasInfoX*             SelectRun(std::string runName="");
   MseFillPolarX*           SelectFillPolar(UInt_t fill);
   MseFillProfileX*         SelectFillProfile(UInt_t fill);
   std::vector<MseMeasInfoX> SelectPriorRuns(MseMeasInfoX& run);
   MseRunPeriodX*           SelectRunPeriod(MseMeasInfoX& run);
	void                     CompleteMeasInfo(MseMeasInfoX& run);
	MseRunPeriodX*           CompleteMeasInfoByRunPeriod(MseMeasInfoX& run);
   void                     Insert(DbEntry *dbrun);
   void                     UpdateInsert(MseMeasInfoX* orun, MseMeasInfoX* nrun);
   void                     UpdateInsert(MseFillPolarX*   ofill, MseFillPolarX*   nfill);
   void                     UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill);
   void                     Dump();
   
};

#endif
