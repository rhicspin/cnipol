#ifndef AnaInfo_h
#define AnaInfo_h

#include <iostream>
#include <bitset>
#include <string>
#include <sys/stat.h>

#include "TObject.h"
#include "TSystem.h"

#include "AsymCommon.h"


/** */
class AnaInfo : public TObject
{
public:

   // Flag options
   enum Flag   {FLAG_COPY              = 0x10000400,
                FLAG_READ_FROM_DB      = 0x10000100,
                FLAG_UPDATE_DB         = 0x10000200, FLAG_NO_UPDATE_DB = 0x90000200,
                FLAG_USE_DB            = 0x10000300,
                FLAG_CREATE_THUMBS     = 0x10000800};

   // Flag options
   enum Option {OPTION_POL_ID           = 0x20004000,
                OPTION_SET_CALIB        = 0x20003000,
                OPTION_SET_CALIB_ALPHA  = 0x20001000,
                OPTION_SET_CALIB_DL     = 0x20002000,
                OPTION_DET_DISABLE      = 0x20000001,
                OPTION_SUFFIX           = 0x20000002,
                OPTION_ALPHA_SOURCES    = 0x20000003};

   // Various histogramming and running modes
   enum Mode   {MODE_GRAPH             = 0x02000000, MODE_NO_GRAPH     = 0x82000000};

protected:

   std::string fOutputName;

public:

   std::string      fAsymVersion;
   std::string      fSuffix;            // Additional unique identifier for analysis job
   ULong_t          fModes;
   time_t           fAnaDateTime;       // Date/time when data analysis started
   Double_t         fAnaTimeReal;       // Time in seconds to process input raw file
   Double_t         fAnaTimeCpu;        // Time in seconds to process input raw file
   Str2StrMap       fAsymEnv;
   FILE            *fFileMeasInfo;      //!
   FILE            *fFileStdLog;        //!
   std::string      fFileStdLogName;
   Bool_t           fFlagCopyResults;
   Bool_t           fFlagUseDb;
   Bool_t           fFlagUpdateDb;
   Bool_t           fFlagCreateThumbs;
   UserGroup_t      fUserGroup;
   Short_t          fAlphaSourceCount;

public:

   AnaInfo();
   ~AnaInfo();

   std::string          GetSuffix() const;
   std::string          GetResultsDir() const;
   std::string          GetOutDir() const;
   std::string          GetImageDir() const;
   virtual std::string  GetAnaInfoFileName() const;
   std::string          GetStdLogFileName() const;
   std::string          GetRootFileName() const;
   FILE*                GetAnaInfoFile() const;
   virtual void         ProcessOptions(int argc, char **argv);
   virtual void         VerifyOptions();
   void                 Print(const Option_t* opt="") const;
   void                 PrintAsPhp(FILE *f=stdout) const;
   virtual void         PrintUsage();
	virtual void         CopyResults();
   Bool_t               HasGraphBit() const;

protected:

   void Init();
   void MakeOutDir();

   ClassDef(AnaInfo, 1)
};

#endif
