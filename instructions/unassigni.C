#include "unassigni.h"

UnAssignmentI::UnAssignmentI(Operator* op,VarRegister* target,
    			       VarRegister* arg):
  AssignmentI(target),unOp_(op),argument_(arg) { }

UnAssignmentI::~UnAssignmentI() {
  delete argument_;
}

string UnAssignmentI::toString() const {
  return
    AssignmentI::toString()+
    string(unOp_->symbol())+" "+
    *(argument_->name());
}
