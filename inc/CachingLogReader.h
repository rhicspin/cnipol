#ifndef CachingLogReader_h
#define CachingLogReader_h

#include <map>
#include <string>

#include <sqlite3.h>

using std::map;
using std::string;

template<class T>
class CachingLogReader : public T
{
private:

   sqlite3      *fDB;
   sqlite3_stmt *fSelectStmt, *fInsertStmt;

public:
   CachingLogReader(string loggers, string cells);
   ~CachingLogReader();

   int ReadTimeRangeMean(time_t start, time_t end, map<string, double> *mean_value);
};

#endif
