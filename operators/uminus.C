#include "uminus.h"
#include "evaluate.h"

bool UMinusOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void UMinusOp::accept(OperatorVisitor* v) {
  v->visitUMinusOp(this);
}

Constant* UMinusOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&uminusFun);
}

int uminusFun(int op1, int op2) {
  return -op1;
}
