#ifndef VARREF_H
#define VARREF_H

#include <iostream>
#include <string>

#include "variable.h"
#include "lexp.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

using namespace std;

/**
 * This is a proxy class to ensure that real Variables will not be
 * shared. This is necessary to ensure proper deletion of all objects.
 * The reference will forward any operations on it to its real subject.
 * This is an application of the Proxy Design Pattern
 */
class VariableRef: public Variable {

   public:

      /**
       * create a reference to another Variable, VariableRef will not
       * delete this reference when it gets destroyed
       */
      VariableRef(Variable* ref): reference_(ref) { }

      virtual ~VariableRef() { }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual string* name() const;

      virtual void setName(string* newName);

      virtual void accept(TreeVisitor* v) {
	v->visitVariable(this);
      }

      virtual Type type() const;

      void setReference(Variable* newRef) {
	reference_ = newRef;
      }

      virtual bool isGlobal() const;

      /**
       * Returns the 'real' variable reference
       */
      virtual Variable* reference() const;

   protected:

      //our reference
      Variable* reference_;
};

#endif
