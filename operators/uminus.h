#ifndef OPERATOR_UMINUS_H
#define OPERATOR_UMINUS_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for negation of integers 
 * or characters (chars need to be converted)
 */

class UMinusOp: public Operator {

   public:
      UMinusOp(): Operator("-") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      /**
       * Not really applicable. Returns false.
       */
      virtual bool isCommutative() const { return false; }

};

int uminusFun(int i1, int i2);

#endif
