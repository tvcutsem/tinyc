#ifndef ARR_ALLOC_INSTRUCTION_H
#define ARR_ALLOC_INSTRUCTION_H

#include "instruction.h"
#include "varregister.h"
#include "arrayregister.h"
#include "numregister.h"

#include <string>

using namespace std;
 
/**
 * An Array Allocation Instruction takes an array target and
 * allocates an array of the given length
 */

class ArrayAllocI: public Instruction {

  public:

    /**
     * Creates a new Array Allocation Instruction of the form
     * A = ARRAY(L,d) where A is the target, L is the length of the
     * one-dimensional array, and d is the number of dimensions
     */
    ArrayAllocI(ArrayRegister* target, VarRegister* length, NumRegister* dimension):
      target_(target), length_(length), dim_(dimension) { }

    ~ArrayAllocI() {
      delete target_;
      delete length_;
      delete dim_;
    }

    //accessors
    ArrayRegister* getTarget()    const { return target_; }
    VarRegister*   getLength()    const { return length_; }
    NumRegister*   getDimension() const { return dim_; }

    //mutators
    void setTarget(ArrayRegister* target) { target_ = target; }
    void setLength(VarRegister* length) { length_ = length; }
    void setDimension(NumRegister* dim) { dim_ = dim; }

    string toString() const {
      return *(target_->name()) + " = ARRAY[" + *(length_->name()) +
	     "," + *(dim_->name()) + "]";
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitArrayAllocInstruction(this);
    }

  private:
    ArrayRegister* target_;
    VarRegister*   length_;
    NumRegister*   dim_;
 
};

#endif
