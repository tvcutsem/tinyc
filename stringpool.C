#include "stringpool.h" 

StringPool* StringPool::instance() {
   if (instance_ == 0)
      instance_ = new StringPool;
   return instance_;
}

void StringPool::destroy() {
  if (instance_)
    delete instance_;
}

//initialize instance_ to zero
StringPool* StringPool::instance_ = 0;

StringPool::~StringPool() {
  for (set<string*>::iterator s=pool_->begin(); s!=pool_->end(); s++) {
    delete (*s);
  }
  delete pool_;
}

string* StringPool::getString(string name) {
   string* found = StringPool::find(name);
   if (found) 
      return found;
   else {
     string* newentry = new string(name);
     pool_->insert(newentry);
     return newentry;
   }
}

string* StringPool::find(string& name){
  StringPoolit found = pool_->find(&name);
  return ((found == pool_->end()) ? 0 : *found);
}
