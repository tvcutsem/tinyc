#include "greater.h"
#include "evaluate.h"

bool GreaterOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void GreaterOp::accept(OperatorVisitor* v) {
  v->visitGreaterOp(this);
}

Constant* GreaterOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&greaterFun);
}

int greaterFun(int i1, int i2) {
  return i1 > i2;
}
