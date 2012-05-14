
#include <string>
#include <climits>

#include "AnaFillExternResult.h"
#include "AnaGlobResult.h"
#include "Target.h"

ClassImp(AnaFillExternResult)

using namespace std;


/** */
AnaFillExternResult::AnaFillExternResult() : TObject(),
   fBluIntensGraph(0), fYelIntensGraph(0),
   fBluRotCurStarGraph(0), fYelRotCurStarGraph(0), fBluRotCurPhenixGraph(0),
   fYelRotCurPhenixGraph(0), fBluSnakeCurGraph(0), fYelSnakeCurGraph(0),
   fTimeEventLumiOn(LONG_MAX), fTimeEventLumiOff(LONG_MIN)
{
}


/** */
AnaFillExternResult::AnaFillExternResult(std::ifstream &file) : TObject(),
   fBluIntensGraph(0), fYelIntensGraph(0),
   fBluRotCurStarGraph(0), fYelRotCurStarGraph(0), fBluRotCurPhenixGraph(0),
   fYelRotCurPhenixGraph(0), fBluSnakeCurGraph(0), fYelSnakeCurGraph(0),
   fTimeEventLumiOn(LONG_MAX), fTimeEventLumiOff(LONG_MIN)
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

   vector<Double_t> vBluRotCurStar;
   vector<Double_t> vYelRotCurStar;
   vector<Double_t> vBluRotCurPhenix;
   vector<Double_t> vYelRotCurPhenix;
   vector<Double_t> vBluSnakeCur;
   vector<Double_t> vYelSnakeCur;

   Int_t iline = 0;

   // loop over the entries
   while ( file.good() && !file.eof() )
   {
      iline++;

      file >> time >> bluIntens >> yelIntens >> bluRotatorStar >>
              yelRotatorStar >> bluRotatorPhenix >> yelRotatorPhenix >>
              bluSnakeCurrent >> yelSnakeCurrent;
      //printf("file: %s\n", fileId.c_str());
      //cout << time << " " << bluIntens << " " << bluRotatorStar << " " << bluRotatorPhenix << " " << bluSnakeCurrent
      //             << " " << yelIntens << " " << yelRotatorStar << " " << yelRotatorPhenix << " " << yelSnakeCurrent << endl;

      // Determine the lumi on/off events
      if (time < fTimeEventLumiOn)  fTimeEventLumiOn  = (time_t) time;
      if (time > fTimeEventLumiOff) fTimeEventLumiOff = (time_t) time;

      if (iline % 5 != 0) continue; // every 5 minutes
      if (bluIntens < 1 || yelIntens < 1) continue;

      vTimes.push_back(time);
      vBluIntens.push_back(bluIntens);
      vYelIntens.push_back(yelIntens);
      vBluIntensErr.push_back(bluIntens < 10 ? 0.1 : bluIntens*0.001); // 0.001 error according to Wolfram
      vYelIntensErr.push_back(yelIntens < 10 ? 0.1 : yelIntens*0.001);

      vBluRotCurStar.push_back(bluRotatorStar);
      vYelRotCurStar.push_back(yelRotatorStar);
      vBluRotCurPhenix.push_back(bluRotatorPhenix);
      vYelRotCurPhenix.push_back(yelRotatorPhenix);
      vBluSnakeCur.push_back(bluSnakeCurrent);
      vYelSnakeCur.push_back(yelSnakeCurrent);
   }

   if (vBluIntens.size() > 2) // require at least 3 points
      fBluIntensGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vBluIntens[0], 0, &vBluIntensErr[0]);

   if (vYelIntens.size() > 2) // require at least 3 points
      fYelIntensGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vYelIntens[0], 0, &vYelIntensErr[0]);

   if (vBluRotCurStar.size() > 2) // require at least 3 points
      fBluRotCurStarGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vBluRotCurStar[0], 0, 0);

   if (vYelRotCurStar.size() > 2) // require at least 3 points
      fYelRotCurStarGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vYelRotCurStar[0], 0, 0);

   if (vBluRotCurPhenix.size() > 2) // require at least 3 points
      fBluRotCurPhenixGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vBluRotCurPhenix[0], 0, 0);

   if (vYelRotCurPhenix.size() > 2) // require at least 3 points
      fYelRotCurPhenixGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vYelRotCurPhenix[0], 0, 0);

   if (vBluSnakeCur.size() > 2) // require at least 3 points
      fBluSnakeCurGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vBluSnakeCur[0], 0, 0);

   if (vYelSnakeCur.size() > 2) // require at least 3 points
      fYelSnakeCurGraph = new TGraphErrors(vTimes.size(), (const Double_t*) &vTimes[0], (const Double_t*) &vYelSnakeCur[0], 0, 0);

   //fBluIntensGraph->Print();
   //fYelIntensGraph->Print();
   //fBluRotCurStarGraph->Print();
   //fYelRotCurStarGraph->Print();
   
   file.close();

} //}}}


/** */
void AnaFillExternResult::Process()
{ //{{{
   //Info("Process", "Executing...");
} //}}}
