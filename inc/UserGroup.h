#ifndef USER_GROUP_H
#define USER_GROUP_H
#include "TObject.h"
#include "TSystem.h"

class UserGroup : public TObject{
 public:
 UserGroup() : fUid(0), fGid(0), fUser(), fGroup(), fPasswd(),
    fRealName (), fShell() { }
  
  Int_t    fUid;          // user id
  Int_t    fGid;          // group id
  TString  fUser;         // user name
  TString  fGroup;        // group name
  TString  fPasswd;       // password
  TString  fRealName;     // user full name
  TString  fShell;        // user preferred shell

  UserGroup& operator = (const UserGroup_t &ug){
    fUid = ug.fUid;
    fGid = ug.fGid;
    fUser = ug.fUser;
    fGroup = ug.fGroup;
    fPasswd = ug.fPasswd;
    fRealName = ug.fRealName;
    fShell = ug.fShell;
    return *this;
  }
 private:
  ClassDef(UserGroup, 1);
};

#endif
