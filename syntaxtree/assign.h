#ifndef ASSIGN_H
#define ASSIGN_H

#include <iostream>
#include <string>

#include "statement.h"
#include "expression.h"
#include "lexp.h"
#include "../visitors/treevisitor.h"

using namespace std;

class Assignment: public Statement {

   public:

      Assignment(LExpression* target, Expression* exp):
	   target_(target),exp_(exp) { }

      virtual ~Assignment();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitAssignment(this);
      }

      //accessors
      LExpression* target()     const { return target_; }
      Expression*  expression() const { return exp_; }

      //mutators
      void setTarget(LExpression* newTarget);
      void setExpression(Expression* newExpression);

   private:

      LExpression* target_; /**< variable of array to be assigned to */
      Expression* exp_; /**< expression to evaluate and assign */

};

#endif
