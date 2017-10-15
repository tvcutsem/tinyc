#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <string>

#include "identifier.h"
#include "lexp.h"
#include "../type.h"

using namespace std;

/**
 * Common superclass of both variabledec's and variableref's.
 * These classes follow the proxy design pattern. A variableref
 * is a proxy for a variabledec.
 */
class Variable: public Identifier {

   public:

      virtual ~Variable() { }; 

      virtual ostream& prettyPrint(ostream& os = cout,int indent = 0) const = 0;
      
      virtual string toString() const = 0;

      virtual Type type() const = 0;

      virtual bool isVariable() const { return true; }

      virtual bool isFunction() const { return false; }

      virtual bool isArray()    const { return false; }

      /**
       * Is this variable globally declared or not?
       */
      virtual bool isGlobal() const = 0;

      /**
       * Returns the 'real' variable reference
       */
      virtual Variable* reference() const = 0;

};

#endif
