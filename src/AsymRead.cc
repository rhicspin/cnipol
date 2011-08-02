//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRead.cc
//
//  Author    :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   10/17/2005
//

#include "AsymRead.h"

#include <sstream>

#include "mysql++.h"

#include "TRandom.h"
#include "TStopwatch.h"

#include "AsymCalculator.h"
#include "AsymProcess.h"
#include "AsymRecover.h"
#include "AsymRoot.h"
#include "AnaInfo.h"
#include "TargetInfo.h"

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
{ //{{{
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
} //}}}


/** */
RawDataProcessor::~RawDataProcessor()
{
}


/** */
void RawDataProcessor::ReadRecBegin(MseRunInfoX* run)
{ //{{{
   recordBeginStruct *recBegin;

   //int nRecs = fread(&recBegin, sizeof(recBegin), 1, fp);

   recBegin = (recordBeginStruct*) fMem;

   //if (nRecs == 1 && (recBegin.header.type & REC_TYPEMASK) == REC_BEGIN) {

      cout << "Begin of data stream Ver: " << recBegin->version << endl;
      cout << "Comment: "                  << recBegin->comment << endl;
      cout << "Time Stamp: "               << ctime(&recBegin->header.timestamp.time);
      cout << "Unix Time Stamp: "          << recBegin->header.timestamp.time << endl;

      gRunInfo->StartTime  = recBegin->header.timestamp.time;
      gRunInfo->fPolBeam   = (recBegin->header.type & REC_MASK_BEAM) >> 16;
      gRunInfo->fPolStream = (recBegin->header.type & REC_MASK_STREAM) >> 20;

      //printf("fPolBeam:   %#x\n", gRunInfo->fPolBeam);
      //printf("fPolStream: %#x, %#x, %#x\n", gRunInfo->fPolStream, (UInt_t) recBegin->header.type, REC_MASK_STREAM);

      int polId = gRunInfo->GetPolarimeterId(gRunInfo->fPolBeam, gRunInfo->fPolStream);

      if (polId < 0)
         printf("WARNING: Polarimeter ID will be defined by other means\n");
      
      stringstream sstr;

      // First, try to get polarimeter id from the data
      if (polId >= 0) {
         sstr << gRunInfo->fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = gRunInfo->fPolId;

      // if failed, get id from the file name
      } else if (gRunInfo->GetPolarimeterId() >= 0) {
         sstr.str("");
         sstr << gRunInfo->fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = gRunInfo->fPolId;

      // see if the polarimeter id was set by command line argument 
      } else if (gRunInfo->fPolId >= 0) {
         sstr.str("");
         sstr << gRunInfo->fPolId;
         gRunDb.fFields["POLARIMETER_ID"] = sstr.str();
         gRunDb.fPolId = gRunInfo->fPolId;

      } else { // cannot proceed

         printf("ERROR: Unknown polarimeter ID\n");
         exit(-1);
      }

      sstr.str("");
      sstr << gRunInfo->StartTime;
      gRunDb.fFields["START_TIME"] = sstr.str();
      gRunDb.timeStamp = gRunInfo->StartTime; // should be always defined in raw data
      //gRunDb.Print();

		if (run) {
		   run->polarimeter_id = gRunDb.fPolId;
         mysqlpp::DateTime dt(gRunDb.timeStamp);
		   run->start_time     = dt;
		}

   //} else
      //printf("ERROR: Cannot read REC_BEGIN record\n");
} //}}}


/** */
void RawDataProcessor::ReadRunInfo(MseRunInfoX &runInfo)
{ //{{{
   Info("ReadRunInfo", "Started reading run info from data file...");

   TStopwatch sw;

   recordHeaderStruct *mHeader;

   char *mSeek = fMem;

   while (true) {

      //int nRecs = fread(&header, sizeof(recordHeaderStruct), 1, fp);

      if (mSeek > fMem + fMemSize - 1) break;
      //if (nRecs != 1) break;

      //printf("Currently consider record: %0#10x, len: %ld\n", (UInt_t) header.type, header.len);

      mHeader = (recordHeaderStruct*) mSeek;

      //printf("Currently consider record: %0#10x, len: %ld (MEM)\n", (UInt_t) mHeader->type, mHeader->len);

      // REC_BEAMADO
      if ((mHeader->type & REC_TYPEMASK) == REC_BEAMADO)
		{
         recordBeamAdoStruct *rec = (recordBeamAdoStruct*) mHeader;

         gRunInfo->SetBeamEnergy(rec->data.beamEnergyM);
         fprintf(stdout, "Beam Energy: %8.2f\n", gRunInfo->GetBeamEnergy());
         fprintf(stdout, "RHIC Beam:   %1d\n", gRunInfo->fPolBeam);

         mSeek = mSeek + mHeader->len;
			continue;
		}

      // REC_POLADO
      if ((mHeader->type & REC_TYPEMASK) == REC_POLADO)
		{
         recordPolAdoStruct *rec = (recordPolAdoStruct*) mHeader;

			ProcessRecord( (recordPolAdoStruct&) *rec, runInfo);

         mSeek = mSeek + mHeader->len;
			continue;
		}

      mSeek = mSeek + mHeader->len;
   }

   sw.Stop();
   printf("Stopped reading data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());

   Info("ReadRunInfo", "End of read run info\n");
} //}}}


/** */
void RawDataProcessor::ReadDataFast()
{ //{{{
   Info("ReadDataFast", "Started reading data file...");

   TStopwatch sw;

   recordHeaderStruct *mHeader;

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

      if ((mHeader->type & REC_TYPEMASK) != REC_READAT)
		{
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

      //recordReadATStruct *ATPtrF;
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

            //if (nReadEvents % gAnaInfo->fThinout == 0)
            //if (nReadEvents > 100) break;

				// Use all events to fill pulser histograms
            if ( gAnaInfo->HasPulserBit() &&
				     gAsymRoot->fChannelEvent->PassCutEmptyBunch() &&
					  gAsymRoot->fChannelEvent->PassCutSiliconChannel() )
				{
               gAsymRoot->fHists->d["pulser"]->FillPreProcess(gAsymRoot->fChannelEvent);
				}

            // Use only a fraction of events
            if (gRandom->Rndm() > gAnaInfo->fFastCalibThinout) continue;

            gAsymRoot->SetChannelEvent(ATPtr->data[j], delim, chId);

            if (gAnaInfo->HasPmtBit() &&
					 gAsymRoot->fChannelEvent->PassCutPmtChannel() &&
                gAsymRoot->fChannelEvent->PassCutPmtNoise() )
            {
               gAsymRoot->fHists->d["pmt"]->FillPreProcess(gAsymRoot->fChannelEvent);
				}

            if ( !gAsymRoot->fChannelEvent->PassCutNoise() )          continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutEnabledChannel() ) continue;
            if ( !gAsymRoot->fChannelEvent->PassCutPulser() )         continue;
            if ( !gAsymRoot->fChannelEvent->PassCutSiliconChannel() ) continue;
            //if ( !gAsymRoot->fChannelEvent->PassCutDepEnergyTime() ) continue;

            gAsymRoot->FillPreProcess();
				//gAsymRoot->PrintChannelEvent();

            nTotalEvents++;
         }
      }
   }

   sw.Stop();
   printf("Stopped reading data file: %f s, %f s\n", sw.RealTime(), sw.CpuTime());

   printf("Total events read:     %12d\n", nReadEvents);
   printf("Total events accepted: %12d\n", nTotalEvents);

   Info("ReadDataFast", "End of read data fast\n");
} //}}}


// read loop routine
void readloop(MseRunInfoX &run)
{ //{{{
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

   //polDataStruct            poldat;
   beamDataStruct           beamdat;
   targetDataStruct         tgtdat1;
   targetDataStruct         tgtdat2;

   processEvent             event;
   AsymRecover              recover;

   FILE *fp;
   int   rval;
   int   recSize;
   int   flag = 0;     // exit from while when flag==1
   //int THINOUT = Flag.feedback ? gAnaInfo->fThinout : Nskip;

   // reading the data till its end ...
   if ((fp = fopen(gAnaInfo->GetRawDataFileName().c_str(), "r")) == NULL) {
      printf("ERROR: %s file not found. Force exit.\n", gAnaInfo->GetRawDataFileName().c_str());
      exit(-1);
   } else
      printf("\nFound file %s\n", gAnaInfo->GetRawDataFileName().c_str());


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

      //printf("type: %#X\n", (rec.header.type & REC_TYPEMASK));

      //Long_t my_i;
      //Long_t *my_pointer;

      // Main Switch
      switch (rec.header.type & REC_TYPEMASK) {

      case REC_BEGIN:
         if (gReadFlag.RECBEGIN) break; // ??? should encounter only once in each file

         cout << "Data stream version: " << rec.begin.version << endl;
         cout << "Comment: "             << rec.begin.comment << endl;
         cout << "Date & Time: "         << ctime(&rec.begin.header.timestamp.time);
         cout << "Timestamp: "           << rec.begin.header.timestamp.time << endl;

         gRunInfo->StartTime = rec.begin.header.timestamp.time;
         gRunInfo->fDataFormatVersion = rec.begin.version;

         gRunInfo->fPolBeam   = (rec.header.type & REC_MASK_BEAM) >> 16;
         gRunInfo->fPolStream = (rec.header.type & REC_MASK_STREAM) >> 20;

         //printf("fPolBeam:   %#x\n", gRunInfo->fPolBeam);
         //printf("fPolStream: %#x, %#x, %#x\n", gRunInfo->fPolStream, rec.header.type, REC_MASK_STREAM);

         // Configure colliding bunch patterns for PHENIX-BRAHMS and STAR
         PrepareCollidingBunchPattern();

         gReadFlag.RECBEGIN = 1;
         
         break;

      //case REC_POLADO:
      //   memcpy(&poldat, &rec.polado.data, sizeof(poldat));
      //   if (!gAnaInfo->HasAlphaBit()) DecodeTargetID(poldat, run);
      //   break;

      case REC_TAGADO:
         memcpy(&tgtdat1, &rec.tagado.data[0], sizeof(tgtdat1));
         memcpy(&tgtdat2, &rec.tagado.data[1], sizeof(tgtdat2));
         break; // later

      case REC_PCTARGET:
			// Do not process this record for calibration runs. May contain
			// invalid ndelim info
         if (!gReadFlag.PCTARGET && !gAnaInfo->HasAlphaBit()) {

            printf("Reading REC_PCTARGET record...\n");

            gNDelimeters  = (rec.header.len - sizeof(rec.header))/(4*sizeof(long));
            long *pointer = (long *) &rec.buffer[sizeof(rec.header)];

            if (gAnaInfo->HasTargetBit()) { ProcessRecordPCTarget(pointer, run); }

            gReadFlag.PCTARGET = 1;
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

         if (gAnaInfo->HasProfileBit()) 
            ProcessRecord( (recordCountRate &) rec.countRate);

         break;

      case REC_WCMADO:
         if (gReadFlag.WCMADO) break; // Read record only once

         printf("Reading REC_WCMADO record... size = %d\n", recSize);

         ProcessRecord( (recordWcmAdoStruct &) rec.wcmado);

         gReadFlag.WCMADO = 1;

         break;

      case REC_BEAMADO:

         if (!gReadFlag.BEAMADO) {

            fprintf(stdout,"Reading Beam Information\n");
            memcpy(&beamdat, &rec.beamado.data, sizeof(beamdat));

            gRunInfo->SetBeamEnergy(beamdat.beamEnergyM);
            fprintf(stdout, "Beam Energy: %8.2f\n", gRunInfo->GetBeamEnergy());
            fprintf(stdout, "RHIC Beam:   %1d\n", gRunInfo->fPolBeam);

            // Add inj_tshift for injection measurements
            if (gRunInfo->GetBeamEnergy() < 30) gAnaInfo->tshift += gAnaInfo->inj_tshift;

            int pat;

            for (int bid=0; bid<120; bid++) {

               pat = beamdat.polarizationFillPatternS[bid*3];

               if (pat > 0) {
                  gSpinPattern[bid] = 1;
               } else if (pat < 0) {
                  gSpinPattern[bid] = -1;
               } else {
                  gSpinPattern[bid] = 0;
               }

               pat = beamdat.measuredFillPatternM[bid*3];

               if (pat > 0) {
                  gFillPattern[bid] = 1;
                  gRunInfo->NFilledBunch++;
               } else if (pat < 0) { // this never happens
                  gFillPattern[bid] = -1;
                  gRunInfo->NFilledBunch++;
               } else {
                  gFillPattern[bid] = 0;
               }
            }

            // Print Spin Pattern and Recover Spin Pattern by User Defined ones
            cout << "\nSpin Pattern Used:" << endl;
            PrintBunchPattern(gSpinPattern);

            if (Flag.spin_pattern >= 0) {
               recover.OverwriteSpinPattern(Flag.spin_pattern);
               PrintBunchPattern(gSpinPattern);
            }

            // Print Fill Pattern and Recover Fill Pattern by User Defined ones
            cout << "\nFill Pattern Used:" << endl;
            PrintBunchPattern(gFillPattern);

            if (Flag.fill_pattern >= 0) {
               recover.OverwriteFillPattern(Flag.fill_pattern);
               PrintBunchPattern(gFillPattern);
            }

            // Mask bad/disabled bunches
            if (gRunInfo->NDisableBunch) {
               recover.MaskFillPattern();
               cout << "\nMasked Fill Pattern : " << endl;
               PrintBunchPattern(gFillPattern);
            }

            // Print active Bunch
            cout << "\nActive bunches: " << endl;

            for (int i=0; i<NBUNCH; i++) {
               if (i%10 == 0) cout << " ";
               ActiveBunch[i] = gFillPattern[i];
               cout << ActiveBunch[i];
            }

            cout << endl;
            gRunInfo->NActiveBunch = gRunInfo->NFilledBunch - gRunInfo->NDisableBunch;
            gReadFlag.BEAMADO = 1;
         }

         gRunInfo->StopTime = rec.header.timestamp.time;
         break;

      case REC_END:
         gRunInfo->StopTime = rec.end.header.timestamp.time;
         break;

      case REC_READRAW:
      case REC_READSUB:
         break;

      case REC_READALL:
         break;

      case REC_WFDV8SCAL:
         if (gAnaInfo->HasScalerBit())
            ProcessRecord(rec.wfd);
         break;

      case REC_SCALERS:
         fprintf(stdout, "Scaler readtime \n");
         fprintf(stdout, "Scaler End Time: %s\n", ctime(&rec.scal.header.timestamp.time));

         gRunInfo->StopTime = rec.header.timestamp.time;

         break;

      case REC_READAT:

        // Display configuration and configure active strips just once
        if (!READ_FLAG) {

           // Configure Active Strip Map
           gRunInfo->ConfigureActiveStrip(mask.detector);
           gRunInfo->PrintConfig();

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
           gAnaInfo->nEventsTotal += Nevent;

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

              if (Nread % gAnaInfo->fThinout == 0) {

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
                 if (gRunInfo->Run >= 6) {
                    cntr.revolution = event.delim*512 + event.rev*2 + event.rev0;

                    if (cntr.revolution > gRunInfo->MaxRevolution)
                       gRunInfo->MaxRevolution = cntr.revolution;

                    if (event.stN == 72 && event.delim != tgt.eventID) {
                       tgt.x += gAnaInfo->target_count_mm * (float)tgt.vector;
                       tgt.vector=-1;
                    }

                    if (event.stN == 72 || event.stN == 73) {
                        switch (event.stN) {
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

                 gAsymRoot->SetChannelEvent(event);

                 if (gAnaInfo->fSaveTrees.any()) { gAsymRoot->AddChannelEvent(); }

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
                 //if (!Flag.feedback && gAnaInfo->RAWHISTOGRAM)
                 //if (event.stN < 72) {
                 //   bunch_dist_raw -> Fill(event.bid);
                 //   strip_dist_raw -> Fill(event.stN);
                 //   tdc_raw        -> Fill(event.tdc);
                 //   adc_raw        -> Fill(event.amp);
                 //   tdc_vs_adc_raw -> Fill(event.amp,event.tdc);

                 //   //ds: Don't we need to compare absolute value of gFillPattern[bid] with 1?
                 //   if (fabs(gFillPattern[event.bid]) != 1)
                 //      tdc_vs_adc_false_bunch_raw->Fill(event.amp, event.tdc);
                 //}

                 // process event for following case:
                 //    fill pattern     = 1
                 //    Calibration mode = 1
                 // also process only if strip is active
                 //if (event.stN >= NSTRIP)
                    //printf("channel111: %d, %d\n", event.stN, gRunInfo->ActiveStrip[event.stN]);
                    //printf("channel111: %d\n", event.stN);

                 //if ( gFillPattern[event.bid] == 1 || gAnaInfo->HasAlphaBit() == 1) // || event.stN >= 72) ) //&&
                 //     //gRunInfo->ActiveStrip[event.stN] )
                 //{
                    event_process(&event);
                 //}

                 if (Nevtot%10000==0) {

                    //ds: Can we simplify this by leaving only one report?
                    //if (Flag.feedback){
                    //   printf("Feedback Mode Ncounts = %ld \r", Nread) ;
                    //} else {
                       printf("%.3f: Proccesing Ncounts = %u \r", gRunInfo->RUNID, Nevtot);
                    //}

                    fflush(stdout);
                 }
              }
           }
        }

        break;

      case REC_RHIC_CONF:

         if (!gReadFlag.RHICCONF) {

            fprintf(stdout, "Reading REC_RHIC_CONF record from file...\n");

            gConfigInfo = (recordConfigRhicStruct *) malloc(sizeof(recordConfigRhicStruct) +
                      (rec.cfg.data.NumChannels - 1) * sizeof(SiChanStruct));

            //XXX printf("TTT: %d\n", rec.cfg.data.NumChannels);

            memcpy(gConfigInfo, &rec.cfg, sizeof(recordConfigRhicStruct) +
                   (rec.cfg.data.NumChannels - 1) * sizeof(SiChanStruct));

            // when we mandatory provide cfg info -- derpecated
            //if (gAnaInfo->RECONFMODE == 1) {
            //   reConfig(gConfigInfo);
            //}

            // Recalculate Run constants
            UpdateRunConst(gConfigInfo);

            gReadFlag.RHICCONF = 1;
         }

         gRunInfo->StopTime = rec.header.timestamp.time;

         break;

      case REC_SUBRUN:
         printf("Processing record: REC_SUBRUN\n");
         gRunInfo->StopTime = rec.header.timestamp.time;
         break;

      default:    // unknown record
         fprintf(stdout, "Encountered Unknown Record \"%#x\"\n",
            (UInt_t) rec.header.type & REC_TYPEMASK);
         break;
      }
   }

   fclose(fp);

   // Post processing
   if (gAnaInfo->HasNormalBit())
      end_process(run);

   fprintf(stdout, "End of data stream \n");
   fprintf(stdout, "End Time: %s\n", ctime(&gRunInfo->StopTime));
   fprintf(stdout, "%ld Carbons are found in\n", Nevcut);
   //fprintf(stdout, "Data Comment: %s\n", rec.end.comment);
   fprintf(stdout, "Total events in file %d\n", gAnaInfo->nEventsTotal);
   fprintf(stdout, "First %d events processed\n", Nread);
   fprintf(stdout, "%d events saved\n", Nevtot);

   gAnaInfo->nEventsProcessed = Nevtot;

   mysqlpp::DateTime dt(gRunInfo->StopTime);
	run.stop_time = dt;

   // Add info to database entry
   stringstream sstr;

   sstr.str(""); sstr << gAnaInfo->nEventsTotal;
   gRunDb.fFields["NEVENTS_TOTAL"] = sstr.str();
   run.nevents_total = gAnaInfo->nEventsTotal;

   sstr.str(""); sstr << gAnaInfo->nEventsProcessed;
   gRunDb.fFields["NEVENTS_PROCESSED"] = sstr.str();
   run.nevents_processed = gAnaInfo->nEventsProcessed;

   sstr.str("");
   sstr << gRunInfo->GetBeamEnergy();

   if (gAnaInfo->HasAlphaBit()) {
      gRunDb.fFields["BEAM_ENERGY"] = "0";
      run.beam_energy = 0;
   } else {
      gRunDb.fFields["BEAM_ENERGY"] = sstr.str();
      run.beam_energy = gRunInfo->GetBeamEnergy();
   }

   // Some incompleted run don't even have REC_READAT flag. Force PrintConfig.
   if (!Nread && !READ_FLAG) {
      gRunInfo->PrintConfig();

      if (gRunDb.run_status_s == "Junk") {
         cout << "\n This is a JUNK run. Force quit. Remove RUN_STATUS=Junk from run.db to process.\n\n";
         exit(-1);
      }
   }
} //}}}


/** */
void UpdateRunConst(TRecordConfigRhicStruct *ci)
{ //{{{
   if (!ci) return;

   //ci->Print();

   float Ct = ci->data.WFDTUnit/2.; // Determine the TDC count unit (ns/channel)
   float L  = ci->data.TOFLength;

   //gRunConsts[0] = RunConst();

   for (UShort_t i=1; i<=ci->data.NumChannels; i++) {

      if (gRunInfo->fDataFormatVersion == 40200 ) // Run 11 version
      {
         L = ci->data.chan[i-1].TOFLength;
      } else
         L = CARBON_PATH_DISTANCE;

      gRunConsts[i] = RunConst(L, Ct);
   }

	// Set values for 0th channel to the average ones
   Float_t sumL      = 0;
	UInt_t  nChannels = 0;

   map<UShort_t, RunConst>::iterator irc = gRunConsts.begin();

	for (; irc!=gRunConsts.end(); ++irc) {
		if (gRunInfo->IsSiliconChannel(irc->first)) {
	      sumL += irc->second.L;
			nChannels++;
	   }
	}

   float avrgL = nChannels > 0 ? sumL / nChannels : CARBON_PATH_DISTANCE;

	gRunConsts[0] = RunConst(avrgL, Ct);

   printf("\nAverage RunConst:\n");
   gRunConsts[0].Print();
} //}}}


// Description : print out spin (Mode=0), fill (Mode=1) pattern
// Input       : Mode
void PrintBunchPattern(int *pattern)
{ //{{{
   char symbol[3][2];

   sprintf(symbol[0], "-"); // -1
   sprintf(symbol[1], "."); //  0
   sprintf(symbol[2], "+"); // +1
 
   for (int i=0; i<NBUNCH; i++) {
 
      if (i%10 == 0) cout << " ";
      cout << symbol[ pattern[i] + 1];
   }
 
   cout << endl;
} //}}}


// Description : Decorde target infomation.
//             : presently the target ID is assumed to be appear at the last of
//             : character string poldat.targetIdS.
// Input       : polDataStruct poldat
void DecodeTargetID(const polDataStruct &poldat, MseRunInfoX &run)
{ //{{{
   cout << endl;
   cout << "target ID = " << poldat.targetIdS << endl;
   //cout << "startTimeS = " << poldat.startTimeS << endl;
   //cout << "stopTimeS = " << poldat.stopTimeS << endl;
 
   gRunDb.fFields["TARGET_ID"] = poldat.targetIdS;
 
   // initiarization
   gRunInfo->fTargetId = '-';
 
   string str(poldat.targetIdS);
 
   if (str.find("Background") < str.size())   gRunInfo->fTargetId = 'B';
   if (str.find("1")         == str.size()-1) gRunInfo->fTargetId = '1';
   if (str.find("2")         == str.size()-1) gRunInfo->fTargetId = '2';
   if (str.find("3")         == str.size()-1) gRunInfo->fTargetId = '3';
   if (str.find("4")         == str.size()-1) gRunInfo->fTargetId = '4';
   if (str.find("5")         == str.size()-1) gRunInfo->fTargetId = '5';
   if (str.find("6")         == str.size()-1) gRunInfo->fTargetId = '6';
   if (str.find("7")         == str.size()-1) gRunInfo->fTargetId = '7';
   if (str.find("8")         == str.size()-1) gRunInfo->fTargetId = '-';

   if (str.find('V') != string::npos) {
      gRunInfo->fTargetOrient = 'V';
      run.target_orient       = 'V';
   }

   if (str.find('H') != string::npos) {
      gRunInfo->fTargetOrient = 'H';
      run.target_orient       = 'H';
   }

   //cout << "target id str: " << str << " " << run.target_orient << endl;

   stringstream sstr; 
   //int target_id;
   sstr << gRunInfo->fTargetId;
   //sstr >> target_id;
   sstr >> run.target_id;
   //run.target_id = target_id;

   // Restore Vertical or Horizontal target information
   // in case this information isn't decorded correctly
   // within REC_PCTARGET routine. If the target is horizontal,
   // then mask 90 degree detector.
   //if (gRunInfo->fTargetOrient == '-') {
 
   //   if (str.find("Vert") == 0 || str.find("V") == 0) {
   //      gRunInfo->fTargetOrient = 'V';
   //      run.target_orient       = 'V';
   //      tgt.VHtarget            = 0;
   //   }
 
   //   if (str.find("Horz") == 0 || str.find("H") == 0) {
   //      gRunInfo->fTargetOrient = 'H';
   //      run.target_orient       = 'H';
   //      tgt.VHtarget            = 1;
   //      // This is too late to reconfigure strip mask because this routine is
   //      // executed at the end of event loop. Too bad. /* March 5,'09 IN */
   //      //      mask.detector = 0x2D;
   //      //      gRunInfo->ConfigureActiveStrip(mask.detector);
   //   }
   //}
} //}}}


// Method name : PrepareCollidingBunchPattern(gRunInfo->fPolBeam)
// Description : Configure phx.bunchpat[] and str.bunchpat[] arrays only for colliding bunches
void PrepareCollidingBunchPattern()
{ //{{{
   for (int i=0; i<NBUNCH; i++){
      phx.bunchpat[i] = 1; // PHENIX bunch patterns
      str.bunchpat[i] = 1; // STAR bunch patterns
   }
 
   if (gRunInfo->fPolBeam == 1) { // Yellow Beam
      for (int j=31; j<40; j++) str.bunchpat[j] = phx.bunchpat[j+40] = 0;
   } else if (gRunInfo->fPolBeam == 2){ // Blue Beam
      for (int j=31; j<40; j++) phx.bunchpat[j] = str.bunchpat[j+40] = 0;
   }
 
   cout << "===== Colliding Bunch pattern =======" << endl;
   cout << " IP2, IP8:  " ; for (int i=0; i<NBUNCH; i++) cout << phx.bunchpat[i] ; cout << endl;
   cout << " IP6, IP10: " ; for (int i=0; i<NBUNCH; i++) cout << str.bunchpat[i] ; cout << endl;
   cout << "=====================================" << endl;
} //}}}


/** */
void ProcessRecord(const recordPolAdoStruct &rec, MseRunInfoX &runInfo)
{ //{{{
   if (!gAnaInfo->HasAlphaBit()) DecodeTargetID((polDataStruct &) rec.data, runInfo);
} //}}}


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
 
   gAsymRoot->FillScallerHists(rec.hist, chId);
 
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
void ProcessRecord(const recordCountRate &rec)
{ //{{{
   UInt_t size = (rec.header.len - sizeof(rec.header))/(sizeof(long));
   Long_t *pointer = (Long_t *) rec.data;
   //Double_t *pointer = (Double_t *) &rec.buffer[sizeof(rec.header)];

   printf("len, size: %ld, %d\n", rec.header.len, size);

   //for (UInt_t i=0; i<size; i++) {
   //   printf("countrate: i: %d, %ld\n", i, *(pointer+i));
   //}

   gAsymRoot->FillProfileHists(size, pointer);
   //gAsymRoot->ProcessProfileHists();
} //}}}


/** */
void ProcessRecordPCTarget(const long* rec, MseRunInfoX &run)
{ //{{{
   const long* pointer = rec;

   //--rec;
   //UShort_t i = 0;
  
   // copy data to a linear array
   Double_t* linRec = new Double_t[gNDelimeters*4 + 8]; // there 2 under and ooverflow bins

   for (Int_t k=0; k<gNDelimeters; k++) {

      *(linRec + k + 1                 ) = *rec++; // Horizontal target
      *(linRec + k + (gNDelimeters+2)  ) = *rec++;
      *(linRec + k + (gNDelimeters+2)*2) = *rec++; // Vertical target
      *(linRec + k + (gNDelimeters+2)*3) = *rec++;

      //printf("%8d %8d %12.3f %12.3f %12.3f %12.3f\n", k, gNDelimeters, *(linRec + k + 1), *(linRec + k + (gNDelimeters+2)  ), *(linRec + k + (gNDelimeters+2)*2), *(linRec + k + (gNDelimeters+2)*3) );
   }

   gAsymRoot->FillTargetHists(gNDelimeters, linRec);
   //gAsymRoot->AnalyzeTargetMoves();

   delete [] linRec;

   // 
   tgt.fNDelim = gNDelimeters;

   //--pointer;
   UShort_t i = 0;

   printf("   index    total        x-pos        y-pos\n");

   for (int k=0; k<gNDelimeters; k++) {

      tgt.Linear[k][1] = *pointer++; // Horizontal target
      tgt.Rotary[k][1] = *pointer++;
      tgt.Linear[k][0] = *pointer++; // Vertical target
      tgt.Rotary[k][0] = *pointer++;

      // force 0 for +/-1 tiny readout as target position.
      if (abs(tgt.Rotary[k][1]) <= 1) tgt.Rotary[k][1] = 0;

      // identify Horizontal or Vertical target from the first target
      // rotary position readout.
      if (k == 0) {
         // From Run09 on, the horizontal/vertical targets are
         // identified by linear motion
         long int tgt_identifyV = gRunInfo->RUNID < 10040 ? tgt.Rotary[k][0] : tgt.Linear[k][0];
         long int tgt_identifyH = gRunInfo->RUNID < 10040 ? tgt.Rotary[k][1] : tgt.Linear[k][1];

         if ( ( !tgt.Rotary[k][0] && !tgt.Rotary[k][1] ) ||
              (  tgt.Rotary[k][0] &&  tgt.Rotary[k][1] ) )
         {
            cout << "ERROR: no target rotary info. Don't know H/V target" << endl;
            //gRunInfo->fTargetOrient = '-';
         }

         if (tgt_identifyV) {
            tgt.VHtarget           = 0;
            //gRunInfo->fTargetOrient = 'V';
            //run.target_orient      = 'V';
            cout << "Vertical Target in finite position - ???" << endl;

         } else if (tgt_identifyH) {
            tgt.VHtarget           = 1;
            //gRunInfo->fTargetOrient = 'H';
            //run.target_orient      = 'H';
            cout << "Horizontal Target in finite position - ???" << endl;

         } else {
            cout << "Warning: Target infomation cannot be recognized - ???" << endl;
         }

         tgt.x       = tgt.Rotary[k][tgt.VHtarget] * gAnaInfo->target_count_mm;
         tgt.Time[i] = k;
         tgt.X[i]    = tgt.x;

         printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
                 TgtIndex[k], tgt.X[TgtIndex[k]],
                 tgt.Rotary[k][0]*gAnaInfo->target_count_mm,
                 tgt.Rotary[k][1]*gAnaInfo->target_count_mm);
      } else {

         TgtIndex[k] = i;

         if (tgt.Rotary[k][1] != tgt.Rotary[k-1][1] ||
             tgt.Rotary[k][0] != tgt.Rotary[k-1][0] )
         {
            TgtIndex[k]                 = ++i;
            tgt.X[TgtIndex[k]]          = tgt.Rotary[k][tgt.VHtarget] * gAnaInfo->target_count_mm;
            tgt.Time[TgtIndex[k]]       = float(k);
            tgt.Interval[TgtIndex[k-1]] = tgt.Time[TgtIndex[k]] - tgt.Time[TgtIndex[k-1]];
            ++nTgtIndex;

            printf("%8d %8d %8d %8d %12.3f %12.3f %12.3f\n", i, k, nTgtIndex,
               TgtIndex[k], tgt.X[TgtIndex[k]],
               tgt.Rotary[k][0]*gAnaInfo->target_count_mm,
               tgt.Rotary[k][1]*gAnaInfo->target_count_mm);

            //++i;
         }
      }

      // target position array including static target motion
      tgt.all.x[k] = tgt.Rotary[k][tgt.VHtarget] * gAnaInfo->target_count_mm ;
   }

   printf("Number of delimiters: %4d\n", gNDelimeters);
   printf("nTgtIndex: %d\n", nTgtIndex);

   tgt.Print();

   // define target histograms (hbook)
   //tgtHistBook();

   // disable 90 degrees detectors for horizontal target 0x2D={10 1101}
   //if (tgt.VHtarget) mask.detector = 0x2D;
} //}}}


/** */
void ProcessRecord(const recordWcmAdoStruct &rec)
{ //{{{

   for (int bid=1; bid<=N_BUNCHES; bid++) {
	   
		float data = rec.data.fillDataM[(bid-1)*3];

		// save non-zero bunches only
      if (!data) continue;

      gRunInfo->fWallCurMon[bid] = data;
      //gRunInfo->fWallCurMonSum += gRunInfo->fWallCurMon[bid] * gFillPattern[bid];
      gRunInfo->fWallCurMonSum += gRunInfo->fWallCurMon[bid];
   }

   //gRunInfo->fWallCurMonAve = gRunInfo->fWallCurMonSum/float(gRunInfo->NActiveBunch);
   gRunInfo->fWallCurMonAve = gRunInfo->fWallCurMonSum / gRunInfo->fWallCurMon.size();

   gAsymRoot->FillRunHists();
} //}}}
