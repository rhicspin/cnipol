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


void DrawObjContainer::ReadFromDir()
{
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
         cout << "ERROR: key not found: " << io->first << endl;
         continue;
      }

      //cout << "Found key: " << endl;
      //key->Print();

      // read obj into memeory and assign a pointer
      TObject* tmpObj = 0;

      if (io->second) {
         //delete io->second;
         tmpObj = io->second;
      }

      io->second = key->ReadObj();

      // overwrite options
      if (((TClass*) io->second->IsA())->InheritsFrom("TH1")) {
         ((TH1*) io->second)->SetOption( ((TH1*) tmpObj)->GetOption() );
      }

   }

   // loop over sub containers
   DrawObjContainerMapIter isub;
   
   for (isub=d.begin(); isub!=d.end(); ++isub) {

      TKey *key = fDir->GetKey(isub->first.c_str());
      
      if (!key) {
         cout << "ERROR: key (dir) not found: " << isub->first << endl;
         continue;
      }

      //cout << "Found key: ";
      //key->Print();

      //key->ls();
      //fDir->cd(isub->first.c_str());

      //gDirectory->ls();

      TDirectory *subdir = (TDirectory*) key->ReadObj();
      //TDirectory *subdir = fDir->GetDirectory(isub->first.c_str());
      isub->second.fDir = subdir;

      //if (!subdir) {
      //   //subdir->Print();
      //   //subdir->ls();
      //} else {
      //   cout << "ERROR: subdir not found: " << isub->first << endl;
      //}

      // replace the object with the one from this dir/file
      //isub->second = tmpContainer;
      //isub->second.fDir = gDirectory;
      isub->second.ReadFromDir();
   }
}

void DrawObjContainer::ReadFromDir(TDirectory *dir)
{
   fDir = dir;
   fDir->cd();
   ReadFromDir();
}


/** Default destructor. */
DrawObjContainer::~DrawObjContainer()
{
};


/**
 *
 */
void DrawObjContainer::Print(const Option_t* opt) const
{
   opt = "";

   //printf("DrawObjContainer:\n");
}


/**
 *
 */
void DrawObjContainer::SaveAllAs(TCanvas &c, string path)
{
   //Bool_t isBatch = gROOT->IsBatch();

   //gROOT->SetBatch(kTRUE);

   if (gSystem->mkdir(path.c_str()) < 0)
      printf("WARNING: Perhaps dir already exists: %s\n", path.c_str());
   else
      printf("created dir: %s\n", path.c_str());

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


      if (io->second) (io->second)->Draw();
      //if (io->second) io->second->Print();

      //sprintf(fName, "%s/%s.eps", path.c_str(), cName);
      //c.SaveAs(fName);
      sprintf(fName, "%s/%s.png", path.c_str(), cName);
      c.SaveAs(fName);
   }

   //return;

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {

      //XXX Temporary cond for test only
      //if (isubd->first.find("channel28") == string::npos) continue;

      string parentPath = path;
      path += "/" + isubd->first;
      isubd->second.SaveAllAs(c, path);
      path = parentPath;
   }

   //gROOT->SetBatch(isBatch);
}


/**
 *
 */
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
      isubd->second.Draw(c);
   }

   //gROOT->SetBatch(isBatch);
}


/**
 *
 */
Int_t DrawObjContainer::Write(const char* name, Int_t option, Int_t bufsize)
{
   if (!fDir) {
      printf("ERROR: DrawObjContainer::Write(): fDir not defined\n");
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
      isubd->second.Write();
   }

   return 0;//((TObject*)this)->Write(name, option, bufsize);
}


/** */
Int_t DrawObjContainer::Write(const char* name, Int_t option, Int_t bufsize) const
{
   return ((const DrawObjContainer*)this)->Write(name, option, bufsize);
}


/** */
void DrawObjContainer::Fill(ChannelEvent *ch, string cutid) { }


/** */
void DrawObjContainer::FillPreProcess(ChannelEvent *ch) { }


/** */
void DrawObjContainer::PostFill()
{
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
      if (io->second) io->second->Delete();
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second.Delete();
   }
}
