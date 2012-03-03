/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "DrawObjContainer.h"

#include "EventConfig.h"

ClassImp(DrawObjContainer)

using namespace std;


/** Default constructor. */
DrawObjContainer::DrawObjContainer() : TObject(), fSignature(""), fDir(), o(),
   d()
{
   fDir = gDirectory;
   fDir->cd();
}


/** */
DrawObjContainer::DrawObjContainer(TDirectory *dir) : TObject(),
   fSignature(""), fDir(dir), o(), d()
{
}


/** */
std::string DrawObjContainer::GetSignature()
{ //{{{
   return fSignature;
} //}}}


/** */
void DrawObjContainer::SetSignature(std::string signature)
{ //{{{
   fSignature = signature;
} //}}}


/** */
void DrawObjContainer::SetDir(TDirectory *dir)
{ //{{{
   fDir = dir;
   fDir->cd();
} //}}}


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
{ //{{{
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
} //}}}


/** */
void DrawObjContainer::SaveAllAs(TCanvas &c, std::string pattern, string path, Bool_t thumbs)
{ //{{{
   if (gSystem->mkdir(path.c_str()) < 0)
      Warning("SaveAllAs", "Perhaps dir already exists: %s", path.c_str());
   else {
      gSystem->Chmod(path.c_str(), 0775);
      Info("SaveAllAs", "Created dir: %s", path.c_str());
   }

   ObjMapIter io;

   char cName[256];
   char fName[256];

   for (io=o.begin(); io!=o.end(); ++io) {

      if (!io->second) {
         printf("Object not found\n");
         continue;
      }


      if (thumbs)
         sprintf(cName, "c_%s_thumb", io->first.c_str());
      else 
         sprintf(cName, "c_%s", io->first.c_str());

      c.cd();
      c.SetName(cName);
      c.SetTitle(cName);

      if ( ((TClass*) io->second->IsA())->InheritsFrom("TH1") ) {

         char *l = strstr(((TH1*)io->second)->GetOption(), "NOIMG");
         if (l) continue;

         l = strstr( ((TH1*) io->second)->GetOption(), "LOGZ");
         //printf("XXX1: set logz %s\n", ((TH1*)io->second)->GetOption());
         if (l) c.SetLogz(kTRUE);
         else c.SetLogz(kFALSE);

         l = strstr( ((TH1*) io->second)->GetOption(), "XX");
         if (l) c.SetLogx(kTRUE);
         else c.SetLogx(kFALSE);

         l = strstr( ((TH1*) io->second)->GetOption(), "XY");
         if (l) c.SetLogy(kTRUE);
         else c.SetLogy(kFALSE);

         l = strstr( ((TH1*) io->second)->GetOption(), "GRIDX");
         if (l) c.SetGridx(kTRUE);
         else c.SetGridx(kFALSE);

         l = strstr( ((TH1*) io->second)->GetOption(), "GRIDY");
         if (l) c.SetGridy(kTRUE);
         else c.SetGridy(kFALSE);
      }

      if (io->second) {

         if ( ((TClass*) io->second->IsA())->InheritsFrom("THStack") ) {
            (io->second)->Draw("nostack");
         } else
            (io->second)->Draw();

         c.Modified();
         c.Update();
         c.RedrawAxis("g");

         TPaveStats *stats = (TPaveStats*) (io->second)->FindObject("stats");

         if (stats) {
            stats->SetX1NDC(0.84);
            stats->SetX2NDC(0.99);
            stats->SetY1NDC(0.10);
            stats->SetY2NDC(0.50);
         } else {
            //printf("could not find stats\n");
         }

         TText signature;
         signature.SetTextSize(0.03);
         signature.DrawTextNDC(0, 0.01, fSignature.c_str());
      }

      //if (io->second) io->second->Print();

      //sprintf(fName, "%s/%s.eps", path.c_str(), cName);
      //c.SaveAs(fName);
      sprintf(fName, "%s/%s.png", path.c_str(), cName);

      if (TPRegexp(pattern).MatchB(fName)) {
         c.SaveAs(fName);
         gSystem->Chmod(fName, 0775);
      } else {
         //Info("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName);
      }
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {

      //XXX Temporary cond for test only
      //if (isubd->first.find("channel28") == string::npos) continue;
      //if (isubd->first.find("preproc") == string::npos) continue;

      // Overwrite signature by setting it to the parent one
      isubd->second->SetSignature(fSignature);

      string parentPath = path;
      path += "/" + isubd->first;
      isubd->second->SaveAllAs(c, pattern, path, thumbs);
      path = parentPath;
   }
} //}}}


/** */
void DrawObjContainer::SaveHStackAs(TCanvas &c, THStack &hstack, std::string path)
{ //{{{

   // Require at least two elements in the hstack
   if (hstack.GetHists()->GetSize() < 2) {
      Error("SaveHStackAs", "At least 2 histograms required in the HStack. Skipping saving request for %s", hstack.GetName());
      return;
   }

   c.cd();

   TH1 *h1 = (TH1*) hstack.GetHists()->At(0);
   TH1 *h2 = (TH1*) hstack.GetHists()->At(1);

   char *l = strstr(h1->GetOption(), "LOGZ");

   //printf("XXX1: set logz %s\n", ((TH1*)io->second)->GetOption());

   if (l) { c.SetLogz(kTRUE);
      //printf("XXX2: set logz \n");
   } else { c.SetLogz(kFALSE); }

   h1->Draw();
   h2->Draw("sames");
   //hstack.Draw("nostack");

   c.Modified();
   c.Update();

   TPaveStats *stats = (TPaveStats*) h2->FindObject("stats");

   if (stats) {
      stats->SetX1NDC(0.84);
      stats->SetX2NDC(0.99);
      stats->SetY1NDC(0.10);
      stats->SetY2NDC(0.50);
   } else {
      Error("SaveHStackAs", "Could not find stats in %s", h2->GetName());
      //return;
   }

   string fName = path + "/" + hstack.GetName() + ".png";
   //printf("Saving %s\n", fName.c_str());

   c.SetName(hstack.GetName());
   c.SetTitle(hstack.GetName());

   TText signature;
   signature.SetTextSize(0.03);
   signature.DrawTextNDC(0, 0.01, fSignature.c_str());

   //if (TPRegexp(pattern).MatchB(fName.c_str())) {
      c.SaveAs(fName.c_str());
      gSystem->Chmod(fName.c_str(), 0775);
   //} else {
      //Warning("SaveAllAs", "Histogram %s name does not match pattern. Skipped", fName.c_str());
   //}
} //}}}


/** */
void DrawObjContainer::Draw(TCanvas &c)
{ //{{{
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
} //}}}


/** */
Int_t DrawObjContainer::Write(const char* name, Int_t option, Int_t bufsize)
{ //{{{
   if (!fDir) {
      Fatal("Write", "Directory fDir not defined");
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

   //fDir->Write();

   return 0;//((TObject*)this)->Write(name, option, bufsize);
} //}}}


/** */
Int_t DrawObjContainer::Write(const char* name, Int_t option, Int_t bufsize) const
{ //{{{
   return ((const DrawObjContainer*) this)->Write(name, option, bufsize);
} //}}}


/** */
void DrawObjContainer::Fill(EventConfig &rc)
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->Fill(rc);
   }
} //}}}


/** */
void DrawObjContainer::Fill(EventConfig &rc, DrawObjContainer &oc)
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->Fill(rc, oc);
   }
} //}}}


/** */
void DrawObjContainer::FillPreProcess(ChannelEvent *ch) { }


/** */
void DrawObjContainer::PreFillPassOne() {}


/** */
void DrawObjContainer::FillPassOne(ChannelEvent *ch)
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->FillPassOne(ch);
   }
} //}}}


/** */
void DrawObjContainer::FillDerivedPassOne()
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->FillDerivedPassOne();
   }
} //}}}


/** */
void DrawObjContainer::PostFillPassOne(DrawObjContainer *oc)
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->PostFillPassOne(oc);
   }
} //}}}


/** */
void DrawObjContainer::PreFill(string sid)
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->PreFill(sid);
   }
} //}}}


/** */
void DrawObjContainer::Fill(ChannelEvent *ch, string sid)
{ //{{{
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
} //}}}


/** */
void DrawObjContainer::FillDerived()
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->FillDerived();
   }
} //}}}


/** */
void DrawObjContainer::PostFill()
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->PostFill();
   }
} //}}}


/** */
void DrawObjContainer::PostFill(AnaGlobResult &agr)
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->PostFill(agr);
   }
} //}}}


/** */
void DrawObjContainer::UpdateLimits()
{ //{{{
   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->UpdateLimits();
   }
} //}}}


/** */
void DrawObjContainer::Delete(Option_t* option)
{ //{{{
   if (!fDir) {
      Fatal("Delete(Option_t* option)", "fDir not defined\n");
   }

   fDir->cd();

   ObjMapIter io;

   for (io=o.begin(); io!=o.end(); ++io) {
      //sprintf(cName, "c_%s", io->first.c_str());
      //cout << "YYY: " << io->first << endl;
      if (io->second) io->second->Delete(option);
   }

   DrawObjContainerMapIter isubd;

   for (isubd=d.begin(); isubd!=d.end(); ++isubd) {
      isubd->second->Delete(option);
   }

   //fDir->Close();
} //}}}
