
#include <string>
#include <climits>

#include "AnaFillExternResult.h"
#include "AnaGlobResult.h"
#include "Target.h"

ClassImp(AnaFillExternResult)

using namespace std;


/** */
AnaFillExternResult::AnaFillExternResult() : TObject(),
   fBluIntensGraph(0), fYelIntensGraph(0), fTimeEventLumiOn(LONG_MAX), fTimeEventLumiOff(LONG_MIN)
{
}


/** */
AnaFillExternResult::AnaFillExternResult(std::ifstream &file) : TObject(),
   fBluIntensGraph(0), fYelIntensGraph(0), fTimeEventLumiOn(LONG_MAX), fTimeEventLumiOff(LONG_MIN)
{
   ReadInfo(file);
}


/** */
AnaFillExternResult::~AnaFillExternResult() { }


TGraphErrors* AnaFillExternResult::GetGrBluIntens() const { return fBluIntensGraph; }
TGraphErrors* AnaFillExternResult::GetGrYelIntens() const { return fYelIntensGraph; }


/** */
void AnaFillExternResult::Print(const Option_t* opt) const
{ //{{{
} //}}}


/** */
void AnaFillExternResult::ReadInfo(std::ifstream &file)
{ //{{{
   if (!file.good()) {
      Error("ReadInfo", "Invalid file provided with external info");
      file.close();
      return;
   }

   Double_t time;
   Double_t bluIntens, bluRotatorStar, bluRotatorPhenix, bluSnakeCurrent;
   Double_t yelIntens, yelRotatorStar, yelRotatorPhenix, yelSnakeCurrent;

   vector<Double_t> vTimes;
   vector<Double_t> vBluIntens;
   vector<Double_t> vYelIntens;
   vector<Double_t> vBluIntensErr;
   vector<Double_t> vYelIntensErr;

   // loop over the entries
   while ( file.good() && !file.eof() ) {
      file >> time >> bluIntens >> yelIntens >> bluRotatorStar >>
              yelRotatorStar >> bluRotatorPhenix >> yelRotatorPhenix >>
              bluSnakeCurrent >> yelSnakeCurrent;
      //printf("file: %s\n", fileId.c_str());
      //cout << time << " " << bluIntens << " " << bluRotatorStar << " " << bluRotatorPhenix << " " << bluSnakeCurrent
      //             << " " << yelIntens << " " << yelRotatorStar << " " << yelRotatorPhenix << " " << yelSnakeCurrent << endl;

      vTimes.push_back(time);
      vBluIntens.push_back(bluIntens);
      vYelIntens.push_back(yelIntens);
      vBluIntensErr.push_back(bluIntens*0.001);
      vYelIntensErr.push_back(yelIntens*0.001);
   }

   if (vBluIntens.size() > 2) { // require at least 3 points
      fBluIntensGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vBluIntens[0], 0, &vBluIntensErr[0]);

      // Determine the lumi on/off events
      Double_t xmin, ymin, xmax, ymax;
      fBluIntensGraph->ComputeRange(xmin, ymin, xmax, ymax);
      fTimeEventLumiOn  = (time_t) xmin;
      fTimeEventLumiOff = (time_t) xmax;
   }

   if (vYelIntens.size() > 2) // require at least 3 points
      fYelIntensGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vYelIntens[0], 0, &vYelIntensErr[0]);

   //fBluIntensGraph->Print();
   //fYelIntensGraph->Print();
   
   file.close();

} //}}}


/** */
void AnaFillExternResult::Process()
{ //{{{
   //Info("Process", "Executing...");
} //}}}
