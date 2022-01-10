#ifndef AsymRoot_h
#define AsymRoot_h

#include <iostream>
#include <math.h>
#include <map>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "AsymGlobals.h"

#include "AnaEvent.h"
#include "Calibrator.h"
#include "ChannelData.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "EventConfig.h"
//zchang
#include "SpinTuneMotor.h"

class AsymAnaInfo;

enum ECut {
   kCUT_UNKNOWN, kCUT_NOCUT, kCUT_PASSONE, kCUT_PASSONE_RAW, kCUT_PASSONE_RAW_EB,
   kCUT_PASSONE_PULSER, kCUT_PASSONE_PMT, kCUT_PASSONE_CALIB,
   kCUT_PASSONE_CALIB_EB, kCUT_RAW, kCUT_RAW_EB, kCUT_NOISE, kCUT_CARBON,
   kCUT_PMT,
   kCUT_CARBON_EB,
   kCUT_PASSTWO,
   kCUT_PASSONE_STEPPER,
   kCUT_NOISE_LOWER //zchang
};

typedef std::map<ECut, std::set<DrawObjContainer*> >   Cut2DrawObjContainerMap;
typedef Cut2DrawObjContainerMap::iterator              Cut2DrawObjContainerMapIter;


class AsymRoot
{

protected:

   TFile                    *fOutRootFile;   //!
   TFile                    *fOutTreeFile;   //!
   UInt_t                    fTreeFileId;    //!
   Cut2DrawObjContainerMap   fHistCuts;      //!< A map of histogram containers groupped by type of cut (event selection)

public:

   TTree                *fRawEventTree;      //!
   TTree                *fAnaEventTree;      //!
   std::vector<TTree*>   fChannelEventTrees; //!
   AnaEvent             *fAnaEvent;          //!
   ChannelEvent         *fChannelEvent;      //!
   ChannelData          *fChannelData;       //!
   ChannelEventSet       fChannelEvents;     //!
   EventConfig          *fEventConfig;       //!
   //zchang
   SpinTuneMotor        *fSpinTuneMotor;
   DrawObjContainer     *fHists;             //!

public:

   AsymRoot();
   ~AsymRoot();

   void         CreateRootFile(std::string filename);
   void         BookHists();
   void         Calibrate();
   void         Finalize();
   void         CreateTrees();
   void         SetChannelEvent(ATStruct &at, long delim, unsigned chId);
   void         AddSpinFlipperMarker();
   void         ProcessEvent() {};
   void         FillPassOne(ECut cut);
   //zchang
   void         FillSpinTuneMotor();
   void         FillDerivedPassOne();
   void         PostFillPassOne();
   void         PreFill();
   void         Fill(ECut cut);
   void         FillDerived();
   void         PostFill();
   void         FillScallerHists(Int_t *hData, UShort_t chId);
   void         FillTargetHists(Int_t n, Double_t *hData);
   void         FillProfileHists(UInt_t n, Int_t *hData);
   void         FillRunHists();
   //void         AfterBurner();
   void         AddChannelEvent();
   void         PrintEventMap();
   void         PrintChannelEvent();
   void         UpdateCalibrator();
   void         UpdateFromChannelEvent();
   const Calibrator*  GetCalibrator() const;
   void         SaveChannelTrees();
   void         SaveEventTree();
   void         WriteTreeFile();
   void         SaveAs(std::string pattern="^.*$", std::string dir=".");
   const EventConfig* GetMeasConfig() const;
   void         GetMeasConfigs(MeasInfo *&ri, AsymAnaInfo *&ai, AnaMeasResult *&ar);
};

#endif
