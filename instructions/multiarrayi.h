#ifndef MULTI_ARRAY_H
#define MULTI_ARRAY_H

#include "instruction.h"
#include "../visitors/codevisitor.h"
#include "arrayregister.h"

#include <string>
#include <iostream>

using namespace std;

/**
 * A MULTIARRAY instruction 'declares' the dimensions of a
 * multidimensional array at runtime, making use of the TinyRuntime class.
 */
class MultiArrayI: public Instruction {

  public:

    MultiArrayI(ArrayRegister* array): array_(array) { }

    virtual ~MultiArrayI(); 

    virtual string toString() const;

    ArrayRegister* getArray() const { return array_; }

    virtual void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitMultiArrayInstruction(this);
    };

  private:
    ArrayRegister* array_;
  
};

#endif
