#include <iostream>
#include <map>
#include <string>
#include <time.h>

using std::cout;
using std::endl;
using std::map;
using std::string;

void macros()
{
   gSystem->Load("~/cnipol/build/src/libcnipol.so");
   TFile _file0("/data1/run15/root/18675.001/18675.001.root");

   EventConfig *measConfig = (EventConfig*)_file0.Get("measConfig");

   MeasInfo *mi = measConfig->GetMeasInfo();
   time_t t = mi->GetStartTime();
   cout << "Measurement type: " << RunConfig::AsString(mi->GetMeasType()) << endl;
   cout << "Measurement started at " << ctime(&t);
   cout << "Beam energy: " << mi->GetBeamEnergyReal() << " GeV" << endl;
   cout << "Fill: " << mi->GetFillId() << endl;
   cout << "Polarimeter id: " << mi->GetPolarimeterId() << endl;
   string polarimeterName = RunConfig::AsString(mi->ExtractPolarimeterId(mi->GetRunName()));
   cout << "Polarimeter name: " << polarimeterName << endl;
   cout << "Target id: " << mi->GetTargetId() << endl;
   cout << "Target orient: " << RunConfig::AsString(mi->GetTargetOrient()) << endl;

   map<UShort_t, ChannelCalib> &calib = measConfig->GetCalibrator()->fChannelCalibs;
   cout << "T0:";
   for(map<UShort_t, ChannelCalib>::iterator it = calib.begin(); it != calib.end(); it++)
   {
      UInt_t channelId = it->first;
      const ChannelCalib &channelCalib = it->second;

      if (channelId == 0) continue; // channel id 0 is used to store "mean calibration"
      cout << " " << channelId << "->" << channelCalib.fT0Coef;
   }
   cout << endl;

   AnaMeasResult *result = measConfig->GetAnaMeasResult();
   ValErrPair polarization = result->GetPCPolar();
   cout << "Polarization: " << polarization.ve.first << " +- " << polarization.ve.second << endl;
   ValErrPair phase = result->GetPCPolarPhase();
   cout << "Phase: " << phase.ve.first << " +- " << phase.ve.second << endl;
   ValErrPair profR = result->GetPCProfR();
   cout << "Profile ratio: " << profR.ve.first << " +- " << profR.ve.second << endl;
}
