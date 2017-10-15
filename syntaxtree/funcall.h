#ifndef FUNCALL_H
#define FUNCALL_H

#include "statement.h"
#include "expression.h"
#include "aggregates.h"
#include "function.h"
#include "../visitors/treevisitor.h"
#include "../type.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Represents an application of a function.
 * Note that a function call is both a statement and an expression,
 * this is conveniently modeled using multiple inheritance. This
 * is also the reason why Statement and Expression inherit virtually
 * from Node. Thus Funcall has only one Node instance.
 */
class Funcall: public Statement, public Expression {

   public:
      /**
       * @param expression denotes whether this function call is used as
       * an expression or as a statement
       */
      Funcall(Function* function,Expressions* args, bool expression):
	fun_(function),args_(args),isExpression_(expression) { }

      virtual ~Funcall();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitFuncall(this);
      }

      /**
       * Denotes whether this function call is an expression or a statement
       */
      bool isExpression() const { return isExpression_; }

      /**
       * The type of a function call is the returnType of the
       * function called
       */
      virtual Type type()       const { return fun_->returnType(); }
      virtual bool isConstant() const { return false; }

      //accessors
      Function*    callee()     const { return fun_; }
      Expressions* arguments()  const { return args_; }

   private:
      Function*    fun_;
      Expressions* args_;
      bool         isExpression_;

};

#endif
