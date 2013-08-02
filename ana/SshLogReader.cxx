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


string SshLogReader::GetSshCommand(const char *export_params)
{
   char buf[8192];

   snprintf(buf, sizeof(buf),
            "ssh acnlina \""
            "setenv PATH /usr/controls/bin; setenv LD_LIBRARY_PATH /ride/release/X86/lib;"
            "exportLoggerData"
            " -logger '%s'"
            " -cells '%s'"
            " -timeformat 'unix'"
            " -showmissingdatawith x"
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


int SshLogReader::Read(FILE *fd, map< string, map<cdev_time_t, double> > *values)
{
   char buf[8192];

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
            char cc;
            do
            {
               cc = getc(fd);
            }
            while((cc == ' ') || (cc == '\t'));
            if (cc == 'x')
            {
               continue;
            }
            ungetc(cc, fd);
            len = fscanf(fd, "%lf", &value);
            if (len != 1)
            {
               Error("SshLogReader", "unexpected end of row");
               return 1;
            }

            if (value >= 90000000000000000000000000000000000000.0)
            {
               continue;
            }

            (*values)[*it][time] = value;
         }
      }
   }
}


int SshLogReader::Run(string cmd, map< string, map<cdev_time_t, double> > *values)
{
   Info("SshLogReader", "Running %s", cmd.c_str());

   FILE *fd = popen(cmd.c_str(), "r");
   if (!fd)
   {
      Error("SshLogReader", "popen failed, errno = %i", errno);
      return 1;
   }

   Read(fd, values);

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


int SshLogReader::ReadTimeRange(time_t start, time_t end, map< string, map<cdev_time_t, double> > *values)
{
   string cmd = GetSshCommandForTimeRange(start, end);
   return Run(cmd, values);
}


int SshLogReader::ReadFill(int fill_id, map< string, map<cdev_time_t, double> > *values)
{
   string cmd = GetSshCommandForFillId(fill_id);
   return Run(cmd, values);
}


void SshLogReader::CalculateMean(const map< string, map<cdev_time_t, double> > &values, map<string, double> *mean_value)
{
   for (map< string, map<cdev_time_t, double> >::const_iterator it = values.begin();
        it != values.end(); it++)
   {
      const string &key = it->first;
      const map<cdev_time_t, double> &m = it->second;
      double mean = -1;

      if (m.size() != 0)
      {
         mean = 0;
         for(map<cdev_time_t, double>::const_iterator it = m.begin();
             it != m.end(); it++)
         {
            mean += it->second;
         }
         mean /= m.size();
      }

      (*mean_value)[key] = mean;
   }
}


int SshLogReader::ReadTimeRangeMean(time_t start, time_t end, map<string, double> *mean_value)
{
   map< string, map<cdev_time_t, double> > values;

   int retcode = ReadTimeRange(start, end, &values);

   if (retcode != 0)
   {
      return retcode;
   }

   CalculateMean(values, mean_value);

   return retcode;
}


int SshLogReader::ReadFillMean(int fill_id, map<string, double> *mean_value)
{
   map< string, map<cdev_time_t, double> > values;

   int retcode = ReadFill(fill_id, &values);

   if (retcode != 0)
   {
      return retcode;
   }

   CalculateMean(values, mean_value);

   return retcode;
}
