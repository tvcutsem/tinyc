#ifndef ARRAY_H
#define ARRAY_H

#include "identifier.h"
#include "aggregates.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Abstract superclass of both arrays and their proxies, following
 * to the Proxy design pattern
 */
class Array: public Identifier {

   public:

      //desctructor
      virtual ~Array() { }; 

      virtual ostream& prettyPrint(ostream& os=cout,int indent=0) const = 0;

      virtual string toString() const = 0;

      virtual Type type() const { return array_t; }

      virtual Type baseType() const = 0;

      /**
       * Returns a list of all expressions representing
       * the length of each dimension
       */
      virtual Expressions& lengths() const = 0;

      /**
       * Returns the number of dimensions in this array,
       * An invariant is: array.lengths().size() == dimension()
       */
      virtual int dimension() const = 0;

      virtual bool isFunction() const { return false; }

      virtual bool isVariable() const { return false; }

      virtual bool isArray() const { return true; }

      /**
       * Accepts a TreeVisitor, according to the visitor pattern
       */
      virtual void accept(TreeVisitor* v) = 0;

      //Three Address Code relevant methods

      /**
       * Set the index registers containing the calculated
       * length of each dimension of the array.
       */
      virtual void setIndices(Variables* indexlist) = 0;

      /**
       * @return the index registers containing the lengths
       * of the dimensions of the array
       */
      virtual Variables* getIndices() const = 0;

      /**
       * Is this array globally declared?
       */
      virtual bool isGlobal() const = 0;

};

#endif
