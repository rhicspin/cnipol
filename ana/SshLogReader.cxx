#include <numeric>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <errno.h>

#include "SshLogReader.h"


SshLogReader::SshLogReader(string loggers, string cells)
      : fLoggersStr(loggers)
      , fCellsStr(cells)
{
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


string SshLogReader::GetSshCommand(time_t start, time_t end)
{
   char buf[1024], startStr[32], endStr[32];

   ctime_r(&start, startStr); // this should have timezone problems
   ctime_r(&end, endStr);
   startStr[strlen(startStr) - 1] = 0; // replace newlines with end-of-string marks
   endStr[strlen(endStr) - 1] = 0;
   snprintf(buf, sizeof(buf),
            "ssh aclina \""
            "setenv PATH /usr/controls/bin; setenv LD_LIBRARY_PATH /ride/release/X86/lib;"
            "exportLoggerData"
            " -logger '%s'"
            " -cells '%s'"
            " -timeformat 'unix'"
            " -dataformat '%%10.6f'"
            " -excluderowswithholes"
            " -start '%s'"
            " -stop '%s'"
            "\"",
            fLoggersStr.c_str(), fCellsStr.c_str(), startStr, endStr);

   return string(buf);
}


int SshLogReader::Read(time_t start, time_t end, map< string, vector<double> > *values)
{
   string cmd = GetSshCommand(start, end);

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
   }

   return retcode;
}


int SshLogReader::ReadMean(time_t start, time_t end, map<string, double> *mean_value)
{
   map< string, vector<double> > values;

   int retcode = Read(start, end, &values);

   if (retcode != 0)
   {
      return retcode;
   }

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

   return retcode;
}
