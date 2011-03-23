
#include "manalyze.h"

#include "utils/utils.h"

using namespace std;

void manalyze()
{
   initialize();
   //analyze_book_histograms();
   //analyze_fill_histograms(nEvents);
   //analyze_fill_histograms_derivative();
   //analyze_finalize();
}


void initialize()
{
   TString filelist = "list.dat";
	TString fileName;
	string  histName = "hPolarizationProfile";

	TH1* havrg;// = new TH1F();

	UInt_t i = 0;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   while (next && (o = (*next)()) ) {
      //chain->AddFile(((TObjString*) o)->GetName());
      cout << (((TObjString*) o)->GetName()) << endl;
		fileName = "";
		fileName += "/usr/local/polarim/root/" + string(((TObjString*) o)->GetName()) + "/" + string(((TObjString*) o)->GetName()) + ".root";

		TFile *f = new TFile(fileName, "READ");
   	if (!f) exit(-1);

      gH = new DrawObjContainer(f);
		gH->d["profile"] = new CnipolProfileHists();
		gH->ReadFromDir(f);

		TH1* h = (TH1*) gH->d["profile"]->o[histName];
		h->Print();
		h->SetBit(TH1::kIsAverage);

		if (i == 0) havrg = h;
		else havrg->Add(h);

		i++;
   }

	havrg->Print();
}
