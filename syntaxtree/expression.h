#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "node.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * An expression is a common superclass of all Expression types
 * in the tiny language, it defines the interface to which all
 * expressions have to adhere.
 */
class Expression: public virtual Node {

   public:

      Expression() { }

      virtual ~Expression() { }

      /**
       * Every expression has a type, this accessor returns that type
       */
      virtual Type type() const = 0;

      /**
       * Tells whether this expression is a constant or not
       */
      virtual bool isConstant() const = 0;

};

#endif
