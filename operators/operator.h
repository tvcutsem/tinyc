#ifndef OPERATOR_H
#define OPERATOR_H

#include "../type.h"
#include "../syntaxtree/constant.h"
#include "../visitors/operatorvisitor.h"

#include <iostream>

using namespace std;

/**
 * Common superclass of all operators.
 * Defines the interface which all operators have to implement
 */
class Operator {

   public:
      Operator(const char* symbol): symbol_(symbol) { }

      virtual ~Operator() { };

      /**
       * Specifies whether this operator can handle a variable of 
       * the given type
       * @return true when this operator is defined on the given type
       */
      virtual bool canHandle(Type t) const = 0;

      /**
       * Handy method for printing operators.
       * @return a character denoting the operator
       */
      const char* symbol() const { return symbol_; }

      /**
       * Accept an OperationVisitor.
       * This can be used to generate code for the operator.
       */
      virtual void accept(OperatorVisitor* v) = 0;

      /**
       * Formally, this method states that op: T1 x T2 -> returnType.
       * i.e. it states what the returntype of the operator is, given
       * two argument types.
       * The default implementation always returns int_t, because
       * most operators will always return integers (-,x,/,<,==,...)
       * Subclasses should override this if this is not the case
       *
       * @return the return type of this operator when applied to
       * the given arguments
       */
      virtual Type mapsTo(Type t1, Type t2) const { return int_t; }

      /**
       * Compile-time computation
       */
      virtual Constant* evaluate(Constant* c1, Constant* c2) const = 0;

      /**
       * Is this operator commutative or not?
       * This is important when optimizing code (especially common subexpressions)
       */
      virtual bool isCommutative() const = 0;

   private:

      const char* symbol_;

};

#endif
