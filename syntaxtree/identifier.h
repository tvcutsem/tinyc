#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include "../type.h"
#include "../visitors/treevisitor.h"
#include "lexp.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Common superclass of both variables, arrays and functions.
 * Defines the interface common to these three identifiers.
 */
class Identifier: public LExpression {

   public:

      virtual ~Identifier() { }; 

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const = 0;

      virtual Type type() const = 0;

      virtual string toString() const = 0;

      /**
       * is this identifier a function or not?
       */
      virtual bool isFunction() const = 0;

      /**
       * is this identifier a variable or not?
       */
      virtual bool isVariable() const = 0;

      /**
       * is this identifier an array or not?
       */
      virtual bool isArray() const = 0;

      //an identifier is never an array access
      virtual bool isArrayAccess() const { return false; }

      /**
       * @return the name of this identifier (shared in string pool!)
       */
      virtual string* name() const = 0;

      /**
       * renames this identifier
       */
      virtual void setName(string* newName) = 0;

      /**
       * accepts a TreeVisitor, according to the visitor pattern
       */
      virtual void accept(TreeVisitor* v) = 0;

};

#endif
