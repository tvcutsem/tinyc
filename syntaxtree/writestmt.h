#ifndef WRITE_STATEMENT_H
#define WRITE_STATEMENT_H

#include "statement.h"
#include "expression.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

class WriteStatement: public Statement {

   public:

      WriteStatement(Expression* e): exp_(e) { }

      virtual ~WriteStatement() { delete exp_; }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitWriteStatement(this);
      }

      //accessors
      Expression* expression() const { return exp_; }

      //mutators
      void setExpression(Expression* newExp);

   private:
      Expression* exp_;

};

#endif
