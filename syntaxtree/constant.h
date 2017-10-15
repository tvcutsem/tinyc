#ifndef CONSTANT_H
#define CONSTANT_H

#include "expression.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Something denoting a constant literal
 */
class Constant: public virtual Expression {

   public:

      /**
       * tells whether this expression is a constant or not
       */
      virtual bool isConstant() const { return true; }

      /**
       * tells whether this literal denotes something is false
       */
      virtual bool isFalse() const = 0;

      virtual bool isTrue() { return !isFalse(); }

      virtual bool isNumber() const = 0;

      virtual bool isQChar() const = 0;

};

#endif
