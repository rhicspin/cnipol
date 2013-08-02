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
   const char *sql = "CREATE TABLE IF NOT EXISTS cache_map (start_time DOUBLE, end_time DOUBLE, fill_id INT, cdev_cell CHAR(255) NOT NULL, data_id INTEGER PRIMARY KEY AUTOINCREMENT);"
                     "CREATE TABLE IF NOT EXISTS cache_data (data_id INTEGER NOT NULL, time DOUBLE NOT NULL, value DOUBLE NOT NULL);";
   int ret = sqlite3_exec(fDB, sql, NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }
   char map_select_query[] = "SELECT cdev_cell, data_id FROM cache_map WHERE start_time = ? AND end_time = ?;";
   ret = sqlite3_prepare_v2(fDB, map_select_query, sizeof(map_select_query), &fMapSelectStmt, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Error preparing map_select_query: %s", sqlite3_errmsg(fDB));
   }
   char data_select_query[] = "SELECT time, value, cdev_cell FROM cache_data INNER JOIN cache_map WHERE start_time = ? AND end_time = ? AND cache_data.data_id = cache_map.data_id;";
   ret = sqlite3_prepare_v2(fDB, data_select_query, sizeof(data_select_query), &fDataSelectStmt, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Error preparing data_select_query: %s", sqlite3_errmsg(fDB));
   }
   const char map_insert_query[] = "INSERT INTO cache_map (start_time, end_time, cdev_cell) VALUES (?, ?, ?);";
   ret = sqlite3_prepare_v2(fDB, map_insert_query, sizeof(map_insert_query), &fMapInsertStmt, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Error preparing map_insert_query: %s", sqlite3_errmsg(fDB));
   }
   const char data_insert_query[] = "INSERT INTO cache_data (data_id, time, value) VALUES (?, ?, ?);";
   ret = sqlite3_prepare_v2(fDB, data_insert_query, sizeof(data_insert_query), &fDataInsertStmt, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "Error preparing data_insert_query: %s", sqlite3_errmsg(fDB));
   }
}


template<class T>
CachingLogReader<T>::~CachingLogReader()
{
   sqlite3_close(fDB);
}


template<class T>
int CachingLogReader<T>::ReadTimeRange(time_t start, time_t end, map< string, map<cdev_time_t, double> > *values)
{
   vector<string>	cells(T::fCells);
   int ret;
   sqlite3_reset(fMapSelectStmt);
   sqlite3_bind_double(fMapSelectStmt, 1, start);
   sqlite3_bind_double(fMapSelectStmt, 2, end);

   while ((ret = sqlite3_step(fMapSelectStmt)) == SQLITE_ROW)
   {
      string	cdev_cell = (const char*)sqlite3_column_text(fMapSelectStmt, 0);
      vector<string>::iterator	it = find(cells.begin(), cells.end(), cdev_cell);
      if (it != cells.end())
      {
         cells.erase(it); // Mark cell as present in cache
      }
   }
   if (ret != SQLITE_DONE)
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   sqlite3_reset(fDataSelectStmt);
   sqlite3_bind_double(fDataSelectStmt, 1, start);
   sqlite3_bind_double(fDataSelectStmt, 2, end);

   while ((ret = sqlite3_step(fDataSelectStmt)) == SQLITE_ROW)
   {
      string	cdev_cell = (const char*)sqlite3_column_text(fDataSelectStmt, 2);
      map<cdev_time_t, double>  &cell_map = (*values)[cdev_cell];
      double      time = sqlite3_column_double(fDataSelectStmt, 0);
      double      value = sqlite3_column_double(fDataSelectStmt, 1);
      cell_map[time] = value;
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

   int retcode = T::ReadTimeRange(start, end, values);

   ret = sqlite3_exec(fDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   for (vector<string>::const_iterator it = T::fCells.begin(); it != T::fCells.end(); it++)
   {
      const string	&cdev_cell = *it;
      sqlite3_reset(fMapInsertStmt);
      sqlite3_bind_double(fMapInsertStmt, 1, start);
      sqlite3_bind_double(fMapInsertStmt, 2, end);
      sqlite3_bind_text(fMapInsertStmt, 3, cdev_cell.c_str(), cdev_cell.size(), SQLITE_TRANSIENT);
      if (sqlite3_step(fMapInsertStmt) != SQLITE_DONE)
      {
         T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
      }
      sqlite3_reset(fDataInsertStmt);
      sqlite3_bind_int(fDataInsertStmt, 1, sqlite3_last_insert_rowid(fDB));
      if (values->count(cdev_cell))
      {
         const map<cdev_time_t, double>  &cell_map = (*values)[cdev_cell];
         for(typename map<cdev_time_t, double>::const_iterator it = cell_map.begin();
             it != cell_map.end(); it++)
         {
            double	time = it->first;
            double	value = it->second;
            sqlite3_bind_double(fDataInsertStmt, 2, time);
            sqlite3_bind_double(fDataInsertStmt, 3, value);
            if (sqlite3_step(fDataInsertStmt) != SQLITE_DONE)
            {
               T::Error("CachingLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
            }
            sqlite3_reset(fDataInsertStmt);
         }
      }
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
