#ifndef SPINTUNEMOTOR_H
#define SPINTUNEMOTOR_H
#include<cstdio>
#include<cstdlib>
#include<cstring>
class SpinTuneMotor
{
 public:
  SpinTuneMotor(const unsigned long size = 6000000){
    //spinTM = (unsigned long long*) (size*sizeof(unsigned long long));
    spinTM = new unsigned long long[size];
    //memset(spinTM, 0, size);// * sizeof(unsigned long long));
    cntTM = 0;
    printf("finish initializing spin tune\n");
  }
  ~SpinTuneMotor(){
    //free(spinTM);
  }
  void Add(unsigned long long value){
    if(cntTM < 6000000){
      spinTM[cntTM] = value;
    }else{
      printf("Memory Out of Range\n");
    }
    cntTM++;
  }
  unsigned long long *Array() const{
    return spinTM;
  }
  unsigned long long At(unsigned int i) const{
    if(i < cntTM){
      return spinTM[i];
    }else
      return 0;
  }
  unsigned int Size() const{
    return cntTM;
  }
 private:
  unsigned long long *spinTM;
  unsigned long cntTM;
  
};
#endif
