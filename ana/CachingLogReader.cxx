#include <vector>
#include <algorithm>

#include <sqlite3.h>

#include "CachingLogReader.h"
#include "SshLogReader.h" // used for <T=SshLogReader> instantiation (see in the end of this file)

using std::vector;


template<class T>
sqlite3*	CachingLogReader<T>::fDB = NULL;
template<class T>
sqlite3_stmt*	CachingLogReader<T>::fSelectStmt = NULL;
template<class T>
sqlite3_stmt*	CachingLogReader<T>::fInsertStmt = NULL;


template<class T>
CachingLogReader<T>::CachingLogReader(string loggers, string cells)
   : T(loggers, cells)
{
   if (!fDB)
   {
      if (sqlite3_open("cdev_cache.sqlite", &fDB) != SQLITE_OK)
      {
         T::Error("CachingLogReader", "Can't open cdev cache");
      }
      const char *sql = "CREATE TABLE IF NOT EXISTS cache (query TEXT PRIMARY KEY, response TEXT);";
      int ret = sqlite3_exec(fDB, sql, NULL, NULL, NULL);
      if (ret != SQLITE_OK)
      {
         T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
      }
      char select_query[] = "SELECT response FROM cache WHERE query = ? LIMIT 1;";
      ret = sqlite3_prepare_v2(fDB, select_query, sizeof(select_query), &fSelectStmt, NULL);
      if (ret != SQLITE_OK)
      {
         T::Error("CachingLogReader", "Error preparing select_query: %s", sqlite3_errmsg(fDB));
      }
      const char insert_query[] = "INSERT INTO cache (query, response) VALUES (?, ?);";
      ret = sqlite3_prepare_v2(fDB, insert_query, sizeof(insert_query), &fInsertStmt, NULL);
      if (ret != SQLITE_OK)
      {
         T::Error("CachingLogReader", "Error preparing insert_query: %s", sqlite3_errmsg(fDB));
      }
   }
}


template<class T>
CachingLogReader<T>::~CachingLogReader()
{
   if (fDB)
   {
      sqlite3_close(fDB);
      fDB = NULL;
   }
}


template<class T>
int CachingLogReader<T>::Run(string cmd, map< string, map<cdev_time_t, double> > *values)
{
   int ret;
   sqlite3_reset(fSelectStmt);
   sqlite3_bind_text(fSelectStmt, 1, cmd.c_str(), cmd.size(), SQLITE_STATIC);

   ret = sqlite3_step(fSelectStmt);
   if (ret == SQLITE_ROW)
   {
      T::Read((const char*)sqlite3_column_text(fSelectStmt, 0), values);
   }
   else if (ret == SQLITE_DONE)
   {
      T::Error("CachingLogReader", "Query result is not in cache, retreiving via ssh.");

      string response;
      int retcode = T::ExecuteCmd(cmd, &response);
      if (retcode)
      {
         T::Error("CachingLogReader", "ExecuteCmd failed");
         return retcode;
      }

      sqlite3_reset(fInsertStmt);
      sqlite3_bind_text(fInsertStmt, 1, cmd.c_str(), cmd.size(), SQLITE_STATIC);
      sqlite3_bind_text(fInsertStmt, 2, response.c_str(), response.size(), SQLITE_STATIC);
      if (sqlite3_step(fInsertStmt) != SQLITE_DONE)
      {
         T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
      }
      T::Info("CachingLogReader", "Saved data to SQLite cache");

      retcode = T::Read(response, values);
      if (retcode)
      {
         T::Error("CachingLogReader", "error parsing response");
         return 1;
      }
   }
   else
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
      return 1;
   }
   return 0;
}

template
class CachingLogReader<SshLogReader>;
