#include "usecount.h"

UseCount& UseCount::instance() {
  return instance_;
}

void UseCount::incCount(string& funname, Register* r) {
  string name(qualify(funname,r));
  if (useCount_.count(name)>0)
    useCount_[name]++;
  else
    useCount_[name]=1;
}

int UseCount::count(string& funname, Register* r) {
  string name(qualify(funname,r));
  if (useCount_.count(name)>0)
    return useCount_[name];
  else
    return 0;
}

UseCount UseCount::instance_;

ostream& operator<<(ostream& os, UseCount& uc) {
  RegCount& useCount(uc.useCount_);
  cout << "usecount statistics: " << endl;
  for (RegCountit r = useCount.begin(); r != useCount.end(); r++)
    //*r is a pair<string,int>
    os << r->first << ": " << r->second << endl;
  return os;
}

string UseCount::qualify(string& funname, Register* r) {
  return string(funname+"::"+*(r->name()));
}
