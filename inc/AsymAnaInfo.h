/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef AsymAnaInfo_h
#define AsymAnaInfo_h

#include <iostream>
#include <bitset>
#include <string>
#include <sys/stat.h>

#include "TObject.h"
#include "TSystem.h"

#include "Asym.h"
#include "AnaInfo.h"

class DbEntry;
class MseMeasInfoX;


/** */
class AsymAnaInfo : public AnaInfo
{
public:

   // Various histogramming and running modes
   enum Mode   {MODE_ALPHA             = 0x01011000,
                MODE_CALIB             = 0x01000000,
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
                MODE_ONLINE            = 0x04040000,
                MODE_NO_SSH            = 0x00001000};

   // Constraint parameter for data processing 
   std::string      fRunName;           // Run name
   ULong_t          fAsymModes;
   int              enel;               // lower kinetic energy threshold (keV)
   int              eneu;               // upper kinetic energy threshold (keV)
   int              widthl;             // lower banana cut (ns)
   int              widthu;             // upper banana cut (ns)
   int              DMODE;              // dead layer study mode 0:off 1:on
   int              TMODE;              // T0 study mode 0:off 1:on
   int              BMODE;              // create banana curve (E-T) plots 0:off 1:on
   int              ZMODE;              // with/out T0 subtraction 0:with 1:without
   int              CBANANA;            // constant width banana cut :1, <sigma> Mass Cut :2
   int              UPDATE;             // 1: keep update of the histogram
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
   float            dx_offset;          // additional deadlayer offset [ug/cm2]
   float            WCMRANGE;           // Wall Current Monitor process Fill range
   float            MassLimit;          // Lower Mass limit for peak position adjustment fit
   Float_t          fThinout;           // Approximate fraction of events to read
   float            reference_rate;     // Expected universal rate for given target
   float            target_count_mm;    // Target count/mm conversion
   std::string      fAlphaCalibRun;     // Name of alpha calib run
   std::string      fDlCalibRun;        // Name of dead layer calib run
   FILE            *fFileRunConf;       //!

public:

   AsymAnaInfo();
   ~AsymAnaInfo();

   void        SetRunName(std::string runName);
   std::string GetRunName() const;
   std::string GetRawDataFileName() const;
   virtual std::string GetAnaInfoFileName() const;
   std::string GetRunConfFileName() const;
   FILE*       GetRunConfFile() const;
   std::string GetAlphaCalibFile() const;
   std::string GetDlCalibFile() const;
   std::string GetRootTreeFileName(UShort_t trid) const;
   void        ProcessOptions(int argc, char **argv);
   void        VerifyOptions();
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintUsage();
   void        Update(MseMeasInfoX& run);

   std::string GetAlphaCalibRun() const;
   std::string GetDlCalibRun() const;
   Bool_t      HasAlphaBit() const;
   Bool_t      HasCalibBit() const;
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
   Bool_t      HasNoSshBit() const;

   ClassDef(AsymAnaInfo, 3)
};

#endif
