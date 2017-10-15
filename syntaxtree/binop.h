#ifndef BINOP_H
#define BINOP_H

#include <iostream>
#include <string>

#include "expression.h"
#include "../visitors/treevisitor.h"
#include "../operators/operators.h"
#include "../type.h"

using namespace std;

class BinaryOp: public Expression {

   public:
      //constructor
      BinaryOp(Expression* e1, Expression* e2, Operator& op):
	 op_(op),first_(e1),second_(e2) { }

      //desctructor
      virtual ~BinaryOp();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitBinaryOp(this);
      }

      /**
       * The type of a binary operation is the type the operator returns when
       * given the two arguments of this binary operation
       */
      virtual Type type() const;

      virtual bool isConstant() const { return false; }

      //accessors
      Operator&   binaryOp()  const { return op_; }
      Expression* firstArg()  const { return first_; }
      Expression* secondArg() const { return second_; }

      //mutators
      void setFirstArg(Expression* newArg);
      void setSecondArg(Expression* newArg);

   private:
      Operator&   op_;
      Expression* first_;
      Expression* second_;

};

#endif
