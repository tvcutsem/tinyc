#ifndef ARRAY_PARAM_I_H
#define ARRAY_PARAM_I_H

#include "instruction.h"
#include "arrayregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * An ArrayParam instruction pushes an array argument for
 * a function call on the stack
 */
class ArrayParamI: public Instruction {

  public:
    ArrayParamI(ArrayRegister* val);

    ~ArrayParamI();

    virtual string toString() const;

    ArrayRegister* getValue() const;

    void setValue(ArrayRegister* newVal);

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitArrayParamInstruction(this);
    }

  private:
    ArrayRegister* value_;
  
};

#endif
