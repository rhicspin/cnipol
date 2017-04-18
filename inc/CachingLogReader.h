#ifndef CachingLogReader_h
#define CachingLogReader_h

#include <map>
#include <string>

#include <sqlite3.h>

#include <opencdev.h>

using std::map;
using std::string;

template<class T>
class CachingLogReader : public T
{
private:

   static sqlite3      *fDB;
   static sqlite3_stmt *fSelectStmt, *fInsertStmt;
   static int           fDBRefCnt;

public:

   CachingLogReader();
   ~CachingLogReader();

   virtual string GetDatabaseFilePath();
   virtual void Run(string cmd, opencdev::result_t *values) override;
};

#endif
