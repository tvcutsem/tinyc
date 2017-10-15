#ifndef OPERATOR_GREATER_H
#define OPERATOR_GREATER_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for comparison
 */

class GreaterOp: public Operator {

   public:
      GreaterOp(): Operator(">") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return false; }

};

int greaterFun(int i1, int i2);

#endif
