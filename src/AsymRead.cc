//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRead.cc
//
//  Author    :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   10/17/2005
//

//#include "AsymRootGlobals.h"
#include "TStopwatch.h"

#include "AsymRead.h"

using namespace std;


/** */
RawDataProcessor::RawDataProcessor() : fFileName(""), fFile(0), fMem(0),
   fFileStream()
{
   fclose(fFile);
   delete[] fMem;
   fFileStream.close();
}


/** */
RawDataProcessor::RawDataProcessor(string fname) : fFileName(fname), fFile(0),
   fMem(0),
   fFileStream(fFileName.c_str(), ios::binary)
{
   FILE *fFile = fopen(fFileName.c_str(), "r");

   // reading the data till its end ...
   if (!fFile) {
      printf("ERROR: %s file not found. Force exit.\n", fFileName.c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", fFileName.c_str());

   // Create a BLOB with file content
   printf("Reading into memory...\n");
   TStopwatch sw;
 
   fFileStream.seekg(0, ios::end);
   fMemSize = fFileStream.tellg(); // in bytes
   printf("fileSize: %d\n", fMemSize);
   fFileStream.seekg(0, ios::beg);
   
   fMem = new char[fMemSize];
   fFileStream.read(fMem, fMemSize);
   fFileStream.close();

   sw.Stop();
   printf("Stopped reading into memory: %f s, %f s\n", sw.RealTime(), sw.CpuTime());
}


/** */
RawDataProcessor::~RawDataProcessor()
{
}


/** */
void RawDataProcessor::ReadRecBegin(TStructRunInfo &ri)
{
   recordBeginStruct *recBegin;

   //int nRecs = fread(&recBegin, sizeof(recBegin), 1, fp);

   recBegin = (recordBeginStruct*) fMem;

   //if (nRecs == 1 && (recBegin.header.type & REC_TYPEMASK) == REC_BEGIN) {


      cout << "Begin of data stream Ver: " << recBegin->version << endl;
      cout << "Comment: "                  << recBegin->comment << endl;
      cout << "Time Stamp: "               << ctime(&recBegin->header.timestamp.time);
      cout << "Unix Time Stamp: "          << recBegin->header.timestamp.time << endl;

      ri.StartTime  = recBegin->header.timestamp.time;
      ri.fPolBeam   = (recBegin->header.type & REC_MASK_BEAM) >> 16;
      ri.fPolStream = (recBegin->header.type & REC_MASK_STREAM) >> 20;

      //printf("fPolBeam:   %#x\n", ri.fPolBeam);
      //printf("fPolStream: %#x, %#x, %#x\n", ri.fPolStream, (UInt_t) recBegin->header.type, REC_MASK_STREAM);

      int polId = ri.GetPolarimeterId(ri.fPolBeam, ri.fPolStream);

      if (polId < 0)
         printf("WARNING: Polarimeter ID will be defined by other means\n");
      
      stringstream sstr;

      // First, try to get polarimeter id from the data
      if (polId >= 0) {
         sstr << ri.fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = ri.fPolId;

      // if failed, get id from the file name
      } else if (ri.GetPolarimeterId() >= 0) {
         sstr.str("");
         sstr << ri.fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = ri.fPolId;

      // see if the polarimeter id was set by command line argument 
      } else if (ri.fPolId >= 0) {
         sstr.str("");
         sstr << ri.fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = ri.fPolId;

      } else { // cannot proceed

         printf("ERROR: Unknown polarimeter ID\n");
         exit(-1);
      }

      sstr.str("");
      sstr << ri.StartTime;
      gRunDb.fFields["START_TIME"] = sstr.str();
      gRunDb.timeStamp = ri.StartTime; // should be always defined in raw data
      //gRunDb.Print();

   //} else
      //printf("ERROR: Cannot read REC_BEGIN record\n");
}


/** */
void RawDataProcessor::ReadDataFast()
{ //{{{
   printf("Started reading data file...\n");
   TStopwatch sw;

   //FILE *fp = fopen(gDataFileName.c_str(), "r");

   // reading the data till its end ...
   //if (!fp) {
   //   printf("ERROR: %s file not found. Force exit.\n", gDataFileName.c_str());
   //   exit(-1);
   //} else
   //   printf("\nFound file %s\n", gDataFileName.c_str());

   //recordHeaderStruct  header;
   recordHeaderStruct *mHeader;
   //recordDataStruct   data;

   // else
   //   printf("ERROR: Cannot read REC_BEGIN record\n");

   //int nEvent       = 0;
   UInt_t nTotalEvents = 0;
   UInt_t nReadEvents  = 0;

   char *mSeek = fMem;

   while (true) {

      //int nRecs = fread(&header, sizeof(recordHeaderStruct), 1, fp);

      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;

      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (recordHeaderStruct*) mSeek;

      //mSeek = mSeek + mHeader->len;

      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      //if ((header.type & REC_TYPEMASK) != REC_READAT) {
      if ((mHeader->type & REC_TYPEMASK) != REC_READAT) {

         //long offset = header.len - sizeof(recordHeaderStruct);
         //fseek(fp, offset, SEEK_CUR);

         mSeek = mSeek + mHeader->len;
         continue;
      }

      // We are here if rec type is REC_READAT
      //long delim = header.timestamp.delim;
      long delim = mHeader->timestamp.delim;

      //nEvent++;

      //if (nEvent > 1000) break;

      //size_t recSize = header.len - sizeof(recordHeaderStruct);
      size_t recSize = mHeader->len - sizeof(recordHeaderStruct);
      //unsigned char* buffer = new unsigned char[recSize];

      //nRecs = fread(buffer, recSize, 1, fp);

      char *mSeekAT = mSeek;

      mSeekAT += sizeof(recordHeaderStruct);

      mSeek = mSeek + mHeader->len;

      recordReadATStruct *ATPtrF;
      recordReadATStruct *ATPtr;

      for (UInt_t i=0; i<recSize; ) {

         //ATPtrF     = (recordReadATStruct*) (buffer + i);
         ATPtr     = (recordReadATStruct*) (mSeekAT + i);
         unsigned chId   = ATPtr->subhead.siNum; // si number
         unsigned Events = ATPtr->subhead.Events + 1;

         //if (Events > 100) printf("events: %d, %d\n", siNum, Events);

         i += sizeof(subheadStruct) + Events*sizeof(ATStruct);

         for (unsigned j=0; j<Events; j++, nReadEvents++) {
         
            //printf("f: %d %d %d\n", ATPtrF->data[j].a, ATPtrF->data[j].t, ATPtrF->data[j].s);
            //printf("m: %d %d %d\n", ATPtr->data[j].a, ATPtr->data[j].t, ATPtr->data[j].s);

            if (gMaxEventsUser > 0 && nTotalEvents >= gMaxEventsUser) break;

            //if (nReadEvents % dproc.thinout == 0)
            //if (nReadEvents > 100) break;

            if (gRandom->Rndm() > dproc.fFastCalibThinout) continue;

            gAsymRoot.SetChannelEvent(ATPtr->data[j], delim, chId);

            if ( !gAsymRoot.fChannelEvent->PassCutNoise() ) continue;
            if ( !gAsymRoot.fChannelEvent->PassCutEnabledChannel() ) continue;
            if ( !gAsymRoot.fChannelEvent->PassCutPulser() ) continue;
            if ( !gAsymRoot.fChannelEvent->PassCutDetectorChannel() ) continue;
            //if ( //!gAsymRoot.fChannelEvent->PassCutDepEnergyTime() ) continue;

            gAsymRoot.FillPreProcess();

            nTotalEvents++;
         }
      }
   }

   sw.Stop();
   printf("Stopped reading data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());

   printf("Total events read:     %12d\n", nReadEvents);
   printf("Total events accepted: %12d\n", nTotalEvents);

   // (Roughly) Process all channel banana
   gAsymRoot.CalibrateFast();

} //}}}


/** */
void readDataFast()
{ //{{{
   printf("Started reading data file...\n");
   TStopwatch sw;

   FILE *fp = fopen(gDataFileName.c_str(), "r");

   // reading the data till its end ...
   if (!fp) {
      printf("ERROR: %s file not found. Force exit.\n", gDataFileName.c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", gDataFileName.c_str());

   recordHeaderStruct header;
   //recordDataStruct   data;

   // else
   //   printf("ERROR: Cannot read REC_BEGIN record\n");

   //int nEvent       = 0;
   UInt_t nTotalEvents = 0;
   UInt_t nReadEvents  = 0;

   while (true) {

      int nRecs = fread(&header, sizeof(recordHeaderStruct), 1, fp);

      if (nRecs != 1) break;

      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      if ((header.type & REC_TYPEMASK) != REC_READAT) {

         long offset = header.len - sizeof(recordHeaderStruct);
         fseek(fp, offset, SEEK_CUR);
         continue;
      }

      // We are here if rec type is REC_READAT
      long delim = header.timestamp.delim;

      //nEvent++;

      //if (nEvent > 1000) break;

      size_t recSize = header.len - sizeof(recordHeaderStruct);
      unsigned char* buffer = new unsigned char[recSize];

      nRecs = fread(buffer, recSize, 1, fp);

      recordReadATStruct *ATPtr;

      for (UInt_t i=0; i<recSize; ) {

         ATPtr     = (recordReadATStruct*) (buffer + i);
         unsigned chId   = ATPtr->subhead.siNum; // si number
         unsigned Events = ATPtr->subhead.Events + 1;

         //if (Events > 100) printf("events: %d, %d\n", siNum, Events);

         i += sizeof(subheadStruct) + Events*sizeof(ATStruct);

         for (unsigned j=0; j<Events; j++, nReadEvents++) {

            if (gMaxEventsUser > 0 && nTotalEvents >= gMaxEventsUser) break;

            //if (nReadEvents % dproc.thinout == 0)
            if (gRandom->Rndm() <= dproc.fFastCalibThinout) {

               gAsymRoot.SetChannelEvent(ATPtr->data[j], delim, chId);

               if (!gAsymRoot.fChannelEvent->PassCutDetectorChannel()  ||
                   !gAsymRoot.fChannelEvent->PassCutPulser() ||
                   !gAsymRoot.fChannelEvent->PassCutNoise() ||
                   //!gAsymRoot.fChannelEvent->PassCutDepEnergyTime() ||
                   !gAsymRoot.fChannelEvent->PassCutEnabledChannel()) continue;

               gAsymRoot.FillPreProcess();

               nTotalEvents++;
            }

            //if (Events > 100) printf("%d, %d\n", amp, tdc);
         }
      }

      delete buffer;
   }

   fclose(fp);

   sw.Stop();
   printf("Stopped reading data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());

   printf("Total events read:     %12d\n", nReadEvents);
   printf("Total events accepted: %12d\n", nTotalEvents);

   // (Roughly) Process all channel banana
   gAsymRoot.CalibrateFast();

} //}}}


/** */
void ReadRecBegin(TStructRunInfo &ri)
{
   FILE *fp = fopen(gDataFileName.c_str(), "r");

   // reading the data till its end ...
   if (!fp) {
      printf("ERROR: %s file not found. Force exit.\n", gDataFileName.c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", gDataFileName.c_str());

   recordBeginStruct recBegin;

   int nRecs = fread(&recBegin, sizeof(recBegin), 1, fp);

   if (nRecs == 1 && (recBegin.header.type & REC_TYPEMASK) == REC_BEGIN) {

      cout << "Begin of data stream Ver: " << recBegin.version << endl;
      cout << "Comment: "                  << recBegin.comment << endl;
      cout << "Time Stamp: "               << ctime(&recBegin.header.timestamp.time);
      cout << "Unix Time Stamp: "          << recBegin.header.timestamp.time << endl;

      ri.StartTime  = recBegin.header.timestamp.time;
      ri.fPolBeam   = (recBegin.header.type & REC_MASK_BEAM) >> 16;
      ri.fPolStream = (recBegin.header.type & REC_MASK_STREAM) >> 20;

      //printf("fPolBeam:   %#x\n", ri.fPolBeam);
      //printf("fPolStream: %#x, %#x, %#x\n", ri.fPolStream, (UInt_t) recBegin.header.type, REC_MASK_STREAM);

      int polId = ri.GetPolarimeterId(ri.fPolBeam, ri.fPolStream);

      if (polId < 0)
         printf("WARNING: Polarimeter ID will be defined by other means\n");
      
      stringstream sstr;

      // First, try to get polarimeter id from the data
      if (polId >= 0) {
         sstr << ri.fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = ri.fPolId;

      // if failed, get id from the file name
      } else if (ri.GetPolarimeterId() >= 0) {
         sstr.str("");
         sstr << ri.fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = ri.fPolId;

      // see if the polarimeter id was set by command line argument 
      } else if (ri.fPolId >= 0) {
         sstr.str("");
         sstr << ri.fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = ri.fPolId;

      } else { // cannot proceed

         printf("ERROR: Unknown polarimeter ID\n");
         exit(-1);
      }

      sstr.str("");
      sstr << ri.StartTime;
      gRunDb.fFields["START_TIME"] = sstr.str();
      gRunDb.timeStamp = ri.StartTime; // should be always defined in raw data
      //gRunDb.Print();

   } else
      printf("ERROR: Cannot read REC_BEGIN record\n");

   fclose(fp);
}


// =================
// read loop routine
// =================
void readloop()
{
   static int READ_FLAG = 0;

   // Common structure for the data format
   static union {
       recordHeaderStruct      header;
       recordBeginStruct       begin;
       recordPolAdoStruct      polado;
       recordTagAdoStruct      tagado;
       recordBeamAdoStruct     beamado;
       recordConfigRhicStruct  cfg;
       recordReadWaveStruct    all;
       recordReadWave120Struct all120;
       recordReadATStruct      at;
       recordWFDV8ArrayStruct  wfd;
       recordEndStruct         end;
       recordScalersStruct     scal;
       recordWcmAdoStruct      wcmado;
       recordCountRate         countRate;
       char                    buffer[BSIZE_OFFLINE*sizeof(int)];
       recordDataStruct        data;
   } rec;

   //recordConfigRhicStruct  *cfginfo;
   polDataStruct            poldat;
   beamDataStruct           beamdat;
   targetDataStruct         tgtdat1;
   targetDataStruct         tgtdat2;
   wcmDataStruct            wcmdat;

   processEvent             event;
   AsymRecover              recover;

   FILE *fp;
   int   rval;
   int   recSize;
   int   flag = 0;     // exit from while when flag==1
   //int THINOUT = Flag.feedback ? dproc.thinout : Nskip;

   // reading the data till its end ...
   if ((fp = fopen(gDataFileName.c_str(), "r")) == NULL) {
      printf("ERROR: %s file not found. Force exit.\n", gDataFileName.c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", gDataFileName.c_str());

   while (flag == 0) {

      if (fread(&rec.header, sizeof(recordHeaderStruct), 1, fp) != 1)
         break;

      //printf("Currently consider record:   %0#10x, len: %ld\n", (UInt_t) rec.header.type, rec.header.len);

      if (feof(fp)) {
         fprintf(stdout, "Expected end of file\n");
         break;
      }

      if ( (UInt_t) rec.header.len > BSIZE_OFFLINE*sizeof(int)) {
         fprintf(stdout, "Not enough buffer d: %ld byte b: %u byte\n",
                 rec.header.len, sizeof(rec));
         break;
      }

      recSize = rec.header.len - sizeof(recordHeaderStruct);

      // Skip empty data records
      if (!recSize) continue;

      // Read rest of the structure
      rval = fread(&rec.begin.version, recSize, 1, fp);

      if (feof(fp)) {
         perror("Unexpected end of file");
         exit(1);
         break;
      }

      if (rval != 1) {
         fprintf(stdout, "IO error in stream (header) %s\n", strerror(errno));
         fprintf(stdout, "IO error in stream (header) %s\n", strerror(ferror(fp)));
         continue;
      }

      //ds
      //printf("type: %#X\n", (rec.header.type & REC_TYPEMASK));

      // Distinguish between blue & yellow beam.
      // Presently, it overwrites pre-defined gRunInfo.fPolBeam
      // in GetPolarimetryID_and_RHICBeam(char RunID[]).
      // Could be used as cross check for the future
      //switch (rec.header.type & REC_BEAMMASK) {
      //case REC_YELLOW:
      //   gRunInfo.fPolBeam = 1;
      //   break;
      //case REC_BLUE:
      //   gRunInfo.fPolBeam = 0;
      //   break;
      //}

      //Long_t my_i;
      //Long_t *my_pointer;

      // Main Switch
      switch (rec.header.type & REC_TYPEMASK) {

      case REC_BEGIN:
         if (ReadFlag.RECBEGIN) break; // ??? should encounter only once in each file

         cout << "Data stream version: " << rec.begin.version << endl;
         cout << "Comment: "             << rec.begin.comment << endl;
         cout << "Date & Time: "         << ctime(&rec.begin.header.timestamp.time);
         cout << "Timestamp: "           << rec.begin.header.timestamp.time << endl;

         gRunInfo.StartTime = rec.begin.header.timestamp.time;
         gRunInfo.fDataFormatVersion = rec.begin.version;

         gRunInfo.fPolBeam   = (rec.header.type & REC_MASK_BEAM) >> 16;
         gRunInfo.fPolStream = (rec.header.type & REC_MASK_STREAM) >> 20;

         //printf("fPolBeam:   %#x\n", gRunInfo.fPolBeam);
         //printf("fPolStream: %#x, %#x, %#x\n", gRunInfo.fPolStream, rec.header.type, REC_MASK_STREAM);

         // Configure colliding bunch patterns for PHENIX-BRAHMS and STAR
         PrepareCollidingBunchPattern();

         ReadFlag.RECBEGIN = 1;
         
         break;

      case REC_POLADO:
         memcpy(&poldat, &rec.polado.data, sizeof(poldat));
         DecodeTargetID(poldat);
         break;

      case REC_TAGADO:
         memcpy(&tgtdat1, &rec.tagado.data[0], sizeof(tgtdat1));
         memcpy(&tgtdat2, &rec.tagado.data[1], sizeof(tgtdat2));
         break; // later

      case REC_PCTARGET:
         // Do not process this record for calibration runs (CMODE). May contain
         // invalid ndelim info
         if (!ReadFlag.PCTARGET && !dproc.CMODE) {

            printf("Reading REC_PCTARGET record...\n");

            ndelim  = (rec.header.len - sizeof(rec.header))/(4*sizeof(long));
            long *pointer = (long *) &rec.buffer[sizeof(rec.header)];

            if (dproc.HasTargetBit()) { ProcessRecordPCTarget(pointer, ndelim); }

            ReadFlag.PCTARGET = 1;
         }

         break;

      case REC_COUNTRATE:
         printf("Reading REC_COUNTRATE record... size = %d\n", recSize);

         //my_i = (rec.header.len - sizeof(rec.header))/(sizeof(long));
         ////my_pointer = (Double_t *) &rec.buffer[sizeof(rec.header)];
         //my_pointer = (Long_t *) rec.countRate.data;

         //printf("len, size: %d, %d\n", rec.header.len, my_i);

         //for (long i=0; i<my_i; i++) {                                        
         //   printf("i: %d, %d\n", i, *(my_pointer+i));
         //}

         if (dproc.HasProfileBit()) 
            ProcessRecord(rec.countRate);
         break;

      case REC_WCMADO:

         if (!ReadFlag.WCMADO) {

            printf("Reading WCM information\n");
            memcpy(&wcmdat, &rec.wcmado.data,sizeof(wcmdat));

            for (int bid=0; bid<NBUNCH; bid++) {
               wcmdist[bid] = wcmdat.fillDataM[bid*3];
               gRunInfo.WcmSum += wcmdist[bid] * fillpat[bid];
            }

            gRunInfo.WcmAve = gRunInfo.WcmSum/float(gRunInfo.NActiveBunch);
            ReadFlag.WCMADO = 1;
         }

         gRunInfo.StopTime = rec.header.timestamp.time;

         break;

      case REC_BEAMADO:

         if (!ReadFlag.BEAMADO) {

            fprintf(stdout,"Reading Beam Information\n");
            memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));

            gRunInfo.BeamEnergy = beamdat.beamEnergyM;
            fprintf(stdout,"Beam Energy: %8.2f\n", gRunInfo.BeamEnergy);
            fprintf(stdout,"RHIC Beam:   %1d\n", gRunInfo.fPolBeam);

            // Add inj_tshift for injection measurements
            if (gRunInfo.BeamEnergy < 30) dproc.tshift += dproc.inj_tshift;

            int pat;

            for (int bid=0; bid<120; bid++) {

               pat = beamdat.polarizationFillPatternS[bid*3];

               if (pat > 0) {
                  spinpat[bid] = 1;
               } else if (pat < 0) {
                  spinpat[bid] = -1;
               } else {
                  spinpat[bid] = 0;
               }

               pat = beamdat.measuredFillPatternM[bid*3];

               if (pat > 0) {
                  fillpat[bid] = 1;
                  gRunInfo.NFilledBunch++;
               } else if (pat < 0) {
                  fillpat[bid] = -1;
                  gRunInfo.NFilledBunch++;
               } else {
                  fillpat[bid] = 0;
               }
            }

            // Print Spin Pattern and Recover Spin Pattern by User Defined ones
            cout << "\nSpin Pattern Used:" << endl;
            PrintBunchPattern(spinpat);

            if (Flag.spin_pattern >= 0) {
               recover.OverwriteSpinPattern(Flag.spin_pattern);
               PrintBunchPattern(spinpat);
            }

            // Print Fill Pattern and Recover Fill Pattern by User Defined ones
            cout << "\nFill Pattern Used:" << endl;
            PrintBunchPattern(fillpat);

            if (Flag.fill_pattern >= 0) {
               recover.OverwriteFillPattern(Flag.fill_pattern);
               PrintBunchPattern(fillpat);
            }

            // Mask bad/disabled bunches
            if (gRunInfo.NDisableBunch) {
               recover.MaskFillPattern();
               cout << "\nMasked Fill Pattern : " << endl;
               PrintBunchPattern(fillpat);
            }

            // Print active Bunch
            cout << "\nActive bunches: " << endl;

            for (int i=0; i<NBUNCH; i++) {
               if (i%10 == 0) cout << " ";
               ActiveBunch[i] = fillpat[i];
               cout << ActiveBunch[i];
            }

            cout << endl;
            gRunInfo.NActiveBunch = gRunInfo.NFilledBunch - gRunInfo.NDisableBunch;
            ReadFlag.BEAMADO = 1;
         }

         gRunInfo.StopTime = rec.header.timestamp.time;
         break;

      case REC_END:
         gRunInfo.StopTime = rec.end.header.timestamp.time;
         break;

      case REC_READRAW:
      case REC_READSUB:
         break;

      case REC_READALL:
         break;

      case REC_WFDV8SCAL:
         if (dproc.HasScalerBit())
            ProcessRecord(rec.wfd);
         break;

      case REC_SCALERS:
         fprintf(stdout, "Scaler readtime \n");
         fprintf(stdout, "Scaler End Time: %s\n", ctime(&rec.scal.header.timestamp.time));

         gRunInfo.StopTime = rec.header.timestamp.time;

         break;

      case REC_READAT:

        // Display configuration and configure active strips just once
        if (!READ_FLAG) {

           // Configure Active Strip Map
           ConfigureActiveStrip(mask.detector);

           printConfig(cfginfo);

           READ_FLAG = 1;

           if (gRunDb.run_status_s == "Junk") {
              cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
              exit(-1);
           }
        }

        event.delim = rec.header.timestamp.delim;
        //int nreadsi     = 0;  // number of Si already read
        //int nreadbyte   = 0;  // number of bite already read

        recordReadATStruct *ATPtr;
        int Nevent;

        for (UInt_t i=0; i<rec.header.len - sizeof(recordHeaderStruct);) {

           ATPtr     = (recordReadATStruct*) (&rec.data.rec[i]);
           event.stN = ATPtr->subhead.siNum; // si number
           Nevent    = ATPtr->subhead.Events + 1;

           // count the total number of event records in raw file
           dproc.nEventsTotal += Nevent;

           i += sizeof(subheadStruct) + Nevent*sizeof(ATStruct);

           if (i > rec.header.len - sizeof(recordHeaderStruct)) {
              cout << "Broken record "<< rec.header.num << "("
                   << rec.header.len << " bytes). Last subhead: siNum= "
                   << event.stN << " Events= " << Nevent << endl;
              break;
           }

           //ds:
           //printf("Nevent(s): %d, si number: %d, gMaxEventsUser %d, Nread %d\n", Nevent,
           //       event.stN, gMaxEventsUser, Nread);

           for (int j=0; j<Nevent; j++, Nread++) {

              //ds: Skip events if already read enough events specified by user
              if (gMaxEventsUser > 0 && Nevtot >= gMaxEventsUser) break;

              if (Nread % dproc.thinout == 0) {

                 //Nread++;
                 Nevtot++;

                 event.amp    = ATPtr->data[j].a;
                 event.tdc    = ATPtr->data[j].t;
                 event.intg   = ATPtr->data[j].s;
                 event.bid    = ATPtr->data[j].b;
                 event.tdcmax = ATPtr->data[j].tmax;
                 event.rev0   = ATPtr->data[j].rev0;
                 event.rev    = ATPtr->data[j].rev;

                 // Finer Target Position Resolution by counting stepping moter
                 // This feature became available after Run06
                 if (gRunInfo.Run >= 6) {
                    cntr.revolution = event.delim*512 + event.rev*2 + event.rev0;

                    if (cntr.revolution > gRunInfo.MaxRevolution)
                       gRunInfo.MaxRevolution = cntr.revolution;

                    if (event.stN == 72 && event.delim != tgt.eventID) {
                       tgt.x += dproc.target_count_mm * (float)tgt.vector;
                       tgt.vector=-1;
                    }

                    if (event.stN == 72 || event.stN == 73) {
                        switch (event.stN){
                        case 72:
                            tgt.eventID = event.delim;
                            ++cntr.tgtMotion;
                            break;
                        case 73:
                            tgt.vector = 1;
                            break;
                        }
                    }
                 }

                 gAsymRoot.SetChannelEvent(event);

                 if (dproc.SAVETREES.any()) { gAsymRoot.AddChannelEvent(); }

                 //cout << " i "            << i
                 //     << " Nevent "       << Nevent
                 //     << " St "           << event.stN
                 //     << " amp "          << event.amp
                 //     << " tdc "          << event.tdc
                 //     << " bid "          << event.bid
                 //     << " rev0 "         << event.rev0
                 //     << " rev  "         << event.rev
                 //     << " revolution #=" << cntr.revolution << endl;

                 // Raw histograms
                 //ds: Why don't we save raw histograms in regular passes?
                 //if (!Flag.feedback && dproc.RAWHISTOGRAM)
                 if (event.stN < 72) {
                    bunch_dist_raw -> Fill(event.bid);
                    strip_dist_raw -> Fill(event.stN);
                    tdc_raw        -> Fill(event.tdc);
                    adc_raw        -> Fill(event.amp);
                    tdc_vs_adc_raw -> Fill(event.amp,event.tdc);

                    //ds: Don't we need to compare absolute value of fillpat[bid] with 1?
                    if (fabs(fillpat[event.bid]) != 1)
                       tdc_vs_adc_false_bunch_raw->Fill(event.amp, event.tdc);
                 }

                 // process event for following case:
                 //    fill pattern     = 1
                 //    Calibration mode = 1
                 // also process only if strip is active
                 //if (event.stN >= NSTRIP)
                    //printf("channel111: %d, %d\n", event.stN, gRunInfo.ActiveStrip[event.stN]);
                    //printf("channel111: %d\n", event.stN);

                 if ( fillpat[event.bid] == 1 || dproc.CMODE == 1) // || event.stN >= 72) ) //&&
                      //gRunInfo.ActiveStrip[event.stN] )
                 {
                    event_process(&event);
                 }

                 if (Nevtot%10000==0) {

                    //ds: Can we simplify this by leaving only one report?
                    //if (Flag.feedback){
                    //   printf("Feedback Mode Ncounts = %ld \r", Nread) ;
                    //} else {
                       printf("%.3f : Proccesing  Ncounts   = %u \r", gRunInfo.RUNID, Nevtot);
                    //}

                    fflush(stdout);
                 }
              }
           }
        }

        break;

      case REC_RHIC_CONF:
         if (!ReadFlag.RHICCONF) {

            fprintf(stdout, "Reading REC_RHIC_CONF record from file...\n");

            cfginfo = (recordConfigRhicStruct *) malloc(sizeof(recordConfigRhicStruct) +
                      (rec.cfg.data.NumChannels - 1) * sizeof(SiChanStruct));

            //XXX printf("TTT: %d\n", rec.cfg.data.NumChannels);

            memcpy(cfginfo, &rec.cfg, sizeof(recordConfigRhicStruct) +
                   (rec.cfg.data.NumChannels - 1) * sizeof(SiChanStruct));

            // when we mandatory provide cfg info
            //if (dproc.RECONFMODE == 1) {
            //   reConfig(cfginfo);
            //}

            // Recalculate Run constants
            UpdateRunConst(cfginfo);

            if (dproc.MESSAGE == 1) exit(0);

            ReadFlag.RHICCONF = 1;
         }

         gRunInfo.StopTime = rec.header.timestamp.time;

         break;

      case REC_SUBRUN:
         printf("Processing record: REC_SUBRUN\n");
         gRunInfo.StopTime = rec.header.timestamp.time;
         break;

      default:    // unknown record
         fprintf(stdout, "Encountered Unknown Record \"%#x\"\n",
            (UInt_t) rec.header.type & REC_TYPEMASK);
         break;
      }
   }

   fclose(fp);

   // Post processing
   if (dproc.HasNormalBit())
      end_process();

   fprintf(stdout, "End of data stream \n");
   fprintf(stdout, "End Time: %s\n", ctime(&gRunInfo.StopTime));
   fprintf(stdout, "%ld Carbons are found in\n", Nevcut);
   //fprintf(stdout, "Data Comment: %s\n", rec.end.comment);
   fprintf(stdout, "Total events in file %d\n", dproc.nEventsTotal);
   fprintf(stdout, "First %d events processed\n", Nread);
   fprintf(stdout, "%d events saved\n", Nevtot);

   dproc.nEventsProcessed = Nevtot;

   // Add info to database entry
   stringstream sstr;

   sstr.str(""); sstr << dproc.nEventsTotal;
   gRunDb.fFields["NEVENTS_TOTAL"] = sstr.str();

   sstr.str(""); sstr << dproc.nEventsProcessed;
   gRunDb.fFields["NEVENTS_PROCESSED"] = sstr.str();

   sstr.str(""); sstr << gRunInfo.BeamEnergy;
   gRunDb.fFields["BEAM_ENERGY"] = sstr.str();

   // Some incompleted run don't even have REC_READAT flag. Force printConfig.
   if (!Nread && !READ_FLAG) {
      printConfig(cfginfo);

      if (gRunDb.run_status_s == "Junk") {
         cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
         exit(-1);
      }
   }
}


/** */
void UpdateRunConst(TRecordConfigRhicStruct *ci)
{
   if (!ci) return;

   //ci->Print();

   float Ct = ci->data.WFDTUnit/2.; // Determine the TDC count unit (ns/channel)
   //float L  = ci->data.TOFLength;
   float L  = ci->data.TOFLength;

   gRunConsts[0] = RunConst();
   gRunConsts[0].Print();

   for (UShort_t i=1; i<=ci->data.NumChannels; i++) {

      //if (gRunInfo.fDataFormatVersion == 40200 ) { // XXX should be like this
      if (gRunInfo.fDataFormatVersion == 40200 && (gRunInfo.fPolId == 1 || gRunInfo.fPolId == 2) ) // downstream
      {
         L = ci->data.chan[i-1].TOFLength;
         //printf("LLL: %f\n", L);
      } else
         L = CARBON_PATH_DISTANCE;

      gRunConsts[i] = RunConst(L, Ct);

      //printf("Channel %-2d consts: \n", i);
      //gRunConsts[i].Print();
   }
}


// Description : print out spin (Mode=0), fill (Mode=1) pattern
// Input       : Mode
void PrintBunchPattern(int *pattern)
{
   char symbol[3][2];

   sprintf(symbol[0], "-"); // -1
   sprintf(symbol[1], "."); //  0
   sprintf(symbol[2], "+"); // +1
 
   for (int i=0; i<NBUNCH; i++) {
 
      if (i%10 == 0) cout << " ";
      cout << symbol[ pattern[i] + 1];
   }
 
   cout << endl;
}


// Description : Decorde target infomation.
//             : presently the target ID is assumed to be appear at the last of
//             : character string poldat.targetIdS.
// Input       : polDataStruct poldat
void DecodeTargetID(polDataStruct poldat)
{
  cout << endl;
  cout << "target ID = " << poldat.targetIdS << endl;
  //cout << "startTimeS = " << poldat.startTimeS << endl;
  //cout << "stopTimeS = " << poldat.stopTimeS << endl;

  gRunDb.fFields["TARGET_ID"] = poldat.targetIdS;

  // initiarization
  gRunInfo.targetID = '-';

  string str(poldat.targetIdS);

  if (str.find("Background") < str.size())   gRunInfo.targetID='B';
  if (str.find("1")         == str.size()-1) gRunInfo.targetID='1';
  if (str.find("2")         == str.size()-1) gRunInfo.targetID='2';
  if (str.find("3")         == str.size()-1) gRunInfo.targetID='3';
  if (str.find("4")         == str.size()-1) gRunInfo.targetID='4';
  if (str.find("5")         == str.size()-1) gRunInfo.targetID='5';
  if (str.find("6")         == str.size()-1) gRunInfo.targetID='6';
  if (str.find("7")         == str.size()-1) gRunInfo.targetID='7';
  if (str.find("8")         == str.size()-1) gRunInfo.targetID='-';

  // Restore Vertical or Horizontal target information
  // in case this information isn't decorded correctly
  // within REC_PCTARGET routine. If the target is horizontal,
  // then mask 90 degree detector.
  if (gRunInfo.target == '-') {
     if (str.find("Vert") == 0 || str.find("V") == 0) { gRunInfo.target='V'; tgt.VHtarget=0;}
     if (str.find("Horz") == 0 || str.find("H") == 0) {
        gRunInfo.target='H';
        tgt.VHtarget=1;
        // This is too late to reconfigure strip mask because this routine is
        // executed at the end of event loop. Too bad. /* March 5,'09 IN */
        //      mask.detector = 0x2D;
        //      ConfigureActiveStrip(mask.detector);
     }
  }
}


// Method name : PrepareCollidingBunchPattern(gRunInfo.fPolBeam)
// Description : Configure phx.bunchpat[] and str.bunchpat[] arrays only for colliding bunches
void PrepareCollidingBunchPattern()
{
   for (int i=0; i<NBUNCH; i++){
      phx.bunchpat[i] = 1; // PHENIX bunch patterns
      str.bunchpat[i] = 1; // STAR bunch patterns
   }
 
   if (gRunInfo.fPolBeam == 1) { // Yellow Beam
      for (int j=31; j<40; j++) str.bunchpat[j] = phx.bunchpat[j+40] = 0;
   } else if (gRunInfo.fPolBeam == 2){ // Blue Beam
      for (int j=31; j<40; j++) phx.bunchpat[j] = str.bunchpat[j+40] = 0;
   }
 
   cout << "===== Colliding Bunch pattern =======" << endl;
   cout << " IP2, IP8:  " ; for (int i=0; i<NBUNCH; i++) cout << phx.bunchpat[i] ; cout << endl;
   cout << " IP6, IP10: " ; for (int i=0; i<NBUNCH; i++) cout << str.bunchpat[i] ; cout << endl;
   cout << "=====================================" << endl;
}


/** */
void ProcessRecord(recordPolAdoStruct &rec)
{
}


/** */
void ProcessRecord(recordWFDV8ArrayStruct &rec)
{ //{{{
   Int_t s1=0, s2=0, s3=0, s4=0;
   //float hist[1536];
 
   UShort_t chId = rec.siNum + 1;

   // Consider only silicon channels
   if (chId > NSTRIP) return;
 
   //for (int i=0; i<1536; i++) {
   //   //hist[i] = rec.hist[i];
   //   //printf("i, hist[i]: %4d, %8.3f\n", i, hist[i]);
   //}

   for (int i=0; i< 128; i++) s1 += rec.hist[i];        // bunch hist
   for (int i=0; i< 128; i++) s2 += rec.hist[i+128];    // unpol energy
   for (int i=0; i< 128; i++) s3 += rec.hist[i+256];    // pol+ energy
   for (int i=0; i< 128; i++) s4 += rec.hist[i+384];    // pol- energy

   //HPAKAD( 200 + chId, &hist[0]);
   //HPAKAD( 300 + chId, &hist[128]);
   //HPAKAD( 400 + chId, &hist[256]);
   //HPAKAD( 500 + chId, &hist[384]);
   //HPAKAD(1300 + chId, &hist[128]);
   //HPAKAD(1400 + chId, &hist[256]);
   //HPAKAD(1500 + chId, &hist[384]);
 
   //if (sipar_.ifine == 0) {
   //   //HPAKAD(600 + chId, &hist[512]);
   //   //HPAKAD(1600 + chId, &hist[512]);
   //} else {
   //   //HPAKAD(700 + chId, &hist[512]);
   //   //HPAKAD(1700 + chId, &hist[512]);
   //}
 
   printf("Si%02d : %12ld %12ld %12ld %12ld %12ld    %12d  %12d  %12d  %12d",
          rec.siNum + 1, rec.scalers[0], rec.scalers[1],
          rec.scalers[2], rec.scalers[3], rec.scalers[4],
          s1, s2, s3, s4);
 
   gAsymRoot.FillScallerHists(rec.hist, chId);
 
   // Check for hardware counts compatibility. Mark channel as bad if something is wrong
   if (s2 < rec.scalers[2] || s3 != rec.scalers[0] ||
       s4 != rec.scalers[1] || s1 < s2 + s3 + s4)
   {
      printf(" <- !!!");
      //sipar_.mark[rec.siNum]++;
   }
 
   printf("\n");
 
   Int_t cnt = 0;

   cnt += rec.scalers[0] + rec.scalers[1] + rec.scalers[2];
 
   for (int i=0; i<3; i++) {
      //sscal_[rec.siNum * 3+i] += rec.scalers[i];
   }
} //}}}


/** */
void ProcessRecord(recordCountRate &rec)
{
   UInt_t size = (rec.header.len - sizeof(rec.header))/(sizeof(long));
   Long_t *pointer = (Long_t *) rec.data;
   //Double_t *pointer = (Double_t *) &rec.buffer[sizeof(rec.header)];

   printf("len, size: %ld, %d\n", rec.header.len, size);

   for (UInt_t i=0; i<size; i++) {
      printf("i: %d, %ld\n", i, *(pointer+i));
   }

   gAsymRoot.FillProfileHists(size, pointer);
   //gAsymRoot.ProcessProfileHists();
}


/** */
void ProcessRecordPCTarget(long* rec, int ndelim)
{ //{{{
   long* pointer = rec;

   --rec;
   //UShort_t i = 0;

   // copy data to a linear array
   Double_t* linRec = new Double_t[ndelim*4 + 8]; // there 2 under and ooverflow bins

   for (UInt_t k=0; k<ndelim; k++) {

      *(linRec + k + 1      )      = *++rec; // Horizontal target
      *(linRec + k + (ndelim+2)  ) = *++rec;
      *(linRec + k + (ndelim+2)*2) = *++rec; // Vertical target
      *(linRec + k + (ndelim+2)*3) = *++rec;

      //printf("%8d %8d %12.3f %12.3f %12.3f %12.3f\n", k, ndelim, *(linRec + k + 1), *(linRec + k + (ndelim+2)  ), *(linRec + k + (ndelim+2)*2), *(linRec + k + (ndelim+2)*3) );
   }

   gAsymRoot.FillTargetHists(ndelim, linRec);

   delete [] linRec;

   // 
   tgt.fNDelim = ndelim;

   --pointer;
   UShort_t i = 0;

   printf("   index    total        x-pos        y-pos\n");

   for (int k=0; k<ndelim; k++) {

      tgt.Linear[k][1] = *++pointer; // Horizontal target
      tgt.Rotary[k][1] = *++pointer;
      tgt.Linear[k][0] = *++pointer; // Vertical target
      tgt.Rotary[k][0] = *++pointer;

      // force 0 for +/-1 tiny readout as target position.
      if (abs(tgt.Rotary[k][1]) <= 1) tgt.Rotary[k][1] = 0;

      // identify Horizontal or Vertical target from the first target
      // rotary position readout.
      if (k == 0) {
         // From Run09 on, the horizontal/vertical targets are
         // identified by linear motion
         long int tgt_identifyV = gRunInfo.RUNID < 10040 ? tgt.Rotary[k][0] : tgt.Linear[k][0];
         long int tgt_identifyH = gRunInfo.RUNID < 10040 ? tgt.Rotary[k][1] : tgt.Linear[k][1];

         if ( ( !tgt.Rotary[k][0] && !tgt.Rotary[k][1] ) ||
              (  tgt.Rotary[k][0] &&  tgt.Rotary[k][1] ) )
         {
            cout << "ERROR: no target rotary info. Don't know H/V target" << endl;
            gRunInfo.target = '-';
         }

         if (tgt_identifyV) {
            tgt.VHtarget    = 0;
            gRunInfo.target = 'V';
            cout << "Vertical Target in finite position" << endl;
         } else if (tgt_identifyH) {
            tgt.VHtarget    = 1;
            gRunInfo.target = 'H';
            cout << "Horizontal Target in finite position" << endl;
         } else {
            cout << "Warning: Target infomation cannot be recognized.." << endl;
         }

         tgt.x       = tgt.Rotary[k][tgt.VHtarget] * dproc.target_count_mm;
         tgt.Time[i] = k;
         tgt.X[i]    = tgt.x;

         printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
                 TgtIndex[k], tgt.X[TgtIndex[k]],
                 tgt.Rotary[k][0]*dproc.target_count_mm,
                 tgt.Rotary[k][1]*dproc.target_count_mm);
      } else {

         TgtIndex[k] = i;

         if (tgt.Rotary[k][1] != tgt.Rotary[k-1][1] ||
             tgt.Rotary[k][0] != tgt.Rotary[k-1][0] )
         {
            TgtIndex[k]                 = ++i;
            tgt.X[TgtIndex[k]]          = tgt.Rotary[k][tgt.VHtarget] * dproc.target_count_mm;
            tgt.Time[TgtIndex[k]]       = float(k);
            tgt.Interval[TgtIndex[k-1]] = tgt.Time[TgtIndex[k]] - tgt.Time[TgtIndex[k-1]];
            ++nTgtIndex;

            printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
               TgtIndex[k], tgt.X[TgtIndex[k]],
               tgt.Rotary[k][0]*dproc.target_count_mm,
               tgt.Rotary[k][1]*dproc.target_count_mm);

            //++i;
         }
      }

      // target position array including static target motion
      tgt.all.x[k] = tgt.Rotary[k][tgt.VHtarget] * dproc.target_count_mm ;
   }

   if (nTgtIndex > TGT_OPERATION) //gRunInfo.TgtOperation=" scan";
      strcpy(gRunInfo.TgtOperation, " scan");

   printf("Number of delimiters: %4d\n", ndelim);
   printf("nTgtIndex: %d\n", nTgtIndex);

   tgt.Print();

   // define target histograms (hbook)
   //tgtHistBook();

   // disable 90 degrees detectors for horizontal target 0x2D={10 1101}
   if (tgt.VHtarget) mask.detector = 0x2D;
} //}}}
