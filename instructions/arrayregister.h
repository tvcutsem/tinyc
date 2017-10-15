#ifndef ARRAY_REGISTER_H
#define ARRAY_REGISTER_H

#include "register.h"
#include "varregister.h"
#include "instructions.h"
#include "../type.h"
#include "../stringpool.h"
#include "../syntaxtree/array.h"
#include "../visitors/codevisitor.h"

#include <list>
#include <string>
#include <sstream>

using namespace std;

/**
 * An ArrayRegister is a placeholder for a (one-dimensional) array
 */
class ArrayRegister: public Register {
  public:

    /**
     * Construct an ArrayRegister out of an array.
     * Aside from the array, an ArrayRegister also needs a codestream of
     * three address code which produces the length indices of each dimension
     * of the original array. It also receives a list of all Variable references
     * containing the 'calculated' index.
     */
    ArrayRegister(Array* arr): Register(arr->isGlobal()),
      			       name_(arr->name()),baseType_(arr->baseType()) { }

    /**
     * The name of this array
     */
    virtual string* name() const { return name_; }

    /**
     * The type of the elements of this array 
     */
    Type baseType() const { return baseType_; }

    virtual void accept(CodeVisitor* v) {
      v->visitArrayRegister(this);
    }

  private:
    string* name_;
    Type baseType_;

};

#endif
