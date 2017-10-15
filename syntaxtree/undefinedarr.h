#ifndef UNDEFINED_ARRAY_H
#define UNDEFINED_ARRAY_H

#include "array.h"
#include "undefined.h"
#include "../type.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Class representing a reference to an array not yet declared
 */

class UndefinedArray: public Array, public UndefinedIdentifier {

  public:
    UndefinedArray(string* name): UndefinedIdentifier(name), lengths_(new Expressions) { }

    virtual ~UndefinedArray() { delete lengths_; }

    virtual ostream& prettyPrint(ostream& os=cout,int indent=0) const {
      os << "undefined array: " << *name();
      return os;
    }

    virtual string toString() const {
      return "undefined array: "+ *name();
    }

    virtual Type type() const { return array_t; }

    virtual Type baseType() const { return void_t; }

    virtual string* name() const { return UndefinedIdentifier::name(); }

    virtual Expressions& lengths() const {
      return *lengths_;
    }

    virtual int dimension() const { return 0; }

    virtual void accept(TreeVisitor* v) {
      v->visitUndefinedIdentifier(this);
    }

  private:
    Expressions* lengths_;

};

#endif
