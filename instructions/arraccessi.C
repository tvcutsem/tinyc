#include "arraccessi.h"

ArrayAccessI::ArrayAccessI(VarRegister* target,ArrayRegister* array,VarRegister* index):
	AssignmentI(target),array_(array),index_(index) { }

ArrayAccessI::~ArrayAccessI() {
  delete array_;
  delete index_;
}

string ArrayAccessI::toString() const {
  return
    AssignmentI::toString()+
    *(array_->name())+"["+
    *(index_->name())+"]";
}
