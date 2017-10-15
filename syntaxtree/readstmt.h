#ifndef READ_STATEMENT_H
#define READ_STATEMENT_H

#include "statement.h"
#include "lexp.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

class ReadStatement: public Statement {

   public:

      ReadStatement(LExpression* e): target_(e) { }

      virtual ~ReadStatement();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitReadStatement(this);
      }

      //accessors
      LExpression* target() const { return target_; }

      //mutators
      void setTarget(LExpression* newTarget);

   private:
      LExpression* target_;

};

#endif
