#ifndef OPERATOR_EQUAL_H
#define OPERATOR_EQUAL_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for testing equality
 */

class EqualOp: public Operator {

   public:
      EqualOp(): Operator("==") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return true; }

};

int equalFun(int i1, int i2);

#endif
