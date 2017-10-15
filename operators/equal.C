#include "equal.h"
#include "evaluate.h"

bool EqualOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void EqualOp::accept(OperatorVisitor* v) {
  v->visitEqualOp(this);
}

Constant* EqualOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&equalFun);
}

int equalFun(int i1, int i2) {
  return i1 == i2;
}
