#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

#include "statement.h"
#include "emptystmt.h"
#include "expression.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

class IfStatement: public Statement {

   public:
      IfStatement(Expression* cond, Statement* thenbranch,
	                            Statement* elsebranch = 0);

      virtual ~IfStatement();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitIfStatement(this);
      }

      //accessors
      Expression* condition()   const { return cond_; }
      Statement*  truebranch()  const { return then_; }
      Statement*  falsebranch() const { return else_; }

      //mutators
      void setCondition(Expression* newCond);
      void setTrueBranch(Statement* newTrue);
      void setFalseBranch(Statement* newFalse);

      /**
       * Called by ConstantFolder to preserve my true branch
       * It signals I should not delete it because it will still
       * be used after my death
       */
      void preserveTrueBranch() { preserveTrue_ = true; }

      /**
       * Called by ConstantFolder to preserve my false branch
       * It signals I should not delete it because it will still
       * be used after my death
       */
      void preserveFalseBranch() { preserveFalse_ = true; }

   private:
      Expression* cond_;
      Statement*  then_;
      Statement*  else_;

      /** indicates destructor should not clean up true branch */
      bool preserveTrue_; 
      /** indicates destructor should not clean up false branch */
      bool preserveFalse_;

};

#endif
