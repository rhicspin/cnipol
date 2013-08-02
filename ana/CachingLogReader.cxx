#include <vector>
#include <algorithm>

#include <sqlite3.h>

#include "CachingLogReader.h"
#include "SshLogReader.h" // used for <T=SshLogReader> instantiation (see in the end of this file)

using std::vector;


template<class T>
CachingLogReader<T>::CachingLogReader(string loggers, string cells)
   : T(loggers, cells)
{
   if (sqlite3_open("cdev_cache.sqlite", &fDB) != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Can't open cdev cache");
   }
   const char *sql = "CREATE TABLE IF NOT EXISTS mean_cache (start_time DOUBLE, end_time DOUBLE, fill_id INT, cdev_cell CHAR(255) NOT NULL, value DOUBLE);";
   int ret = sqlite3_exec(fDB, sql, NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }
   char select_query[] = "SELECT cdev_cell, value FROM mean_cache WHERE start_time = ? AND end_time = ?;";
   ret = sqlite3_prepare_v2(fDB, select_query, sizeof(select_query), &fSelectStmt, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Error preparing select_query: %s", sqlite3_errmsg(fDB));
   }
   const char insert_query[] = "INSERT INTO mean_cache (start_time, end_time, cdev_cell, value) VALUES (?, ?, ?, ?);";
   ret = sqlite3_prepare_v2(fDB, insert_query, sizeof(insert_query), &fInsertStmt, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Error preparing insert_query: %s", sqlite3_errmsg(fDB));
   }
}


template<class T>
CachingLogReader<T>::~CachingLogReader()
{
   sqlite3_close(fDB);
}


template<class T>
int CachingLogReader<T>::ReadTimeRangeMean(time_t start, time_t end, map<string, double> *mean_value)
{
   vector<string>	cells(T::fCells);
   int ret;
   sqlite3_bind_double(fSelectStmt, 1, start);
   sqlite3_bind_double(fSelectStmt, 2, end);

   while ((ret = sqlite3_step(fSelectStmt)) == SQLITE_ROW)
   {
      string	cdev_cell = (const char*)sqlite3_column_text(fSelectStmt, 0);
      vector<string>::iterator	it = find(cells.begin(), cells.end(), cdev_cell);
      if (it != cells.end())
      {
         if (sqlite3_column_type(fSelectStmt, 1) != SQLITE_NULL)
         {
            (*mean_value)[cdev_cell] = sqlite3_column_double(fSelectStmt, 1);
         }
         cells.erase(it);
      }
   }
   if (ret == SQLITE_DONE)
   {
      if (cells.empty())
      {
         return 0;
      }
      else
      {
         T::Error("CachingLogReader", "Cells are not in cache, retreiving via ssh.");
      }
   }
   else
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   int retcode = T::ReadTimeRangeMean(start, end, mean_value);

   ret = sqlite3_exec(fDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   for (vector<string>::const_iterator it = T::fCells.begin(); it != T::fCells.end(); it++)
   {
      const string	cdev_cell = *it;
      sqlite3_bind_double(fInsertStmt, 1, start);
      sqlite3_bind_double(fInsertStmt, 2, end);
      ret = sqlite3_bind_text(fInsertStmt, 3, cdev_cell.c_str(), cdev_cell.size(), SQLITE_TRANSIENT);
      if (ret != SQLITE_OK)
         T::Error("CachingLogReader", "%s", sqlite3_errmsg(fDB));
      if (mean_value->count(cdev_cell))
      {
         sqlite3_bind_double(fInsertStmt, 4, (*mean_value)[cdev_cell]);
      }
      else
      {
         sqlite3_bind_null(fInsertStmt, 4);
      }
      ret = sqlite3_step(fInsertStmt);
      if (ret != SQLITE_DONE)
      {
         T::Error("CachingLogReader", "insert: ret = %i, %s", ret, sqlite3_errmsg(fDB));
      }
      sqlite3_reset(fInsertStmt);
   }

   ret = sqlite3_exec(fDB, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   T::Info("CachingLogReader", "Saved data to SQLite cache");
   return retcode;
}

template
class CachingLogReader<SshLogReader>;
