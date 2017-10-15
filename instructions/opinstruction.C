#include "opinstruction.h"

OperationI::OperationI(Operation* o, VarRegister* source):
	op_(o),source_(source) { }

OperationI::~OperationI() {
  delete source_;
  delete op_;
}

string OperationI::toString() const {
  return
    op_->getName()+" "+
    *(source_->name());
}

void OperationI::accept(CodeVisitor* v) {
  Instruction::accept(v);
  op_->accept(v);
}
