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


string SshLogReader::GetSshCommand(const string &logger, const char *export_params)
{
   char buf[8192];

   if (index(logger.c_str(), '\'') || index(logger.c_str(), '"'))
   {
      throw "Wrong params.";
   }

   snprintf(buf, sizeof(buf),
            "ssh acnlina \""
            "setenv PATH /usr/controls/bin; setenv LD_LIBRARY_PATH /ride/release/X86/lib;"
            "exportLoggerData"
            " -logger '%s'"
            " -timeformat 'unix'"
            " -showmissingdatawith x"
            " -timetolerance 0"
            "%s\"",
            logger.c_str(), export_params);

   return string(buf);
}


string SshLogReader::GetSshCommandForTimeRange(const string &logger, time_t start, time_t end)
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

   return GetSshCommand(logger, export_params);
}


string SshLogReader::GetSshCommandForFillId(const string &logger, int fill_id)
{
   char export_params[1024];

   snprintf(export_params, sizeof(export_params),
	    " -fill '%i'",
	    fill_id);

   return GetSshCommand(logger, export_params);
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
   assert(string(strtok(copy, " \t")) == "Time");
   const char *tok;
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
            gSystem->Info("SshLogReader", "Read comment: %s", buf);
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
         gSystem->Error("SshLogReader", "process returned %i", retcode);
         gSystem->Error("SshLogReader",
            "You need to create a gateway to acnlina machine.\n"
            "In order to do that, add following into your ~/.ssh/config file:\n\n"
            "Host acnlina\n"
            "\tIdentityFile    /star/u/veprbl/.ssh/acnlina5_rsa\n"
            "\tUser            dsmirnov\n"
            "\tHostName        localhost\n"
            "\tPort            8022\n"
            "\tBatchMode       yes\n\n"
            "To start gateway itself use following command:\n\n"
            "ssh -l your_user_name ssh.pbn.bnl.local -L 8022:acnlina5.pbn.bnl.gov:22\n\n"
            "replacing your_user_name with your MS Exchange (RSA SecurID token) account.\n"
            "If you have problems, try using --no-ssh option."
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
   string cmd = GetSshCommandForTimeRange(logger, start, end);
   Run(cmd, values);
}


void SshLogReader::query_fill(const string &logger, int fill_id, opencdev::result_t *values)
{
   string cmd = GetSshCommandForFillId(logger, fill_id);
   Run(cmd, values);
}
