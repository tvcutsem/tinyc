#ifndef OPERATOR_NEQUAL_H
#define OPERATOR_NEQUAL_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for testing difference
 */

class NEqualOp: public Operator {

   public:
      NEqualOp(): Operator("!=") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return true; }

};

int nequalFun(int i1, int i2);

#endif
