#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <stdio.h>

#include "mysql++.h"

#include "AsymDb.h"
#include "DbEntry.h"

//#include "MstRunInfo.h"
#include "MseRunInfo.h"
#include "MseRunPeriod.h"


class AsymDbSql : public AsymDb {

public:

   //MseRunInfo fMstRunInfo; 
	mysqlpp::Connection *fConnection;

   AsymDbSql();
   ~AsymDbSql();
   
   DbEntry*     Select(std::string runName="");
   MseRunInfoX* SelectRun(std::string runName="");
	void         CompleteRunInfo(MseRunInfoX& run);
	void         CompleteRunInfoByRunPeriod(MseRunInfoX& run);
   std::vector<MseRunInfoX> SelectPriorRuns(MseRunInfoX& run);
   MseRunPeriodX*           SelectRunPeriod(MseRunInfoX& run);
   void         Insert(DbEntry *dbrun);
   void         UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun);
   void         Dump();
   
};

#endif
