#include "binassigni.h"

BinAssignmentI::BinAssignmentI(Operator* op,VarRegister* target,
    			       VarRegister* a,VarRegister* b):
  AssignmentI(target),binOp_(op),firstArg_(a),secondArg_(b) { }

BinAssignmentI::~BinAssignmentI() {
  delete firstArg_;
  delete secondArg_;
}

string BinAssignmentI::toString() const {
  return
    AssignmentI::toString()+
    *(firstArg_->name())+" "+
    string(binOp_->symbol())+" "+
    *(secondArg_->name());
}
