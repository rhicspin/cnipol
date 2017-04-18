#ifndef CachingLogReader_h
#define CachingLogReader_h

#include <map>
#include <string>
#include <vector>

#include <sqlite3.h>

#include <opencdev.h>

using std::map;
using std::string;
using std::vector;

template<class T>
class CachingLogReader : public T
{
private:

   static sqlite3      *fDB;
   static sqlite3_stmt *fSelectStmt, *fInsertStmt, *fDeleteStmt;
   static int           fDBRefCnt;

   string fLastCmd;

   void RemoveLastCache();

public:

   CachingLogReader();
   ~CachingLogReader();

   virtual string GetDatabaseFilePath();
   virtual void Run(string cmd, opencdev::result_t *values) override;
   virtual void get_fill_events(int fill_id, const string &ev_name, vector<opencdev::cdev_time_t> *values) override;
};

#endif
