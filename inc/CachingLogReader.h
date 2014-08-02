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

   static sqlite3      *fDB;
   static sqlite3_stmt *fSelectStmt, *fInsertStmt;

public:

   typedef typename T::cdev_time_t	cdev_time_t;

   CachingLogReader(string logger);
   ~CachingLogReader();

   virtual int Run(string cmd, map< string, map<cdev_time_t, double> > *values);
};

#endif
