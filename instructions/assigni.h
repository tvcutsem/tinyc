#ifndef ASSIGN_I_H
#define ASSIGN_I_H

#include "varregister.h"
#include "instruction.h"
#include "../syntaxtree/identifier.h"

#include <string>

using namespace std;

/**
 * An abstract instruction, representing any instruction in which a result is
 * assigned to a variable
 */
class AssignmentI: public Instruction {

  public:

    AssignmentI(VarRegister* target): target_(target) { }

    ~AssignmentI() { delete target_; }

    /**
     * Returns the target variable to which the result of this instruction
     * will be assigned
     */
    VarRegister* getTarget() const { return target_; }

    void setTarget(VarRegister* target) {
      delete target_;
      target_ = target;
    }

    string toString() const {
      return *(target_->name())+" = ";
    }

  private:
    VarRegister* target_;
  
};

#endif
