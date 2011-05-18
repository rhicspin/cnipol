/**
 *
 * 15 Oct, 2010 - Dmitri Smirnov
 *    - Modified readloop to take an object of Root class as an argument 
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Made the Root object global, redefined readloop back
 *
 */

#ifndef ASYM_READ_H
#define ASYM_READ_H

#include <errno.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"
#include "RunInfo.h"
#include "MseRunInfo.h"

#define BSIZE_OFFLINE 0x1000000                 // 16 MByte

class RawDataProcessor
{
public:
   std::string    fFileName;
   FILE          *fFile;
   char          *fMem;
   UInt_t         fMemSize;
   std::ifstream  fFileStream;

public:
   RawDataProcessor();
   RawDataProcessor(std::string fname);
   ~RawDataProcessor();

   void ReadRecBegin(MseRunInfoX* run=0);
   void ReadDataFast();
};

void readloop(MseRunInfoX &run);
void UpdateRunConst(TRecordConfigRhicStruct *ci);
void PrintBunchPattern(int *pattern);
void calcRunConst(recordConfigRhicStruct *cfginfo);
void DecodeTargetID(polDataStruct poldat, MseRunInfoX &run);
void PrepareCollidingBunchPattern();

void ProcessRecord(recordPolAdoStruct &rec);
void ProcessRecord(recordWFDV8ArrayStruct &rec);
void ProcessRecord(recordCountRate &rec);
void ProcessRecordPCTarget(long* rec, MseRunInfoX &run);
void ProcessRecord(recordWcmAdoStruct &rec);

#endif
