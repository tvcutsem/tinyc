#ifndef OPERATOR_TIMES_H
#define OPERATOR_TIMES_H

#include "operator.h"
#include "../type.h"
#include "../visitors/operatorvisitor.h"

using namespace std;

/**
 * Operator for multiplication
 */

class TimesOp: public Operator {

   public:
      TimesOp(): Operator("*") { }

      virtual bool canHandle(Type t) const;

      virtual void accept(OperatorVisitor* v);

      virtual Constant* evaluate(Constant* c1, Constant* c2) const;

      virtual bool isCommutative() const { return true; }

};

int timesFun(int d1, int d2);

#endif
