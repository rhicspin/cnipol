#ifndef AsymDbSql_h
#define AsymDbSql_h

#include <memory>

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

   std::unique_ptr<mysqlpp::Connection> fConnection;

   AsymDbSql();
   
   void                       OpenConnection();
   void                       CloseConnection();
   MseMeasInfoX*              SelectRun(std::string runName="");
   MseFillPolarX*             SelectFillPolar(UInt_t fill);
   MseFillPolarNewX*          SelectFillPolar(UInt_t fill, EPolarimeterId polId, ERingId ringId);
   MseFillPolarNewXSet        SelectFillPolars(UInt_t fill);
   MseFillProfileX*           SelectFillProfile(UInt_t fill);
   MseFillProfileNewX*        SelectFillProfile(UInt_t fill, EPolarimeterId polId, ETargetOrient tgtOrient);
   std::vector<MseMeasInfoX>  SelectPriorRuns(const MseMeasInfoX& run);
   void                       UpdateInsert(const MseMeasInfoX* orun, const MseMeasInfoX* nrun);
   void                       UpdateInsert(const MseFillPolarX*   ofill, const MseFillPolarX*   nfill);
   void                       UpdateInsert(const MseFillPolarNewX* ofill, const MseFillPolarNewX* nfill);
   void                       UpdateInsert(const MseFillProfileX* ofill, const MseFillProfileX* nfill);
   void                       UpdateInsert(const MseFillProfileNewX* ofill, const MseFillProfileNewX* nfill);

private:

   static const char*         GetSetting(const char *key, const char *defval = NULL);
};

#endif
