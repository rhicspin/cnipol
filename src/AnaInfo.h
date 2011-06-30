/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef AnaInfo_h
#define AnaInfo_h

//#include <cstdlib>
#include <iostream>
#include <bitset>
#include <string>
#include <sys/stat.h>

#include "TObject.h"
#include "TSystem.h"

#include "Asym.h"

class DbEntry;
class MseRunInfoX;


/** */
class AnaInfo : public TObject
{
public:

   // Flag options
   enum Flag   {FLAG_COPY              = 0x10000400,
                FLAG_READ_FROM_DB      = 0x10000100,
                FLAG_UPDATE_DB         = 0x10000200, FLAG_NO_UPDATE_DB = 0x90000200,
                FLAG_USE_DB            = 0x10000300};

   // Flag options
   enum Option {OPTION_POL_ID          = 0x20004000,
                OPTION_SET_CALIB       = 0x20003000,
                OPTION_SET_CALIB_ALPHA = 0x20001000,
                OPTION_SET_CALIB_DL    = 0x20002000,
                OPTION_DET_DISABLE     = 0x20000001,
                OPTION_SUFFIX          = 0x20000002};

   // Various histogramming and running modes
   enum Mode   {MODE_ALPHA             = 0x01010000,
                MODE_CALIB             = 0x01000000,
                MODE_GRAPH             = 0x02000000, MODE_NO_GRAPH     = 0x82000000,
                MODE_NORMAL            = 0x00020000, MODE_NO_NORMAL    = 0x80020000,
                MODE_SCALER            = 0x00040000,
                MODE_RAW               = 0x00080000,
                MODE_RUN               = 0x00100000,
                MODE_TARGET            = 0x00200000,
                MODE_PROFILE           = 0x00400000,
                MODE_ASYM              = 0x00800000,
                MODE_KINEMAT           = 0x08000000,
                MODE_PMT               = 0x00000100,
                MODE_FULL              = 0x007f0000,
                MODE_ONLINE            = 0x04040000};

   // Constraint parameter for data processing 
   std::string      fRunName;           // Run name
   std::string      fSuffix;            // Additional unique identifier for analysis job
   int              enel;               // lower kinetic energy threshold (keV)
   int              eneu;               // upper kinetic energy threshold (keV)
   int              widthl;             // lower banana cut (ns)
   int              widthu;             // upper banana cut (ns)
   ULong_t          fModes;
   int              FEEDBACKMODE;       // fit 12C peak first and feedback tshift and sigma
   int              RAWHISTOGRAM;       // Fill raw histograms
   int              DMODE;              // dead layer study mode 0:off 1:on
   int              TMODE;              // T0 study mode 0:off 1:on
   int              AMODE;              // A0,A1 study (signal Amp vs. Int) mode 0:off 1:on
   int              BMODE;              // create banana curve (E-T) plots 0:off 1:on
   int              ZMODE;              // with/out T0 subtraction 0:with 1:without
   int              MESSAGE;            // message mode 1: exit just after run begin
   int              CBANANA;            // constant width banana cut :1, <sigma> Mass Cut :2
   int              UPDATE;             // 1: keep update of the histogram
   UShort_t         QUICK_MODE;         // 0: default, 1: quick mode
   int              MMODE;              // mass mode
   int              NTMODE;             // if 1 store NTUPLEv
   int              RECONFMODE;         // if 1 reconfigure from file
   int              RAMPMODE;           // if 1 prepare the histograms for ramp measurement
   int              STUDYMODE;          // if 1 study mode
   std::bitset<3>   fSaveTrees;         //! bitmask telling which ROOT trees to save
   std::bitset<N_DETECTORS>   fDisabledDetectors;   //! bitmask with disabled detector indeces
   float            MassSigma;          // banana curve cut within <MassSigma> away from the 12C mass
   float            MassSigmaAlt;       // banana curve alternative cut within
                                        // <MassSigmaAlt> away from the 12C mass
   float            OneSigma;           // 1-sigma of 12C mass distribution in [keV]
   float            tshift;             // time shift in [ns]
   float            inj_tshift;         // time shift in [ns] for injection w.r.t. flattop
   float            dx_offset;          // additional deadlayer offset [ug/cm2]
   float            WCMRANGE;           // Wall Current Monitor process Fill range
   float            MassLimit;          // Lower Mass limit for peak position adjustment fit
   UInt_t           nEventsProcessed;   // number of events processed from raw data file
   UInt_t           nEventsTotal;       // number of total events in raw data file
   UInt_t           thinout;            // Every <thinout> event to be feed into feedback routine
   Float_t          fFastCalibThinout;  // Approximate fraction of events to pass
   float            reference_rate;     // Expected universal rate for given target
   float            target_count_mm;    // Target count/mm conversion
   time_t           fAnaDateTime;       // Date/time when data analysis started
   Double_t         fAnaTimeReal;       // Time in seconds to process input raw file
   Double_t         fAnaTimeCpu;        // Time in seconds to process input raw file
   std::string      fAlphaCalibRun;     // Name of alpha calib run
   std::string      fDlCalibRun;        // Name of dead layer calib run
   Str2StrMap       fAsymEnv;
   FILE            *fFileRunInfo;       //!
   FILE            *fFileRunConf;       //!
   FILE            *fFileStdLog;        //!
   std::string      fFileStdLogName;
   Bool_t           fFlagCopyResults;
   Bool_t           fFlagUseDb;
   Bool_t           fFlagUpdateDb;
   UserGroup_t      fUserGroup;

public:

   AnaInfo();
   AnaInfo(std::string runId);
   ~AnaInfo();

   std::string GetRunName() const;
   std::string GetSuffix() const;
   std::string GetRawDataFileName() const;
   std::string GetResultsDir() const;
   std::string GetOutDir() const;
   std::string GetImageDir() const;
   std::string GetRunInfoFileName() const;
   std::string GetRunConfFileName() const;
   std::string GetStdLogFileName() const;
   std::string GetRootFileName() const;
   FILE*       GetRunInfoFile() const;
   FILE*       GetRunConfFile() const;
   std::string GetAlphaCalibFile() const;
   std::string GetDlCalibFile() const;
   std::string GetRootTreeFileName(UShort_t trid) const;
   void        ProcessOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintUsage();
   //void        Streamer(TBuffer &buf);
   //void        Update(DbEntry &rundb); // Deprecated
   void        Update(MseRunInfoX& run);
	void        CopyResults();

   std::string GetAlphaCalibRun() const;
   std::string GetDlCalibRun() const;
   Bool_t      HasAlphaBit() const;
   Bool_t      HasCalibBit() const;
   Bool_t      HasGraphBit() const;
   Bool_t      HasNormalBit() const;
   Bool_t      HasScalerBit() const;
   Bool_t      HasRawBit() const;
   Bool_t      HasRunBit() const;
   Bool_t      HasTargetBit() const;
   Bool_t      HasProfileBit() const;
   Bool_t      HasAsymBit() const;
   Bool_t      HasKinematBit() const;
   Bool_t      HasPmtBit() const;

private:
   void Init();
   void MakeOutDir();

   ClassDef(AnaInfo, 1)
};


//TBuffer & operator<<(TBuffer &buf, AnaInfo *&rec);
//TBuffer & operator>>(TBuffer &buf, AnaInfo *&rec);

#endif
