#ifndef VARDECL_H
#define VARDECL_H

#include <iostream>
#include <string>

#include "variable.h"
#include "lexp.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

using namespace std;

class VariableDec: public Variable {

   public:

      VariableDec(string* name,Type type,bool isGlobal):
	name_(name),type_(type),isGlobal_(isGlobal) { }

      virtual ~VariableDec() { }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual string* name() const { return name_; }

      virtual void setName(string* newName) { name_ = newName; }

      virtual void accept(TreeVisitor* v) {
	 v->visitVariable(this);
      }
      
      virtual Type type() const { return type_; }

      virtual bool isGlobal() const { return isGlobal_; }

      /**
       * Just a reference to myself
       */
      Variable* reference() const;

   protected:

      string* name_; //shared in the string pool
      Type type_;
      bool isGlobal_;
      
};

#endif
