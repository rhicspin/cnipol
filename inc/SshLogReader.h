#ifndef SshLogReader_h
#define SshLogReader_h

#include <map>
#include <string>
#include <vector>
#include <ctime>

#include <TObject.h>

#include <opencdev.h>

using std::map;
using std::string;
using std::vector;

class SshLogReader : public opencdev::LogReader
{
private:

   vector<string> ParseCellList(string line);
   string ReadStream(FILE *fd);
   string GetSshCommand(const string &logger, const char *export_params);
   string GetSshCommandForTimeRange(const string &logger, time_t start, time_t end);
   string GetSshCommandForFillId(const string &logger, int fill_id);

protected:

   vector<string>  fCells;
   string fAdditionalArgs;

public:

   void set_additional_args(const string &additional_args);
   void Read(string response, opencdev::result_t *values);
   void ExecuteCmd(string cmd, string *response);
   virtual void Run(string cmd, opencdev::result_t *values);
   virtual void query_timerange(const string &logger, opencdev::cdev_time_t start, opencdev::cdev_time_t end, opencdev::result_t *values);
   virtual void query_fill(const string &logger, int fill_id, opencdev::result_t *values);
};

#endif
