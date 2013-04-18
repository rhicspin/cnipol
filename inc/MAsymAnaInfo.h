/**
 *
 * 12 Jan, 2012 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef MAsymAnaInfo_h
#define MAsymAnaInfo_h

#include <iostream>
#include <bitset>
#include <string>
#include <sys/stat.h>

#include "TObject.h"
#include "TSystem.h"

#include "AsymCommon.h"
#include "AnaInfo.h"


/** */
class MAsymAnaInfo : public AnaInfo
{
public:

   // Flag options
   enum Option {OPTION_MLIST           = 0x20000001};

   // Constraint parameter for data processing 
   std::string fMListFileName;     // File name

public:

   MAsymAnaInfo();
   ~MAsymAnaInfo();

   void        SetMListFileName(std::string listName);
   std::string GetMListFileName();
   std::string GetMListFullPath();
   std::string GetExternInfoPath();
   void        ProcessOptions(int argc, char **argv);
   void        VerifyOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintUsage();

   ClassDef(MAsymAnaInfo, 1)
};

#endif
