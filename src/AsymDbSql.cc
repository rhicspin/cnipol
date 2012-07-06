
#include <sstream>

#include "AsymDbSql.h"


using namespace std;
using namespace mysqlpp;


/** */
AsymDbSql::AsymDbSql() : AsymDb(), fConnection(0)
{
   MseMeasInfoX::table("run_info");
   MseRunPeriodX::table("run_period");
   MseFillPolarX::table("fill_polar");
   MseFillPolarNewX::table("fill_polar_new");
   MseFillProfileX::table("fill_profile");
   MseFillProfileNewX::table("fill_profile_new");
}


/** */
AsymDbSql::~AsymDbSql()
{
   CloseConnection();
}


/** */
void AsymDbSql::OpenConnection()
{ //{{{
   // connection already established
   if (fConnection) return;

   try {
      // Establish the connection to the database server.
      //fConnection = new Connection("cnipol", "127.0.0.1", "cnipol", "(n!P0l", 3306);
      fConnection = new Connection("cnipol", "pc2pc.phy.bnl.gov", "cnipol", "(n!P0l", 3306);
   } catch (const BadQuery& er) {
      // Handle any query errors
      cerr << "Query error: " << er.what() << endl;
   } catch (const BadConversion& er) {
      // Handle bad conversions; e.g. type mismatch populating 'stock'
      cerr << "Conversion error: " << er.what() << endl <<
              "\tretrieved data size: " << er.retrieved <<
              ", actual size: " << er.actual_size << endl;
   } catch (const Exception& er) {
      // Catch-all for any other MySQL++ exceptions
      cerr << "Error: " << er.what() << endl;
      fConnection = 0;
   }
} //}}}


/** */
void AsymDbSql::CloseConnection()
{ //{{{
   if (fConnection) {
      delete fConnection;
      fConnection = 0;
   }
} //}}}


/** */
DbEntry* AsymDbSql::Select(std::string runName)
{ //{{{
   if (!fConnection) {
      Error("Select", "Connection with MySQL server not established");
      return 0;
   }

   // Retrieve a subset of the stock table's columns, and store
   // the data in a vector of 'stock' SSQLS structures.  See the
   // user manual for the consequences arising from this quiet
   // ability to store a subset of the table in the stock SSQLS.
   Query query = fConnection->query("select run_name from run_info");

   if (StoreQueryResult res = query.store()) {
      cout << "We have:" << endl;
      for (size_t i = 0; i < res.num_rows(); ++i) {
          cout << '\t' << res[i][0] << endl;
      }
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

   return 0;
} //}}}


/** */
MseMeasInfoX* AsymDbSql::SelectRun(std::string runName)
{ //{{{
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
} //}}}


/** */
MseFillPolarX* AsymDbSql::SelectFillPolar(UInt_t fill)
{ //{{{
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
} //}}}


/** */
MseFillPolarNewX* AsymDbSql::SelectFillPolar(UInt_t fill, EPolarimeterId polId, ERingId ringId)
{ //{{{
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
} //}}}


/** */
MseFillPolarNewXSet AsymDbSql::SelectFillPolars(UInt_t fill)
{ //{{{
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
} //}}}


/** */
MseFillProfileX* AsymDbSql::SelectFillProfile(UInt_t fill)
{ //{{{
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
} //}}}


/** */
MseFillProfileNewX* AsymDbSql::SelectFillProfile(UInt_t fill, EPolarimeterId polId, ETargetOrient tgtOrient)
{ //{{{
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
} //}}}


/** */
void AsymDbSql::CompleteMeasInfo(MseMeasInfoX& run)
{ //{{{
   vector<MseMeasInfoX> runs = SelectPriorRuns(run);
   vector<MseMeasInfoX>::iterator irun;

   for (irun=runs.begin(); irun!=runs.end(); irun++) {

      //cout << setw(10) << irun->run_name;
      //cout << setw(10) << irun->polarimeter_id;
      //cout << setw(10) << irun->start_time;
      //cout << setw(10) << irun->stop_time;
      //cout << setw(10) << irun->beam_energy;
      //cout << endl;

      if ( !run.alpha_calib_run_name.empty() && !run.dl_calib_run_name.empty() &&
           !run.disabled_channels.empty()    && !run.disabled_bunches.empty() )
         break;

      if (run.alpha_calib_run_name.empty() && !irun->alpha_calib_run_name.empty())
         run.alpha_calib_run_name = irun->alpha_calib_run_name;

      if (run.dl_calib_run_name.empty() && !irun->dl_calib_run_name.empty())
         run.dl_calib_run_name = irun->dl_calib_run_name;

      if (run.disabled_channels.empty() && !irun->disabled_channels.empty())
         run.disabled_channels = irun->disabled_channels;

      if (run.disabled_bunches.empty() && !irun->disabled_bunches.empty())
         run.disabled_bunches = irun->disabled_bunches;
   }
} //}}}


/** */
MseRunPeriodX* AsymDbSql::CompleteMeasInfoByRunPeriod(MseMeasInfoX& run)
{ //{{{
   MseRunPeriodX *runPeriod = SelectRunPeriod(run);

   if (runPeriod)
      runPeriod->Print();
   else {
      Error("CompleteMeasInfoByRunPeriod", "No run period selected");
      return 0;
   }

   run.alpha_calib_run_name = runPeriod->alpha_calib_run_name;
   run.dl_calib_run_name    = runPeriod->dl_calib_run_name;
   run.disabled_channels    = runPeriod->disabled_channels;
   run.disabled_bunches     = runPeriod->disabled_bunches;

   return runPeriod; 
} //}}}


/** */
vector<MseMeasInfoX> AsymDbSql::SelectPriorRuns(MseMeasInfoX& run)
{ //{{{
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
} //}}}


/** */
MseRunPeriodX* AsymDbSql::SelectRunPeriod(MseMeasInfoX& run)
{ //{{{
   MseRunPeriodX* mserp = 0;

   OpenConnection();

   if (!fConnection) {
      Error("SelectRunPeriod", "Connection with MySQL server not established");
      return mserp;
   }

   stringstream sstr;

   sstr << "select * from `run_period` where `start_time` <= '" << run.start_time << "' "
        << "AND `polarimeter_id`='" << run.polarimeter_id << "' ORDER BY `start_time` DESC";

   Query query = fConnection->query(sstr.str());

   cout << "Query: " << query << endl;

   if (StoreQueryResult result = query.store()) {
      if (!result.empty())
         mserp = new MseRunPeriodX(result[0]);

   } else {
      cerr << "Failed to get item list: " << query.error() << endl;
   }

   CloseConnection();

   return mserp;
} //}}}


/** */
void AsymDbSql::Insert(DbEntry *dbrun)
{ //{{{
   if (!dbrun) return;

   //MseMeasInfoX MseMeasInfoX("", 0, sql_datetime(""), sql_datetime(""), 0);

   stringstream sstr;

   short polarimeter_id;
   sstr.str("");
   sstr << dbrun->fFields["POLARIMETER_ID"];
   sstr >> polarimeter_id;

   time_t start_time;
   sstr.str("");
   sstr << dbrun->fFields["START_TIME"];
   sstr >> start_time;

   time_t stop_time;
   sstr.str("");
   sstr << dbrun->fFields["STOP_TIME"];
   sstr >> stop_time;

   float beam_energy;
   sstr.str("");
   sstr << dbrun->fFields["BEAM_ENERGY"];
   sstr >> beam_energy;

   MseMeasInfoX MseMeasInfoX(dbrun->fRunName,
                         sql_smallint(polarimeter_id),
                         sql_datetime(start_time),
                         sql_datetime(stop_time),
                         sql_float(beam_energy));

   //MseMeasInfoX.instance_table("run_info");

   Query query = fConnection->query();
   query.insert(MseMeasInfoX);

   cout << "Query: " << query << endl;
   query.execute();
} //}}}


/** */
void AsymDbSql::UpdateInsert(MseMeasInfoX* orun, MseMeasInfoX* nrun)
{ //{{{
   OpenConnection();

   if (!fConnection) {
      Error("UpdateInsert", "Connection with MySQL server not established");
      return;
   }

   Query query = fConnection->query();

   // if original run is not defined just insert the new one
   if (!orun) {
      query.insert(*nrun);
      cout << "Query: " << query << endl;
      query.execute();

   } else {
      query.update(*orun, *nrun);
      cout << "Query: " << query << endl;
      query.execute();
   }

   CloseConnection();
} //}}}


/** */
void AsymDbSql::UpdateInsert(MseFillPolarX* ofill, MseFillPolarX* nfill)
{ //{{{
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
} //}}}


/** */
void AsymDbSql::UpdateInsert(MseFillPolarNewX* ofill, MseFillPolarNewX* nfill)
{ //{{{
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
} //}}}


/** */
void AsymDbSql::UpdateInsert(MseFillProfileX* ofill, MseFillProfileX* nfill)
{ //{{{
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
} //}}}


/** */
void AsymDbSql::UpdateInsert(MseFillProfileNewX* ofill, MseFillProfileNewX* nfill)
{ //{{{
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
} //}}}


/** */
void AsymDbSql::Dump()
{}
