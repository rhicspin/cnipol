
#include <sstream>

#include <TString.h>
#include <TSystem.h>

#include "AsymDbSql.h"


using namespace std;
using namespace mysqlpp;


/** */
AsymDbSql::AsymDbSql() : fConnection()
{
   MseMeasInfoX::table("run_info");
   MseFillPolarX::table("fill_polar");
   MseFillPolarNewX::table("fill_polar_new");
   MseFillProfileX::table("fill_profile");
   MseFillProfileNewX::table("fill_profile_new");
}


/** */
const char* AsymDbSql::GetSetting(const char *key, const char *defval)
{
   const char *value = gSystem->Getenv(key);
   if (!value)
   {
      if (defval) {
         return defval;
      } else {
         throw Form("Failed to determine MySQL settings. "
                    "Please define '%s' shell enviroment variable.", key);
      }
   }
   return value;
}


/** */
void AsymDbSql::OpenConnection()
{
   // connection already established
   if (fConnection) return;

   try {
      // Establish the connection to the database server.
      const char *db_name =     GetSetting("CNIPOL_DB_NAME");
      const char *db_host =     GetSetting("CNIPOL_DB_HOST");
      const char *db_port_str = GetSetting("CNIPOL_DB_PORT", "3306");
      const char *db_user =     GetSetting("CNIPOL_DB_USER");
      const char *db_password = GetSetting("CNIPOL_DB_PASSWORD");
      int db_port = strtol(db_port_str, NULL, 10);

      fConnection.reset(new Connection(db_name, db_host, db_user, db_password, db_port));
   } catch (const BadQuery& er) {
      // Handle any query errors
      cerr << "Query error: " << er.what() << endl;
   } catch (const BadConversion& er) {
      // Handle bad conversions; e.g. type mismatch populating 'stock'
      cerr << "Conversion error: " << er.what() << endl <<
              "\tretrieved data size: " << er.retrieved << ", actual size: " << er.actual_size << endl;
   } catch (const Exception& er) {
      // Catch-all for any other MySQL++ exceptions
      cerr << "Error: " << er.what() << endl;
   } catch (const char *msg) {
      cerr << msg << endl;
   }
}


/** */
void AsymDbSql::CloseConnection()
{
   fConnection.reset();
}


/** */
MseMeasInfoX* AsymDbSql::SelectRun(std::string runName)
{
   OpenConnection();

   if (!fConnection) {
      Error("SelectRun", "Connection with MySQL server not established");
      return 0;
   }

   stringstream sstr;

   sstr << "select * from `run_info` where `run_name` = '" << runName << "'";

   //Query query;

   Query query = fConnection->query(sstr.str());

   MseMeasInfoX* mseri = 0;

   cout << "Query: " << query << endl;

   //query.execute();
   //vector<MseMeasInfoX> res;
   //query.storein(res);

   StoreQueryResult result;

   try {
      result = query.store();
   } catch (const Exception& er) {
      // Catch-all for any other MySQL++ exceptions
      cerr << "Error: " << er.what() << endl;
      return 0;
   }

   if (result) {
      //cout << "We have:" << endl;
      //for (size_t i = 0; i < result.num_rows(); ++i) {
      //    cout << '\t' << result[i][0] << endl;
      //}
      if (!result.empty())
         mseri = new MseMeasInfoX(result[0]);

   } else {
      cerr << "Failed to get item list: " << query.error() << endl;
   }

   //vector<stock> res;
   //query.storein(res);

   //// Display the items
   //cout << "We have:" << endl;
   //vector<stock>::iterator it;
   //for (it = res.begin(); it != res.end(); ++it) {
   //    cout << '\t' << it->item;
   //    if (it->description != mysqlpp::null) {
   //        cout << " (" << it->description << ")";
   //    }
   //    cout << endl;
   //}

   CloseConnection();

   return mseri;
}


/** */
MseFillPolarX* AsymDbSql::SelectFillPolar(UInt_t fill)
{
   if (!fConnection) {
      Error("SelectFillPolar", "Connection with MySQL server not established");
      return 0;
   }

   stringstream sstr;

   sstr << "select * from `fill_polar` where `fill` = '" << fill << "'";

   Query query = fConnection->query(sstr.str());

   MseFillPolarX* msefp = 0;

   cout << "Query: " << query << endl;
   //query.execute();

   if (StoreQueryResult result = query.store()) {

      if (!result.empty()) msefp = new MseFillPolarX(result[0]);

   } else {
      cerr << "Failed to get item list: " << query.error() << endl;
   }

   return msefp;
}


/** */
MseFillPolarNewX* AsymDbSql::SelectFillPolar(UInt_t fill, EPolarimeterId polId, ERingId ringId)
{
   if (!fConnection) {
      Error("SelectFillPolar", "Connection with MySQL server not established");
      return 0;
   }

   stringstream sstr;

   sstr << "select * from `fill_polar_new` where `fill` = '"
        << fill << "' and `polarimeter_id` = '" << polId << "' and `ring_id` = '" << ringId << "'";

   Query query = fConnection->query(sstr.str());

   MseFillPolarNewX* msefpn = 0;

   cout << "Query: " << query << endl;

   if (StoreQueryResult result = query.store())
   {
      if (!result.empty()) msefpn = new MseFillPolarNewX(result[0]);
   } else {
      cerr << "Failed to get item: " << query.error() << endl;
   }

   return msefpn;
}


/** */
MseFillPolarNewXSet AsymDbSql::SelectFillPolars(UInt_t fill)
{
   MseFillPolarNewXSet polars;

   if (!fConnection) {
      Error("SelectFillPolars", "Connection with MySQL server not established");
      return polars;
   }

   stringstream sstr;
   sstr << "select * from `fill_polar_new` where `fill` = '" << fill << "'";

   Query query = fConnection->query(sstr.str());
   cout << "Query: " << query << endl;

   query.storein(polars);

   return polars;
}


/** */
MseFillProfileX* AsymDbSql::SelectFillProfile(UInt_t fill)
{
   if (!fConnection) {
      Error("SelectFillProfile", "Connection with MySQL server not established");
      return 0;
   }

   stringstream sstr;

   sstr << "select * from `fill_profile` where `fill` = '" << fill << "'";

   Query query = fConnection->query(sstr.str());

   MseFillProfileX* msefp = 0;

   cout << "Query: " << query << endl;
   //query.execute();

   if (StoreQueryResult result = query.store()) {

      if (!result.empty()) msefp = new MseFillProfileX(result[0]);

   } else {
      cerr << "Failed to get item list: " << query.error() << endl;
   }

   return msefp;
}


/** */
MseFillProfileNewX* AsymDbSql::SelectFillProfile(UInt_t fill, EPolarimeterId polId, ETargetOrient tgtOrient)
{
   if (!fConnection) {
      Error("SelectFillPolar", "Connection with MySQL server not established");
      return 0;
   }

   stringstream sstr;

   sstr << "select * from `fill_profile_new` where `fill` = '"
        << fill << "' and `polarimeter_id` = '" << polId << "' and `target_orient` = '" << tgtOrient << "'";

   Query query = fConnection->query(sstr.str());

   MseFillProfileNewX* msefpn = 0;

   cout << "Query: " << query << endl;

   if (StoreQueryResult result = query.store())
   {
      if (!result.empty()) msefpn = new MseFillProfileNewX(result[0]);
   } else {
      cerr << "Failed to get item: " << query.error() << endl;
   }

   return msefpn;
}


/** */
vector<MseMeasInfoX> AsymDbSql::SelectPriorRuns(const MseMeasInfoX& run)
{
   if (!fConnection) {
      Error("SelectPriorRuns", "Connection with MySQL server not established");
      vector<MseMeasInfoX> dummy;
      return dummy;
   }

   stringstream sstr;

   sstr << "select * from `run_info` where `start_time` < '" << run.start_time << "' "
        << "AND `polarimeter_id`='" << run.polarimeter_id << "' ORDER BY `start_time` DESC";

   Query query = fConnection->query(sstr.str());

   vector<MseMeasInfoX> results;

   cout << "Query: " << query << endl;
   //query.execute();

   //if (StoreQueryResult result = query.store()) {

      query.storein(results);

   //} else {
   //   cerr << "Failed to get item list: " << query.error() << endl;
   //}

   //// Display the items
   //cout << "We have:" << endl;
   //vector<stock>::iterator it;
   //for (it = res.begin(); it != res.end(); ++it) {
   //    cout << '\t' << it->item;
   //    if (it->description != mysqlpp::null) {
   //        cout << " (" << it->description << ")";
   //    }
   //    cout << endl;
   //}

   return results;
}


/** */
void AsymDbSql::UpdateInsert(const MseMeasInfoX* orun, const MseMeasInfoX* nrun)
{
   OpenConnection();
   Info("UpdateInsert", "Opening");
   if (!fConnection) {
      Error("UpdateInsert", "Connection with MySQL server not established");
      return;
   }
   Info("UpdateInsert", "Querying");

   Query query = fConnection->query();

   // if original run is not defined just insert the new one
   if (!orun) {
      Info("UpdateInsert", "Inserting");
      //query.update(*nrun, *nrun);
      query.insert(*nrun);
      cout << "Query: " << query << endl;
      query.execute();

   } else {
      Info("UpdateInsert", "Updating");
      query.update(*orun, *nrun);
      cout << "Query: " << query << endl;
      query.execute();
   }

   CloseConnection();
}


/** */
void AsymDbSql::UpdateInsert(const MseFillPolarX* ofill, const MseFillPolarX* nfill)
{
   if (!fConnection) {
      Error("UpdateInsert", "Connection with MySQL server not established");
      return;
   }

   Query query = fConnection->query();

   // if original run is not defined just insert the new one
   if (!ofill) {
      query.insert(*nfill);
      cout << "Query: " << query << endl;
      query.execute();

   } else {
      query.update(*ofill, *nfill);
      cout << "Query: " << query << endl;
      query.execute();
   }
}


/** */
void AsymDbSql::UpdateInsert(const MseFillPolarNewX* ofill, const MseFillPolarNewX* nfill)
{
   if (!fConnection) {
      Error("UpdateInsert", "Connection with MySQL server not established");
      return;
   }

   Query query = fConnection->query();

   // if original run is not defined just insert the new one
   if (!ofill) {
      query.insert(*nfill);
      cout << "Query: " << query << endl;
      query.execute();

   } else {
      query.update(*ofill, *nfill);
      cout << "Query: " << query << endl;
      query.execute();
   }
}


/** */
void AsymDbSql::UpdateInsert(const MseFillProfileX* ofill, const MseFillProfileX* nfill)
{
   if (!fConnection) {
      Error("UpdateInsert", "Connection with MySQL server not established");
      return;
   }

   Query query = fConnection->query();

   // if original run is not defined just insert the new one
   if (!ofill) {
      query.insert(*nfill);
      cout << "Query: " << query << endl;
      query.execute();

   } else {
      query.update(*ofill, *nfill);
      cout << "Query: " << query << endl;
      query.execute();
   }
}


/** */
void AsymDbSql::UpdateInsert(const MseFillProfileNewX* ofill, const MseFillProfileNewX* nfill)
{
   if (!fConnection) {
      Error("UpdateInsert", "Connection with MySQL server not established");
      return;
   }

   Query query = fConnection->query();

   // if original run is not defined just insert the new one
   if (!ofill) {
      query.insert(*nfill);
      cout << "Query: " << query << endl;
      query.execute();

   } else {
      query.update(*ofill, *nfill);
      cout << "Query: " << query << endl;
      query.execute();
   }
}
