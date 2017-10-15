#ifndef RETURN_STMT_H
#define RETURN_STMT_H

#include "statement.h"
#include "expression.h"

#include <iostream>
#include <string>

using namespace std;

class ReturnStatement: public Statement {

   public:
      //constructor
      ReturnStatement(Expression* e): exp_(e) { }

      //desctructor
      virtual ~ReturnStatement() { delete exp_; }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitReturnStatement(this);
      }

      //accessors
      Expression* expression() const { return exp_; }

      //mutators
      void setExpression(Expression* newExp);

   private:
      Expression* exp_;

};

#endif
