
#include <sstream>

#include "AsymDbSql.h"


using namespace std;
using namespace mysqlpp;


/** */
AsymDbSql::AsymDbSql() // : fMstRunInfo() //fMstRunInfo((const sql_varchar)"", 0, 0, 0, 0)
{
   try {
      // Establish the connection to the database server.
      fConnection = new Connection("cnipol", "127.0.0.1", "cnipol2", "cnipol");
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

   MseRunInfoX::table("run_info");
   MseRunPeriodX::table("run_period");
}


/** */
AsymDbSql::~AsymDbSql()
{
}


/** */
DbEntry* AsymDbSql::Select(std::string runName)
{
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
}


/** */
MseRunInfoX* AsymDbSql::SelectRun(std::string runName)
{
   if (!fConnection) {
      Error("Select", "Connection with MySQL server not established");
      return 0;
   }

   string q = "select * from run_info where run_name=\"" + runName + "\"";

   Query query = fConnection->query(q);

   MseRunInfoX* mseri = 0;

   cout << "Query: " << query << endl;
   //query.execute();

   if (StoreQueryResult result = query.store()) {
      //cout << "We have:" << endl;
      //for (size_t i = 0; i < result.num_rows(); ++i) {
      //    cout << '\t' << result[i][0] << endl;
      //}
      if (!result.empty())
         mseri = new MseRunInfoX(result[0]);

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

   return mseri;
}


/** */
void AsymDbSql::CompleteRunInfo(MseRunInfoX& run)
{
   vector<MseRunInfoX> runs = SelectPriorRuns(run);
   vector<MseRunInfoX>::iterator irun;

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
}


/** */
void AsymDbSql::CompleteRunInfoByRunPeriod(MseRunInfoX& run)
{
   MseRunPeriodX* runPeriod = SelectRunPeriod(run);

   if (runPeriod)
      runPeriod->Print();
   else {
      Error("CompleteRunInfoByRunPeriod", "No run period selected");
      return;
   }

   run.alpha_calib_run_name = runPeriod->alpha_calib_run_name;
   run.dl_calib_run_name    = runPeriod->dl_calib_run_name;
   run.disabled_channels    = runPeriod->disabled_channels;
   run.disabled_bunches     = runPeriod->disabled_bunches;
   //run.cut_proto_slope      = runPeriod->cut_proto_slope;
   //run.cut_proto_offset     = runPeriod->cut_proto_offset;
}


/** */
vector<MseRunInfoX> AsymDbSql::SelectPriorRuns(MseRunInfoX& run)
{
   if (!fConnection) {
      Error("Select", "Connection with MySQL server not established");
      vector<MseRunInfoX> dummy;
      return dummy;
   }

   stringstream sstr;

   sstr << "select * from `run_info` where `start_time` < '" << run.start_time << "' "
        << "AND `polarimeter_id`='" << run.polarimeter_id << "' ORDER BY `start_time` DESC";

   Query query = fConnection->query(sstr.str());

   vector<MseRunInfoX> results;

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
MseRunPeriodX* AsymDbSql::SelectRunPeriod(MseRunInfoX& run)
{
   MseRunPeriodX* mserp = 0;

   if (!fConnection) {
      Error("Select", "Connection with MySQL server not established");
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

   return mserp;
}


/** */
void AsymDbSql::Insert(DbEntry *dbrun)
{
   if (!dbrun) return;

   //MseRunInfoX mseRunInfoX("", 0, sql_datetime(""), sql_datetime(""), 0);

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

   MseRunInfoX mseRunInfoX(dbrun->fRunName,
                         sql_smallint(polarimeter_id),
                         sql_datetime(start_time),
                         sql_datetime(stop_time),
                         sql_float(beam_energy));

   //mseRunInfoX.instance_table("run_info");

   Query query = fConnection->query();
   query.insert(mseRunInfoX);

   cout << "Query: " << query << endl;
   query.execute();
}


/** */
void AsymDbSql::UpdateInsert(MseRunInfoX* orun, MseRunInfoX* nrun)
{
   if (!fConnection) {
      Error("Select", "Connection with MySQL server not established");
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
}


/** */
void AsymDbSql::Dump()
{}
