#include "plus.h"
#include "evaluate.h"

bool PlusOp::canHandle(Type t) const {
   if ((t == int_t)||(t == char_t))
      return true;
   else
      return false;
}

void PlusOp::accept(OperatorVisitor* v) {
  v->visitPlusOp(this);
}


Constant* PlusOp::evaluate(Constant* c1, Constant* c2) const {
  return eval(c1,c2,&plusFun);
}

int plusFun(int i1,int i2) {
  return i1 + i2;
}
