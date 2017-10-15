#ifndef UNDEFINED_VARIABLE_H
#define UNDEFINED_VARIABLE_H

#include <iostream>
#include <string>

#include "variable.h"
#include "undefined.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

using namespace std;

class UndefinedVariable: public Variable, public UndefinedIdentifier {

   public:
      UndefinedVariable(string* name): UndefinedIdentifier(name) { }

      virtual ostream& prettyPrint(ostream& os = cout,int indent = 0) const {
	os << "undefined identifier: " << *name();
	return os;
      }
      
      virtual string toString() const { return "undefined identifier: "+*name(); }

      virtual Type type() const { return void_t; }

      virtual string* name() const { return UndefinedIdentifier::name(); }

      virtual void accept(TreeVisitor* v) {
	v->visitUndefinedIdentifier(this);
      }

};

#endif
