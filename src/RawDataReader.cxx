#include "RawDataReader.h"

#include <sstream>

#include "mysql++.h"

#include "TRandom.h"

#include "AsymCalculator.h"
#include "AsymRecover.h"
#include "AsymRoot.h"
#include "AsymAnaInfo.h"
#include "TargetInfo.h"

using namespace std;

static void ProcessRecordATPassOne(const char *mSeek, RecordHeaderStruct *mHeader);
static void ProcessRecordATPassTwo(const char *mSeek, RecordHeaderStruct *mHeader);
static void ProcessRecordLongPassOne(const char *mSeek, RecordHeaderStruct *mHeader);


/**
 * Constructs a reader to process a raw data file from a single measurement.
 */
RawDataReader::RawDataReader(string fname) : fFileName(fname), fFile(0),
   fMem(0), fMemSize(0),
   fFileStream(fFileName.c_str(), ios::binary), fSeenRecords()
{
   fFile = fopen(fFileName.c_str(), "r");

   // Make sure the file exists
   if (!fFile) {
      Fatal("RawDataReader", "File \"%s\" not found. Cannot proceed", fFileName.c_str());
   } else
      Info("RawDataReader", "Found file \"%s\"", fFileName.c_str());

   // Create a BLOB on the heap with the file content
   Info("RawDataReader", "Reading into memory...");

   fFileStream.seekg(0, ios::end);
   fMemSize = fFileStream.tellg(); // in bytes
   Info("RawDataReader", "Input file size: %d bytes", fMemSize);
   fFileStream.seekg(0, ios::beg);

   fMem = new char[fMemSize];
   fFileStream.read(fMem, fMemSize);
   fFileStream.close();

   RecordHeaderStruct *mHeader = (RecordHeaderStruct*) fMem;
   cout << "Currently consider record: fMem: " << hex << fMem
        << ", type: " << hex << mHeader->type << ", len: " << mHeader->len << endl;
   cout << "size RecordHeaderStruct: " << sizeof(RecordHeaderStruct) << endl;
   cout << "size recordHeaderStruct: " << sizeof(recordHeaderStruct) << endl;
}


/** */
RawDataReader::~RawDataReader()
{
   if (fFile) fclose(fFile);
   delete[] fMem;
}


/** */
void RawDataReader::ReadRecBegin(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadRecBegin", "Start reading begin record from data file...");

   char *mSeek = fMem;

   RecordHeaderStruct *mHeader = (RecordHeaderStruct*) mSeek;

   cout << "Currently consider record: " << hex << mHeader->type << ", len: " << mHeader->len << " (MEM)" << endl;

   // Take the very first record from the raw data file
   recordBeginStruct *recBegin = (recordBeginStruct*) fMem;

   // We'd better check the record type...
   if ( (recBegin->header.type & REC_TYPEMASK) == REC_BEGIN) {
      Info("ReadRecBegin", "Found REC_BEGIN record... size: %i", recBegin->header.len);
   } else {
      Fatal("ReadRecBegin", "Could not find REC_BEGIN record");
   }

   cout << "Begin of data stream version: " << recBegin->version << endl;
   cout << "Comment: "                      << recBegin->comment << endl;
   time_t       time  = recBegin->header.timestamp.time;
   cout << "Time Stamp: "                   << ctime(&time);
   cout << "Unix Time Stamp: "              << recBegin->header.timestamp.time << endl;

   gMeasInfo->fStartTime         = recBegin->header.timestamp.time;
   gMeasInfo->fDataFormatVersion = recBegin->version;
   gMeasInfo->fPolBeam           = (recBegin->header.type & REC_MASK_BEAM)   >> 16;
   gMeasInfo->fPolStream         = (recBegin->header.type & REC_MASK_STREAM) >> 20;

   int polId = gMeasInfo->GetPolarimeterId(gMeasInfo->fPolBeam, gMeasInfo->fPolStream);

   if (polId < 0)
      Warning("ReadRecBegin", "Cannot read polarimeter ID from data record. Will guess it from file name or user option");


   // First, try to get polarimeter id from the data
   if (polId >= 0) {
      gRunDb.fPolId = gMeasInfo->fPolId;

   // if failed, get id from the file name
   } else if (gMeasInfo->GetPolarimeterId() >= 0) {
      gRunDb.fPolId = gMeasInfo->fPolId;

   // see if the polarimeter id was set by command line argument
   } else if (gMeasInfo->fPolId >= 0) {
      gRunDb.fPolId = gMeasInfo->fPolId;

   } else { // cannot proceed
      Fatal("ReadRecBegin", "Unknown polarimeter ID");
   }

   gRunDb.timeStamp = gMeasInfo->fStartTime; // should be always defined in raw data
   //gRunDb.Print();

   mseMeasInfo.polarimeter_id = gRunDb.fPolId;
   mysqlpp::DateTime dt(gMeasInfo->fStartTime);
   mseMeasInfo.start_time     = dt;

   Info("ReadRecBegin", "Finished reading begin record from data file");
}


/**
 * This method is supposed to read all information about the measurement from
 * raw data. It does not read events.
 */
void RawDataReader::ReadMeasInfo(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadMeasInfo", "Start reading run info from data file...");

   RecordHeaderStruct *mHeader;

   char *mSeek = fMem;

   while (true)
   {
      if (mSeek > fMem + fMemSize - 1) break;

      mHeader = (RecordHeaderStruct*) mSeek;

      switch(mHeader->type & REC_TYPEMASK)
      {
      case REC_BEAMADO:
      {
         cout << "Reading REC_BEAMADO record... size = " << mHeader->len << endl;

         recordBeamAdoStruct *rec = (recordBeamAdoStruct*) mHeader;

         gMeasInfo->SetBeamEnergy(rec->data.beamEnergyM);
         printf("Beam energy: %8.2f\n", gMeasInfo->GetBeamEnergyReal());
         printf("RHIC beam:   %8d\n",   gMeasInfo->fPolBeam);

         ProcessRecord( (recordBeamAdoStruct&) *rec);

         break;
      }

      case REC_MACHINEPARAMS:
      {
         cout << "Reading REC_MACHINEPARAMS record... size = " << mHeader->len << endl;

         // Extract voltages, currents, etc
         RecordMachineParams *rec = (RecordMachineParams*) mHeader;
         ProcessRecord( (RecordMachineParams&) *rec);

         break;
      }

      case REC_TARGETPARAMS:
      {
         cout << "Reading REC_TARGETPARAMS record... size = " << mHeader->len << endl;

         // Extract target info
         RecordTargetParams *rec = (RecordTargetParams*) mHeader;
         ProcessRecord( (RecordTargetParams&) *rec);

         break;
      }

      case REC_MEASTYPE:
      {
         cout << "Reading REC_MEASTYPE record... size = " << mHeader->len << endl;

         recordMeasTypeStruct *rec = (recordMeasTypeStruct*) mHeader;
         ProcessRecord( (recordMeasTypeStruct&) *rec);

         break;
      }

      case REC_POLADO:
      {
         cout << "Reading REC_POLADO record... size = " << mHeader->len << endl;

         recordPolAdoStruct *rec = (recordPolAdoStruct*) mHeader;
         ProcessRecord( (recordPolAdoStruct&) *rec, mseMeasInfo);

         break;
      }

      case REC_PCTARGET:
      {
         // Do not process this record for calibration runs. May contain
         // invalid ndelim info
         cout << "Reading REC_PCTARGET record... size = " << mHeader->len << endl;

         recordpCTagAdoStruct *rec = (recordpCTagAdoStruct*) mHeader;
         ProcessRecord( (recordpCTagAdoStruct&) *rec, mseMeasInfo);

         break;
      }

      case REC_WCMADO:
      {
         cout << "Reading REC_WCMADO record... size = " << mHeader->len << endl;

         recordWcmAdoStruct *rec = (recordWcmAdoStruct*) mHeader;
         ProcessRecord( (recordWcmAdoStruct &) *rec);

         break;
      }

      case REC_WCM_NEW:
      {
         cout << "Reading REC_WCM_NEW record... size = " << mHeader->len << endl;

         RecordWcm *rec = (RecordWcm*) mHeader;
         ProcessRecord( (RecordWcm &) *rec);

         break;
      }

      case REC_WFDV8SCAL:
      {
         if (fSeenRecords.find(REC_WFDV8SCAL) == fSeenRecords.end())
         {
            cout << "Reading REC_WFDV8SCAL record... size = " << mHeader->len << endl;

            recordWFDV8ArrayStruct *rec = (recordWFDV8ArrayStruct*) mHeader;
            ProcessRecord( (recordWFDV8ArrayStruct &) *rec);

            fSeenRecords.insert(REC_WFDV8SCAL);
         }

         break;
      }

      case REC_COUNTRATE:
      {
         cout << "Reading REC_COUNTRATE record... size = " << mHeader->len << endl;

         recordCountRate *rec = (recordCountRate*) mHeader;
         ProcessRecord( (recordCountRate &) *rec);

         break;
      }

      case REC_RHIC_CONF:
      {
         cout << "Reading REC_RHIC_CONF record... size = " << mHeader->len << endl;

         recordConfigRhicStruct *rec = (recordConfigRhicStruct*) mHeader;
         ProcessRecord( (recordConfigRhicStruct &) *rec);

         break;
      }

      case REC_TAGADO:
      {
         cout << "Reading REC_TAGADO record... size = " << mHeader->len << endl;

         break;
      }

      case REC_SCALERS:
      {
         cout << "Reading REC_SCALERS record... size = " << mHeader->len << endl;
         //recordScalersStruct *rec = (recordScalersStruct*) mHeader;
         //ProcessRecord( (recordScalersStruct &) *rec);
         Info("ReadMeasInfo", "Not implemented yet");

         break;
      }

      case REC_READRAW:
      {
         cout << "Reading REC_READRAW record... size = " << mHeader->len << endl;
         Info("ReadMeasInfo", "Not implemented yet");

         break;
      }

      case REC_END:
      {
         cout << "Reading REC_END record... size = " << mHeader->len << endl;
         recordEndStruct *rec = (recordEndStruct*) mHeader;
         gMeasInfo->fStopTime = rec->header.timestamp.time;

         break;
      }

      case REC_READSUB:
      case REC_READALL:
      default:
         break;
      }

      mSeek = mSeek + mHeader->len;
   }
}


/** */
void RawDataReader::ReadDataPassOne(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadDataPassOne", "Start reading events from data file...");

   gMeasInfo->fNEventsProcessed = 0;
   gMeasInfo->fNEventsTotal     = 0;
   gMeasInfo->fNEventsSilicon   = 0;

   RecordHeaderStruct *mHeader;
   const char *mSeek = fMem;

   while (true) {

      if (mSeek > fMem + fMemSize - 1) break;

      mHeader = (RecordHeaderStruct*) mSeek;

      if ((mHeader->type & REC_TYPEMASK) == REC_READAT)
      {
         ProcessRecordATPassOne(mSeek, mHeader);
      }
      else if ((mHeader->type & REC_TYPEMASK) == REC_READLONG)
      {
         ProcessRecordLongPassOne(mSeek, mHeader);
      }

      mSeek = mSeek + mHeader->len;
   }

   printf("Total events read:      %12u\n", gMeasInfo->fNEventsTotal);
   printf("Total events processed: %12u\n", gMeasInfo->fNEventsProcessed);

   gMeasInfo->CalcSpinFlipperPhase();

   // Save info into database global object
   mseMeasInfo.stop_time         = mysqlpp::DateTime(gMeasInfo->fStopTime);
   mseMeasInfo.nevents_total     = gMeasInfo->fNEventsTotal;
   mseMeasInfo.nevents_processed = gMeasInfo->fNEventsProcessed;
}


/** Read loop routine */
void RawDataReader::ReadDataPassTwo(MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   Info("ReadDataPassTwo", "Start reading events from data file...");

   gMeasInfo->fNEventsProcessed = 0;
   gMeasInfo->fNEventsTotal     = 0;

   RecordHeaderStruct *mHeader;
   const char *mSeek = fMem;

   while (true) {

      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;
      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (RecordHeaderStruct*) mSeek;
      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      if ((mHeader->type & REC_TYPEMASK) == REC_READAT)
      {
         ProcessRecordATPassTwo(mSeek, mHeader);
      }

      mSeek = mSeek + mHeader->len;
   }

   // Post processing
   if ( gAsymAnaInfo->HasNormalBit() )
   {
      AsymCalculator::CalcStatistics();

      // Energy Yeild Weighted Average Analyzing Power
      gAnaMeasResult->A_N[1] = AsymCalculator::WeightAnalyzingPower(10050); // banana cut in energy spectra

      AsymCalculator::PrintWarning();
      AsymCalculator::PrintRunResults();
   }

   printf("End of data stream \n");
   printf("End time: %s\n", ctime(&gMeasInfo->fStopTime));
   printf("Total events read:      %12u\n", gMeasInfo->fNEventsTotal);
   printf("Total events processed: %12u\n", gMeasInfo->fNEventsProcessed);

   // Save info into database global object
   mseMeasInfo.stop_time         = mysqlpp::DateTime(gMeasInfo->fStopTime);
   mseMeasInfo.nevents_total     = gMeasInfo->fNEventsTotal;
   mseMeasInfo.nevents_processed = gMeasInfo->fNEventsProcessed;
   mseMeasInfo.nevents_silicon   = gMeasInfo->fNEventsSilicon;

   if (gAsymAnaInfo->HasAlphaBit())
      mseMeasInfo.beam_energy = 0;
   else
      mseMeasInfo.beam_energy = gMeasInfo->GetBeamEnergyReal();
}


/** */
void UpdateRunConst(TRecordConfigRhicStruct *ci)
{
   if (!ci) return;

   float Ct = ci->data.WFDTUnit/2.; // Determine the TDC count unit (ns/channel)
   float L  = ci->data.TOFLength;

   for (UShort_t i=1; i<=ci->data.NumChannels; i++) {

      if (gMeasInfo->fDataFormatVersion >= 40200 ) // >= Run 11 version
      {
         L = ci->data.chan[i-1].TOFLength; // TOF length in cm
      } else {
         printf("RawDataReader.cxx: UpdateRunConst(): This should not happen unless you process an old file.");
         exit(EXIT_FAILURE);
         L = CARBON_PATH_DISTANCE;
      }

      gRunConsts[i] = RunConst(L, Ct);
   }

   // Set values for 0th channel to the average ones
   Float_t sumL      = 0;
   UInt_t  nChannels = 0;

   map<UShort_t, RunConst>::iterator irc = gRunConsts.begin();

   for (; irc!=gRunConsts.end(); ++irc) {
      if (gMeasInfo->IsSiliconChannel(irc->first)) {
         sumL += irc->second.L;
         nChannels++;
      }
   }

   float avrgL = nChannels > 0 ? sumL / nChannels : CARBON_PATH_DISTANCE;

   gRunConsts[0] = RunConst(avrgL, Ct);

   printf("\nAverage RunConst:\n");
   gRunConsts[0].Print();
}


/**
 * Decorde target infomation. Presently the target ID is assumed to be appear at
 * the last of character string poldat.targetIdS
 */
void DecodeTargetID(const polDataStruct &poldat, MseMeasInfoX &mseMeasInfo)
{
   cout << endl;
   cout << "target ID = " << poldat.targetIdS << endl;
   //cout << "startTimeS = " << poldat.startTimeS << endl;
   //cout << "stopTimeS = " << poldat.stopTimeS << endl;

   gRunDb.fFields["TARGET_ID"] = poldat.targetIdS;

   // initiarization
   gMeasInfo->fTargetId = '-';

   string str(poldat.targetIdS);

   if (str.find("Background") < str.size())   gMeasInfo->fTargetId = 'B';
   if (str.find("1")         == str.size()-1) gMeasInfo->fTargetId = '1';
   if (str.find("2")         == str.size()-1) gMeasInfo->fTargetId = '2';
   if (str.find("3")         == str.size()-1) gMeasInfo->fTargetId = '3';
   if (str.find("4")         == str.size()-1) gMeasInfo->fTargetId = '4';
   if (str.find("5")         == str.size()-1) gMeasInfo->fTargetId = '5';
   if (str.find("6")         == str.size()-1) gMeasInfo->fTargetId = '6';
   if (str.find("7")         == str.size()-1) gMeasInfo->fTargetId = '7';
   if (str.find("8")         == str.size()-1) gMeasInfo->fTargetId = '-';

   if (str.find('V') != string::npos) {
      gMeasInfo->fTargetOrient = 'V';
      mseMeasInfo.target_orient       = 'V';
   }

   if (str.find('H') != string::npos) {
      gMeasInfo->fTargetOrient = 'H';
      mseMeasInfo.target_orient       = 'H';
   }

   //cout << "target id str: " << str << " " << mseMeasInfo.target_orient << endl;

   stringstream sstr;
   //int target_id;
   sstr << gMeasInfo->fTargetId;
   //sstr >> target_id;
   sstr >> mseMeasInfo.target_id;
   //mseMeasInfo.target_id = target_id;

   // Restore Vertical or Horizontal target information
   // in case this information isn't decorded correctly
   // within REC_PCTARGET routine. If the target is horizontal,
   // then mask 90 degree detector.
   //if (gMeasInfo->fTargetOrient == '-') {

   //   if (str.find("Vert") == 0 || str.find("V") == 0) {
   //      gMeasInfo->fTargetOrient = 'V';
   //      mseMeasInfo.target_orient       = 'V';
   //      tgt.VHtarget            = 0;
   //   }

   //   if (str.find("Horz") == 0 || str.find("H") == 0) {
   //      gMeasInfo->fTargetOrient = 'H';
   //      mseMeasInfo.target_orient       = 'H';
   //      tgt.VHtarget            = 1;
   //      // This is too late to reconfigure strip mask because this routine is
   //      // executed at the end of event loop. Too bad. /* March 5,'09 IN */
   //      //      mask.detector = 0x2D;
   //      //      gMeasInfo->ConfigureActiveStrip(mask.detector);
   //   }
   //}
}

static void ProcessRecordLongPassOne(const char *mSeek, RecordHeaderStruct *mHeader)
{
   // This function does very basic processing of the HJet full waveform from REC_READLONG record.

   const char *p = mSeek + sizeof(RecordHeaderStruct);

   unsigned  wfLen = (gConfigInfo->data.JetDelay & 0xFF) * 6; // waveform length in points
   unsigned  evLen = sizeof(longWaveStruct) - 1 + wfLen;

   while(p != mSeek + mHeader->len)
   {
      const     recordLongWaveStruct      *longWave = (const recordLongWaveStruct*)p;
      unsigned  chId = longWave->subhead.siNum;
      unsigned  nEvents = longWave->subhead.Events + 1;

      // cnipol has number of strips hardcoded in many places
      // HJet has more strips so will have to drop some
      if (chId < N_SILICON_CHANNELS)
      {
         for (unsigned iEvent = 0; iEvent < nEvents; iEvent++)
         {
            const longWaveStruct *event = (const longWaveStruct*)&longWave->data[evLen * iEvent];
            ATStruct       atEmu;

            // Here we only fill in amplitude value. This will be enough to do alpha calib's.
            long delim = 0;
            atEmu.rev = 0;
            atEmu.rev0 = 0;
            atEmu.b = 0;
            atEmu.a = 0;
            atEmu.s = 0;
            atEmu.t = 0;
            atEmu.tmax = 0;

            int amp = 0;
            for(unsigned i = 0; i < wfLen; i++)
            {
               int val = event->d[i] - V10BASELINE;
               if (val > amp)
               {
                  amp = val;
               }
            }
            atEmu.a = amp;

            // Now feed our emulated structure into cnipol's data flow.
            gAsymRoot->SetChannelEvent(atEmu, delim, chId);
            gAsymRoot->UpdateFromChannelEvent();
            gAsymRoot->FillPassOne(kCUT_PASSONE);
         }
      }

      p += sizeof(subheadStruct) + nEvents * evLen;
      if (p > mSeek + mHeader->len)
      {
         Error("", "corrupted record");
      }
   }
}

static void ProcessRecordATPassOne(const char *mSeek, RecordHeaderStruct *mHeader)
{
   // We are here if rec type is REC_READAT
   long    delim   = mHeader->timestamp.delim;
   size_t  recSize = mHeader->len - sizeof(RecordHeaderStruct);
   const char *mSeekAT = mSeek + sizeof(RecordHeaderStruct);

   for (UInt_t i=0; i<recSize; )
   {
      recordReadATStruct *ATPtr   = (recordReadATStruct*) (mSeekAT + i);
      unsigned chId    = ATPtr->subhead.siNum; // si number
      unsigned nEvents = ATPtr->subhead.Events + 1;

      // count the total number of event records in raw file
      gMeasInfo->fNEventsTotal += nEvents;

      i += sizeof(subheadStruct) + nEvents*sizeof(ATStruct);

      for (unsigned iEvent=0; iEvent<nEvents; iEvent++)
      {
         if (gAsymAnaInfo->fMaxEventsUser > 0 && gMeasInfo->fNEventsProcessed >= gAsymAnaInfo->fMaxEventsUser) break;

         gAsymRoot->SetChannelEvent(ATPtr->data[iEvent], delim, chId);
         gAsymRoot->UpdateFromChannelEvent();

         // Use only a fraction of events
         if (gRandom->Rndm() > gAsymAnaInfo->fThinout) continue;

         if ( gAsymRoot->fChannelEvent->PassCutSiliconChannel() )
         {
            gMeasInfo->fNEventsSilicon++;

            // Fill hists with raw data
            gAsymRoot->FillPassOne(kCUT_PASSONE);

            // Use data from empty bunches to fill corresponding histograms
            if ( gAsymRoot->fChannelEvent->PassCutEmptyBunch() )
               gAsymRoot->FillPassOne(kCUT_PASSONE_RAW_EB);
         }

         if ( gAsymRoot->fChannelEvent->PassCutPmtChannel() )
         {
            gAsymRoot->FillPassOne(kCUT_PASSONE_PMT);
         }

         if ( gAsymRoot->fChannelEvent->PassCutStepperChannel() )
         {
            gAsymRoot->FillPassOne(kCUT_PASSONE_STEPPER);
         }

         if ( gAsymAnaInfo->HasStudiesBit() && gAsymRoot->fChannelEvent->IsSpinFlipperMarkerChannel() )
         {
            gAsymRoot->AddSpinFlipperMarker();
         }

         gMeasInfo->fNEventsProcessed++;

         if (gMeasInfo->fNEventsProcessed%50000 == 0)
         {
            printf("%s: Pass 1: Processed events %u\r", gMeasInfo->GetRunName().c_str(), gMeasInfo->fNEventsProcessed);
            fflush(stdout);
         }
      }
   }
}

static void ProcessRecordATPassTwo(const char *mSeek, RecordHeaderStruct *mHeader)
{
   // We are here if rec type is REC_READAT
   long    delim   = mHeader->timestamp.delim;
   size_t  recSize = mHeader->len - sizeof(RecordHeaderStruct);
   const char *mSeekAT = mSeek + sizeof(RecordHeaderStruct);

   for (UInt_t i=0; i<recSize; )
   {
      recordReadATStruct *ATPtr   = (recordReadATStruct*) (mSeekAT + i);
      unsigned chId    = ATPtr->subhead.siNum; // si number
      unsigned nEvents = ATPtr->subhead.Events + 1;

      // count the total number of event records in raw file
      gMeasInfo->fNEventsTotal += nEvents;

      i += sizeof(subheadStruct) + nEvents*sizeof(ATStruct);

      for (unsigned iEvent=0; iEvent<nEvents; iEvent++)
      {
         if (gAsymAnaInfo->fMaxEventsUser > 0 && gMeasInfo->fNEventsProcessed >= gAsymAnaInfo->fMaxEventsUser) break;

         gAsymRoot->SetChannelEvent(ATPtr->data[iEvent], delim, chId);

         // Use only a fraction of events
         if (gRandom->Rndm() > gAsymAnaInfo->fThinout) continue;

         gMeasInfo->fNEventsProcessed++;

         // Finer Target Position Resolution by counting stepping moter
         // This feature became available after Run06
         if (gMeasInfo->fRunId >= 6) {
            cntr.revolution = gAsymRoot->fChannelEvent->GetRevolutionId();

            if (cntr.revolution > gMeasInfo->MaxRevolution)
               gMeasInfo->MaxRevolution = cntr.revolution;

            if (gAsymRoot->fChannelEvent->GetChannelId() == 73 && delim != tgt.eventID) {
               tgt.x += gAsymAnaInfo->target_count_mm * (float) tgt.vector;
               tgt.vector=-1;
            }

            if (gAsymRoot->fChannelEvent->GetChannelId() == 73 || gAsymRoot->fChannelEvent->GetChannelId() == 74) {
                switch (gAsymRoot->fChannelEvent->GetChannelId()) {
                case 73:
                    tgt.eventID = delim;
                    ++cntr.tgtMotion;
                    break;
                case 74:
                    tgt.vector = 1;
                    break;
                }
            }
         }

         if (gAsymAnaInfo->fSaveTrees.any()) { gAsymRoot->AddChannelEvent(); }

         // Fill target histograms
         if (gAsymAnaInfo->HasTargetBit() && gAsymRoot->fChannelEvent->PassCutTargetChannel())
         {
            gAsymRoot->fHists->d["targets"]->Fill(gAsymRoot->fChannelEvent);
         }

         if (gAsymAnaInfo->HasNormalBit() &&
             gAsymRoot->fChannelEvent->PassCutSiliconChannel() &&
             gAsymRoot->fChannelEvent->PassCutKinEnergyAEDepend() &&
             gAsymRoot->fChannelEvent->PassCutEnabledChannel() )
         {
	         gAsymRoot->Fill(kCUT_NOISE);

            if (gAsymRoot->fChannelEvent->PassCutCarbonMass())
	            gAsymRoot->Fill(kCUT_CARBON);
         }

         if ( gAsymRoot->fChannelEvent->PassCutSiliconChannel() )
         {
            gAsymRoot->Fill(kCUT_PASSTWO);
         }

         if ( gAsymRoot->fChannelEvent->PassCutPmtChannel() )
         {
            gAsymRoot->Fill(kCUT_PMT);
         }

         if (gMeasInfo->fNEventsProcessed%50000 == 0)
         {
            printf("%s: Pass 2: Processed events %u\r", gMeasInfo->GetRunName().c_str(), gMeasInfo->fNEventsProcessed);
            fflush(stdout);
         }
      }
   }
}

/** */
void ProcessRecordPCTarget(const pCTargetStruct *rec, MseMeasInfoX &mseMeasInfo)
{
   const pCTargetStruct* pointer = rec;

   // copy data to a linear array
   Double_t* linRec = new Double_t[gNDelimeters*4 + 8]; // there 2 under and ooverflow bins

   for (Int_t k=0; k<gNDelimeters; k++) {
      *(linRec + k + 1                 ) = pointer->VertLinear;
      *(linRec + k + (gNDelimeters+2)  ) = pointer->VertRotary;
      *(linRec + k + (gNDelimeters+2)*2) = pointer->HorLinear;
      *(linRec + k + (gNDelimeters+2)*3) = pointer->HorRotary;

      pointer++;
   }

   gAsymRoot->FillTargetHists(gNDelimeters, linRec);
   //gAsymRoot->AnalyzeTargetMoves();

   delete [] linRec;

   //
   tgt.fNDelim = gNDelimeters;

   UShort_t i = 0;

   printf("   index    total        x-pos        y-pos\n");

   pointer = rec;

   for (int k=0; k<gNDelimeters; k++) {

      tgt.Linear[k][1] = pointer->VertLinear;
      tgt.Rotary[k][1] = pointer->VertRotary;
      tgt.Linear[k][0] = pointer->HorLinear;
      tgt.Rotary[k][0] = pointer->HorRotary;

      pointer++;

      // force 0 for +/-1 tiny readout as target position.
      if (abs(tgt.Rotary[k][1]) <= 1) tgt.Rotary[k][1] = 0;

      // identify Horizontal or Vertical target from the first target
      // rotary position readout.
      if (k == 0) {
         // From Run09 on, the horizontal/vertical targets are
         // identified by linear motion
         long int tgt_identifyV = gMeasInfo->RUNID < 10040 ? tgt.Rotary[k][0] : tgt.Linear[k][0];
         long int tgt_identifyH = gMeasInfo->RUNID < 10040 ? tgt.Rotary[k][1] : tgt.Linear[k][1];

         if ( ( !tgt.Rotary[k][0] && !tgt.Rotary[k][1] ) ||
              (  tgt.Rotary[k][0] &&  tgt.Rotary[k][1] ) )
         {
            cout << "ERROR: no target rotary info. Don't know H/V target" << endl;
            //gMeasInfo->fTargetOrient = '-';
         }

         if (tgt_identifyV) {
            tgt.VHtarget           = 0;
            //gMeasInfo->fTargetOrient = 'V';
            //mseMeasInfo.target_orient      = 'V';
            cout << "Vertical Target in finite position - ???" << endl;

         } else if (tgt_identifyH) {
            tgt.VHtarget           = 1;
            //gMeasInfo->fTargetOrient = 'H';
            //mseMeasInfo.target_orient      = 'H';
            cout << "Horizontal Target in finite position - ???" << endl;

         } else {
            cout << "Warning: Target infomation cannot be recognized - ???" << endl;
         }

         tgt.x       = tgt.Rotary[k][tgt.VHtarget] * gAsymAnaInfo->target_count_mm;
         tgt.Time[i] = k;
         tgt.X[i]    = tgt.x;

         printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
                 TgtIndex[k], tgt.X[TgtIndex[k]],
                 tgt.Rotary[k][0]*gAsymAnaInfo->target_count_mm,
                 tgt.Rotary[k][1]*gAsymAnaInfo->target_count_mm);
      } else {

         TgtIndex[k] = i;

         if (tgt.Rotary[k][1] != tgt.Rotary[k-1][1] ||
             tgt.Rotary[k][0] != tgt.Rotary[k-1][0] )
         {
            TgtIndex[k]                 = ++i;
            tgt.X[TgtIndex[k]]          = tgt.Rotary[k][tgt.VHtarget] * gAsymAnaInfo->target_count_mm;
            tgt.Time[TgtIndex[k]]       = float(k);
            tgt.Interval[TgtIndex[k-1]] = tgt.Time[TgtIndex[k]] - tgt.Time[TgtIndex[k-1]];
            ++nTgtIndex;

            printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
               TgtIndex[k], tgt.X[TgtIndex[k]],
               tgt.Rotary[k][0]*gAsymAnaInfo->target_count_mm,
               tgt.Rotary[k][1]*gAsymAnaInfo->target_count_mm);

            //++i;
         }
      }

      // target position array including static target motion
      tgt.all.x[k] = tgt.Rotary[k][tgt.VHtarget] * gAsymAnaInfo->target_count_mm ;
   }

   printf("Number of delimiters: %4d\n", gNDelimeters);
   printf("nTgtIndex: %d\n", nTgtIndex);

   tgt.Print();

   // define target histograms (hbook)
   //tgtHistBook();

   // disable 90 degrees detectors for horizontal target 0x2D={10 1101}
   //if (tgt.VHtarget) mask.detector = 0x2D;
}


/** */
void ProcessRecord(const recordConfigRhicStruct &rec)
{
   gConfigInfo = (recordConfigRhicStruct *) malloc(sizeof(recordConfigRhicStruct) +
                 (rec.data.NumChannels - 1) * sizeof(SiChanStruct));

   memcpy(gConfigInfo, &rec, sizeof(recordConfigRhicStruct) + (rec.data.NumChannels - 1) * sizeof(SiChanStruct));

   // Recalculate Run constants
   UpdateRunConst(gConfigInfo);
}


/** */
void ProcessRecord(const recordMeasTypeStruct &rec)
{
   gMeasInfo->SetMeasType(rec.type);
}


/** */
void ProcessRecord(const recordPolAdoStruct &rec, MseMeasInfoX &mseMeasInfo)
{
   if (!gAsymAnaInfo->HasAlphaBit()) DecodeTargetID( (polDataStruct &) rec.data, mseMeasInfo);
}


/** */
void ProcessRecord(const recordpCTagAdoStruct &rec, MseMeasInfoX &mseMeasInfo)
{
   if (!gAsymAnaInfo->HasTargetBit() || gAsymAnaInfo->HasAlphaBit()) return;

   gNDelimeters  = (rec.header.len - sizeof(rec.header)) / sizeof(pCTargetStruct);

   ProcessRecordPCTarget((const pCTargetStruct *) rec.data, mseMeasInfo);
}


/** */
void ProcessRecord(const recordWFDV8ArrayStruct &rec)
{
   Int_t s1=0, s2=0, s3=0, s4=0;

   UShort_t chId = rec.siNum + 1;

   // Consider only silicon channels
   if (chId > NSTRIP) return;

   for (int i=0; i< 128; i++) s1 += rec.hist[i];        // bunch hist
   for (int i=0; i< 128; i++) s2 += rec.hist[i+128];    // unpol energy
   for (int i=0; i< 128; i++) s3 += rec.hist[i+256];    // pol+ energy
   for (int i=0; i< 128; i++) s4 += rec.hist[i+384];    // pol- energy

   printf("Si%02d : %12d %12d %12d %12d %12d    %12d  %12d  %12d  %12d",
          rec.siNum + 1, rec.scalers[0], rec.scalers[1],
          rec.scalers[2], rec.scalers[3], rec.scalers[4],
          s1, s2, s3, s4);

   gAsymRoot->FillScallerHists((int32_t*) rec.hist, chId);

   // Check for hardware counts compatibility. Mark channel as bad if something is wrong
   if (s2 < rec.scalers[2] || s3 != rec.scalers[0] ||
       s4 != rec.scalers[1] || s1 < s2 + s3 + s4)
   {
      printf(" <- !!!");
   }

   printf("\n");

   Int_t cnt = 0;

   cnt += rec.scalers[0] + rec.scalers[1] + rec.scalers[2];
}


/** */
void ProcessRecord(const recordCountRate &rec)
{
   UInt_t size = (rec.header.len - sizeof(rec.header))/(sizeof(int32_t));
   int32_t *pointer = (int32_t *) rec.data;
   //Double_t *pointer = (Double_t *) &rec.buffer[sizeof(rec.header)];

   cout << "len, size: " << rec.header.len << ", " << size << endl;

   //for (UInt_t i=0; i<size; i++) {
   //   printf("countrate: i: %d, %ld\n", i, *(pointer+i));
   //}

   gAsymRoot->FillProfileHists(size, pointer);
}


/** */
void ProcessRecord(const RecordMachineParams &rec)
{
    gMeasInfo->SetMachineParams(rec);
}


/** */
void ProcessRecord(const RecordTargetParams &rec)
{
    gMeasInfo->SetTargetParams(rec);
}


/** */
void ProcessRecord(const recordWcmAdoStruct &rec)
{
   for (int bid=1; bid<=N_BUNCHES; bid++) {

      float data = rec.data.fillDataM[(bid-1)*3];

      // save non-zero bunches only
      if (!data) continue;

      gMeasInfo->fWallCurMon[bid] = data;
      //gMeasInfo->fWallCurMonSum += gMeasInfo->fWallCurMon[bid] * gFillPattern[bid];
      gMeasInfo->fWallCurMonSum += gMeasInfo->fWallCurMon[bid];
   }

   //gMeasInfo->fWallCurMonAve = gMeasInfo->fWallCurMonSum/float(gMeasInfo->NActiveBunch);
   gMeasInfo->fWallCurMonAve = gMeasInfo->fWallCurMonSum / gMeasInfo->fWallCurMon.size();

   gAsymRoot->FillRunHists();
}


/** */
void ProcessRecord(const RecordWcm &rec)
{
   for (int bid=1; bid<=N_BUNCHES; bid++)
   {
      BeamBunchIter  ibb = gMeasInfo->fBeamBunches.find(bid);
      BeamBunch     &bb  = ibb->second;

      bb.SetLength((Float_t) rec.fBunchDataM[(bid-1)*3]);
   }
}


/** */
void ProcessRecord(const recordBeamAdoStruct &rec)
{
   //memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));
   //beamDataStruct beamdat;

   //gMeasInfo->SetBeamEnergy(beamdat.beamEnergyM);
   //fprintf(stdout, "Beam Energy: %8.2f\n", gMeasInfo->GetBeamEnergyReal());
   //fprintf(stdout, "RHIC Beam:   %1d\n", gMeasInfo->fPolBeam);

   for (int bid=0; bid<N_BUNCHES; bid++) {

      BeamBunchIter  ibb = gMeasInfo->fBeamBunches.find(bid+1);
      BeamBunch     &bb  = ibb->second;

      int pat = rec.data.polarizationFillPatternS[bid*3];

      if (pat > 0) {
         bb.SetBunchSpin(kSPIN_UP);
      } else if (pat < 0) {
         bb.SetBunchSpin(kSPIN_DOWN);
      } else {
         bb.SetBunchSpin(kSPIN_NULL);
      }

      pat = rec.data.measuredFillPatternM[bid*3];

      if (pat > 0) {
         bb.SetFilled(kTRUE);
      } else {
         bb.SetFilled(kFALSE);
      }
   }

   AsymRecover recover;

   if (Flag.spin_pattern >= 0) {
      cout << "\nSpin pattern has been overwritten" << endl;
      recover.OverwriteSpinPattern(Flag.spin_pattern);
   }

   if (Flag.fill_pattern >= 0) {
      cout << "\nFill pattern has been overwritten" << endl;
      recover.OverwriteFillPattern(Flag.fill_pattern);
   }

   // Mask bad/disabled bunches
   //if (gMeasInfo->NDisableBunch) {
   //   cout << "\nFill pattern has been overwritten again" << endl;
   //   recover.MaskFillPattern();
   //}

   // Print active Bunch
   //cout << "\nActive bunches: " << endl;

   //for (int i=0; i<NBUNCH; i++) {
   //   if (i%10 == 0) cout << " ";
   //   ActiveBunch[i] = gFillPattern[i];
   //   cout << ActiveBunch[i];
   //}

   //cout << endl;

   gMeasInfo->PrintBunchPatterns();
}
