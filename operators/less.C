#include "less.h"
#include "evaluate.h"

bool LessOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void LessOp::accept(OperatorVisitor* v) {
  v->visitLessOp(this);
}

Constant* LessOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&lessFun);
}

int lessFun(int i1, int i2) {
  return i1 < i2;
}
