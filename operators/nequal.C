#include "nequal.h"
#include "evaluate.h"

bool NEqualOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void NEqualOp::accept(OperatorVisitor* v) {
  v->visitNEqualOp(this);
}


Constant* NEqualOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&nequalFun);
}

int nequalFun(int i1, int i2) {
  return (i1 != i2);
}
