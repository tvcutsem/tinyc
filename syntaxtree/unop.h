#ifndef UNOP_H
#define UNOP_H

#include <iostream>
#include <string>

#include "expression.h"
#include "../visitors/treevisitor.h"
#include "../operators/operator.h"
#include "../type.h"

using namespace std;

class UnaryOp: public Expression {

   public:

      UnaryOp(Expression* e,Operator& op): op_(op),exp_(e) { }

      virtual ~UnaryOp();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitUnaryOp(this);
      }

      /**
       * The type of a unary operation is the type the unary operator maps
       * his argument to
       */
      virtual Type type() const;

      virtual bool isConstant() const { return false; }
      
      //accessors
      Operator&   unaryOp()    const { return op_; }
      Expression* expression() const { return exp_; }

      //mutators
      void setExpression(Expression* newExp);

   private:
      Operator& op_;
      Expression* exp_;

};

#endif
