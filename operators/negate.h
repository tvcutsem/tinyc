#ifndef OPERATOR_NEGATE_H
#define OPERATOR_NEGATE_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for the boolean NOT
 */

class NegateOp: public Operator {

   public:
      NegateOp(): Operator("!") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      /**
       * Not really applicable. Returns false.
       */
      virtual bool isCommutative() const { return false; }

};

#endif
