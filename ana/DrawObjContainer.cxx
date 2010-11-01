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
};

DrawObjContainer::DrawObjContainer(TDirectory *dir) : TObject(), fDir(dir), d()
{
}


void DrawObjContainer::ReadFromDir()
{
   ObjMapIter io;
   
   for (io=o.begin(); io!=o.end(); ++io) {
      //cout << "Considering dir: " << endl;
      //fDir->ls();

      // first find the key
      TKey *key = fDir->GetKey(io->first.c_str());

      if (!key) {
         cout << "ERROR: key not found: " << io->first << endl;
         continue;
      }

      //cout << "Found key: " << endl;
      //key->Print();

      // read obj into memeory and assign a pointer
      if (io->second) delete io->second;
      io->second = key->ReadObj();
   }

   // loop over sub containers
   DrawObjContainerMapIter isub;
   
   for (isub=d.begin(); isub!=d.end(); ++isub) {

      TKey *key = fDir->GetKey(isub->first.c_str());
      
      //if (!key) { printf("error"); continue; }
      //else continue;

      //key->ls();
      fDir->cd(isub->first.c_str());

      //gDirectory->ls();

      //TDirectory *subdir = (TDirectory*) key->ReadObj();
      //TDirectory *subdir = fDir->GetDirectory(isub->first.c_str());

      //if (!subdir) {
      //   //subdir->Print();
      //   //subdir->ls();
      //} else {
      //   cout << "ERROR: subdir not found: " << isub->first << endl;
      //}

      // replace the object with the one from this dir/file
      //isub->second = tmpContainer;
      isub->second.fDir = gDirectory;
      isub->second.ReadFromDir();
   }
}

void DrawObjContainer::ReadFromDir(TDirectory *dir)
{
   fDir = dir;
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

   ObjMapIter io;

   char cName[256];
   char fName[256];

   for (io=o.begin(); io!=o.end(); ++io) {

      sprintf(cName, "c_%s", io->first.c_str());
      c.cd();
      c.SetName(cName);
      c.SetTitle(cName);

      if (io->second) (io->second)->Draw();
      if (io->second) io->second->Print();

      sprintf(fName, "%s/%s.eps", path.c_str(), cName);
      c.SaveAs(fName);
      sprintf(fName, "%s/%s.png", path.c_str(), cName);
      c.SaveAs(fName);
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second.SaveAllAs(c, path);
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
