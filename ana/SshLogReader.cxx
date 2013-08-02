#include <numeric>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>

#include <errno.h>

#include <sqlite3.h>

#include "SshLogReader.h"


SshLogReader::SshLogReader(string loggers, string cells)
      : fLoggersStr(loggers)
      , fCellsStr(cells)
{
   if (sqlite3_open("cdev_cache.sqlite", &fDB) != SQLITE_OK)
   {
      Error("SshLogReader", "Can't open cdev cache");
   }
   const char *sql = "CREATE TABLE IF NOT EXISTS mean_cache (start_time DOUBLE, end_time DOUBLE, fill_id INT, cdev_cell CHAR(255) NOT NULL, value DOUBLE);";
   int ret = sqlite3_exec(fDB, sql, NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      Error("SshLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }
   char select_query[] = "SELECT cdev_cell, value FROM mean_cache WHERE start_time = ? AND end_time = ?;";
   ret = sqlite3_prepare_v2(fDB, select_query, sizeof(select_query), &fSelectStmt, NULL);
   if (ret != SQLITE_OK)
   {
      Error("SshLogReader", "Error preparing select_query: %s", sqlite3_errmsg(fDB));
   }
   const char insert_query[] = "INSERT INTO mean_cache (start_time, end_time, cdev_cell, value) VALUES (?, ?, ?, ?);";
   ret = sqlite3_prepare_v2(fDB, insert_query, sizeof(insert_query), &fInsertStmt, NULL);
   if (ret != SQLITE_OK)
   {
      Error("SshLogReader", "Error preparing insert_query: %s", sqlite3_errmsg(fDB));
   }

   if (index(fLoggersStr.c_str(), '\'') || (index(fCellsStr.c_str(), '\''))
       || index(fLoggersStr.c_str(), '"') || (index(fCellsStr.c_str(), '"')))
   {
      Error("SshLogReader", "Wrong params.");
   }

   char *copy = strdup(fCellsStr.c_str());
   fCells.push_back(strtok(copy, ","));
   const char *tok;
   while ((tok = strtok(NULL, ",")))
   {
      fCells.push_back(tok);
   }
   free(copy);
}


SshLogReader::~SshLogReader()
{
   sqlite3_close(fDB);
}


string SshLogReader::GetSshCommand(const char *export_params)
{
   char buf[1024];

   snprintf(buf, sizeof(buf),
            "ssh acnlina \""
            "setenv PATH /usr/controls/bin; setenv LD_LIBRARY_PATH /ride/release/X86/lib;"
            "exportLoggerData"
            " -logger '%s'"
            " -cells '%s'"
            " -timeformat 'unix'"
            " -dataformat '%%10.6f'"
            " -excluderowswithholes"
            "%s\"",
            fLoggersStr.c_str(), fCellsStr.c_str(), export_params);

   return string(buf);
}


string SshLogReader::GetSshCommandForTimeRange(time_t start, time_t end)
{
   char export_params[1024], startStr[32], endStr[32];

   ctime_r(&start, startStr); // this should have timezone problems
   ctime_r(&end, endStr);
   startStr[strlen(startStr) - 1] = 0; // replace newlines with end-of-string marks
   endStr[strlen(endStr) - 1] = 0;
   snprintf(export_params, sizeof(export_params),
            " -start '%s'"
            " -stop '%s'",
            startStr, endStr);

   return GetSshCommand(export_params);
}


string SshLogReader::GetSshCommandForFillId(int fill_id)
{
   char export_params[1024];

   snprintf(export_params, sizeof(export_params),
	    " -fill '%i'",
	    fill_id);

   return GetSshCommand(export_params);
}


int SshLogReader::Run(string cmd, map< string, vector<double> > *values)
{
   Info("SshLogReader", "Running %s", cmd.c_str());

   FILE *fd = popen(cmd.c_str(), "r");
   if (!fd)
   {
      Error("SshLogReader", "popen failed, errno = %i", errno);
      return 1;
   }

   char buf[1024];

   while (!feof(fd))
   {
      char c = ungetc(fgetc(fd), fd);
      if (c == '#')
      {
         if (fgets(buf, sizeof(buf), fd))
         {
            buf[strlen(buf) - 1] = 0;
            Info("SshLogReader", "Read comment: %s", buf);
         }
      }
      else
      {
         int len;
         double time, value;
         len = fscanf(fd, "%lf", &time);
         if (len != 1) break;

         for (vector<string>::const_iterator it = fCells.begin(); it != fCells.end(); it++)
         {
            len = fscanf(fd, "%lf", &value);
            if (len != 1)
            {
               Error("SshLogReader", "unexpected end of row");
               return 1;
            }

            (*values)[*it].push_back(value);
         }
      }
   }

   int retcode = pclose(fd);

   if (retcode == -1)
   {
      Error("SshLogReader", "pclose failed, errno = %i", errno);
      return 1;
   }
   if (retcode)
   {
      Error("SshLogReader", "process returned %i", retcode);
      Error("SshLogReader",
         "You need to create a gateway to acnlina machine.\n"
         "In order to do that, add following into your ~/.ssh/config file:\n\n"
         "Host acnlina\n"
         "\tIdentityFile    /star/u/veprbl/.ssh/acnlina5_rsa\n"
         "\tUser            dsmirnov\n"
         "\tHostName        localhost\n"
         "\tPort            8022\n\n"
         "To start gateway itself use following command:\n\n"
         "ssh -l your_user_name ssh.pbn.bnl.local -L 8022:acnlina5.pbn.bnl.gov:22\n\n"
         "replacing your_user_name with your MS Exchange (RSA SecurID token) account.\n"
         "If you have problems, try using --no-ssh option."
         );
   }

   return retcode;
}


int SshLogReader::ReadTimeRange(time_t start, time_t end, map< string, vector<double> > *values)
{
   string cmd = GetSshCommandForTimeRange(start, end);
   return Run(cmd, values);
}


int SshLogReader::ReadFill(int fill_id, map< string, vector<double> > *values)
{
   string cmd = GetSshCommandForFillId(fill_id);
   return Run(cmd, values);
}


void SshLogReader::CalculateMean(const map< string, vector<double> > &values, map<string, double> *mean_value)
{
   for (map< string, vector<double> >::const_iterator it = values.begin();
        it != values.end(); it++)
   {
      const string &key = it->first;
      const vector<double> &v = it->second;
      double mean = -1;

      if (v.size() != 0)
      {
         mean = std::accumulate(v.begin(), v.end(), (double)0.0) / v.size();
      }

      (*mean_value)[key] = mean;
   }
}


int SshLogReader::ReadTimeRangeMean(time_t start, time_t end, map<string, double> *mean_value)
{
   map< string, vector<double> > values;
   vector<string>	cells(fCells);
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
         Error("SshLogReader", "Cells are not in cache, retreiving via ssh.");
      }
   }
   else
   {
      Error("SshLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   int retcode = ReadTimeRange(start, end, &values);

   if (retcode != 0)
   {
      return retcode;
   }

   CalculateMean(values, mean_value);

   ret = sqlite3_exec(fDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      Error("SshLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   for (vector<string>::const_iterator it = fCells.begin(); it != fCells.end(); it++)
   {
      const string	cdev_cell = *it;
      sqlite3_bind_double(fInsertStmt, 1, start);
      sqlite3_bind_double(fInsertStmt, 2, end);
      ret = sqlite3_bind_text(fInsertStmt, 3, cdev_cell.c_str(), cdev_cell.size(), SQLITE_TRANSIENT);
      if (ret != SQLITE_OK)
         Error("SshLogReader", "%s", sqlite3_errmsg(fDB));
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
         Error("SshLogReader", "insert: ret = %i, %s", ret, sqlite3_errmsg(fDB));
      }
      sqlite3_reset(fInsertStmt);
   }

   ret = sqlite3_exec(fDB, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   if (ret != SQLITE_OK)
   {
      Error("SshLogReader", "sqlite error: %s", sqlite3_errmsg(fDB));
   }

   Info("SshLogReader", "Saved data to SQLite cache");
   return retcode;
}


int SshLogReader::ReadFillMean(int fill_id, map<string, double> *mean_value)
{
   map< string, vector<double> > values;

   int retcode = ReadFill(fill_id, &values);

   if (retcode != 0)
   {
      return retcode;
   }

   CalculateMean(values, mean_value);

   return retcode;
}
