#include <numeric>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cassert>

#include <errno.h>

#include <TSystem.h>
#include <sqlite3.h>

#include "SshLogReader.h"


string SshLogReader::GetSshCommand(const string &logger, const string &args)
{
   char buf[8192];

   if (index(logger.c_str(), '\'') || index(logger.c_str(), '"'))
   {
      throw "Wrong params.";
   }

   snprintf(buf, sizeof(buf),
            "ssh yellowpc \""
            "setenv PATH /usr/controls/bin; setenv LD_LIBRARY_PATH /ride/release/X86/lib;"
            "exportLoggerData"
            " -logger '%s'"
            // Print 15 digits after decimal point. We are going to check if
            // the value is not too small during the read. The main point here
            // is that we can't use %e because that it precision for big
            // numbers (e.g. unix timestamps)
            " -dataformat '%%.15f'"
            " -timeformat 'unix'"
            " -showmissingdatawith x"
            " -timetolerance 0"
            "%s"
            "\"",
            logger.c_str(), args.c_str());

   return string(buf);
}


string SshLogReader::GetSshCommandForTimeRange(const string &logger, time_t start, time_t end, const string &additional_args)
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

   string args = export_params + additional_args;

   return GetSshCommand(logger, args);
}


string SshLogReader::GetSshCommandForFillId(const string &logger, int fill_id, const string &additional_args)
{
   char export_params[1024];

   snprintf(export_params, sizeof(export_params),
	    " -fill '%i'",
	    fill_id);

   string args = export_params + additional_args;

   return GetSshCommand(logger, args);
}


string SshLogReader::ReadStream(FILE *fd)
{
   size_t       buf_size = 1024*64, data_size = 0;
   char *buf = (char*)malloc(buf_size), *pos = buf;

   while(!feof(fd))
   {
      size_t read_len = buf_size - data_size;
      size_t len = fread(pos, 1, read_len, fd);
      if ((len < read_len) && ferror(fd))
      {
         perror("SshLogReader::ReadALL");
         throw "fread() error";
      }
      data_size += len;
      if (data_size == buf_size)
      {
         buf_size *= 2;
         buf = (char*)realloc(buf, buf_size);
      }
      pos = buf + data_size;
   }
   return string(buf, data_size);
}

vector<string> SshLogReader::ParseCellList(string line)
{
   vector<string> result;
   char *copy = strdup(line.c_str());
   const char *tok = strtok(copy, " \t");
   assert(string(tok) == "Time");
   while ((tok = strtok(NULL, " \t")))
   {
      if (strlen(tok))
      {
         result.push_back(tok);
      }
   }
   free(copy);
   return result;
}

void SshLogReader::set_additional_args(const string &additional_args)
{
   if (additional_args.find("\"") != string::npos)
   {
      gSystem->Error("SshLogReader", "invalid input: additional_args must not contain \". use ' instead");
   }
   fAdditionalArgs = additional_args;
};

void SshLogReader::Read(string response, map< string, map<opencdev::cdev_time_t, double> > *values)
{
   FILE *fd = fmemopen((void*)response.c_str(), response.size(), "r");
   char buf[8192];
   int line = 1;
   vector<string> cells;

   while (!feof(fd))
   {
      char c = ungetc(fgetc(fd), fd);
      if (c == '#')
      {
         if (fgets(buf, sizeof(buf), fd))
         {
            assert(strlen(buf) > strlen("# Time"));
            buf[strlen(buf) - 1] = 0;
            if (line == 3)
            {
               cells = ParseCellList(&buf[2]);
            }
            line++;
         }
      }
      else
      {
         int len;
         double time, value;
         len = fscanf(fd, "%lf", &time);
         if (len != 1) break;

         for (vector<string>::const_iterator it = cells.begin(); it != cells.end(); it++)
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
            if ((value != 0) && (fabs(value) < 1e-10))
            {
               // Here we try to detect small values. Because exportLoggerData
               // already did rounding for us we can't detect non-zero values
               // that are less than 1e-15 (we use '%.15f' format string
               // above), but we at least can complain about somewhat small
               // values. If you are reading this you probably should switch to
               // opencdev interface that reads binary files and doesn't have
               // this problem.
               gSystem->Error("SshLogReader", "Detected a small non-zero value %e. "
                     "This interface is not designed to handle these.", value);
            }
            if (len != 1)
            {
               throw "unexpected end of row";
            }

            (*values)[*it][time] = value;
         }
      }
   }

   fclose(fd);
}


void SshLogReader::ExecuteCmd(string cmd, string *response)
{
   gSystem->Info("SshLogReader", "Running %s", cmd.c_str());
   int retries = 5;
   int retcode;

   while(retries--)
   {
      FILE *fd = popen(cmd.c_str(), "r");
      if (!fd)
      {
         gSystem->Error("SshLogReader", "popen failed, errno = %i", errno);
         throw "popen failed";
      }

      *response = ReadStream(fd);

      retcode = pclose(fd);

      if (retcode == -1)
      {
         gSystem->Error("SshLogReader", "pclose failed, errno = %i", errno);
         throw "pclose failed";
      }
      if (retcode)
      {
         if ((response->find("Can't find start time for event specified") != string::npos)
             || (response->find("Can't find stop time for event specified") != string::npos))
         {
            gSystem->Error("SshLogReader", "event selector failed. Additional args: '%s'", fAdditionalArgs.c_str());
            return;
         }

         gSystem->Error("SshLogReader", "process returned %i", retcode);
         gSystem->Error("SshLogReader",
            "You need to create a gateway to yellowpc.\n"
            "See https://wiki.bnl.gov/rhicspin/Automated_pC_setup for more information.\n"
            );
         const int delay = 10;
         gSystem->Error("SshLogReader", "Retrying in %i seconds... Retries left %i", delay, retries);
         sleep(delay);
      }
      else
      {
         break;
      }
   }

   if (retcode)
   {
      throw "non-zero retcode";
   }
}


void SshLogReader::Run(string cmd, opencdev::result_t *values)
{
   string response;
   sleep(2); // a delay to give the exportLoggerData a break
   ExecuteCmd(cmd, &response);
   Read(response, values);
}


void SshLogReader::query_timerange(const string &logger, opencdev::cdev_time_t start, opencdev::cdev_time_t end, opencdev::result_t *values)
{
   string cmd = GetSshCommandForTimeRange(logger, start, end, fAdditionalArgs);
   Run(cmd, values);
}


void SshLogReader::query_fill(const string &logger, int fill_id, opencdev::result_t *values)
{
   string cmd = GetSshCommandForFillId(logger, fill_id, fAdditionalArgs);
   Run(cmd, values);
}
