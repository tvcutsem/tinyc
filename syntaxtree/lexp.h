#ifndef LEXP_H
#define LEXP_H

#include <iostream>
#include <string>

#include "expression.h"

using namespace std;

/**
 * Left Expression: anything that can be assigned to
 */
class LExpression: public Expression {

   public:
      //constructor
      LExpression() { }

      //desctructor
      virtual ~LExpression() { }

      /**
       * is the lexpression an array access or not?
       */
      virtual bool isArrayAccess() const = 0;

      virtual bool isConstant() const { return false; }

   private:

};

#endif
