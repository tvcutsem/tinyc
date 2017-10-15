#include "divides.h"
#include "evaluate.h"
#include "../exceptions.h"

bool DividesOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void DividesOp::accept(OperatorVisitor* v) {
  v->visitDividesOp(this);
}

Constant* DividesOp::evaluate(Constant* c1, Constant* c2) const {
  //if c2 is false, thus equal to 0...
  if (c2->isFalse()) {
    throw DivisionByZeroError(c1);
  }
  return eval(c1,c2,&divideFun);
}

int divideFun(int d1, int d2) {
  return (d1/d2);
}
