#include "minus.h"
#include "evaluate.h"

bool MinusOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void MinusOp::accept(OperatorVisitor* v) {
  v->visitMinusOp(this);
}

Constant* MinusOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&minusFun);
}

int minusFun(int i1, int i2) {
  return i1 - i2;
}
