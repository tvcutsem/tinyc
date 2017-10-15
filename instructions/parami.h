#ifndef PARAM_I_H
#define PARAM_I_H

#include "instruction.h"
#include "varregister.h"
#include "../visitors/codevisitor.h"

#include <string>

using namespace std;

/**
 * A Param instruction pushes an argument for a function call on the stack
 */
class ParamI: public Instruction {

  public:
    ParamI(VarRegister* val);

    ~ParamI();

    virtual string toString() const;

    VarRegister* getValue() const;

    void setValue(VarRegister* newVal);

    void accept(CodeVisitor* v) {
      Instruction::accept(v);
      v->visitParamInstruction(this);
    }

  private:
    VarRegister* value_;
  
};

#endif
