#ifndef SshLogReader_h
#define SshLogReader_h

#include <map>
#include <string>
#include <vector>
#include <ctime>

#include <TObject.h>

#include <sqlite3.h>

using std::map;
using std::string;
using std::vector;


class SshLogReader : TObject
{
private:

   string          fLoggersStr;
   string          fCellsStr;
   vector<string>  fCells;
   sqlite3	*fDB;
   sqlite3_stmt	*fSelectStmt, *fInsertStmt;

   string GetSshCommand(const char *export_params);
   string GetSshCommandForTimeRange(time_t start, time_t end);
   string GetSshCommandForFillId(int fill_id);
   static void CalculateMean(const map< string, vector<double> > &values, map<string, double> *mean_value);

public:

   /**
    * @param loggers comma separated list of loggers
    * @param cells   comma separated list of cells
    */
   SshLogReader(string loggers, string cells);
   ~SshLogReader();
   int Run(string cmd, map< string, vector<double> > *values);
   /**
    * @returns 0 if everything was ok
    */
   int ReadTimeRange(time_t start, time_t end, map< string, vector<double> > *values);
   int ReadFill(int fill_id, map< string, vector<double> > *values);
   /**
    * @returns 0 if everything was ok
    */
   int ReadTimeRangeMean(time_t start, time_t end, map<string, double> *mean_value);
   int ReadFillMean(int fill_id, map<string, double> *mean_value);
};

#endif
