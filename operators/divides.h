#ifndef OPERATOR_DIVIDES_H
#define OPERATOR_DIVIDES_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for division of integers (or characters)
 */

class DividesOp: public Operator {

   public:
      DividesOp(): Operator("/") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return false; }

};


int divideFun(int d1, int d2);

#endif
