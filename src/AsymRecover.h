// Header file for AsymRecover
//
// Contributors: Itaru Nakagawa
//               Dmitri Smirnov
//
// Creation : 04/12/2006         
//

#ifndef ASYM_RECOVER_H
#define ASYM_RECOVER_H

#include <iostream>

#include "AsymGlobals.h"
#include "MeasInfo.h"


class AsymRecover
{
private:
    
public:
   int index;
   int flag;

   struct StructSpinPattern {
      int index;
      int flag;
   } spin_pattern;
 
   void OverwriteSpinPattern(int index);
   void OverwriteFillPattern(int index);
   void MaskFillPattern();
};

#endif
