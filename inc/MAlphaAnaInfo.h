#ifndef MAlphaAnaInfo_h
#define MAlphaAnaInfo_h

#include <string>

#include "TObject.h"
#include "TSystem.h"

#include "AnaInfo.h"


/**
 * This class takes care of the command line options specific to the 'malpha'
 * executable. The common options are taken care of by the AnaInfo class.
 */
class MAlphaAnaInfo : public AnaInfo {
public:

   // Flag options
   enum Option {
      OPTION_MLIST           = 0x20000001,
      OPTION_OFILE           = 0x20000002
   };

   // Constraint parameter for data processing 
   std::string fMListFileName;     // File name

public:

   std::string fOutputFileName;

   MAlphaAnaInfo();
   ~MAlphaAnaInfo();

   void        SetMListFileName(std::string listName);
   std::string GetMListFileName();
   std::string GetMListFullPath();
   std::string GetExternInfoPath();
   void        ProcessOptions(int argc, char **argv);
   void        VerifyOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintUsage();

   ClassDef(MAlphaAnaInfo, 1)
};

#endif
