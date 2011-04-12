#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <stdio.h>

#include "mysql++.h"

#include "AsymDb.h"
#include "DbEntry.h"

#include "MstRunInfo.h"


class AsymDbSql : public AsymDb {

public:

   //MseRunInfo fMstRunInfo; 
	mysqlpp::Connection *fConnection;

   AsymDbSql();
   ~AsymDbSql();
   
   DbEntry*     Select(std::string runName="");
   MseRunInfoX* SelectRun(std::string runName="");
	void         CompleteRunInfo(MseRunInfoX& run);
   std::vector<MseRunInfoX> SelectPriorRuns(MseRunInfoX& run);
   void         Insert(DbEntry *dbrun);
   void         UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun);
   void         Dump();
   
};

#endif
