#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <stdio.h>

#include "mysql++.h"

#include "AsymDb.h"
#include "DbEntry.h"

//#include "MstRunInfo.h"
#include "MseRunInfo.h"
#include "MseRunPeriod.h"
#include "MseFillPolar.h"
#include "MseFillProfile.h"


class AsymDbSql : public AsymDb {

public:

   //MseRunInfo fMstRunInfo; 
	mysqlpp::Connection *fConnection;

   AsymDbSql();
   ~AsymDbSql();
   
   DbEntry*                 Select(std::string runName="");
   MseRunInfoX*             SelectRun(std::string runName="");
   MseFillPolarX*           SelectFillPolar(UInt_t fill);
   MseFillProfileX*         SelectFillProfile(UInt_t fill);
   std::vector<MseRunInfoX> SelectPriorRuns(MseRunInfoX& run);
   MseRunPeriodX*           SelectRunPeriod(MseRunInfoX& run);
	void                     CompleteRunInfo(MseRunInfoX& run);
	MseRunPeriodX*           CompleteRunInfoByRunPeriod(MseRunInfoX& run);
   void                     Insert(DbEntry *dbrun);
   void                     UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun);
   void                     UpdateInsert(MseFillPolarX*   ofill, MseFillPolarX*   nfill);
   void                     UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill);
   void                     Dump();
   
};

#endif
