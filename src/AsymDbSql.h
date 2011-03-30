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
   
   DbEntry*    Select(std::string runName="");
   MseRunInfo* SelectRun(std::string runName="");
   std::vector<MseRunInfo>& SelectPriorRuns(MseRunInfo& run);
   void        Insert(DbEntry *dbrun);
   void        Dump();
   
};

#endif
