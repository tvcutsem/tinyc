#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H

#include "statement.h"
#include "expression.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

class WhileStatement: public Statement {

   public:

      WhileStatement(Expression* cond,Statement* body);

      virtual ~WhileStatement() {
	 delete cond_;
	 delete body_;
      }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitWhileStatement(this);
      }

      //accessors
      Expression* condition() const { return cond_; }
      Statement*  body()      const { return body_; }

      //mutators
      void setCond(Expression* newCond);
      void setBody(Statement* newBody);
      
   private:

      Expression* cond_;
      Statement*  body_;

};

#endif
