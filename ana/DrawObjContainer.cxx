/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "DrawObjContainer.h"

ClassImp(DrawObjContainer)

using namespace std;

/** Default constructor. */
DrawObjContainer::DrawObjContainer() : TObject(), fDir(), o(), d()
{
   fDir = gDirectory;
   fDir->cd();
}

DrawObjContainer::DrawObjContainer(TDirectory *dir) : TObject(), fDir(dir), d()
{
}


/** */
void DrawObjContainer::SetDir(TDirectory *dir) { fDir = dir; fDir->cd(); }


/** */
void DrawObjContainer::ReadFromDir()
{ //{{{
   ObjMapIter io;
   
   for (io=o.begin(); io!=o.end(); ++io) {
      //cout << "Considering dir: " << fDir->GetName() << endl;
      //fDir->ls();
      //fDir->Name();

      // first find the key
      //fDir->cd();
      TKey *key = fDir->GetKey(io->first.c_str());
      //TKey *key = gDirectory->GetKey(io->first.c_str());

      if (!key) {
         Warning("ReadFromDir", "Key %s not found", io->first.c_str());
         continue;
      }

      //cout << "Found key: " << endl;
      //key->Print();

      // read obj into memory and assign a pointer
      TObject* tmpObj = 0;

      if (io->second) {
         //delete io->second;
         tmpObj = io->second;
      }

      io->second = key->ReadObj();

      // overwrite options
      if (((TClass*) io->second->IsA())->InheritsFrom("TH1") && tmpObj) {
         ((TH1*) io->second)->SetOption( ((TH1*) tmpObj)->GetOption() );
      }
   }

   // loop over sub containers and call this function recursively
   DrawObjContainerMapIter isub;
   
   for (isub=d.begin(); isub!=d.end(); ++isub) {

      TKey *key = fDir->GetKey(isub->first.c_str());
      
      if (!key) {
         Warning("ReadFromDir", "Directory key %s not found", isub->first.c_str());
         continue;
      }

      //cout << "Found key: ";
      //key->Print();

      //key->ls();
      //fDir->cd(isub->first.c_str());

      //gDirectory->ls();

      TDirectory *subdir = (TDirectory*) key->ReadObj();
      //TDirectory *subdir = fDir->GetDirectory(isub->first.c_str());
      isub->second->fDir = subdir;

      //if (!subdir) {
      //   //subdir->Print();
      //   //subdir->ls();
      //} else {
      //   cout << "ERROR: subdir not found: " << isub->first << endl;
      //}

      // replace the object with the one from this dir/file
      //isub->second = tmpContainer;
      //isub->second.fDir = gDirectory;
      isub->second->ReadFromDir();
   }
} //}}}


/** */
void DrawObjContainer::ReadFromDir(TDirectory *dir)
{ //{{{
   fDir = dir;
   fDir->cd();
   ReadFromDir();
} //}}}


/**
 * Recursively adds objects (histograms) from another DrawObjContainer to this
 * one.
 */
void DrawObjContainer::Add(DrawObjContainer *oc)
{ //{{{
   o.insert(oc->o.begin(), oc->o.end());

   DrawObjContainerMapIter isubd;
   pair<DrawObjContainerMapIter, bool> result;

   //oc->Print();

   for (isubd=oc->d.begin(); isubd!=oc->d.end(); ++isubd) {

      //isubd->second->Print();
         
      result = d.insert(*isubd);
      if (result.second) {
         //d
         continue;
      }

      // This and oc objects contain subdir with the same name. Therefore add
      // objects to the existing one. result.first is a pointer to pair<string, DrawObjContainer*>
      DrawObjContainer *existingOc = (result.first)->second;
      existingOc->Add(isubd->second);
   }
} //}}}


/** Default destructor. */
DrawObjContainer::~DrawObjContainer()
{
}


/** */
void DrawObjContainer::Print(const Option_t* opt) const
{
   //opt = "";

   //printf("DrawObjContainer:\n");

   if (!fDir) {
      Error("Print", "fDir not defined");
      return;
   }

   fDir->cd();

   ObjMapConstIter io;

   for (io=o.begin(); io!=o.end(); ++io) {
      if (io->second) io->second->Print();
   }

   DrawObjContainerMapConstIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      cout << "Content of " << isubd->first << endl;
      isubd->second->Print();
   }
}


/** */
void DrawObjContainer::SaveAllAs(TCanvas &c, std::string pattern, string path)
{
   //Bool_t isBatch = gROOT->IsBatch();

   //gROOT->SetBatch(kTRUE);

   if (gSystem->mkdir(path.c_str()) < 0)
      Warning("SaveAllAs", "Perhaps dir already exists: %s", path.c_str());
   else
      Info("SaveAllAs", "Created dir: %s", path.c_str());

   ObjMapIter io;

   char cName[256];
   char fName[256];

   for (io=o.begin(); io!=o.end(); ++io) {

      if (!io->second) {
         printf("Object not found\n");
         continue;
      }

      sprintf(cName, "c_%s", io->first.c_str());
      c.cd();
      c.SetName(cName);
      c.SetTitle(cName);

      if (((TClass*) io->second->IsA())->InheritsFrom("TH1")) {
         //c.SetLogz(kTRUE);
         char *l = strstr(((TH1*)io->second)->GetOption(), "LOGZ");
         //printf("XXX1: set logz %s\n", ((TH1*)io->second)->GetOption());
         if (l) { c.SetLogz(kTRUE);
            //printf("XXX2: set logz \n");
         } else { c.SetLogz(kFALSE); }
      }// else { c.SetLogz(kFALSE); }

      if (io->second) {
         (io->second)->Draw();

         c.Modified();
         c.Update();

         TPaveStats *stats = (TPaveStats*) (io->second)->FindObject("stats");

         if (stats) {
            printf("found stats\n");
            stats->SetX1NDC(0.84);
            stats->SetX2NDC(0.99);
            stats->SetY1NDC(0.10);
            stats->SetY2NDC(0.50);
         } else {
            //printf("could not find stats\n");
         }
      }

      //if (io->second) io->second->Print();

      //sprintf(fName, "%s/%s.eps", path.c_str(), cName);
      //c.SaveAs(fName);
      sprintf(fName, "%s/%s.png", path.c_str(), cName);

      if (TPRegexp(pattern).MatchB(fName)) {
         c.SaveAs(fName);
      } else {
         //Info("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName);
      }
   }

   //return;

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {

      //XXX Temporary cond for test only
      //if (isubd->first.find("channel28") == string::npos) continue;
      //if (isubd->first.find("preproc") == string::npos) continue;

      string parentPath = path;
      path += "/" + isubd->first;
      isubd->second->SaveAllAs(c, pattern, path);
      path = parentPath;
   }

   //gROOT->SetBatch(isBatch);
}


/** */
void DrawObjContainer::Draw(TCanvas &c)
{
   //Bool_t isBatch = gROOT->IsBatch();

   //gROOT->SetBatch(kTRUE);

   ObjMapIter io;

   char cName[256];

   for (io=o.begin(); io!=o.end(); ++io) {

      sprintf(cName, "c_%s", io->first.c_str());
      c.cd();
      c.SetName(cName);
      c.SetTitle(cName);

      //if (io->second) (io->second)->Draw();
      if (io->second) io->second->Print();
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->Draw(c);
   }

   //gROOT->SetBatch(isBatch);
}


/** */
Int_t DrawObjContainer::Write(const char* name, Int_t option, Int_t bufsize)
{
   if (!fDir) {
      Error("Write", "Directory fDir not defined");
      return 0;
   }

   fDir->cd();
   //fDir->Write();

   ObjMapIter io;

   for (io=o.begin(); io!=o.end(); ++io) {
      //sprintf(cName, "c_%s", io->first.c_str());
      if (io->second) io->second->Write();
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      cout << "Writing content of " << isubd->first << endl;
      isubd->second->Write();
   }

   return 0;//((TObject*)this)->Write(name, option, bufsize);
}


/** */
Int_t DrawObjContainer::Write(const char* name, Int_t option, Int_t bufsize) const
{
   return ((const DrawObjContainer*)this)->Write(name, option, bufsize);
}


/** */
void DrawObjContainer::Fill(ChannelEvent *ch, string sid)
{
   //ObjMapIter io;

   //for (io=o.begin(); io!=o.end(); ++io) {

   //   //TObject* tmpObj = io->second ? io->second : 0;

   //   // overwrite options
   //   if (io->second && ((TClass*) io->second->IsA())->InheritsFrom("TH1") ) {
   //      //sprintf(cName, "c_%s", io->first.c_str());
   //      ((TH1*) io->second)->Fill(ch, sid);
   //   }
   //}

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->Fill(ch, sid);
   }
}


/** */
void DrawObjContainer::FillPreProcess(ChannelEvent *ch) { }


/** */
void DrawObjContainer::PreFill(string sid)
{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->PreFill(sid);
   }
}


/** */
void DrawObjContainer::PostFill()
{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->PostFill();
   }
}


/** */
void DrawObjContainer::Delete()
{
   if (!fDir) {
      printf("ERROR: DrawObjContainer::Delete(): fDir not defined\n");
      return;
   }

   fDir->cd();

   ObjMapIter io;

   for (io=o.begin(); io!=o.end(); ++io) {
      //sprintf(cName, "c_%s", io->first.c_str());
      //cout << "YYY: " << io->first << endl;
      if (io->second) io->second->Delete();
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->Delete();
   }
}
