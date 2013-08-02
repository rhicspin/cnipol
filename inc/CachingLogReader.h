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

   typedef typename T::cdev_time_t	cdev_time_t;

   CachingLogReader(string loggers, string cells);
   ~CachingLogReader();

   virtual int Run(string cmd, map< string, map<cdev_time_t, double> > *values);
};

#endif
