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

#include "Asym.h"

class DbEntry;


/** */
class MAsymAnaInfo : public TObject
{
public:

   // Flag options
   enum Flag   {FLAG_COPY              = 0x10000400,
                FLAG_READ_FROM_DB      = 0x10000100,
                FLAG_UPDATE_DB         = 0x10000200, FLAG_NO_UPDATE_DB = 0x90000200,
                FLAG_USE_DB            = 0x10000300,
                FLAG_CREATE_THUMBS     = 0x10000800};

   // Flag options
   enum Option {OPTION_MLIST           = 0x20000001,
                OPTION_POL_ID          = 0x20004000,
                OPTION_SET_CALIB       = 0x20003000,
                OPTION_SET_CALIB_ALPHA = 0x20001000,
                OPTION_SET_CALIB_DL    = 0x20002000,
                OPTION_SUFFIX          = 0x20000002};

   // Various histogramming and running modes
   enum Mode   {MODE_ALPHA             = 0x01010000,
                MODE_CALIB             = 0x01000000,
                MODE_GRAPH             = 0x02000000, MODE_NO_GRAPH     = 0x82000000,
                MODE_NORMAL            = 0x00020000, MODE_NO_NORMAL    = 0x80020000,
                MODE_SCALER            = 0x00040000,
                MODE_RAW               = 0x00080000,
                MODE_RAW_EXTENDED      = 0x40080000,
                MODE_RUN               = 0x00100000,
                MODE_TARGET            = 0x00200000,
                MODE_PROFILE           = 0x00400000,
                MODE_ASYM              = 0x00800000,
                MODE_KINEMA            = 0x08000000,
                MODE_PMT               = 0x00000100,
                MODE_PULSER            = 0x00000200,
                MODE_FULL              = 0x007f0000,
                MODE_ONLINE            = 0x04040000};

   // Constraint parameter for data processing 
   std::string      fMListFileName;     // File name
   std::string      fSuffix;            // Additional unique identifier for analysis job
   ULong_t          fModes;
   std::bitset<N_DETECTORS>   fDisabledDetectors;   //! bitmask with disabled detector indeces
   float            tshift;             // time shift in [ns]
   float            dx_offset;          // additional deadlayer offset [ug/cm2]
   float            WCMRANGE;           // Wall Current Monitor process Fill range
   float            MassLimit;          // Lower Mass limit for peak position adjustment fit
   Float_t          fThinout;           // Approximate fraction of events to read
   time_t           fAnaDateTime;       // Date/time when data analysis started
   Double_t         fAnaTimeReal;       // Time in seconds to process input raw file
   Double_t         fAnaTimeCpu;        // Time in seconds to process input raw file
   std::string      fAlphaCalibRun;     // Name of alpha calib run
   std::string      fDlCalibRun;        // Name of dead layer calib run
   Str2StrMap       fAsymEnv;
   FILE            *fFileMeasInfo;      //!
   FILE            *fFileRunConf;       //!
   FILE            *fFileStdLog;        //!
   std::string      fFileStdLogName;
   Bool_t           fFlagCopyResults;
   Bool_t           fFlagUseDb;
   Bool_t           fFlagUpdateDb;
   Bool_t           fFlagCreateThumbs;
   UserGroup_t      fUserGroup;

public:

   MAsymAnaInfo();
   ~MAsymAnaInfo();

   void        SetMListFileName(std::string runName);
   std::string GetMListFileName();
   std::string GetMListFullPath();
   std::string GetRunName() const;
   std::string GetSuffix() const;
   std::string GetRawDataFileName() const;
   std::string GetResultsDir() const;
   std::string GetOutDir() const;
   std::string GetImageDir() const;
   std::string GetMeasInfoFileName() const;
   std::string GetRunConfFileName() const;
   std::string GetStdLogFileName() const;
   std::string GetRootFileName() const;
   FILE*       GetMeasInfoFile() const;
   FILE*       GetRunConfFile() const;
   void        ProcessOptions(int argc, char **argv);
   void        VerifyOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintUsage();
	void        CopyResults();

   std::string GetAlphaCalibRun() const;
   std::string GetDlCalibRun() const;
   Bool_t      HasAlphaBit() const;
   Bool_t      HasCalibBit() const;
   Bool_t      HasGraphBit() const;
   Bool_t      HasNormalBit() const;
   Bool_t      HasScalerBit() const;
   Bool_t      HasRawBit() const;
   Bool_t      HasRawExtendedBit() const;
   Bool_t      HasRunBit() const;
   Bool_t      HasTargetBit() const;
   Bool_t      HasProfileBit() const;
   Bool_t      HasAsymBit() const;
   Bool_t      HasKinematBit() const;
   Bool_t      HasPmtBit() const;
   Bool_t      HasPulserBit() const;
   Bool_t      HasOnlineBit() const;

private:
   void Init();
   void MakeOutDir();

   ClassDef(MAsymAnaInfo, 1)
};

#endif
