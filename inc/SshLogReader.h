#ifndef SshLogReader_h
#define SshLogReader_h

#include <map>
#include <string>
#include <vector>
#include <ctime>

#include <TObject.h>

using std::map;
using std::string;
using std::vector;

/**
 * You will need to have aclina host defined in your ssh_config.
 *
 * Example:
 *
 * Host aclina
 *    IdentityFile    /star/u/veprbl/.ssh/acnlina5_rsa
 *    User            dsmirnov
 *    HostName        localhost
 *    Port            8022
 */
class SshLogReader : TObject
{
private:

   string          fLoggersStr;
   string          fCellsStr;
   vector<string>  fCells;

   string GetSshCommand(time_t start, time_t end);

public:

   /**
    * @param loggers comma separated list of loggers
    * @param cells   comma separated list of cells
    */
   SshLogReader(string loggers, string cells);
   /**
    * @returns 0 if everything was ok
    */
   int Read(time_t start, time_t end, map< string, vector<double> > *values);
   /**
    * @returns 0 if everything was ok
    */
   int ReadMean(time_t start, time_t end, map<string, double> *mean_value);
};

#endif
