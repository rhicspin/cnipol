#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <iostream.h>

#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include "rhicpol.h"
#include "rpoldata.h"
#include "rhic_offline.h"
#include "rhic_class.h"


int TAsym::CutBadWf(double cut_slope,int chan,int amp, double square ,double cut_seppen){
  int ans; 
  double slope,seppen;
  double intersection;
  int id;
  double d_slope;

  d_slope=1.3;
  ans = -1;
  if(amp!=0){
    slope=square/(double)amp;
    if( sqrt(pow(slope-cut_slope,2)) < d_slope ) ans=1;
    if(ans==1 ){
       seppen=sqrt(pow(square-cut_slope*amp,2));
       if(seppen < cut_seppen){
           ans =1;
       }else{
           ans=-1;
       }
    }
  }

  return ans;
}

