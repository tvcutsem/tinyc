#include "negate.h"
#include "evaluate.h"

bool NegateOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void NegateOp::accept(OperatorVisitor* v) {
  v->visitNegateOp(this);
}

Constant* NegateOp::evaluate(Constant* c1, Constant* c2) const {
  if (c1->isFalse())
    return new Number(1);
  else
    return new Number(0);
}
