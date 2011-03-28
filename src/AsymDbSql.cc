
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
   }
}


/** */
AsymDbSql::~AsymDbSql()
{
}


/** */
DbEntry* AsymDbSql::Select(std::string runName)
{
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
void AsymDbSql::Insert(DbEntry *dbrun)
{}


/** */
void AsymDbSql::Dump()
{}