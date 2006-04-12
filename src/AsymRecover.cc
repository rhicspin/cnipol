//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymRecover.cc
//
//  Decscription: Subroutines to handle non-standard analysis
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   4/12/2006
//                


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "rpoldata.h"
#include "Asym.h"
#include "AsymRecover.h"
#include "AsymRecoverUserDefined.h"



//
// Class name  : AsymRecover
// Method name : OverwriteSpinPattern(int index)
//
// Description : Overwrite the spin pattern to save runs whose spin patterns
//             : were screwed up
// Input       : int index
// Return      : 
//
int
AsymRecover::OverwriteSpinPattern(int index){
  
  cerr << "Recovery: Spin pattern is ovewritten by user defined pattern index(" 
       << index << ")" << endl;

  for (int i=0; i<RHIC_MAX_FILL; i++) spinpat[i] = UserDefinedSpinPattern[index][i] ;

  return 1 ;

}

