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
   static string GetSshCommand(const string &logger, const string &args);
   static string GetSshCommandForTimeRange(const string &logger, time_t start, time_t end, const string additional_args = "");
   static string GetSshCommandForFillId(const string &logger, int fill_id, const string additional_args = "");

protected:

   vector<string>  fCells;

public:

   void Read(string response, opencdev::result_t *values);
   void ExecuteCmd(string cmd, string *response);
   virtual void Run(string cmd, opencdev::result_t *values);
   virtual void query_timerange(const string &logger, opencdev::cdev_time_t start, opencdev::cdev_time_t end, opencdev::result_t *values) override;
   virtual void query_fill(const string &logger, int fill_id, opencdev::result_t *values) override;
   void get_fill_events(int fill_id, const string &ev_name, vector<opencdev::cdev_time_t> *values);
};

#endif
