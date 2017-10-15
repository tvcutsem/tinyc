#ifndef UNDEFINED_FUNCTION_H
#define UNDEFINED_FUNCTION_H

#include <list>
#include <iostream>
#include <string>

#include "function.h"
#include "undefined.h"
#include "block.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

using namespace std;

class UndefinedFunction: public Function, public UndefinedIdentifier {

   public:

     UndefinedFunction(string* name): UndefinedIdentifier(name) { }

     virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const {
       os << "undefined function: " << *name();
       return os;
     }

     virtual string toString() const {
       return "undefined function: " + *name();
     }

     virtual ostream& prettyPrintBody(ostream& os = cout,int indent=0) const {
       return prettyPrint(os,indent);
     }

     virtual string toStringBody() const {
       return toString();
     }

     virtual Type type() const { return fun_t; }

     virtual Type returnType() const { return void_t; }

     virtual string* name() const { return UndefinedIdentifier::name(); }

     virtual void parameters(Identifiers& output) const { }

     virtual Block* body() const { return 0; }

     virtual void accept(TreeVisitor* v) {
       v->visitUndefinedIdentifier(this);
     }

     virtual bool operator==(Function& other) const {
       return (this == (UndefinedFunction*) &other);
     }

};

#endif
