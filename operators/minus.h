#ifndef OPERATOR_MINUS_H
#define OPERATOR_MINUS_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for subtraction of two integers
 * or characters (chars need to be converted)
 */

class MinusOp: public Operator {

   public:
      MinusOp(): Operator("-") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return false; }

};

int minusFun(int i1, int i2);

#endif
