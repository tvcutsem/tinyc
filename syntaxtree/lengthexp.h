#ifndef LENGTH_EXP_H
#define LENGTH_EXP_H

#include "expression.h"
#include "lexp.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

class LengthExpression: public Expression {

   public:

      LengthExpression(LExpression* lexp): lexp_(lexp) { }

      virtual ~LengthExpression(); 

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitLengthExpression(this);
      }

      /**
       * Length(exp) should always return an integer
       */
      virtual Type type() const { return int_t; }

      virtual bool isConstant() const { return false; }

      //accessors
      LExpression* lexpression() const { return lexp_; }

      //mutators
      void setLExpression(LExpression* newExp);

   private:

      LExpression* lexp_;

};

#endif
