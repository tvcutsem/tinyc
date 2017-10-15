#ifndef ARR_ASSIGN_I_H
#define ARR_ASSIGN_I_H

#include "instruction.h"
#include "varregister.h"
#include "arrayregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An array assignment assigns a value to a slot of an array
 */
class ArrayAssignmentI: public Instruction {

  public:
    ArrayAssignmentI(ArrayRegister* array,VarRegister* index,VarRegister* value);

    ~ArrayAssignmentI();

    string toString() const;

    //accessors
    ArrayRegister* getArrayRegister() const { return array_; }
    VarRegister*   getIndex()         const { return index_; }
    VarRegister*   getValue()         const { return value_; }

    //mutators
    void setArrayRegister(ArrayRegister* array) {
      delete array_;
      array_ = array;
    }

    void setIndex(VarRegister* index) {
      delete index_;
      index_ = index;
    }

    void setValue(VarRegister* value) {
      delete index_;
      value_ = value;
    }

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitArrayAssInstruction(this);
    }
    
  private:
    ArrayRegister* array_;
    VarRegister* index_;
    VarRegister* value_;
  
};

#endif
