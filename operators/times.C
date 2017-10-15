#include "times.h"
#include "evaluate.h"

bool TimesOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void TimesOp::accept(OperatorVisitor* v) {
  v->visitTimesOp(this);
}

Constant* TimesOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&timesFun);
}

int timesFun(int i1, int i2) {
  return i1 * i2;
}
