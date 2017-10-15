#ifndef OPERATOR_PLUS_H
#define OPERATOR_PLUS_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for addition of integers and concatenation
 * of strings
 */

class PlusOp: public Operator {

   public:
      PlusOp(): Operator("+") { }

      /**
       * PlusOp can handle integers as well as characters.
       * It converts characters first.
       */
      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return true; }

};

int plusFun(int i1, int i2);

#endif
