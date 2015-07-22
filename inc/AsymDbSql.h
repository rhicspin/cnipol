#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <stdio.h>

#include "mysql++.h"

#include "MseMeasInfo.h"
#include "MseFillPolar.h"
#include "MseFillPolarNew.h"
#include "MseFillProfile.h"
#include "MseFillProfileNew.h"


/**
 * select information about run periods from an ascii file.
 */
class AsymDbSql
{

public:

	mysqlpp::Connection *fConnection;

   AsymDbSql();
   ~AsymDbSql();
   
   void                       OpenConnection();
   void                       CloseConnection();
   MseMeasInfoX*              SelectRun(std::string runName="");
   MseFillPolarX*             SelectFillPolar(UInt_t fill);
   MseFillPolarNewX*          SelectFillPolar(UInt_t fill, EPolarimeterId polId, ERingId ringId);
   MseFillPolarNewXSet        SelectFillPolars(UInt_t fill);
   MseFillProfileX*           SelectFillProfile(UInt_t fill);
   MseFillProfileNewX*        SelectFillProfile(UInt_t fill, EPolarimeterId polId, ETargetOrient tgtOrient);
   std::vector<MseMeasInfoX>  SelectPriorRuns(const MseMeasInfoX& run);
   void                       CompleteMeasInfo(MseMeasInfoX& run);
   void                       UpdateInsert(const MseMeasInfoX* orun, const MseMeasInfoX* nrun);
   void                       UpdateInsert(const MseFillPolarX*   ofill, const MseFillPolarX*   nfill);
   void                       UpdateInsert(const MseFillPolarNewX* ofill, const MseFillPolarNewX* nfill);
   void                       UpdateInsert(const MseFillProfileX* ofill, const MseFillProfileX* nfill);
   void                       UpdateInsert(const MseFillProfileNewX* ofill, const MseFillProfileNewX* nfill);

private:

   const char*                GetSetting(const char *key);
};

#endif
