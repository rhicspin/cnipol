#include <vector>
#include <algorithm>

#include <TSystem.h>

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
CachingLogReader<T>::CachingLogReader()
{
   if (!fDB)
   {
      if (sqlite3_open(GetDatabaseFilePath().c_str(), &fDB) != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "Can't open cdev cache");
      }
      const char *sql = "CREATE TABLE IF NOT EXISTS cache (query TEXT PRIMARY KEY, response TEXT);";
      int ret = sqlite3_exec(fDB, sql, NULL, NULL, NULL);
      if (ret != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
      }
      char select_query[] = "SELECT response FROM cache WHERE query = ? LIMIT 1;";
      ret = sqlite3_prepare_v2(fDB, select_query, sizeof(select_query), &fSelectStmt, NULL);
      if (ret != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "Error preparing select_query: %s", sqlite3_errmsg(fDB));
      }
      const char insert_query[] = "INSERT INTO cache (query, response) VALUES (?, ?);";
      ret = sqlite3_prepare_v2(fDB, insert_query, sizeof(insert_query), &fInsertStmt, NULL);
      if (ret != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "Error preparing insert_query: %s", sqlite3_errmsg(fDB));
      }
   }
}


template<class T>
CachingLogReader<T>::~CachingLogReader()
{
   if (fDB)
   {
      if (sqlite3_finalize(fSelectStmt) != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "Error finalizing select_query");
      }
      if (sqlite3_finalize(fInsertStmt) != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "Error finalizing insert_query");
      }
      if (sqlite3_close(fDB) != SQLITE_OK)
      {
         gSystem->Error("CachingLogReader", "Failed to close cache");
      }
      fDB = NULL;
   }
}


template<class T>
string CachingLogReader<T>::GetDatabaseFilePath()
{
   return ("/tmp/cdev_cache-" + gSystem->GetUserInfo()->fUser + ".sqlite").Data();
}


template<class T>
void CachingLogReader<T>::Run(string cmd, opencdev::result_t *values)
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
      gSystem->Info("CachingLogReader", "Query result is not in cache, retreiving via ssh.");

      string response;
      T::ExecuteCmd(cmd, &response);

      sqlite3_reset(fInsertStmt);
      sqlite3_bind_text(fInsertStmt, 1, cmd.c_str(), cmd.size(), SQLITE_STATIC);
      sqlite3_bind_text(fInsertStmt, 2, response.c_str(), response.size(), SQLITE_STATIC);
      if (sqlite3_step(fInsertStmt) != SQLITE_DONE)
      {
         gSystem->Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
      }
      gSystem->Info("CachingLogReader", "Saved data to SQLite cache");

      T::Read(response, values);
   }
   else
   {
      throw sqlite3_errmsg(fDB);
   }
}

template
class CachingLogReader<SshLogReader>;
