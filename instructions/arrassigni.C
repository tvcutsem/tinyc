#include "arrassigni.h"

ArrayAssignmentI::ArrayAssignmentI(ArrayRegister* array,VarRegister* index,
    			 	   VarRegister* value):
	array_(array),index_(index),value_(value) { }

ArrayAssignmentI::~ArrayAssignmentI() {
  delete array_;
  delete index_;
  delete value_;
}

string ArrayAssignmentI::toString() const {
  return
    *(array_->name())+"["+
    *(index_->name())+"] = "+
    *(value_->name());
}
