// Header file for AsymRecover
// Author   : Itaru Nakagawa
// Creation : 04/12/2006         

#ifndef ASYM_RECOVER_H
#define ASYM_RECOVER_H



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


  int OverwriteSpinPattern(int index);
  int MaskFillPattern();

};



#endif /* ASYM_RECOVER_H */
